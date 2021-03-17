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

#ifndef ANVIL_BYTEPIPE_ENDIAN_HPP
#define ANVIL_BYTEPIPE_ENDIAN_HPP

#ifdef _MSC_VER 
#include <stdlib.h>
#endif
#include "anvil/byte-pipe/BytePipeCore.hpp"

namespace anvil { namespace BytePipe {

	enum Endianness {
		ENDIAN_BIG,
		ENDIAN_LITTLE
	};

	static inline Endianness GetEndianness() {
		union { uint32_t u32; uint8_t u8[4u]; };
		enum : uint32_t { WORD = 1u | (2u << 8u) | (3u << 16u) | (4u << 24u) };
		u32 = WORD;
		return u8[0u] == 1u ? ENDIAN_LITTLE : ENDIAN_BIG;
	}

	static const Endianness test = GetEndianness();

	static inline uint16_t SwapByteOrder(uint16_t word) {
#ifdef _MSC_VER 
		return _byteswap_ushort(word);
#else
		const uint32_t lo = word & 255u;
		const uint32_t hi = word >> 8u;
		return (lo << 8u) | hi;
#endif
	}

	static inline uint32_t SwapByteOrder(uint32_t word) {
#ifdef _MSC_VER 
		return _byteswap_ulong(word);
#else
		const uint32_t a = word & 255u;
		const uint32_t b = (word >> 8u) & 255u;
		const uint32_t c = (word >> 16u) & 255u;
		const uint32_t d = word >> 24u;
		return (a << 24u) | (b << 16u) | (c << 8u) | d;
#endif
	}

	static inline uint64_t SwapByteOrder(uint64_t word) {
#ifdef _MSC_VER 
		return _byteswap_uint64(word);
#else
		const uint64_t a = word & 255ull;
		const uint64_t b = (word >> 8ull) & 255ull;
		const uint64_t c = (word >> 16ull) & 255ull;
		const uint64_t d = (word >> 24ull) & 255ull;
		const uint64_t e = (word >> 32ull) & 255ull;
		const uint64_t f = (word >> 40ull) & 255ull;
		const uint64_t g = (word >> 48ull) & 255ull;
		const uint64_t h = word >> 54ull;
		return (a << 54u) | (b << 48u) | (c << 40u) | (d << 32u) | (e << 24u) | (f << 16u) | (g << 8u) | h;
#endif
	}

}}

#endif
