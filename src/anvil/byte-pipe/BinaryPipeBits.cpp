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

#include "anvil/byte-pipe/BytePipeBits.hpp"

namespace anvil { namespace BytePipe {

	// BitOutputStream

	BitOutputStream::BitOutputStream(uint8_t* o) :
		_out(o),
		_buffer(0u),
		_buffered_bits(0u)
	{}

	void BitOutputStream::WriteBits(uint32_t bits, uint32_t bit_count) {
		while(bit_count > 0u) {
			if (_buffered_bits == 0u) {
				// Write directly to the output
				while (bit_count >= 8u) {
					*_out = static_cast<uint8_t>(bits >> (bit_count - 8u));
					++_out;
					bit_count -= 8u;
					bits &= (1u << bit_count) - 1u;
				}

				// Buffer the remaining bits
				_buffer = bits;
				_buffered_bits = bit_count;
				//bit_count = 0u;
				return;
			} else {
				// We will write as many bits into the buffer as possible
				uint32_t bits_to_write = 8u - _buffered_bits;
				if (bit_count < bits_to_write) bits_to_write = bit_count;

				// Extract the left most bits of the data and add them to the buffer
				const uint32_t extracted = bits >> (bit_count - bits_to_write);
				_buffer <<= bits_to_write;
				_buffer |= extracted;
				_buffered_bits += bits_to_write;

				// Flush the buffer
				if (_buffered_bits == 8u) {
					*_out = static_cast<uint8_t>(_buffer);
					++_out;
					_buffer = 0u;
					_buffered_bits = 0u;
				}

				// Remove bits from the data
				bit_count -= bits_to_write;
				bits &= (1u << bit_count) - 1u;
			}
		}
	}

	// BitInputStream

	void BitInputStream::NextByte() {
		_buffer = *_in;
		++_in;
		_buffered_bits = 8u;
	}

	uint32_t BitInputStream::_ReadBits(uint32_t bit_count) {
		uint32_t bits;

		if (_buffered_bits <= bit_count) {
			// Take all of the bits in the buffer
			bits = _buffer;
			const uint32_t count = _buffered_bits;
			_buffered_bits = 0u;

			if (count < bit_count) {
				// Read the next byte
				NextByte();

				// Read more bits
				const uint32_t bits_remaining = bit_count - count;
				uint32_t next_bits = _ReadBits(bits_remaining);
				bits <<= bits_remaining;
				bits |= next_bits;
			}

		} else {
			// Extract the right most bits from the buffer
			bits = _buffer >> (_buffered_bits - bit_count);
			_buffered_bits -= bit_count;
			_buffer &= (1u << _buffered_bits) - 1u;
		}

		return bits;
	}

	BitInputStream::BitInputStream(const uint8_t* i) :
		_in(i),
		_buffer(0u),
		_buffered_bits(0u)
	{}

	uint32_t BitInputStream::ReadBits(uint32_t bit_count) {
		if (_buffered_bits == 0u) NextByte();
		return _ReadBits(bit_count);
	}

}}