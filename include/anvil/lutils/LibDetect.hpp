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

#ifndef ANVIL_LUTILS_LIBDETECT_HPP
#define ANVIL_LUTILS_LIBDETECT_HPP

// GSL
#if defined(GSL_GSL_H) || defined(GSL_UTIL_H) || defined(GSL_BYTE_H) || defined(GSL_ALGORITHM_H) || defined(GSL_STRING_SPAN_H) || defined(GSL_SPAN_H) || defined(GSL_POINTERS_H) || defined(GSL_MULTI_SPAN_H) || defined(GSL_CONTRACTS_H)
	#define ANVIL_GSL_SUPPORT 1
#else
	#define ANVIL_GSL_SUPPORT 0
#endif

#endif
