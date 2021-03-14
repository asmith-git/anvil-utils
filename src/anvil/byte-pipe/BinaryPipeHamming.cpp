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

	// Hamming(7,4)

	// Encode Hamming74 bits for 4 input bits, output is 7 bits
	static ANVIL_CONSTEXPR uint32_t EncodeHamming74_4(uint32_t input) {
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

	// Encode Hamming74 bits for 8 input bits, output is 14 bits
	static ANVIL_CONSTEXPR uint32_t EncodeHamming74_8(uint32_t input) {
		uint32_t lo = EncodeHamming74_4(input & 15u);
		uint32_t hi = EncodeHamming74_4((input >> 4u) & 15u);
		return lo | (hi << 7u);
	}

	// Encode Hamming74 bits for 16 input bits, output is 24 bits
	static ANVIL_CONSTEXPR uint32_t EncodeHamming74_16(uint32_t input) {
		uint32_t lo = EncodeHamming74_8(input & 255u);
		uint32_t hi = EncodeHamming74_8((input >> 8u) & 255u);
		return lo | (hi << 14u);
	}

	// Decode Hamming74 bits for 7 input bits, output is 4 bits
	static ANVIL_CONSTEXPR uint32_t DecodeHamming74_4(uint32_t input) {
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

	// Decode Hamming74 bits for 14 input bits, output is 8 bits
	static ANVIL_CONSTEXPR uint32_t DecodeHamming74_8(uint32_t input) {
		uint32_t lo = DecodeHamming74_4(input & 127u);
		uint32_t hi = DecodeHamming74_4((input >> 7u) & 127u);
		return lo | (hi << 4u);
	}

	// Decode Hamming74 bits for 24 input bits, output is 16 bits
	static ANVIL_CONSTEXPR uint32_t DecodeHamming74_16(uint32_t input) {
		uint32_t lo = DecodeHamming74_8(input & 16383u);
		uint32_t hi = DecodeHamming74_8((input >> 14u) & 16383u);
		return lo | (hi << 8u);
	}

#ifndef ANVIL_LEGACY_COMPILER_SUPPORT
	// Test encoding without errors
	static_assert(DecodeHamming74_8(EncodeHamming74_8(0)) == 0, "Error detected in Hamming(7,4) encoder");
	static_assert(DecodeHamming74_8(EncodeHamming74_8(15)) == 15, "Error detected in Hamming(7,4) encoder");
	static_assert(DecodeHamming74_8(EncodeHamming74_8(64)) == 64, "Error detected in Hamming(7,4) encoder");
	static_assert(DecodeHamming74_8(EncodeHamming74_8(255)) == 255, "Error detected in Hamming(7,4) encoder");

	// Test encoding with errors
	static_assert(DecodeHamming74_4(EncodeHamming74_4(0) | 1) == 0, "Error detected in Hamming(7,4) error correction");
	static_assert(DecodeHamming74_4(EncodeHamming74_4(0) | 2) == 0, "Error detected in Hamming(7,4) error correction");
	static_assert(DecodeHamming74_4(EncodeHamming74_4(0) | 4) == 0, "Error detected in Hamming(7,4) error correction");
	static_assert(DecodeHamming74_4(EncodeHamming74_4(0) | 8) == 0, "Error detected in Hamming(7,4) error correction");
#endif

	// Hamming (15,11)

	// Hamming (15,11) - 11 Data bits, 4 parity bits. Total = 15 bits.
	static uint32_t ANVIL_CONSTEXPR EncodeHamming1511(uint32_t input) {
		//! \todo Optimise

		// Input bits
		uint32_t i[11] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,0u, 0u, 0u };
		i[0u] = input & 1u;
		input >>= 1u;
		i[1u] = input & 1u;
		input >>= 1u;
		i[2u] = input & 1u;
		input >>= 1u;
		i[3u] = input & 1u;
		input >>= 1u;
		i[4u] = input & 1u;
		input >>= 1u;
		i[5u] = input & 1u;
		input >>= 1u;
		i[6u] = input & 1u;
		input >>= 1u;
		i[7u] = input & 1u;
		input >>= 1u;
		i[8u] = input & 1u;
		input >>= 1u;
		i[9u] = input & 1u;
		input >>= 1u;
		i[10u] = input & 1u;

		// Parity bits
		uint32_t p[4u] = { 0u, 0u, 0u, 0u };
		//! \todo Implement Hamming (15,11) parity calculation

		uint32_t output = 0u;
		output |= p[0u];	// Bit 1
		output <<= 1u;
		output |= p[1u];	// Bit 2
		output <<= 1u;
		output |= i[0u];	// Bit 3
		output <<= 1u;
		output |= p[2u];	// Bit 4
		output <<= 1u;
		output |= i[1u];	// Bit 5
		output <<= 1u;
		output |= i[2u];	// Bit 6
		output <<= 1u;
		output |= i[3u];	// Bit 7
		output <<= 1u;
		output |= p[3u];	// Bit 8
		output <<= 1u;
		output |= i[4u];	// Bit 9
		output <<= 1u;
		output |= i[5u];	// Bit 10
		output <<= 1u;
		output |= i[6u];	// Bit 11
		output <<= 1u;
		output |= i[7u];	// Bit 12
		output <<= 1u;
		output |= i[8u];	// Bit 13
		output <<= 1u;
		output |= i[9u];	// Bit 14
		output <<= 1u;
		output |= i[10u];	// Bit 15

		return output;
	}



	// Hamming (15,11) - 11 Data bits, 4 parity bits. Total = 15 bits.
	static uint32_t ANVIL_CONSTEXPR DecodeHamming1511(uint32_t input) {
		//! \todo Optimise

		// Input bits
		uint32_t i[11] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,0u, 0u, 0u };
		uint32_t p[4u] = { 0u, 0u, 0u, 0u };

		p[0u] = input & 1u;		// Bit 1
		input >>= 1u;
		p[1u] = input & 1u;		// Bit 2
		input >>= 1u;
		i[0u] = input & 1u;		// Bit 3
		input >>= 1u;
		p[2u] = input & 1u;		// Bit 4
		input >>= 1u;
		i[1u] = input & 1u;		// Bit 5
		input >>= 1u;
		i[2u] = input & 1u;		// Bit 6
		input >>= 1u;
		i[3u] = input & 1u;		// Bit 7
		input >>= 1u;
		p[3u] = input & 1u;		// Bit 8
		input >>= 1u;
		i[4u] = input & 1u;		// Bit 9
		input >>= 1u;
		i[5u] = input & 1u;		// Bit 10
		input >>= 1u;
		i[6u] = input & 1u;		// Bit 11
		input >>= 1u;
		i[7u] = input & 1u;		// Bit 12
		input >>= 1u;
		i[8u] = input & 1u;		// Bit 13
		input >>= 1u;
		i[9u] = input & 1u;		// Bit 14
		input >>= 1u;
		i[10u] = input & 1u;	// Bit 15

		// Error correction
		//! \todo Implement Hamming (15,11) error correction 

		uint32_t output = 0u;
		output |= i[0u];	// Bit 1
		output <<= 1u;
		output |= i[1u];	// Bit 2
		output <<= 1u;
		output |= i[2u];	// Bit 3
		output <<= 1u;
		output |= i[3u];	// Bit 4
		output <<= 1u;
		output |= i[4u];	// Bit 5
		output <<= 1u;
		output |= i[5u];	// Bit 6
		output <<= 1u;
		output |= i[6u];	// Bit 7
		output <<= 1u;
		output |= i[7u];	// Bit 8
		output <<= 1u;
		output |= i[8u];	// Bit 9
		output <<= 1u;
		output |= i[9u];	// Bit 10
		output <<= 1u;
		output |= i[10u];	// Bit 11

		return output;
	}

//#ifndef ANVIL_LEGACY_COMPILER_SUPPORT
//	// Test encoding without errors
//	static_assert(DecodeHamming1511(EncodeHamming1511(0)) == 0, "Error detected in Hamming(15,11) encoder");
//	static_assert(DecodeHamming1511(EncodeHamming1511(15)) == 15, "Error detected in Hamming(15,11) encoder");
//	static_assert(DecodeHamming1511(EncodeHamming1511(64)) == 64, "Error detected in Hamming(15,11) encoder");
//	static_assert(DecodeHamming1511(EncodeHamming1511(255)) == 255, "Error detected in Hamming(15,11) encoder");
//
//	// Test encoding with errors
//	static_assert(DecodeHamming1511(EncodeHamming1511(0) | 1) == 0, "Error detected in Hamming(15,11) error correction");
//	static_assert(DecodeHamming1511(EncodeHamming1511(0) | 2) == 0, "Error detected in Hamming(15,11) error correction");
//	static_assert(DecodeHamming1511(EncodeHamming1511(0) | 4) == 0, "Error detected in Hamming(15,11) error correction");
//	static_assert(DecodeHamming1511(EncodeHamming1511(0) | 8) == 0, "Error detected in Hamming(15,11) error correction");
//#endif

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

	// RawHamming74InputPipe

	RawHamming74InputPipe::RawHamming74InputPipe(InputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	RawHamming74InputPipe::~RawHamming74InputPipe() {

	}

	uint32_t RawHamming74InputPipe::ReadBytes(void* dst, const uint32_t decoded_bytes) {
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 4u) * 3u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHamming74OutputPipe::ReadBytes : Encoded bit count is not divisible by 8");

		// Allocate temporary storage for the encoded data
		uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));

		// Read encoded data from the downstream pipe
		if(_downstream_pipe.ReadBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHamming74OutputPipe::ReadBytes : Error reading from downstream pipe");

		// Decode the data
		BitInputStream stream(buffer);
		for (uint32_t i = 0; i < decoded_bytes; ++i) {
			static_cast<uint8_t*>(dst)[i] = static_cast<uint8_t>(DecodeHamming74_8(stream.ReadBits(14u)));
		}

		return decoded_bytes;
	}

	// RawHamming74OutputPipe

	RawHamming74OutputPipe::RawHamming74OutputPipe(OutputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	RawHamming74OutputPipe::~RawHamming74OutputPipe() {

	}

	uint32_t RawHamming74OutputPipe::WriteBytes(const void* src, const uint32_t decoded_bytes) {
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 4u) * 3u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if(encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHamming74OutputPipe::WriteBytes : Encoded bit count is not divisible by 8");

		// Allocate temporary storage for the encoded data
		uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));
		BitOutputStream stream(buffer);

		// Encode the data
		for (uint32_t i = 0; i < decoded_bytes; ++i) {
			stream.WriteBits(EncodeHamming74_8(static_cast<const uint8_t*>(src)[i]), 14u);
		}

		// Write the encoded data downstream
		if(_downstream_pipe.WriteBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHamming74OutputPipe::WriteBytes : Error writing to downstream Pipe");

		return decoded_bytes;
	}

	void RawHamming74OutputPipe::Flush() {
		// Flush the downstream pipe
		_downstream_pipe.Flush();
	}

	// Hamming74InputPipe

	Hamming74InputPipe::Hamming74InputPipe(InputPipe& downstream_pipe) :
		_packet_pipe(downstream_pipe),
		_hamming_pipe(_packet_pipe)
	{}

	Hamming74InputPipe::~Hamming74InputPipe() {

	}

	uint32_t Hamming74InputPipe::ReadBytes(void* dst, const uint32_t bytes) {
		return _hamming_pipe.ReadBytes(dst, bytes);
	}

	// Hamming74OutputPipe

	Hamming74OutputPipe::Hamming74OutputPipe(OutputPipe& downstream_pipe, uint32_t packet_size) :
		_hamming_pipe(downstream_pipe),
		_packet_pipe(_hamming_pipe, packet_size, 0u)
	{
		const uint32_t decoded_bits = packet_size * 8u;
		const uint32_t parity_bits = (decoded_bits / 4u) * 3u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("Hamming74OutputPipe::Hamming74OutputPipe : Encoded bit count is not divisible by 8");
	}

	Hamming74OutputPipe::~Hamming74OutputPipe() {

	}

	uint32_t Hamming74OutputPipe::WriteBytes(const void* src, const uint32_t bytes) {
		return _packet_pipe.WriteBytes(src, bytes);
	}

	void Hamming74OutputPipe::Flush() {
		_packet_pipe.Flush();
	}


	namespace dev {

		// RawHamming1511OutputPipe

		RawHamming1511OutputPipe::RawHamming1511OutputPipe(OutputPipe& downstream_pipe) :
			_downstream_pipe(downstream_pipe)
		{}

		RawHamming1511OutputPipe::~RawHamming1511OutputPipe() {

		}

		uint32_t RawHamming1511OutputPipe::WriteBytes(const void* src, const uint32_t decoded_bytes) {
			const uint32_t decoded_bits = decoded_bytes * 8u;
			const uint32_t parity_bits = (decoded_bits / 11u) * 4u;
			const uint32_t encoded_bits = decoded_bits + parity_bits;
			const uint32_t encoded_bytes = encoded_bits / 8u;
			if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHamming1511OutputPipe::WriteBytes : Decoded bit count is not divisible by 11");

			// Allocate temporary storage for the encoded data
			uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));
			BitInputStream in(static_cast<const uint8_t*>(src));
			BitOutputStream out(buffer);

			for (uint32_t i = 0u; i < encoded_bits; i += 15u) {
				// Read data bits from upstream
				uint32_t tmp = in.ReadBits(11u);

				// Calculate and add the parity bits
				tmp = EncodeHamming1511(tmp);

				// Write to the downstream
				out.WriteBits(tmp, 15u);
			}

			// Write the encoded data downstream
			if (_downstream_pipe.WriteBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHamming1511OutputPipe::WriteBytes : Error writing to downstream Pipe");

			return decoded_bytes;
		}

		void RawHamming1511OutputPipe::Flush() {
			_downstream_pipe.Flush();
		}
	}

}}