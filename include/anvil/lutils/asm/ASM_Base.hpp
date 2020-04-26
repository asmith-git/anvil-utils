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

	// Helpers for determining vector implementation

	template<class T, size_t S, InstructionSets IS>
	struct OptimisedVectorType {
		typedef void type;
		enum { value = false };
	};

	// Helper for determining vector properties

	template<class T>
	struct VectorTypeProperties;

	template<>
	struct VectorTypeProperties<int8_t> {
		typedef int8_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<int16_t> {
		typedef int16_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<int32_t> {
		typedef int32_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<int64_t> {
		typedef int64_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<uint8_t> {
		typedef uint8_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<uint16_t> {
		typedef uint16_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<uint32_t> {
		typedef uint32_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<uint64_t> {
		typedef uint64_t type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<float> {
		typedef float type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<>
	struct VectorTypeProperties<double> {
		typedef double type;
		typedef type element_type;
		enum : bool { optimised = false };
		enum : size_t { length = 1u };
		enum : InstructionSets { min_instruction_set = 0u };
	};

	template<class T, class T2>
	struct VectorTypeProperties<std::pair<T, T2>> {
		typedef std::pair<T, T2> type;
		enum : bool { optimised = VectorTypeProperties<T>::optimised || VectorTypeProperties<2T>::optimised  };
		enum : size_t { length = VectorTypeProperties<T>::length + VectorTypeProperties<T2>::length };
		enum : InstructionSets { min_instruction_set = VectorTypeProperties<T>::min_instruction_set | VectorTypeProperties<T2>::min_instruction_set };
	};

	template<class T, size_t S>
	struct VectorTypeProperties<std::array<T, S>> {
		typedef std::array<T, S> type;
		enum : bool { optimised = false };
		enum : size_t { length = VectorTypeProperties<T>::length * S };
		enum : InstructionSets { min_instruction_set = VectorTypeProperties<T>::min_instruction_set };
	};

	// x86 Optimisations

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

	namespace detail {
		template<class T>
		class __m128iT {
		private:
			__m128i _value;
		public:
			inline __m128iT() throw() :
				_value()
			{}

			inline __m128iT(const __m128i value) throw() :
				_value(value)
			{}

			inline operator __m128i() const throw() {
				return _value;
			}
		};

		template<class T>
		class __m256iT {
		private:
			__m256i _value;
		public:
			inline __m256iT() throw() :
				_value()
			{}

			inline __m256iT(const __m256i value) throw() :
				_value(value)
			{}

			inline operator __m256i() const throw() {
				return _value;
			}
		};

		template<class T>
		class __m512iT {
		private:
			__m512i _value;
		public:
			inline __m512iT() throw() :
				_value()
			{}

			inline __m512iT(const __m512i value) throw() :
				_value(value)
			{}

			inline operator __m512i() const throw() {
				return _value;
			}
		};
	}

	//template<class T, InstructionSets IS>
	//struct OptimisedVectorType<detail::__m128iT<T>, 16u / sizeof(T), IS> {
	//	typedef detail::__m128iT<T> type;
	//	enum { value = (IS & ANVIL_SSE2) != 0ull };
	//};

	// Vector properties definitions

	template<>
	struct VectorTypeProperties<__m128> {
		typedef __m128 type;
		typedef float element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_SSE };
	};

	template<>
	struct VectorTypeProperties<__m256> {
		typedef __m256 type;
		typedef float element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_AVX };
	};

	template<>
	struct VectorTypeProperties<__m512> {
		typedef __m512 type;
		typedef float element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_AVX512F | ANVIL_AVX512VL };
	};

	template<>
	struct VectorTypeProperties<__m128d> {
		typedef __m128d type;
		typedef double element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_SSE2 };
	};

	template<>
	struct VectorTypeProperties<__m256d> {
		typedef __m256d type;
		typedef double element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_AVX };
	};

	template<>
	struct VectorTypeProperties<__m512d> {
		typedef __m512d type;
		typedef double element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_AVX512F | ANVIL_AVX512VL };
	};

	template<class T>
	struct VectorTypeProperties<detail::__m128iT<T>> {
		typedef detail::__m128iT<T> type;
		typedef T element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_SSE2 };
	};

	template<class T>
	struct VectorTypeProperties<detail::__m256iT<T>> {
		typedef detail::__m256iT<T> type;
		typedef T element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_AVX2 };
	};

	template<class T>
	struct VectorTypeProperties<detail::__m512iT<T>> {
		typedef detail::__m512iT<T> type;
		typedef T element_type;
		enum : bool { optimised = true };
		enum : size_t { length = sizeof(type) / sizeof(element_type) };
		enum : InstructionSets { min_instruction_set = ANVIL_AVX512F | ANVIL_AVX512VL };
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
		enum : uint64_t { value = detail::DefaultMask_<VectorTypeProperties<T>::length>::value };
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

		template<class T, InstructionSets IS>
		struct VectorType_<T, 1u, IS, false> {
			typedef T type;
		};
	}



	// Vector Types

	template<class T, size_t S, InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using VectorType = typename detail::VectorType_<T, S, IS>::type;

	// 8-bit types

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x1 = VectorType<int8_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x2 = VectorType<int8_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x4 = VectorType<int8_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x8 = VectorType<int8_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x16 = VectorType<int8_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x32 = VectorType<int8_t, 32u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S8x64 = VectorType<int8_t, 64u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U8x1 = VectorType<uint8_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U8x2 = VectorType<uint8_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U8x4 = VectorType<uint8_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U8x8 = VectorType<uint8_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U8x16 = VectorType<uint8_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U8x32 = VectorType<uint8_t, 32u, IS>;

	// 16-bit types

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S16x1 = VectorType<int16_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S16x2 = VectorType<int16_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S16x4 = VectorType<int16_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S16x8 = VectorType<int16_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S16x16 = VectorType<int16_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S16x32 = VectorType<int16_t, 32u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U16x1 = VectorType<uint16_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U16x2 = VectorType<uint16_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U16x4 = VectorType<uint16_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U16x8 = VectorType<uint16_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U16x16 = VectorType<uint16_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U16x32 = VectorType<uint16_t, 32u, IS>;

	// 32-bit types

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S32x1 = VectorType<int32_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S32x2 = VectorType<int32_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S32x4 = VectorType<int32_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S32x8 = VectorType<int32_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S32x16 = VectorType<int32_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U32x1 = VectorType<uint32_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U32x2 = VectorType<uint32_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U32x4 = VectorType<uint32_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U32x8 = VectorType<uint32_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U32x16 = VectorType<uint32_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F32x1 = VectorType<float, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F32x2 = VectorType<float, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F32x4 = VectorType<float, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F32x8 = VectorType<float, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F32x16 = VectorType<float, 16u, IS>;

	// 64-bit types

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S64x1 = VectorType<int64_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S64x2 = VectorType<int64_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S64x4 = VectorType<int64_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S64x8 = VectorType<int64_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using S64x16 = VectorType<int64_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U64x1 = VectorType<uint64_t, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U64x2 = VectorType<uint64_t, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U64x4 = VectorType<uint64_t, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U64x8 = VectorType<uint64_t, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using U64x16 = VectorType<uint64_t, 16u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F64x1 = VectorType<double, 1u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F64x2 = VectorType<double, 2u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F64x4 = VectorType<double, 4u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F64x8 = VectorType<double, 8u, IS>;

	template<InstructionSets IS = INSTRUCTION_SETS_DEFAULT>
	using F64x16 = VectorType<double, 16u, IS>;
}}}

#endif