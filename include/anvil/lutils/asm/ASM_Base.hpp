//Copyright 2020 Adam G. Smith
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

#ifndef ANVIL_LUTILS_ASM_BASE_HPP
#define ANVIL_LUTILS_ASM_BASE_HPP

#include <cstdint>
#include <array>
#include <algorithm>

#ifndef ANVIL_EXPERIMENTAL_X86
	#define ANVIL_EXPERIMENTAL_X86 0
#endif 

#if ANVIL_EXPERIMENTAL_X86
	#include <immintrin.h>
#endif

namespace anvil { namespace lutils { namespace experimental {

	typedef uint64_t InstructionSets;

	// Primative Types

	typedef int8_t Signed8x1;
	typedef int16_t Signed16x1;
	typedef int32_t Signed32x1;
	typedef int64_t Signed64x1;
	typedef uint8_t Unsigned8x1;
	typedef uint16_t Unsigned16x1;
	typedef uint32_t Unsigned32x1;
	typedef uint64_t Unsigned64x1;
	//typedef half Float16x1;
	typedef float Float32x1;
	typedef double Float64x1;

	// Helpers for determining vector implementation

	template<class T, size_t S, InstructionSets IS>
	struct OptimisedVectorType {
		typedef void type;
		enum { value = false };
	};

	// Helper for determining vector length

	template<class T>
	struct VectorLength;

	template<>
	struct VectorLength<Signed8x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Signed16x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Signed32x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Signed64x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Unsigned8x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Unsigned16x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Unsigned32x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Unsigned64x1> {
		enum : size_t { value = 1u };
	};

	//template<>
	//struct VectorLength<Float16x1> {
	//	enum : size_t { value = 1u };
	//};

	template<>
	struct VectorLength<Float32x1> {
		enum : size_t { value = 1u };
	};

	template<>
	struct VectorLength<Float64x1> {
		enum : size_t { value = 1u };
	};

	template<class T>
	struct VectorLength<std::pair<T, T>> {
		enum : size_t { value = VectorLength<T>::value * 2u };
	};

	template<class T, size_t S>
	struct VectorLength<std::array<T, S>> {
		enum : size_t { value = VectorLength<T>::value * S };
	};

	template<class T>
	struct MinInstructionSet {
		enum : InstructionSets {
			value = INSTRUCTION_SETS_DEFAULT
		};
	};

#if ANVIL_EXPERIMENTAL_X86
	// MMX isn't supported currently
	enum : InstructionSets { 
		ANVIL_SSE =				1ull << 0ull,
		ANVIL_SSE2 =			1ull << 1ull,
		ANVIL_SSE3 =			1ull << 2ull,
		ANVIL_SSSE3 =			1ull << 3ull,
		ANVIL_SSE41 =			1ull << 4ull,
		ANVIL_SSE42 =			1ull << 5ull,
		ANVIL_AVX =				1ull << 6ull,
		ANVIL_AVX2 =			1ull << 7ull,
		ANVIL_FMA3 =			1ull << 8ull,
		ANVIL_AVX512F =			1ull << 9ull,
		ANVIL_AVX512VL =		1ull << 10ull
	};


	enum : InstructionSets { 
		INSTRUCTION_SETS_DEFAULT = ANVIL_SSE | ANVIL_SSE2 | ANVIL_SSE3 | ANVIL_SSSE3
	};

	// Vector type definitions

	template<InstructionSets IS>
	struct OptimisedVectorType<float, 4u, IS> {
		typedef __m128 type;
		enum { value = (IS & ANVIL_SSE) != 0ull };
	};

	template<InstructionSets IS>
	struct OptimisedVectorType<float, 8u, IS> {
		typedef __m256 type;
		enum { value = (IS & ANVIL_AVX) != 0ull };
	};

	template<InstructionSets IS>
	struct OptimisedVectorType<float, 16u, IS> {
		typedef __m512 type;
		enum { value = (IS & ANVIL_AVX512VL) != 0ull };
	};

	template<InstructionSets IS>
	struct OptimisedVectorType<double, 2u, IS> {
		typedef __m128d type;
		enum { value = (IS & ANVIL_SSE2) != 0ull };
	};

	template<InstructionSets IS>
	struct OptimisedVectorType<double, 4u, IS> {
		typedef __m256d type;
		enum { value = (IS & ANVIL_AVX) != 0ull };
	};

	template<InstructionSets IS>
	struct OptimisedVectorType<double, 8u, IS> {
		typedef __m512d type;
		enum { value = (IS & ANVIL_AVX512VL) != 0ull };
	};

	// Vector length definitions

	template<>
	struct VectorLength<__m128> {
		enum : size_t { value = 4u };
	};

	template<>
	struct VectorLength<__m256> {
		enum : size_t { value = 8u };
	};

	template<>
	struct VectorLength<__m512> {
		enum : size_t { value = 16u };
	};

	template<>
	struct VectorLength<__m128d> {
		enum : size_t { value = 2u };
	};

	template<>
	struct VectorLength<__m256d> {
		enum : size_t { value = 4u };
	};

	template<>
	struct VectorLength<__m512d> {
		enum : size_t { value = 8u };
	};

	template<>
	struct MinInstructionSet<__m128> {
		enum : InstructionSets {
			value = INSTRUCTION_SETS_DEFAULT | ANVIL_SSE
		};
	};
#else
	enum : InstructionSets { 
		INSTRUCTION_SETS_DEFAULT = 0ull 
	};
#endif

	// Mask Helper

	namespace detail {
		template<size_t LEN>
		struct DefaultMask_ {
			enum : uint64_t { value = (DefaultMask_<LEN - 1ull>::value << 1ull) | 1ull };
		};

		template<>
		struct DefaultMask_<0u> {
			enum : uint64_t { value = 0ull };
		};

		template<>
		struct DefaultMask_<1u> {
			enum : uint64_t { value = 1ull };
		};
	}

	template<class T>
	struct DefaultMask {
		enum : uint64_t { value = detail::DefaultMask_<VectorLength<T>::value>::value };
	};

	// Vector implementation selection

	namespace detail {
		template<class T, size_t S, InstructionSets IS, bool OPTIMISED = OptimisedVectorType<T,S,IS>::value>
		struct VectorType_;

		template<class T, size_t S, InstructionSets IS>
		struct VectorType_<T, S, IS, true> {
			//! \todo Compound vector out of smaller optimised types
			typedef typename OptimisedVectorType<T, S, IS>::type type;
		};

		template<class T, size_t S, InstructionSets IS>
		struct VectorType_<T, S, IS, false> {
			//! \todo Compound vector out of smaller optimised types
			typedef std::array<T, S> type;
		};
	}

	template<class T, size_t S, InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using VectorType = typename detail::VectorType_<T, S, IS>::type;


	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using Float32x4 = VectorType<float, 4u, IS>;
}}}

#endif