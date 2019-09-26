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

#ifndef ANVIL_CPU_ARCHITECUTE
#define ANVIL_CPU_ARCHITECUTE ANVIL_CPU_UNKNOWN
#endif

namespace anvil {
	enum CpuArchitecture : uint8_t {
		CPU_UNKNOWN = ANVIL_CPU_UNKNOWN,
	};

	static const constexpr CpuArchitecture CPU_ARCHITECUTE = static_cast<CpuArchitecture>(ANVIL_CPU_ARCHITECUTE);
}

#endif
