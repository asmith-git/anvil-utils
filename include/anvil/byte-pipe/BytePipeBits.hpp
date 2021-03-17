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

#ifndef ANVIL_LUTILS_BYTEPIPE_BITSTREAM_HPP
#define ANVIL_LUTILS_BYTEPIPE_BITSTREAM_HPP

#include "anvil/byte-pipe/BytePipeCore.hpp"

namespace anvil { namespace BytePipe {

	struct BitOutputStream {
	private:
		uint8_t* _out;				//!< The location that the next complete byte will be written to
		uint32_t _buffer;			//!< The bits that were leftover from the last write
		uint32_t _buffered_bits;	//!< How many bits were leftover from the last write
	public:
		BitOutputStream(uint8_t* o);
		void WriteBits(uint32_t bits, uint32_t bit_count);
	};

	struct BitInputStream {
	private:
		const uint8_t* _in;			//!< The location that the next byte will be read from
		uint32_t _buffer;			//!< The bits that were leftover from the last read
		uint32_t _buffered_bits;	//!< How many bits were leftover from the last read

		void NextByte();
		uint32_t _ReadBits(uint32_t bit_count);
	public:
		BitInputStream(const uint8_t* i);
		uint32_t ReadBits(uint32_t bit_count);
	};


}}

#endif
