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
}

#endif
