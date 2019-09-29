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

#ifndef ANVIL_LUTILS_ARITHMETIC_HPP
#define ANVIL_LUTILS_ARITHMETIC_HPP

#include <cstdint>
#include "anvil/lutils/Assert.hpp"
#include "anvil/lutils/CPU.hpp"

#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
#include <nmmintrin.h>
#include <immintrin.h>
#include <ammintrin.h>
#include <intrin.h>
#endif

namespace anvil {

	// IsOdd
	template<class T>
	static bool IsOdd(const T) throw();

	template<>
	static inline bool IsOdd<uint64_t>(const uint64_t value) throw() {
		return (value & 1ull) == 1ull;
	}

	template<>
	static inline bool IsOdd<uint32_t>(const uint32_t value) throw() {
		return (value & 1u) == 1u;
	}

	template<>
	static inline bool IsOdd<uint16_t>(const uint16_t value) throw() {
		return (value & 1u) == 1u;
	}

	template<>
	static inline bool IsOdd<uint8_t>(const uint8_t value) throw() {
		return (value & 1u) == 1u;
	}

	template<>
	static inline bool IsOdd<int64_t>(const int64_t value) throw() {
		return (value & 1ll) == 1ll;
	}

	template<>
	static inline bool IsOdd<int32_t>(const int32_t value) throw() {
		return (value & 1) == 1;
	}

	template<>
	static inline bool IsOdd<int16_t>(const int16_t value) throw() {
		return (value & 1) == 1;
	}

	template<>
	static inline bool IsOdd<int8_t>(const int8_t value) throw() {
		return (value & 1) == 1;
	}

	template<>
	static inline bool IsOdd<float>(const float value) throw() {
		return IsOdd<int64_t>(static_cast<int64_t>(value));
	}

	template<>
	static inline bool IsOdd<double>(const double value) throw() {
		return IsOdd<int64_t>(static_cast<int64_t>(value));
	}

	// IsEven

	template<class T>
	static bool IsEven(const T) throw();

	template<>
	static inline bool IsEven<uint64_t>(const uint64_t value) throw() {
		return (value & 1ull) == 0ull;
	}

	template<>
	static inline bool IsEven<uint32_t>(const uint32_t value) throw() {
		return (value & 1u) == 0u;
	}

	template<>
	static inline bool IsEven<uint16_t>(const uint16_t value) throw() {
		return (value & 1u) == 0u;
	}

	template<>
	static inline bool IsEven<uint8_t>(const uint8_t value) throw() {
		return (value & 1u) == 0u;
	}

	template<>
	static inline bool IsEven<int64_t>(const int64_t value) throw() {
		return (value & 1ll) == 0ll;
	}

	template<>
	static inline bool IsEven<int32_t>(const int32_t value) throw() {
		return (value & 1) == 0;
	}

	template<>
	static inline bool IsEven<int16_t>(const int16_t value) throw() {
		return (value & 1) == 0;
	}

	template<>
	static inline bool IsEven<int8_t>(const int8_t value) throw() {
		return (value & 1) == 0;
	}

	template<>
	static inline bool IsEven<float>(const float value) throw() {
		return IsEven<int64_t>(static_cast<int64_t>(value));
	}

	template<>
	static inline bool IsEven<double>(const double value) throw() {
		return IsEven<int64_t>(static_cast<int64_t>(value));
	}

	// RoundUpOdd

	template<class T>
	static T RoundUpOdd(const T) throw();

	template<>
	static inline uint64_t RoundUpOdd<uint64_t>(const uint64_t value) throw() {
		return value | 1ull;
	}

	template<>
	static inline uint32_t RoundUpOdd<uint32_t>(const uint32_t value) throw() {
		return value | 1u;
	}

	template<>
	static inline uint16_t RoundUpOdd<uint16_t>(const uint16_t value) throw() {
		return value | 1u;
	}

	template<>
	static inline uint8_t RoundUpOdd<uint8_t>(const uint8_t value) throw() {
		return value | 1u;
	}

	template<>
	static inline int64_t RoundUpOdd<int64_t>(const int64_t value) throw() {
		return value | 1ll;
	}

	template<>
	static inline int32_t RoundUpOdd<int32_t>(const int32_t value) throw() {
		return value | 1;
	}

	template<>
	static inline int16_t RoundUpOdd<int16_t>(const int16_t value) throw() {
		return value | 1;
	}

	template<>
	static inline int8_t RoundUpOdd<int8_t>(const int8_t value) throw() {
		return value | 1;
	}

	template<>
	static inline float RoundUpOdd<float>(const float value) throw() {
		return static_cast<float>(RoundUpOdd(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundUpOdd<double>(const double value) throw() {
		return static_cast<double>(RoundUpOdd(static_cast<int64_t>(value)));
	}

	// RoundDownOdd

	template<class T>
	static T RoundDownOdd(const T) throw();

	template<>
	static inline uint64_t RoundDownOdd<uint64_t>(const uint64_t value) throw() {
		enum : uint64_t { MASK = ~1ull };
		return value & MASK;
	}

	template<>
	static inline uint32_t RoundDownOdd<uint32_t>(const uint32_t value) throw() {
		enum : uint32_t { MASK = ~1u };
		return value & MASK;
	}

	template<>
	static inline uint16_t RoundDownOdd<uint16_t>(const uint16_t value) throw() {
		enum : uint16_t { MASK = static_cast<uint16_t>(~1u) };
		return value & MASK;
	}

	template<>
	static inline uint8_t RoundDownOdd<uint8_t>(const uint8_t value) throw() {
		enum : uint8_t { MASK = static_cast<uint8_t>(~1u) };
		return value & MASK;
	}

	template<>
	static inline int64_t RoundDownOdd<int64_t>(const int64_t value) throw() {
		enum : int64_t { MASK = ~1ll };
		return value & MASK;
	}

	template<>
	static inline int32_t RoundDownOdd<int32_t>(const int32_t value) throw() {
		enum : int32_t { MASK = ~1 };
		return value & MASK;
	}

	template<>
	static inline int16_t RoundDownOdd<int16_t>(const int16_t value) throw() {
		enum : int16_t { MASK = ~1 };
		return value & MASK;
	}

	template<>
	static inline int8_t RoundDownOdd<int8_t>(const int8_t value) throw() {
		enum : int8_t { MASK = ~1 };
		return value & MASK;
	}

	template<>
	static inline float RoundDownOdd<float>(const float value) throw() {
		return static_cast<float>(RoundDownOdd(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundDownOdd<double>(const double value) throw() {
		return static_cast<double>(RoundDownOdd(static_cast<int64_t>(value)));
	}

	// RoundUpEven

	template<class T>
	static T RoundUpEven(const T) throw();

	template<>
	static inline uint64_t RoundUpEven<uint64_t>(const uint64_t value) throw() {
		return value + (value & 1ull);
	}

	template<>
	static inline uint32_t RoundUpEven<uint32_t>(const uint32_t value) throw() {
		return value + (value & 1u);
	}

	template<>
	static inline uint16_t RoundUpEven<uint16_t>(const uint16_t value) throw() {
		return value + (value & 1u);
	}

	template<>
	static inline uint8_t RoundUpEven<uint8_t>(const uint8_t value) throw() {
		return value + (value & 1u);
	}

	template<>
	static inline int64_t RoundUpEven<int64_t>(const int64_t value) throw() {
		return value + (value & 1ll);
	}

	template<>
	static inline int32_t RoundUpEven<int32_t>(const int32_t value) throw() {
		return value + (value & 1);
	}

	template<>
	static inline int16_t RoundUpEven<int16_t>(const int16_t value) throw() {
		return value + (value & 1);
	}

	template<>
	static inline int8_t RoundUpEven<int8_t>(const int8_t value) throw() {
		return value + (value & 1);
	}

	template<>
	static inline float RoundUpEven<float>(const float value) throw() {
		return static_cast<float>(RoundUpEven(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundUpEven<double>(const double value) throw() {
		return static_cast<double>(RoundUpEven(static_cast<int64_t>(value)));
	}

	// RoundDownEven

	template<class T>
	static T RoundDownEven(const T) throw();

	template<>
	static inline uint64_t RoundDownEven<uint64_t>(const uint64_t value) throw() {
		return value - (value & 1ull);
	}

	template<>
	static inline uint32_t RoundDownEven<uint32_t>(const uint32_t value) throw() {
		return value - (value & 1u);
	}

	template<>
	static inline uint16_t RoundDownEven<uint16_t>(const uint16_t value) throw() {
		return value - (value & 1u);
	}

	template<>
	static inline uint8_t RoundDownEven<uint8_t>(const uint8_t value) throw() {
		return value - (value & 1u);
	}

	template<>
	static inline int64_t RoundDownEven<int64_t>(const int64_t value) throw() {
		return value - (value & 1ll);
	}

	template<>
	static inline int32_t RoundDownEven<int32_t>(const int32_t value) throw() {
		return value - (value & 1);
	}

	template<>
	static inline int16_t RoundDownEven<int16_t>(const int16_t value) throw() {
		return value - (value & 1);
	}

	template<>
	static inline int8_t RoundDownEven<int8_t>(const int8_t value) throw() {
		return value - (value & 1);
	}

	template<>
	static inline float RoundDownEven<float>(const float value) throw() {
		return static_cast<float>(RoundDownEven(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundDownEven<double>(const double value) throw() {
		return static_cast<double>(RoundDownEven(static_cast<int64_t>(value)));
	}

	// PopulationCount

	template<class T>
	static size_t PopulationCount(const T) throw();

	template<>
	static size_t PopulationCount<uint64_t>(const uint64_t value) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		// Technically POPCNT flag on Intel and ABM flag on AMD CPUs, but SSE 4.2 is a close approximation
		if constexpr ((ASM_MINIMUM & ASM_SSE42) != 0ull) {
			int64_t count = _mm_popcnt_u64(value);
			ANVIL_ASSUME(count >= 0);
			ANVIL_ASSUME(count <= 64);
			return static_cast<size_t>(count);
		} 
#endif
		uint64_t b = value;
		// Based on implementation : https://rosettacode.org/wiki/Population_count#C
		b -= (b >> 1) & 0x5555555555555555ull;
		b = (b & 0x3333333333333333ull) + ((b >> 2ull) & 0x3333333333333333ull);
		b = (b + (b >> 4ull)) & 0x0f0f0f0f0f0f0f0full;
		b = (b * 0x0101010101010101ull) >> 56ull;
		ANVIL_ASSUME(b <= 64ull);
		return static_cast<size_t>(b);
	}

	template<>
	static size_t PopulationCount<uint32_t>(const uint32_t value) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		// Technically POPCNT flag on Intel and ABM flag on AMD CPUs, but SSE 4.2 is a close approximation
		if constexpr ((ASM_MINIMUM & ASM_SSE42) != 0ull) {
			int count = _mm_popcnt_u32(value);
			ANVIL_ASSUME(count >= 0);
			ANVIL_ASSUME(count <= 32);
			return static_cast<size_t>(count);
		}
#endif
		// Based on implementation : https://rosettacode.org/wiki/Population_count#C
		uint32_t b = value;
		b -= (b >> 1) & 0x55555555u;
		b = (b & 0x33333333u) + ((b >> 2u) & 0x33333333u);
		b = (b + (b >> 4u)) & 0x0f0f0f0fu;
		b = (b * 0x01010101u) >> 24u;
		ANVIL_ASSUME(b <= 32u);
		return static_cast<size_t>(b);
	}

	template<>
	static inline size_t PopulationCount<uint16_t>(const uint16_t value) throw() {
		const size_t tmp = PopulationCount<uint32_t>(value);
		ANVIL_ASSUME(tmp <= 16u);
		return tmp;
	}

	template<>
	static inline size_t PopulationCount<uint8_t>(const uint8_t value) throw() {
		const size_t tmp = PopulationCount<uint32_t>(value);
		ANVIL_ASSUME(tmp <= 8u);
		return tmp;
	}

	template<>
	static inline size_t PopulationCount<int64_t>(const int64_t value) throw() {
		union {
			uint64_t u;
			int64_t s;
		};
		s = value;
		return PopulationCount<uint64_t>(u);
	}

	template<>
	static inline size_t PopulationCount<int32_t>(const int32_t value) throw() {
		union {
			uint32_t u;
			int32_t s;
		};
		s = value;
		return PopulationCount<uint32_t>(u);
	}

	template<>
	static size_t PopulationCount<int16_t>(const int16_t value) throw() {
		union {
			uint16_t u;
			int16_t s;
		};
		s = value;
		return PopulationCount<uint16_t>(u);
	}

	template<>
	static inline size_t PopulationCount<int8_t>(const int8_t value) throw() {
		union {
			uint8_t u;
			int8_t s;
		};
		s = value;
		return PopulationCount<uint8_t>(u);
	}

	template<>
	static inline size_t PopulationCount<float>(const float value) throw() {
		union {
			uint32_t u;
			float f;
		};
		f = value;
		return PopulationCount<uint32_t>(u);
	}

	template<>
	static inline size_t PopulationCount<double>(const double value) throw() {
		union {
			uint64_t u;
			double f;
		};
		f = value;
		return PopulationCount<uint64_t>(u);
	}

	static size_t PopulationCount(const void* src, const size_t bytes) throw() {
		size_t count = 0u;

		const size_t aligned_size = bytes / sizeof(uintptr_t);
		const uintptr_t* src1 = static_cast<const uintptr_t*>(src);
		const uintptr_t* const end1 = src1 + aligned_size;
		while (src1 != end1) {
			count += PopulationCount<uintptr_t>(*src1);
			++src1;
		}

		const size_t aligned_size_bytes = aligned_size * sizeof(uintptr_t);
		if (aligned_size_bytes != bytes) {
			const size_t unaligned_size_bytes = (bytes - aligned_size_bytes);
			const uint8_t* src2 = reinterpret_cast<const uint8_t*>(end1);
			const uint8_t* const end2 = src2 + unaligned_size_bytes;
			while (src2 != end2) {
				count += PopulationCount<uint8_t>(*src2);
				++src2;
			}
		}

		return count;
	}

	// CountOnes

	template<class T>
	static inline size_t CountOnes(const T value) throw() {
		return PopulationCount<T>(value);
	}

	static size_t CountOnes(const void* src, const size_t bytes) throw() {
		return PopulationCount(src, bytes);
	}

	// CountZeros

	template<class T>
	static inline size_t CountZeros(const T value) throw() {
		return PopulationCount<T>(~ value);
	}

	template<>
	static inline size_t CountZeros<float>(const float value) throw() {
		union {
			uint32_t u;
			float f;
		};
		f = value;
		return CountZeros<uint32_t>(u);
	}

	template<>
	static inline size_t CountZeros<double>(const double value) throw() {
		union {
			uint64_t u;
			double f;
		};
		f = value;
		return CountZeros<uint64_t>(u);
	}

	static size_t CountZeros(const void* src, const size_t bytes) throw() {
		size_t count = 0u;

		const size_t aligned_size = bytes / sizeof(uintptr_t);
		const uintptr_t* src1 = static_cast<const uintptr_t*>(src);
		const uintptr_t* const end1 = src1 + aligned_size;
		while (src1 != end1) {
			count += CountZeros<uintptr_t>(*src1);
			++src1;
		}

		const size_t aligned_size_bytes = aligned_size * sizeof(uintptr_t);
		if (aligned_size_bytes != bytes) {
			const size_t unaligned_size_bytes = (bytes - aligned_size_bytes);
			const uint8_t* src2 = reinterpret_cast<const uint8_t*>(end1);
			const uint8_t* const end2 = src2 + unaligned_size_bytes;
			while (src2 != end2) {
				count += CountZeros<uint8_t>(*src2);
				++src2;
			}
		}

		return count;
	}

	// AllZeros

	template<class T>
	static bool AllZeros(const T value) throw() {
		enum : size_t {
			ALIGNED_COUNT = sizeof(T) / sizeof(intptr_t),
			UNALIGNED_COUNT = sizeof(T) % sizeof(intptr_t)
		};

		const intptr_t* const ptr = reinterpret_cast<const intptr_t*>(&value);
		for (size_t i = 0u; i < ALIGNED_COUNT; ++i) {
			if (ptr[i] != 0) return false;
		}

		for (size_t i = 0u; i < ALIGNED_COUNT; ++i) {
			if (reinterpret_cast<const int8_t*>(ptr)[ALIGNED_COUNT + i] != 0) return false;
		}
		
		return true;
	}

	template<>
	static inline bool AllZeros<uint64_t>(const uint64_t value) throw() {
		return value == 0u;
	}

	template<>
	static inline bool AllZeros<uint32_t>(const uint32_t value) throw() {
		return value == 0u;
	}

	template<>
	static inline bool AllZeros<uint16_t>(const uint16_t value) throw() {
		return value == 0u;
	}

	template<>
	static inline bool AllZeros<uint8_t>(const uint8_t value) throw() {
		return value == 0u;
	}

	template<>
	static inline bool AllZeros<int64_t>(const int64_t value) throw() {
		return value == 0;
	}

	template<>
	static inline bool AllZeros<int32_t>(const int32_t value) throw() {
		return value == 0;
	}

	template<>
	static inline bool AllZeros<int16_t>(const int16_t value) throw() {
		return value == 0;
	}

	template<>
	static inline bool AllZeros<int8_t>(const int8_t value) throw() {
		return value == 0;
	}

	template<>
	static inline bool AllZeros<float>(const float value) throw() {
		return value == 0.f;
	}

	template<>
	static inline bool AllZeros<double>(const double value) throw() {
		return value == 0.0;
	}

	static bool AllZeros(const void* src, const size_t bytes) throw() {
		const size_t aligned_size = bytes / sizeof(uintptr_t);
		const uintptr_t* src1 = static_cast<const uintptr_t*>(src);
		const uintptr_t* const end1 = src1 + aligned_size;
		while (src1 != end1) {
			if(!AllZeros<uintptr_t>(*src1)) return false;
			++src1;
		}

		const size_t aligned_size_bytes = aligned_size * sizeof(uintptr_t);
		if (aligned_size_bytes != bytes) {
			const size_t unaligned_size_bytes = (bytes - aligned_size_bytes);
			const uint8_t* src2 = reinterpret_cast<const uint8_t*>(end1);
			const uint8_t* const end2 = src2 + unaligned_size_bytes;
			while (src2 != end2) {
				if (!AllZeros<uint8_t>(*src2)) return false;
				++src2;
			}
		}

		return true;
	}

	// AllOnes

	template<class T>
	static bool AllOnes(const T value) throw() {
		enum : size_t {
			ALIGNED_COUNT = sizeof(T) / sizeof(intptr_t),
			UNALIGNED_COUNT = sizeof(T) % sizeof(intptr_t)
		};

		const intptr_t* const ptr = reinterpret_cast<const intptr_t*>(&value);
		for (size_t i = 0u; i < ALIGNED_COUNT; ++i) {
			if (ptr[i] != -1) return false;
		}

		for (size_t i = 0u; i < ALIGNED_COUNT; ++i) {
			if (reinterpret_cast<const int8_t*>(ptr)[ALIGNED_COUNT + i] != -1) return false;
		}

		return true;
	}

	template<>
	static inline bool AllOnes<uint64_t>(const uint64_t value) throw() {
		return value == UINT64_MAX;
	}

	template<>
	static inline bool AllOnes<uint32_t>(const uint32_t value) throw() {
		return value == UINT32_MAX;
	}

	template<>
	static inline bool AllOnes<uint16_t>(const uint16_t value) throw() {
		return value == UINT16_MAX;
	}

	template<>
	static inline bool AllOnes<uint8_t>(const uint8_t value) throw() {
		return value == UINT8_MAX;
	}

	template<>
	static inline bool AllOnes<int64_t>(const int64_t value) throw() {
		return value == -1;
	}

	template<>
	static inline bool AllOnes<int32_t>(const int32_t value) throw() {
		return value == -1;
	}

	template<>
	static inline bool AllOnes<int16_t>(const int16_t value) throw() {
		return value == -1;
	}

	template<>
	static inline bool AllOnes<int8_t>(const int8_t value) throw() {
		return value == -1;
	}

	template<>
	static inline bool AllOnes<float>(const float value) throw() {
		union {
			uint32_t u;
			float f;
		};
		f = value;
		return AllOnes<uint32_t>(u);
	}

	template<>
	static inline bool AllOnes<double>(const double value) throw() {
		union {
			uint64_t u;
			double f;
		};
		f = value;
		return AllOnes<uint64_t>(u);
	}

	static bool AllOnes(const void* src, const size_t bytes) throw() {
		const size_t aligned_size = bytes / sizeof(uintptr_t);
		const uintptr_t* src1 = static_cast<const uintptr_t*>(src);
		const uintptr_t* const end1 = src1 + aligned_size;
		while (src1 != end1) {
			if(!AllOnes<uintptr_t>(*src1)) return false;
			++src1;
		}

		const size_t aligned_size_bytes = aligned_size * sizeof(uintptr_t);
		if (aligned_size_bytes != bytes) {
			const size_t unaligned_size_bytes = (bytes - aligned_size_bytes);
			const uint8_t* src2 = reinterpret_cast<const uint8_t*>(end1);
			const uint8_t* const end2 = src2 + unaligned_size_bytes;
			while (src2 != end2) {
				if (!AllOnes<uint8_t>(*src2)) return false;
				++src2;
			}
		}

		return true;
	}

	// BitAnd

	template<class T>
	static inline T BitAnd(const T lhs, const T rhs) throw() {
		return lhs & rhs;
	}

	template<>
	static inline float BitAnd(const float lhs, const float rhs) throw() {
		union Union {
			uint32_t u;
			float f;
		};
		Union a, b;
		a.f = lhs;
		b.f = rhs;
		a.u &= b.u;
		return a.f;
	}

	template<>
	static inline double BitAnd(const double lhs, const double rhs) throw() {
		union Union {
			uint64_t u;
			double f;
		};
		Union a, b;
		a.f = lhs;
		b.f = rhs;
		a.u &= b.u;
		return a.f;
	}

	// BitOr

	template<class T>
	static inline T BitOr(const T lhs, const T rhs) throw() {
		return lhs | rhs;
	}

	template<>
	static inline float BitOr(const float lhs, const float rhs) throw() {
		union Union {
			uint32_t u;
			float f;
		};
		Union a, b;
		a.f = lhs;
		b.f = rhs;
		a.u |= b.u;
		return a.f;
	}

	template<>
	static inline double BitOr(const double lhs, const double rhs) throw() {
		union Union {
			uint64_t u;
			double f;
		};
		Union a, b;
		a.f = lhs;
		b.f = rhs;
		a.u |= b.u;
		return a.f;
	}

	// BitXor

	template<class T>
	static inline T BitXor(const T lhs, const T rhs) throw() {
		return lhs | rhs;
	}

	template<>
	static inline float BitXor(const float lhs, const float rhs) throw() {
		union Union {
			uint32_t u;
			float f;
		};
		Union a, b;
		a.f = lhs;
		b.f = rhs;
		a.u ^= b.u;
		return a.f;
	}

	template<>
	static inline double BitXor(const double lhs, const double rhs) throw() {
		union Union {
			uint64_t u;
			double f;
		};
		Union a, b;
		a.f = lhs;
		b.f = rhs;
		a.u ^= b.u;
		return a.f;
	}

	// BitNot

	template<class T>
	static inline T BitNot(const T value) throw() {
		return ~value;
	}

	template<>
	static inline float BitNot(const float value) throw() {
		union {
			uint32_t u;
			float f;
		};
		f = value;
		u = ~u;
		return f;
	}

	template<>
	static inline double BitNot(const double value) throw() {
		union {
			uint64_t u;
			double f;
		};
		f = value;
		u = ~u;
		return f;
	}

	// BitOrN

	template<class T>
	static inline T BitOrN(const T lhs, const T rhs) throw() {
		return BitOr<T>(BitNot<T>(lhs), rhs);
	}

	// BitXorN

	template<class T>
	static inline T BitXorN(const T lhs, const T rhs) throw() {
		return BitXor<T>(BitNot<T>(lhs), rhs);
	}

	// BitAndN

	template<class T>
	static inline T BitAndN(const T lhs, const T rhs) throw() {
		return BitAnd<T>(BitNot<T>(lhs), rhs);
	}
	
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64

	template<>
	static inline uint64_t BitAndN(const uint64_t lhs, const uint64_t rhs) throw() {
		//! \bug Should check for BMI1
		if constexpr ((ASM_MINIMUM & ASM_AVX2) != 0u) {
			return _andn_u64(lhs, rhs);
		} else {
			return (~lhs) & rhs;
		}
	}

	template<>
	static inline int64_t BitAndN(const int64_t lhs, const int64_t rhs) throw() {
		//! \bug Should check for BMI1
		if constexpr ((ASM_MINIMUM & ASM_AVX2) != 0u) {
			union Union {
				uint64_t u;
				int64_t s;
			};
			Union a, b;
			a.s = _andn_u64(a.s, b.s);
			return a.u;
		} else {
			return (~lhs) & rhs;
		}
	}

#endif

#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64

	template<>
	static inline uint32_t BitAndN(const uint32_t lhs, const uint32_t rhs) throw() {
		//! \bug Should check for BMI1
		if constexpr ((ASM_MINIMUM & ASM_AVX2) != 0u) {
			return _andn_u32(lhs, rhs);
		} else {
			return (~lhs) & rhs;
		}
	}

	template<>
	static inline int32_t BitAndN(const int32_t lhs, const int32_t rhs) throw() {
		//! \bug Should check for BMI1
		if constexpr ((ASM_MINIMUM & ASM_AVX2) != 0u) {
			union Union {
				uint32_t u;
				int32_t s;
			};
			Union a, b;
			a.s = _andn_u32(a.s, b.s);
			return a.u;
		} else {
			return (~lhs) & rhs;
		}
	}

#endif

	// Blend

	template<class T>
	static inline T Blend(const T ifOne, const T ifZero, const T mask) throw() {
		return BitOr<T>(BitAnd<T>(mask, ifOne),  BitAndN<T>(mask, ifZero));
	}

	template<>
	static inline float Blend<float>(const float ifOne, const float ifZero, const float mask) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		if constexpr ((ASM_MINIMUM & ASM_SSE41) != 0ull) {
			return _mm_cvtss_f32 (_mm_blendv_ps(_mm_load_ss(&ifZero), _mm_load_ss(&ifOne), _mm_load_ss(&mask)));
		} else if constexpr ((ASM_MINIMUM & ASM_SSE) != 0ull) {
			const __m128 xmm0 = _mm_load_ss(&mask);
			return _mm_cvtss_f32 (_mm_or_ps(_mm_and_ps(xmm0, _mm_load_ss(&ifOne)), _mm_andnot_ps(xmm0, _mm_load_ss(&ifZero))));
		} else
#endif
		{
			union Union {
				uint32_t u;
				float f;
			};
			Union a, b, c;
			a.f = ifOne;
			b.f = ifZero;
			c.f = mask;
			a.u = Blend<uint32_t>(a.u, b.u, c.u);
			return a.f;
		}
	}

	template<>
	static inline double Blend<double>(const double ifOne, const double ifZero, const double mask) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		if constexpr ((ASM_MINIMUM & ASM_SSE41) != 0ull) {
			return _mm_cvtsd_f64(_mm_blendv_pd(_mm_load_sd(&ifZero), _mm_load_sd(&ifOne), _mm_load_sd(&mask)));
		} else if constexpr ((ASM_MINIMUM & ASM_SSE2) != 0ull) {
			const __m128d xmm0 = _mm_load_sd(&mask);
			return _mm_cvtsd_f64(_mm_or_pd(_mm_and_pd(xmm0, _mm_load_sd(&ifOne)), _mm_andnot_pd(xmm0, _mm_load_sd(&ifZero))));
		}
		else
#endif
		{
			union Union {
				uint64_t u;
				double f;
			};
			Union a, b, c;
			a.f = ifOne;
			b.f = ifZero;
			c.f = mask;
			a.u = Blend<uint64_t>(a.u, b.u, c.u);
			return a.f;
		}
	}

	// BitTest

	template<class T>
	static bool BitTest(const T value, const size_t index) throw();

	template<>
	static inline bool BitTest<uint64_t>(const uint64_t value, const size_t index) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		static_assert(sizeof(long long) == sizeof(int64_t), "Expected sizeof(long) == sizeof(int32_t)");
		static_cast<bool>(_bittest64(reinterpret_cast<const long long*>(&value), static_cast<long long>(index)));
#else
		return (value & (1ull << index)) != 0ull;
#endif
	}

	template<>
	static inline bool BitTest<uint32_t>(const uint32_t value, const size_t index) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		static_assert(sizeof(long) == sizeof(int32_t), "Expected sizeof(long) == sizeof(int32_t)");
		static_cast<bool>(_bittest(reinterpret_cast<const long*>(&value), static_cast<int32_t>(index)));
#else
		return (value & (1u << index)) != 0u;
#endif
	}

	template<>
	static inline bool BitTest<uint16_t>(const uint16_t value, const size_t index) throw() {
		return BitTest<uint32_t>(value, index);
	}

	template<>
	static inline bool BitTest<uint8_t>(const uint8_t value, const size_t index) throw() {
		return BitTest<uint32_t>(value, index);
	}

	template<>
	static inline bool BitTest<int64_t>(const int64_t value, const size_t index) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		static_assert(sizeof(long long) == sizeof(int64_t), "Expected sizeof(long) == sizeof(int32_t)");
		static_cast<bool>(_bittest64(reinterpret_cast<const long long*>(&value), static_cast<long long>(index)));
#else
		return (value & (1ll << index)) != 0ll;
#endif
	}

	template<>
	static inline bool BitTest<int32_t>(const int32_t value, const size_t index) throw() {
#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
		static_assert(sizeof(long) == sizeof(int32_t), "Expected sizeof(long) == sizeof(int32_t)");
		static_cast<bool>(_bittest(reinterpret_cast<const long*>(&value), static_cast<int32_t>(index)));
#else
		return (value & (1 << index)) != 0;
#endif
	}

	template<>
	static inline bool BitTest<int16_t>(const int16_t value, const size_t index) throw() {
		return BitTest<int32_t>(value, index);
	}

	template<>
	static inline bool BitTest<int8_t>(const int8_t value, const size_t index) throw() {
		return BitTest<int32_t>(value, index);
	}

	template<>
	static inline bool BitTest<float>(const float value, const size_t index) throw() {
		union {
			int32_t s;
			float f;
		};
		f = value;
		return BitTest<int32_t>(s, index);
	}

	template<>
	static inline bool BitTest<double>(const double value, const size_t index) throw() {
		union {
			int64_t s;
			double f;
		};
		f = value;
		return BitTest<int64_t>(s, index);
	}
}

#endif
