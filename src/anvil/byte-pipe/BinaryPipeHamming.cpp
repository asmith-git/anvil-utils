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

#include "anvil/byte-pipe/BytePipeHamming.hpp"

namespace anvil { namespace BytePipe {

	// Encode Hamming bits for 4 input bits, output is 7 bits
	static ANVIL_CONSTEXPR uint32_t EncodeHamming4(uint32_t input) {
		//! \todo Optimise
		uint32_t bit0 = input & 1u;
		uint32_t bit1 = (input & 2u) >> 1u;
		uint32_t bit2 = (input & 4u) >> 2u;
		uint32_t bit4 = (input & 8u) >> 3u; // Bit 3 of input

		uint32_t bit6 = bit0 ^ bit2 ^ bit4;
		uint32_t bit5 = bit0 ^ bit1 ^ bit4;
		uint32_t bit3 = bit0 ^ bit1 ^ bit2;

		return bit0 | (bit1 << 1u) | (bit2 << 2u) | (bit3 << 3u) | (bit4 << 4u) | (bit5 << 5u) | (bit6 << 6u);
	}

	// Encode Hamming bits for 8 input bits, output is 14 bits
	static ANVIL_CONSTEXPR uint32_t EncodeHamming8(uint32_t input) {
		uint32_t lo = EncodeHamming4(input & 15u);
		uint32_t hi = EncodeHamming4((input >> 4u) & 15u);
		return lo | (hi << 7u);
	}

	// Encode Hamming bits for 16 input bits, output is 24 bits
	static ANVIL_CONSTEXPR uint32_t EncodeHamming16(uint32_t input) {
		uint32_t lo = EncodeHamming8(input & 255u);
		uint32_t hi = EncodeHamming8((input >> 8u) & 255u);
		return lo | (hi << 14u);
	}

	// Decode Hamming bits for 7 input bits, output is 4 bits
	static ANVIL_CONSTEXPR uint32_t DecodeHamming4(uint32_t input) {
		//! \todo Optimise
		const uint32_t bit0 = input & 1u;
		const uint32_t bit1 = (input & 2u) >> 1u;
		const uint32_t bit2 = (input & 4u) >> 2u;
		const uint32_t bit3 = (input & 8u) >> 3u;
		uint32_t bit4 = (input & 16u) >> 4u;
		const uint32_t bit5 = (input & 32u) >> 5u;
		const uint32_t bit6 = (input & 64u) >> 6u;

		const uint32_t c1 = bit6 ^ bit4 ^ bit2 ^ bit0;
		const uint32_t c2 = bit5 ^ bit4 ^ bit1 ^ bit0;
		const uint32_t c3 = bit3 ^ bit2 ^ bit1 ^ bit0;
		uint32_t c = c3 * 4u + c2 * 2u + c1;

		// If there is an error
		if (c != 0u) {
			// Flip the incorrect bit
			uint32_t tmp = bit0 | (bit1 << 1u) | (bit2 << 2u) | (bit3 << 3u) | (bit4 << 4u) | (bit5 << 5u) | (bit6 << 6u);
			uint32_t flag = 1u << (7u - c);
			if (tmp & flag) {
				tmp &= ~flag;
			} else {
				tmp |= flag;
			}

			// Return the corrected bits
			bit4 = (tmp & 32) >> 4u;
			tmp &= 7u;
			return tmp | (bit4 << 3u);
		} else {
			return bit0 | (bit1 << 1u) | (bit2 << 2u) | (bit4 << 3u);
		}
	}

	// Decode Hamming bits for 14 input bits, output is 8 bits
	static ANVIL_CONSTEXPR uint32_t DecodeHamming8(uint32_t input) {
		uint32_t lo = DecodeHamming4(input & 127u);
		uint32_t hi = DecodeHamming4((input >> 7u) & 127u);
		return lo | (hi << 4u);
	}

	// Decode Hamming bits for 24 input bits, output is 16 bits
	static ANVIL_CONSTEXPR uint32_t DecodeHamming16(uint32_t input) {
		uint32_t lo = DecodeHamming8(input & 16383u);
		uint32_t hi = DecodeHamming8((input >> 14u) & 16383u);
		return lo | (hi << 8u);
	}

#ifndef ANVIL_LEGACY_COMPILER_SUPPORT
	// Test encoding without errors
	static_assert(DecodeHamming8(EncodeHamming8(0)) == 0, "Error detected in hamming encoder");
	static_assert(DecodeHamming8(EncodeHamming8(15)) == 15, "Error detected in hamming encoder");
	static_assert(DecodeHamming8(EncodeHamming8(64)) == 64, "Error detected in hamming encoder");
	static_assert(DecodeHamming8(EncodeHamming8(255)) == 255, "Error detected in hamming encoder");

	// Test encoding with errors
	static_assert(DecodeHamming4(EncodeHamming4(0) | 1) == 0, "Error detected in hamming encoder");
	static_assert(DecodeHamming4(EncodeHamming4(0) | 2) == 0, "Error detected in hamming encoder");
	static_assert(DecodeHamming4(EncodeHamming4(0) | 4) == 0, "Error detected in hamming encoder");
	static_assert(DecodeHamming4(EncodeHamming4(0) | 8) == 0, "Error detected in hamming encoder");
#endif

	struct BitOutputStream {
		uint8_t* out;
		uint32_t buffer;
		uint32_t buffered_bits;

		BitOutputStream(uint8_t* o) :
			out(o),
			buffer(0u),
			buffered_bits(0u)
		{}

		void _WriteBit(uint32_t bit) {
			// Add the next bit to the buffer
			buffer <<= 1u;
			buffer |= bit;
			++buffered_bits;

			// If there is a complete byte then output it
			if (buffered_bits == 8u) {
				*out = static_cast<uint8_t>(buffer);
				++out;
				buffer = 0u;
				buffered_bits = 0u;
			}
		}

		void WriteBits(uint32_t bits, uint32_t bit_count) {
			//! \todo Optimise

			while (bit_count != 0u) {
				uint32_t bits_to_extract = 8u;
				if (bits < bits_to_extract) bits = bits_to_extract;

				// Extract the left most 8 bits
				uint32_t shift = bit_count - bits_to_extract;
				uint32_t extracted_bits = (bits >> shift) & 255u;

				// Remove the extracted bits from the input
				bits ^= (extracted_bits << shift);
				bit_count -= bits_to_extract;

				if (bits_to_extract + buffered_bits >= 8u) {
					// Add bits to the buffer until there are 8 bits
					const uint32_t bits_to_add = 8u - buffered_bits;
					buffer <<= bits_to_add;
					buffer |= extracted_bits >> buffered_bits;

					// Output the buffer
					*out = static_cast<uint8_t>(buffer);
					++out;
					
					// Replace the buffer with the remaining bits (this should be the same number as it had before)
					buffer = ((extracted_bits << bits_to_add) & 255u) >> bits_to_add;

				} else {
					// Add extracted bits to the buffer
					buffer <<= bits_to_extract;
					buffered_bits += bits_to_extract;
				}

			}
		}
	};

	struct BitInputStream {
		const uint8_t* in;
		uint32_t buffer;
		uint32_t buffered_bits;

		BitInputStream(const uint8_t* i) :
			in(i),
			buffer(0u),
			buffered_bits(0u)
		{}

		uint32_t _ReadBit() {
			// If there are no buffered bits then read the next byte
			if (buffered_bits == 0u) {
				buffer = *in;
				++in;
				buffered_bits = 8u;
			}

			// Read the next bit from the stream
			uint32_t tmp = buffer & 128u ? 1u : 0u;
			buffer <<= 1u;
			--buffered_bits;
			return tmp;
		}

		uint32_t ReadBits(uint32_t bit_count) {
			//! \todo Optimise
			uint32_t tmp = 0u;
			while (bit_count > 0u) {
				--bit_count;
				tmp <<= 1u;
				tmp |= _ReadBit();
			}
			return tmp;
		}
	};

	// RawHammingInputPipe

	RawHammingInputPipe::RawHammingInputPipe(InputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	RawHammingInputPipe::~RawHammingInputPipe() {

	}

	uint32_t RawHammingInputPipe::ReadBytes(void* dst, const uint32_t decoded_bytes) {
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 4u) * 3u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHammingOutputPipe::ReadBytes : Encoded bit count is not divisible by 8");

		// Allocate temporary storage for the encoded data
		uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));

		// Read encoded data from the downstream pipe
		if(_downstream_pipe.ReadBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHammingOutputPipe::ReadBytes : Error reading from downstream pipe");

		// Decode the data
		BitInputStream stream(buffer);
		for (uint32_t i = 0; i < decoded_bytes; ++i) {
			static_cast<uint8_t*>(dst)[i] = static_cast<uint8_t>(DecodeHamming8(stream.ReadBits(14u)));
		}

		return decoded_bytes;
	}

	// RawHammingOutputPipe

	RawHammingOutputPipe::RawHammingOutputPipe(OutputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	RawHammingOutputPipe::~RawHammingOutputPipe() {

	}

	uint32_t RawHammingOutputPipe::WriteBytes(const void* src, const uint32_t decoded_bytes) {
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 4u) * 3u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if(encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHammingOutputPipe::WriteBytes : Encoded bit count is not divisible by 8");

		// Allocate temporary storage for the encoded data
		uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));
		BitOutputStream stream(buffer);

		// Encode the data
		for (uint32_t i = 0; i < decoded_bytes; ++i) {
			stream.WriteBits(EncodeHamming8(static_cast<const uint8_t*>(src)[i]), 14u);
		}

		// Write the encoded data downstream
		if(_downstream_pipe.WriteBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHammingOutputPipe::WriteBytes : Error writing to downstream Pipe");

		return decoded_bytes;
	}

	void RawHammingOutputPipe::Flush() {
		// Flush the downstream pipe
		_downstream_pipe.Flush();
	}

	// HammingInputPipe

	HammingInputPipe::HammingInputPipe(InputPipe& downstream_pipe) :
		_packet_pipe(downstream_pipe),
		_hamming_pipe(_packet_pipe)
	{}

	HammingInputPipe::~HammingInputPipe() {

	}

	uint32_t HammingInputPipe::ReadBytes(void* dst, const uint32_t bytes) {
		return _hamming_pipe.ReadBytes(dst, bytes);
	}

	// HammingOutputPipe

	HammingOutputPipe::HammingOutputPipe(OutputPipe& downstream_pipe, uint32_t packet_size) :
		_hamming_pipe(downstream_pipe),
		_packet_pipe(_hamming_pipe, packet_size, 0u)
	{
		const uint32_t decoded_bits = packet_size * 8u;
		const uint32_t parity_bits = (decoded_bits / 4u) * 3u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("HammingOutputPipe::HammingOutputPipe : Encoded bit count is not divisible by 8");
	}

	HammingOutputPipe::~HammingOutputPipe() {

	}

	uint32_t HammingOutputPipe::WriteBytes(const void* src, const uint32_t bytes) {
		return _packet_pipe.WriteBytes(src, bytes);
	}

	void HammingOutputPipe::Flush() {
		_packet_pipe.Flush();
	}

}}