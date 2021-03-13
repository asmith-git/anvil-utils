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

	// PacketInputStream

	PacketInputStream::PacketInputStream(InputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	PacketInputStream::~PacketInputStream() {

	}

	void PacketInputStream::ReadNextPacket() {
		// Read the packet header version
		PacketHeader header;
		uint32_t read = _downstream_pipe.ReadBytes(&header, 1u);

		// Error checking
		if (read != 1u) throw std::runtime_error("PacketInputStream::ReadNextPacket : Failed to read packet version");
		if(header.packet_version != 1u) throw std::runtime_error("PacketInputStream::ReadNextPacket : Packet version is not supported");

		// Read the rest of the header
		read = _downstream_pipe.ReadBytes(reinterpret_cast<uint8_t*>(&header) + 1u, sizeof(PacketHeader) - 1u);

		// Allocate a temporary buffer for the data
		uint32_t used_bytes = header.used_size;
		uint32_t unused_bytes = (header.packet_size - sizeof(PacketHeader)) - used_bytes;
		uint8_t* tmp = static_cast<uint8_t*>(_alloca(used_bytes > unused_bytes ? used_bytes : unused_bytes));

		// Read the used payload data
		read = _downstream_pipe.ReadBytes(tmp, used_bytes);
		if (read != used_bytes) throw std::runtime_error("PacketInputStream::ReadNextPacket : Failed reading used packet data");
		for (uint32_t i = 0u; i < used_bytes; ++i) _buffer.push_back(tmp[i]); //! \todo This could be optimised

		// Read the unused payload data
		read = _downstream_pipe.ReadBytes(tmp, unused_bytes);
		if (read != unused_bytes) throw std::runtime_error("PacketInputStream::ReadNextPacket : Failed reading unused packet data");
	}

	uint32_t PacketInputStream::ReadBytes(void* dst, const uint32_t bytes) {
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

	void PacketInputStream::OnReadPacket(PacketHeader& header, void* data) {
		// Do nothing
	}

	// PacketOutputStream

	PacketOutputStream::PacketOutputStream(OutputPipe& downstream_pipe, const size_t packet_size, const uint8_t default_word) :
		_downstream_pipe(downstream_pipe),
		_buffer(nullptr),
		_max_packet_size(packet_size - sizeof(PacketHeader)),
		_current_packet_size(0u),
		_default_word(default_word)
	{
		if (_max_packet_size + sizeof(PacketHeader) > UINT16_MAX) throw std::runtime_error("PacketOutputStream::PacketOutputStream : Packet size is too large");
		_buffer = new uint8_t[packet_size];
	}

	PacketOutputStream::~PacketOutputStream() {
		 _Flush();
		 delete[] _buffer;
		 _buffer = nullptr;
	}

	uint32_t PacketOutputStream::WriteBytes(const void* src, const uint32_t bytes) {
		PacketHeader& header = *reinterpret_cast<PacketHeader*>(_buffer);
		uint8_t* payload = _buffer + sizeof(PacketHeader);

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

	void PacketOutputStream::_Flush() {
		PacketHeader& header = *reinterpret_cast<PacketHeader*>(_buffer);
		uint8_t* payload = _buffer + sizeof(PacketHeader);

		// 'Zero' unused data in the packet
		memset(payload + _current_packet_size, _default_word, _max_packet_size - _current_packet_size);

		// Create the header and format the data
		header.packet_version = 1u;
		header.reseved = 0u;
		header.used_size = _current_packet_size;
		header.packet_size = _max_packet_size + sizeof(PacketHeader);
		OnWritePacket(header, payload);

		// Write the packet to the downstream pipe
		_downstream_pipe.WriteBytes(_buffer, _max_packet_size + sizeof(PacketHeader));

		// Reset the state of this pipe
		_current_packet_size = 0u;
	}

	void PacketOutputStream::Flush() {
		_Flush();
	}

	void PacketOutputStream::OnWritePacket(PacketHeader& header, void* data) {
		// Do nothing
	}

}}