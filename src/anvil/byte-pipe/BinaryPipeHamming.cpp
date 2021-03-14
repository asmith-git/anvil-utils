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

#include <intrin.h>
#include "anvil/byte-pipe/BytePipeHamming.hpp"

// Constexpr popcount for compile time tests and CPUs that dont have a popcount instruction
static ANVIL_CONSTEXPR uint32_t SlowPopcount(uint32_t val) {
	uint32_t ones = 0u;
	while (val != 0u) {
		ones += val & 1u;
		val >>= 1u;
	}
	return ones;
}

#define HAMMING_POPCOUNT(VAL) __popcnt(VAL)
//#define HAMMING_POPCOUNT(VAL) SlowPopcount(VAL)

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

	// Extended Hamming (15,11) - 11 Data bits, 4+1 parity bits. Total = 16 bits.
	static uint32_t /*ANVIL_CONSTEXPR*/ EncodeHamming1511(uint32_t input) {
		//! \todo Optimise

		/*
			P0,P1,P2,D0,
			P3,D1,D2,D3,
			P4,D4,D5,D6,
			D7,D8,D9,D10
		*/

#define bitpos(x,y) (y * 4u + x)
#define get_bit(x,y) ((encoded & (1u << bitpos(x,y))) >> bitpos(x,y))
#define set_bit(x,y,b) encoded |= ((b) << bitpos(x,y))
		uint32_t encoded = 0u;

		// Input bits
		set_bit(3u, 0u, input & 1u);	//D0
		input >>= 1u;
		set_bit(1u, 1u, input & 1u);	//D1
		input >>= 1u;
		set_bit(2u, 1u, input & 1u);	//D2
		input >>= 1u;
		set_bit(3u, 1u, input & 1u);	//D3
		input >>= 1u;
		set_bit(1u, 2u, input & 1u);	//D4
		input >>= 1u;
		set_bit(2u, 2u, input & 1u);	//D5
		input >>= 1u;
		set_bit(3u, 2u, input & 1u);	//D6
		input >>= 1u;
		set_bit(0u, 3u, input & 1u);	//D7
		input >>= 1u;
		set_bit(1u, 3u, input & 1u);	//D8
		input >>= 1u;
		set_bit(2u, 3u, input & 1u);	//D9
		input >>= 1u;
		set_bit(3u, 3u, input & 1u);	//D9

		// Parity bits
		enum : uint32_t {
			mask1 = (1u << bitpos(1u, 1u)) | (1u << bitpos(1u, 2u)) | (1u << bitpos(1u, 3u)) | (1u << bitpos(3u, 0u)) | (1u << bitpos(3u, 1u)) | (1u << bitpos(3u, 2u)) | (1u << bitpos(3u, 3u)),
			mask2 = (1u << bitpos(3u, 0u)) | (1u << bitpos(2u, 1u)) | (1u << bitpos(3u, 1u)) | (1u << bitpos(2u, 2u)) | (1u << bitpos(3u, 2u)) | (1u << bitpos(2u, 3u)) | (1u << bitpos(3u, 3u)),
			mask3 = (1u << bitpos(1u, 1u)) | (1u << bitpos(2u, 1u)) | (1u << bitpos(3u, 1u)) | (1u << bitpos(0u, 3u)) | (1u << bitpos(1u, 3u)) | (1u << bitpos(2u, 3u)) | (1u << bitpos(3u, 3u)),
			mask4 = (1u << bitpos(1u, 2u)) | (1u << bitpos(2u, 2u)) | (1u << bitpos(3u, 2u)) | (1u << bitpos(0u, 3u)) | (1u << bitpos(1u, 3u)) | (1u << bitpos(2u, 3u)) | (1u << bitpos(3u, 3u))
		};

		set_bit(1u, 0u, HAMMING_POPCOUNT(encoded & mask1) & 1u); //P1 - Odd columns
		set_bit(2u, 0u, HAMMING_POPCOUNT(encoded & mask2) & 1u); //P2 - Right half
		set_bit(0u, 1u, HAMMING_POPCOUNT(encoded & mask3) & 1u); // P3 = Odd columns
		set_bit(0u, 2u, HAMMING_POPCOUNT(encoded & mask4) & 1u); // P4 = Lower half

		// Calculate final parity
		set_bit(0u, 0u, HAMMING_POPCOUNT(encoded) & 1u); // If there are an odd number of 1s then the bit is 1, otherwise 0

		return encoded;

#undef bitpos
#undef get_bit
#undef set_bit
	}



	// Extended Hamming (15,11) - 11 Data bits, 4+1 parity bits. Total = 16 bits.
	static uint32_t /*ANVIL_CONSTEXPR*/ DecodeHamming1511(uint32_t encoded) {
		//! \todo Optimise

		/*
			P0,P1,P2,D0,
			P3,D1,D2,D3,
			P4,D4,D5,D6,
			D7,D8,D9,D10
		*/

#define bitpos(x,y) (y * 4u + x)
#define get_bit(x,y) ((encoded & (1u << bitpos(x,y))) >> bitpos(x,y))
#define flip_bit(x,y) encoded ^= (1u << bitpos(x,y))

		// Error correction
		enum : uint32_t {
			mask1 = (1u << bitpos(1u, 0u)) | (1u << bitpos(1u, 1u)) | (1u << bitpos(1u, 2u)) | (1u << bitpos(1u, 3u)) | (1u << bitpos(3u, 0u)) | (1u << bitpos(3u, 1u)) | (1u << bitpos(3u, 2u)) | (1u << bitpos(3u, 3u)),
			mask2 = (1u << bitpos(2u, 0u)) | (1u << bitpos(3u, 0u)) | (1u << bitpos(2u, 1u)) | (1u << bitpos(3u, 1u)) | (1u << bitpos(2u, 2u)) | (1u << bitpos(3u, 2u)) | (1u << bitpos(2u, 3u)) | (1u << bitpos(3u, 3u)),
			mask3 = (1u << bitpos(0u, 1u)) | (1u << bitpos(1u, 1u)) | (1u << bitpos(2u, 1u)) | (1u << bitpos(3u, 1u)) | (1u << bitpos(0u, 3u)) | (1u << bitpos(1u, 3u)) | (1u << bitpos(2u, 3u)) | (1u << bitpos(3u, 3u)),
			mask4 = (1u << bitpos(0u, 2u)) | (1u << bitpos(1u, 2u)) | (1u << bitpos(2u, 2u)) | (1u << bitpos(3u, 2u)) | (1u << bitpos(0u, 3u)) | (1u << bitpos(1u, 3u)) | (1u << bitpos(2u, 3u)) | (1u << bitpos(3u, 3u))
		};
		uint32_t parityBlock1 = HAMMING_POPCOUNT(encoded & mask1) & 1u; //P1 - Odd columns
		uint32_t parityBlock2 = HAMMING_POPCOUNT(encoded & mask2) & 1u; //P2 - Right half
		uint32_t parityBlock3 = HAMMING_POPCOUNT(encoded & mask3) & 1u; // P3 = Odd columns
		uint32_t parityBlock4 = HAMMING_POPCOUNT(encoded & mask4) & 1u; // P4 = Lower half

		uint32_t error = 0u;
		uint32_t row = 0u;
		uint32_t col = 0u;

		row |= parityBlock1;
		error |= parityBlock1;

		row += parityBlock2 ? 2u : 0u;
		error |= parityBlock2;

		col |= parityBlock3;
		error |= parityBlock3;

		col += parityBlock4 ? 2u : 0u;
		error |= parityBlock4;


		if (error) {
			// Flip the bit with error
			flip_bit(row, col);

			// Check the parity for the entire block (p0)
			if (HAMMING_POPCOUNT(encoded) & 1u) throw std::runtime_error("DecodeHamming1511 : Detected second error, cannot correct");

		} else { // Check for two bit errors
			// Bit 0 could be flipped, but in this case it wont effect the output data so we ignore it
		}

		// Return data bits
		uint32_t output = 0u;

		output |= get_bit(3u, 3u);	//D10
		output <<= 1u;
		output |= get_bit(2u, 3u);	//D9
		output <<= 1u;
		output |= get_bit(1u, 3u);	//D8
		output <<= 1u;
		output |= get_bit(0u, 3u);	//D7
		output <<= 1u;
		output |= get_bit(3u, 2u);	//D6
		output <<= 1u;
		output |= get_bit(2u, 2u);	//D5
		output <<= 1u;
		output |= get_bit(1u, 2u);	//D4
		output <<= 1u;
		output |= get_bit(3u, 1u);	//D3
		output <<= 1u;
		output |= get_bit(2u, 1u);	//D2
		output <<= 1u;
		output |= get_bit(1u, 1u);	//D1
		output <<= 1u;
		output |= get_bit(3u, 0u);	//D0

		return output;

#undef bitpos
#undef get_bit
#undef flip_bit
	}

#ifdef _DEBUG
	static bool HammingTest1511(uint32_t data, uint32_t error) {
		uint32_t encoded = EncodeHamming1511(data);
		encoded ^= error;
		uint32_t decoded = DecodeHamming1511(encoded);
		if(decoded != data) throw std::runtime_error("Test");
		return true;
	}

	static const bool g_hamming_tests =
		// Test hamming (15,11) Encode / Decode
		HammingTest1511(0u, 0u) &&
		HammingTest1511(1u, 0u) &&
		HammingTest1511(15u, 0u) &&
		HammingTest1511(44u, 0u) &&

		// Test hamming (15,11) error correction
		HammingTest1511(0u, 1u << 0u) &&
		HammingTest1511(0u, 1u << 1u) &&
		HammingTest1511(0u, 1u << 2u) &&
		HammingTest1511(0u, 1u << 3u) &&
		HammingTest1511(0u, 1u << 4u) &&
		HammingTest1511(0u, 1u << 5u) &&
		HammingTest1511(0u, 1u << 6u) &&
		HammingTest1511(0u, 1u << 7u) &&
		HammingTest1511(0u, 1u << 8u) &&
		HammingTest1511(0u, 1u << 9u) &&
		HammingTest1511(0u, 1u << 10u) &&
		HammingTest1511(0u, 1u << 11u) &&
		HammingTest1511(0u, 1u << 12u) &&
		HammingTest1511(0u, 1u << 13u) &&
		HammingTest1511(0u, 1u << 14u);
#endif

	struct BitOutputStream {
	private:
		uint8_t* _out;				//!< The location that the next complete byte will be written to
		uint32_t _buffer;			//!< The bits that were leftover from the last write
		uint32_t _buffered_bits;	//!< How many bits were leftover from the last write
	public:
		BitOutputStream(uint8_t* o) :
			_out(o),
			_buffer(0u),
			_buffered_bits(0u)
		{}

		void WriteBits(uint32_t bits, uint32_t bit_count) {
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
	};

	struct BitInputStream {
	private:
		const uint8_t* _in;			//!< The location that the next byte will be read from
		uint32_t _buffer;			//!< The bits that were leftover from the last read
		uint32_t _buffered_bits;	//!< How many bits were leftover from the last read

		void NextByte() {
			_buffer = *_in;
			++_in;
			_buffered_bits = 8u;
		}

		uint32_t _ReadBits(uint32_t bit_count) {
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
	public:
		BitInputStream(const uint8_t* i) :
			_in(i),
			_buffer(0u),
			_buffered_bits(0u)
		{}

		uint32_t ReadBits(uint32_t bit_count) {
			if (_buffered_bits == 0u) NextByte();
			return _ReadBits(bit_count);
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

	// RawHamming1511OutputPipe

	RawHamming1511OutputPipe::RawHamming1511OutputPipe(OutputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	RawHamming1511OutputPipe::~RawHamming1511OutputPipe() {

	}

	uint32_t RawHamming1511OutputPipe::WriteBytes(const void* src, const uint32_t decoded_bytes) {
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 11u) * 5u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHamming1511OutputPipe::WriteBytes : Decoded bit count is not divisible by 11");

		// Allocate temporary storage for the encoded data
		uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));
		BitInputStream in(static_cast<const uint8_t*>(src));
		uint16_t* out = reinterpret_cast<uint16_t*>(buffer);

		for (uint32_t i = 0u; i < encoded_bits; i += 15u) {
			// Read data bits from upstream
			uint32_t tmp = in.ReadBits(11u);

			// Calculate and add the parity bits
			tmp = EncodeHamming1511(tmp);

			// Write to the downstream
			*out = static_cast<uint16_t>(tmp);
			++out;
		}

		// Write the encoded data downstream
		if (_downstream_pipe.WriteBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHamming1511OutputPipe::WriteBytes : Error writing to downstream Pipe");

		return decoded_bytes;
	}

	void RawHamming1511OutputPipe::Flush() {
		_downstream_pipe.Flush();
	}

	// RawHamming1511InputPipe

	RawHamming1511InputPipe::RawHamming1511InputPipe(InputPipe& downstream_pipe) :
		_downstream_pipe(downstream_pipe)
	{}

	RawHamming1511InputPipe::~RawHamming1511InputPipe() {

	}

	uint32_t RawHamming1511InputPipe::ReadBytes(void* dst, const uint32_t decoded_bytes) {
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 11u) * 5u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("RawHamming1511InputPipe::ReadBytes : Decoded bit count is not divisible by 11");

		// Allocate temporary storage for the encoded data
		uint8_t* buffer = static_cast<uint8_t*>(_alloca(encoded_bytes));
		BitOutputStream out(static_cast<uint8_t*>(dst));
		uint16_t* in = reinterpret_cast<uint16_t*>(buffer);

		// Read encoded data from downstream
		if(_downstream_pipe.ReadBytes(buffer, encoded_bytes) != encoded_bytes) throw std::runtime_error("RawHamming1511InputPipe::ReadBytes : Failed to read from downstream pipe");

		for (uint32_t i = 0u; i < encoded_bits; i += 15u) {
			// Read data bits from downstream
			uint32_t tmp = *in;
			++in;

			// Calculate and add the parity bits
			tmp = DecodeHamming1511(tmp);

			// Write to upstream
			out.WriteBits(tmp, 11u);
		}

		return decoded_bytes;
	}

	// Hamming1511InputPipe

	Hamming1511InputPipe::Hamming1511InputPipe(InputPipe& downstream_pipe) :
		_packet_pipe(downstream_pipe),
		_hamming_pipe(_packet_pipe)
	{}

	Hamming1511InputPipe::~Hamming1511InputPipe() {

	}

	uint32_t Hamming1511InputPipe::ReadBytes(void* dst, const uint32_t bytes) {
		return _hamming_pipe.ReadBytes(dst, bytes);
	}

	// Hamming1511OutputPipe

	Hamming1511OutputPipe::Hamming1511OutputPipe(OutputPipe& downstream_pipe, uint32_t packet_size) :
		_hamming_pipe(downstream_pipe),
		_packet_pipe(_hamming_pipe, packet_size, 0u)
	{
		const uint32_t decoded_bytes = packet_size;
		const uint32_t decoded_bits = decoded_bytes * 8u;
		const uint32_t parity_bits = (decoded_bits / 11u) * 5u;
		const uint32_t encoded_bits = decoded_bits + parity_bits;
		const uint32_t encoded_bytes = encoded_bits / 8u;
		if (encoded_bytes * 8u != encoded_bits) throw std::runtime_error("Hamming1511OutputPipe::Hamming1511OutputPipe : Encoded bit count is not divisible by 11");
	}

	Hamming1511OutputPipe::~Hamming1511OutputPipe() {

	}

	uint32_t Hamming1511OutputPipe::WriteBytes(const void* src, const uint32_t bytes) {
		return _packet_pipe.WriteBytes(src, bytes);
	}

	void Hamming1511OutputPipe::Flush() {
		_packet_pipe.Flush();
	}

}}