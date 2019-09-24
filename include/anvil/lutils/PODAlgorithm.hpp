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

#ifndef ANVIL_LUTILS_POD_ALGORITHM_HPP
#define ANVIL_LUTILS_POD_ALGORITHM_HPP

#include <cstdint>
#include <type_traits>

namespace anvil { namespace lutils {
	
	template<class F, class T>
	static void transform(T* begin, const T* const end, const F& unary_op) {
		static_assert(std::is_pod<T>::value, "type must be POD");

		while (begin != end) {
			binary_op(*begin);
			++begin;
		}
	}

	template<class F, class T>
	static void transform(const T* input_begin, const T* const input_end, T* output_begin, const F& unary_op) {
		static_assert(std::is_pod<T>::value, "type must be POD");

		const uint32_t count = static_cast<uint32_t>(input_end - input_begin);
		memcpy(output_begin, input_begin, count * sizeof(T));
		transform<F, T>(output_begin, output_begin + count, binary_op);
	}
}}

#endif
