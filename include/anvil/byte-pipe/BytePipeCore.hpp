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

#ifndef ANVIL_BYTEPIPE_CORE_HPP
#define ANVIL_BYTEPIPE_CORE_HPP

#include <cstdint>

namespace anvil {namespace BytePipe {
	enum Version : uint8_t {
		VERSION_1 = 1,
		VERSION_2 = 2,
		VERSION_3 = 3
	};
}}

#endif
