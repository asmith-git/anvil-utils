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

#ifndef ANVIL_LUTILS_ASSERT_HPP
#define ANVIL_LUTILS_ASSERT_HPP

#define ANVIL_CONTRACT_IGNORE 0

#ifndef ANVIL_CONTRACT_MODE
	#define ANVIL_CONTRACT_MODE ANVIL_CONTRACT_IGNORE
#endif

#if ANVIL_CONTRACT_MODE < ANVIL_CONTRACT_IGNORE || ANVIL_CONTRACT_MODE > ANVIL_CONTRACT_IGNORE
	#error Invalid contract mode
#endif

#if ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_IGNORE
#define ANVIL_CONTRACT(CONDITION,MESSAGE)
#endif

#if _DEBUG
#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) ANVIL_CONTRACT(CONDITION,MESSAGE)
#else
#define ANVIL_CONTRACT(CONDITION,MESSAGE)
#endif

#endif
