//Copyright 2019 Adam G. Smith
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//http ://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#include "anvil/byte-pipe/BytePipePacket.hpp"

namespace anvil { namespace BytePipe {

	static uint32_t PacketVersionFromSize(const uint64_t size) {
		if (size < 32766u) {
			return 2u;
		} else if (size > UINT16_MAX + 1u) {
			return 3u;
		} else {
			return 1u;
		}
	}

	static ANVIL_CONSTEXPR const uint8_t g_header_sizes[] = {
		sizeof(PacketHeaderVersion1),
		sizeof(PacketHeaderVersion2),
		sizeof(PacketHeaderVersion3)
	};

	// PacketInputPipe

	PacketInputPipe::PacketInputPipe(InputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	PacketInputPipe::~PacketInputPipe() {

	}

	void PacketInputPipe::ReadNextPacket() {
		// Read the packet header version
		PacketHeader header;
		uint32_t read = _downstream_pipe.ReadBytes(&header, 1u);

		// Error checking
		if (read != 1u) throw std::runtime_error("PacketInputPipe::ReadNextPacket : Failed to read packet version");
		uint32_t version = header.v1.packet_version;
		if (version >= 3u) version = header.v3.packet_version; // Read the extended version number
		if(header.v1.packet_version > 3u) throw std::runtime_error("PacketInputPipe::ReadNextPacket : Packet version is not supported");

		// Read the rest of the header
		read = _downstream_pipe.ReadBytes(reinterpret_cast<uint8_t*>(&header) + 1u, g_header_sizes[header.v1.packet_version] - 1u);

		// Allocate a temporary buffer for the data
		uint32_t used_bytes;
		uint32_t packet_size;
		//! \bug Packets larger than UINT32_MAX will cause an integer overflow on the byte count

		if (version == 1u) {
			used_bytes = header.v1.used_size;
			packet_size = header.v1.packet_size;
		} else if (version == 2u) {
			used_bytes = header.v2.used_size;
			packet_size = header.v2.packet_size;
		} else if (version == 3u) {
			used_bytes = header.v3.used_size;
			packet_size = header.v3.packet_size;
		}

		used_bytes += 1u;
		packet_size += 1u;

		// Read the data into the buffer
		const uint32_t unused_bytes = (packet_size - g_header_sizes[version]) - used_bytes;
		uint8_t* tmp = static_cast<uint8_t*>(_alloca(used_bytes + unused_bytes));
		read = _downstream_pipe.ReadBytes(tmp, used_bytes + unused_bytes);
		if (read != used_bytes + unused_bytes) throw std::runtime_error("PacketInputPipe::ReadNextPacket : Failed reading used packet data");

		// Copy the used data into the main buffer
		for (uint32_t i = 0u; i < used_bytes; ++i) _buffer.push_back(tmp[i]); //! \todo This could be optimised
	}

	uint32_t PacketInputPipe::ReadBytes(void* dst, const uint32_t bytes) {
		uint8_t* data = static_cast<uint8_t*>(dst);
		uint32_t b = bytes;

		//! \todo This could be optimised
		while (b != 0u) {
			if (_buffer.empty()) ReadNextPacket();

			*data = _buffer.front();
			_buffer.pop_front();

			--b;
			++data;
		}

		return bytes;
	}

	// PacketOutputPipe

	PacketOutputPipe::PacketOutputPipe(OutputPipe& downstream_pipe, const size_t packet_size, const uint8_t default_word) :
		_downstream_pipe(downstream_pipe),
		_buffer(nullptr),
		_max_packet_size(0u),
		_current_packet_size(0u),
		_default_word(default_word)
	{
		uint32_t version = PacketVersionFromSize(packet_size);
		uint32_t header_size = g_header_sizes[version];
		_max_packet_size = packet_size - header_size;
		_buffer = new uint8_t[packet_size]; // _max_packet_size + header_size
	}

	PacketOutputPipe::~PacketOutputPipe() {
		 _Flush();
		 delete[] _buffer;
		 _buffer = nullptr;
	}

	uint32_t PacketOutputPipe::WriteBytes(const void* src, const uint32_t bytes) {
		const uint32_t version = PacketVersionFromSize(_max_packet_size);
		const uint32_t header_size = g_header_sizes[version];

		PacketHeader& header = *reinterpret_cast<PacketHeader*>(_buffer);
		uint8_t* payload = _buffer + header_size;

		const uint8_t* data = static_cast<const uint8_t*>(src);
		uint32_t b = bytes;

		while (b != 0u) {
			// Copy to the packet buffer
			uint32_t bytes_to_buffer = _max_packet_size - _current_packet_size;
			if (b < bytes_to_buffer) bytes_to_buffer = b;

			memcpy(payload + _current_packet_size, data, bytes_to_buffer);

			data += bytes_to_buffer;
			b -= bytes_to_buffer;
			_current_packet_size += bytes_to_buffer;

			// If the packet is ready then write it
			if (_current_packet_size == _max_packet_size) _Flush();
		}

		return bytes;
	}

	void PacketOutputPipe::_Flush() {
		if (_current_packet_size == 0u) return;

		const uint32_t version = PacketVersionFromSize(_max_packet_size);
		const uint32_t header_size = g_header_sizes[version];

		PacketHeader& header = *reinterpret_cast<PacketHeader*>(_buffer);
		uint8_t* payload = _buffer + header_size;

		// 'Zero' unused data in the packet
		memset(payload + _current_packet_size, _default_word, _max_packet_size - _current_packet_size);

		if (version == 1u) {
			// Create the header
			header.v1.packet_version = 1u;
			header.v1.reseved = 0u;
			header.v1.used_size = _current_packet_size - 1u;
			header.v1.packet_size = (_max_packet_size + header_size) - 1u;
		} else if (version == 2u) {
			// Create the header
			header.v2.packet_version = 2u;
			header.v2.used_size = _current_packet_size - 1u;
			header.v2.packet_size = (_max_packet_size + header_size) - 1u;
		} else if (version == 3u) {
			// Create the header
			header.v3.packet_version = 3u;
			header.v3.reseved = 0u;
			header.v3.used_size = _current_packet_size - 1u;
			header.v3.packet_size = (_max_packet_size + header_size) - 1u;
		}

		// Write the packet to the downstream pipe
		//! \bug Packets larger than UINT32_MAX will cause an integer overflow on the byte count
		_downstream_pipe.WriteBytes(_buffer, _max_packet_size + header_size);

		// Reset the state of this pipe
		_current_packet_size = 0u;
	}

	void PacketOutputPipe::Flush() {
		_Flush();
		_downstream_pipe.Flush();
	}

}}