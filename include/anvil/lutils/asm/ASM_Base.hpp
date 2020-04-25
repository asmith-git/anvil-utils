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

#ifndef ANVIL_EXPERIMENTAL_X86
	#define ANVIL_EXPERIMENTAL_X86 0
#endif 

#if ANVIL_EXPERIMENTAL_X86
	#include <immintrin.h>
#endif

namespace anvil { namespace lutils { namespace experimental {

	typedef uint64_t InstructionSets;

#if ANVIL_EXPERIMENTAL_X86
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
		ANVIL_AVX512F =			1ull << 9ull
	};


	enum : InstructionSets { 
		INSTRUCTION_SETS_DEFAULT = ANVIL_SSE | ANVIL_SSE2 | ANVIL_SSE3 | ANVIL_SSSE3
	};
#else
	enum : InstructionSets { 
		INSTRUCTION_SETS_DEFAULT = 0ull 
	};
#endif

}}}

#endif