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

#ifndef ANVIL_LUTILS_CPU_HPP
#define ANVIL_LUTILS_CPU_HPP

#include <cstdint>

#define ANVIL_CPU_UNKNOWN 0u
#define ANVIL_CPU_X86 1u
#define ANVIL_CPU_X86_64 2u

#ifndef ANVIL_CPU_ARCHITECUTE
	#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
		#define ANVIL_CPU_ARCHITECUTE ANVIL_CPU_X86_64
	#elif defined(i386) ||  defined(__i386) ||  defined(__i386__) ||  defined(__i486__) ||  defined(__i586__) ||  defined(__i686__) ||  defined(__IA32__) ||  defined(_M_I86) ||  defined(_M_IX86) ||  defined(__X86__) ||  defined(_X86_) ||  defined(__THW_INTEL__) ||   defined(__INTEL__) ||   defined(__THW_INTEL__) ||   defined(__386)
		#define ANVIL_CPU_ARCHITECUTE ANVIL_CPU_X86
	#else
		#define ANVIL_CPU_ARCHITECUTE ANVIL_CPU_UNKNOWN
	#endif
#endif

namespace anvil {
	enum CpuArchitecture : uint8_t {
		CPU_UNKNOWN = ANVIL_CPU_UNKNOWN,
		CPU_X86 = ANVIL_CPU_X86,
		CPU_X86_64 = ANVIL_CPU_X86_64
	};

	static const constexpr CpuArchitecture CPU_ARCHITECUTE = static_cast<CpuArchitecture>(ANVIL_CPU_ARCHITECUTE);

#if ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86 || ANVIL_CPU_ARCHITECUTE == ANVIL_CPU_X86_64
	#ifndef ANVIL_MIN_INSTRUCTION_SET
		#define ANVIL_MIN_INSTRUCTION_SET (ASM_MMX | ASM_SSE | ASM_SSE2)
	#endif

	enum InstructionSets : uint64_t {
		ASM_MMX =		1ull << 0ull,
		ASM_SSE =		1ull << 1ull,
		ASM_SSE2 =		1ull << 2ull,
		ASM_SSE3 =		1ull << 3ull,
		ASM_SSSE3 =		1ull << 4ull,
		ASM_SSE41 =		1ull << 5ull,
		ASM_SSE42 =		1ull << 6ull,
		ASM_AVX =		1ull << 7ull,
		ASM_AVX2 =		1ull << 8ull,
		ASM_FMA3 =		1ull << 9ull,
		ASM_AVX512F =	1ull << 10ull,
		ASM_AVX512VL =	1ull << 11ull,
		ASM_AVX512BW =	1ull << 12ull,
	};

	static constexpr const uint64_t ASM_MINIMUM = ANVIL_MIN_INSTRUCTION_SET;

	static uint64_t CheckSupportedInstructionSets() throw() {
		//! \todo Implement
		return ASM_MINIMUM;
	}

#endif

}

#endif
