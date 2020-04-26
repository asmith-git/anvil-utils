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

#ifndef ANVIL_LUTILS_ASM_ADD_HPP
#define ANVIL_LUTILS_ASM_ADD_HPP

#include "anvil/lutils/asm/ASM_Base.hpp"
#include "anvil/lutils/asm/ASM_Blend.hpp"
#include "anvil/lutils/asm/ASM_Operator2.hpp"

namespace anvil { namespace lutils { namespace experimental {

	// Typedefs

	template<class T, const uint64_t MASK = DefaultMask<T>::value, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	using Add = Operator2<OPERATOR_ADD, T, MASK, IS>;

	template<class T, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	using AddRT = Operator2RT<OPERATOR_ADD, T, IS>;

	// Primative Implementations

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, float, IS> {
		enum { 
			optimised_blend_ct = false,
			optimised_blend_rt = false,
			has_context = false
		};

		inline float operator()(const float lhs, const float rhs) const throw() {
			return lhs + rhs;
		}
	};

	// x86 Implementation

#if ANVIL_EXPERIMENTAL_X86

	// Float32

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m128, IS> {
		enum {
			optimised_blend_ct = (IS & ANVIL_AVX512VL) != 0ull,
			optimised_blend_rt = (IS & ANVIL_AVX512VL) != 0ull,
			has_context = false
		};

		inline __m128 operator()(const __m128 lhs, const __m128 rhs) const throw() {
			return _mm_add_ps(lhs, rhs);
		}

		template<uint64_t MASK>
		inline __m128 OptimisedBlendCT(const __m128 src, const __m128 lhs, const __m128 rhs) const throw() {
			return _mm_mask_add_ps(src, static_cast<__mmask8>(MASK), lhs, rhs);
		}

		inline __m128 OptimisedBlendRT(const __m128 src, const __m128 lhs, const __m128 rhs, const uint64_t mask) const throw() {
			return _mm_mask_add_ps(src, static_cast<__mmask8>(mask), lhs, rhs);
		}
	};

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m256, IS> {
		enum {
			optimised_blend_ct = (IS & ANVIL_AVX512VL) != 0ull,
			optimised_blend_rt = (IS & ANVIL_AVX512VL) != 0ull,
			has_context = false
		};

		inline __m256 operator()(const __m256 lhs, const __m256 rhs) const throw() {
			return _mm256_add_ps(lhs, rhs);
		}

		template<uint64_t MASK>
		inline __m256 OptimisedBlendCT(const __m256 src, const __m256 lhs, const __m256 rhs) const throw() {
			return _mm256_mask_add_ps(src, static_cast<__mmask8>(MASK), lhs, rhs);
		}

		inline __m256 OptimisedBlendRT(const __m256 src, const __m256 lhs, const __m256 rhs, const uint64_t mask) const throw() {
			return _mm256_mask_add_ps(src, static_cast<__mmask8>(mask), lhs, rhs);
		}
	};

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m512, IS> {
		enum {
			optimised_blend_ct = true,
			optimised_blend_rt = true,
			has_context = false
		};

		inline __m512 operator()(const __m512 lhs, const __m512 rhs) const throw() {
			return _mm512_add_ps(lhs, rhs);
		}

		template<uint64_t MASK>
		inline __m512 OptimisedBlendCT(const __m512 src, const __m512 lhs, const __m512 rhs) const throw() {
			return _mm512_mask_add_ps(src, static_cast<__mmask8>(MASK), lhs, rhs);
		}

		inline __m512 OptimisedBlendRT(const __m512 src, const __m512 lhs, const __m512 rhs, const uint64_t mask) const throw() {
			return _mm512_mask_add_ps(src, static_cast<__mmask8>(mask), lhs, rhs);
		}
	};

	// Float64



	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m128d, IS> {
		enum {
			optimised_blend_ct = (IS & ANVIL_AVX512VL) != 0ull,
			optimised_blend_rt = (IS & ANVIL_AVX512VL) != 0ull,
			has_context = false
		};

		inline __m128d operator()(const __m128d lhs, const __m128d rhs) const throw() {
			return _mm_add_pd(lhs, rhs);
		}

		template<uint64_t MASK>
		inline __m128d OptimisedBlendCT(const __m128d src, const __m128d lhs, const __m128d rhs) const throw() {
			return _mm_mask_add_pd(src, static_cast<__mmask8>(MASK), lhs, rhs);
		}

		inline __m128d OptimisedBlendRT(const __m128d src, const __m128d lhs, const __m128d rhs, const uint64_t mask) const throw() {
			return _mm_mask_add_pd(src, static_cast<__mmask8>(mask), lhs, rhs);
		}
	};

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m256d, IS> {
		enum {
			optimised_blend_ct = (IS & ANVIL_AVX512VL) != 0ull,
			optimised_blend_rt = (IS & ANVIL_AVX512VL) != 0ull,
			has_context = false
		};

		inline __m256d operator()(const __m256 lhs, const __m256 rhs) const throw() {
			return _mm256_add_pd(lhs, rhs);
		}

		template<uint64_t MASK>
		inline __m256d OptimisedBlendCT(const __m256d src, const __m256d lhs, const __m256d rhs) const throw() {
			return _mm256_mask_add_pd(src, static_cast<__mmask8>(MASK), lhs, rhs);
		}

		inline __m256d OptimisedBlendRT(const __m256d src, const __m256d lhs, const __m256d rhs, const uint64_t mask) const throw() {
			return _mm256_mask_add_pd(src, static_cast<__mmask8>(mask), lhs, rhs);
		}
	};

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m512d, IS> {
		enum {
			optimised_blend_ct = true,
			optimised_blend_rt = true,
			has_context = false
		};

		inline __m512d operator()(const __m512d lhs, const __m512d rhs) const throw() {
			return _mm512_add_pd(lhs, rhs);
		}

		template<uint64_t MASK>
		inline __m512d OptimisedBlendCT(const __m512d src, const __m512d lhs, const __m512d rhs) const throw() {
			return _mm512_mask_add_pd(src, static_cast<__mmask8>(MASK), lhs, rhs);
		}

		inline __m512d OptimisedBlendRT(const __m512d src, const __m512d lhs, const __m512d rhs, const uint64_t mask) const throw() {
			return _mm512_mask_add_pd(src, static_cast<__mmask8>(mask), lhs, rhs);
		}
	};

#endif

}}}

#endif