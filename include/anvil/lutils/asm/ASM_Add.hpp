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

	template<class T, const uint64_t MASK = DefaultMask<T>::value, InstructionSets IS = MinInstructionSet<T>::value>
	using Add = Operator2<OPERATOR_ADD, T, MASK, IS>;

	template<class T, InstructionSets IS = MinInstructionSet<T>::value>
	using AddRT = Operator2RT<OPERATOR_ADD, T, IS>;

	// Primative Implementations

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, float, IS> {
		inline float operator()(const float lhs, const float rhs) {
			return lhs + rhs;
		}
	};

	// x86 Implementation

#if ANVIL_EXPERIMENTAL_X86
	//! \todo Optimise for AVX512

	template<InstructionSets IS>
	struct Operator2Primative<OPERATOR_ADD, __m128, IS> {
		inline __m128 operator()(const __m128 lhs, const __m128 rhs) {
			return _mm_add_ps(lhs, rhs);
		}
	};

#endif

}}}

#endif