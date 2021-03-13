//Copyright 2021 Adam G. Smith
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

#ifndef ANVIL_LUTILS_BYTEPIPE_PACKET_HPP
#define ANVIL_LUTILS_BYTEPIPE_PACKET_HPP

#include <deque>
#include "anvil/byte-pipe/BytePipeReader.hpp"
#include "anvil/byte-pipe/BytePipeWriter.hpp"

namespace anvil { namespace BytePipe {

	/*!
		\page Packet Pipes
		\brief Packet pipes guarantee that downstream pipes will operate on a fixed data size 
		\details This may be required for certain kinds of pipes
	*/

#pragma pack(push, 1)
	struct PacketHeaderVersion1 {
		uint64_t packet_version : 2;	//!< Defines the layout of the packet header, values : 0 - 3
		uint64_t used_size : 16;		//!< The number of bytes in the payload that contain valid data
		uint64_t packet_size : 16;		//!< The size of the packet in bytes (including the header)
		uint64_t reseved : 30;			//!< Unused bits, zeroed by default. May be used by user clases.
	};

	// Small packets
	struct PacketHeaderVersion2 {
		uint32_t packet_version : 2;	//!< Defines the layout of the packet header, values : 0 - 3
		uint32_t used_size : 15;		//!< The number of bytes in the payload that contain valid data
		uint32_t packet_size : 15;		//!< The size of the packet in bytes (including the header)
	};

	// Large packets
	struct PacketHeaderVersion3 {
		uint8_t packet_version;
		uint64_t used_size;		//!< The number of bytes in the payload that contain valid data
		uint64_t packet_size;	//!< The size of the packet in bytes (including the header)
		uint32_t reseved;		//!< Unused bits, zeroed by default. May be used by user clases.
	};

	// Large packets
#pragma pack(pop)

	static_assert(sizeof(PacketHeaderVersion1) == 8u, "Expected PacketHeaderVersion1 to be 8 bytes");
	static_assert(sizeof(PacketHeaderVersion2) == 4u, "Expected PacketHeaderVersion2 to be 4 bytes");
	static_assert(sizeof(PacketHeaderVersion3) == 21u, "Expected PacketHeaderVersion3 to be 21 bytes");

	union PacketHeader {
		PacketHeaderVersion1 v1;
		PacketHeaderVersion2 v2;
		PacketHeaderVersion3 v3;
	};


	class PacketInputStream : public InputPipe {
	private:
		std::deque<uint8_t> _buffer;
		InputPipe& _downstream_pipe;

		void ReadNextPacket();
	protected:
		virtual void OnReadPacket(PacketHeader& header, void* data);
	public:
		PacketInputStream(InputPipe& downstream_pipe);
		virtual ~PacketInputStream();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	class PacketOutputStream : public OutputPipe {
	private:
		OutputPipe& _downstream_pipe;
		uint8_t* _buffer;
		size_t _max_packet_size;
		size_t _current_packet_size;
		uint8_t _default_word;

		void _Flush();
	protected:
		virtual void OnWritePacket(PacketHeader& header, void* data);
	public:
		PacketOutputStream(OutputPipe& downstream_pipe, const size_t packet_size, const uint8_t default_word = 0u);
		virtual ~PacketOutputStream();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

}}

#endif
