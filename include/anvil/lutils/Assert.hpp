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

#include "anvil/lutils/LibDetect.hpp"

#ifndef ANVIL_LUTILS_ASSERT_HPP
#define ANVIL_LUTILS_ASSERT_HPP

#if ANVIL_GSL_SUPPORT
	#define ANVIL_ASSUME(CONDITION) GSL_ASSUME(CONDITION)
#else 
	#define ANVIL_ASSUME(CONDITION) __assume(CONDITION)
#endif
#define ANVIL_ASSUME_IMPOSSIBLE __assume(0)

#define ANVIL_CONTRACT_IGNORE 0
#define ANVIL_CONTRACT_ASSUME 1
#define ANVIL_CONTRACT_EXCEPTION 2
#define ANVIL_CONTRACT_STD_ERR 3
#define ANVIL_CONTRACT_CPP_ASSERT 4
#define ANVIL_CONTRACT_GSL_PREDICATE 5

#ifndef ANVIL_CONTRACT_MODE
	#define ANVIL_CONTRACT_MODE ANVIL_CONTRACT_EXCEPTION
#endif

#ifndef ANVIL_DEBUG_CONTRACT_MODE
	#define ANVIL_DEBUG_CONTRACT_MODE ANVIL_CONTRACT_ASSUME
#endif

#if ANVIL_CONTRACT_MODE < ANVIL_CONTRACT_IGNORE || ANVIL_CONTRACT_MODE > ANVIL_CONTRACT_GSL_PREDICATE
	#error Invalid contract mode
#endif

#if ANVIL_DEBUG_CONTRACT_MODE < ANVIL_CONTRACT_IGNORE || ANVIL_DEBUG_CONTRACT_MODE > ANVIL_CONTRACT_GSL_PREDICATE
	#error Invalid debug contract mode
#endif

#if ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_IGNORE
	#define ANVIL_CONTRACT(CONDITION,MESSAGE)
#elif ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_ASSUME
	#define ANVIL_CONTRACT(CONDITION,MESSAGE) ANVIL_ASSUME(CONDITION)
#elif ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_EXCEPTION
	#include <stdexcept>
	#define ANVIL_CONTRACT(CONDITION,MESSAGE) if(! (CONDITION)) throw std::runtime_error(MESSAGE);
#elif ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_STD_ERR
	#include <iostream>
	#define ANVIL_CONTRACT(CONDITION,MESSAGE) if(! (CONDITION)) std::cerr << MESSAGE << std::endl;
#elif ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_CPP_ASSERT
	#include <cassert>
	#define ANVIL_CONTRACT(CONDITION,MESSAGE) assert(CONDITION)
#elif ANVIL_CONTRACT_MODE == ANVIL_CONTRACT_GSL_PREDICATE
	#include <gsl\gsl_assert.hpp>
	#define ANVIL_CONTRACT(CONDITION,MESSAGE) Expects(CONDITION)
#endif

#if _DEBUG
	#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) ANVIL_CONTRACT(CONDITION,MESSAGE)
#else
	#if ANVIL_DEBUG_CONTRACT_MODE == ANVIL_CONTRACT_IGNORE
		#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE)
	#elif ANVIL_DEBUG_CONTRACT_MODE == ANVIL_CONTRACT_ASSUME
		#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) ANVIL_ASSUME(CONDITION)
	#elif ANVIL_DEBUG_CONTRACT_MODE == ANVIL_CONTRACT_EXCEPTION
		#include <stdexcept>
		#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) if(! (CONDITION)) throw std::runtime_error(MESSAGE);
	#elif ANVIL_DEBUG_CONTRACT_MODE == ANVIL_CONTRACT_STD_ERR
		#include <iostream>
		#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) if(! (CONDITION)) std::cerr << MESSAGE << std::endl;
	#elif ANVIL_DEBUG_CONTRACT_MODE == ANVIL_CONTRACT_CPP_ASSERT
		#include <cassert>
		#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) assert(CONDITION)
	#elif ANVIL_DEBUG_CONTRACT_MODE == ANVIL_CONTRACT_GSL_PREDICATE
		#include <gsl\gsl_assert.hpp>
		#define ANVIL_DEBUG_CONTRACT(CONDITION,MESSAGE) Expects(CONDITION)
	#endif
#endif

#endif
