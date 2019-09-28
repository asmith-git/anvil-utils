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

#ifndef ANVIL_LUTILS_ARITHMETIC_HPP
#define ANVIL_LUTILS_ARITHMETIC_HPP

#include <cstdint>

namespace anvil {
	template<class T>
	static bool IsOdd(const T) throw();

	template<class T>
	static bool IsEven(const T) throw();

	// IsOdd

	template<>
	static inline bool IsOdd<uint64_t>(const uint64_t value) throw() {
		return (value & 1ull) == 1ull;
	}

	template<>
	static inline bool IsOdd<uint32_t>(const uint32_t value) throw() {
		return (value & 1u) == 1u;
	}

	template<>
	static inline bool IsOdd<uint16_t>(const uint16_t value) throw() {
		return (value & 1u) == 1u;
	}

	template<>
	static inline bool IsOdd<uint8_t>(const uint8_t value) throw() {
		return (value & 1u) == 1u;
	}

	template<>
	static inline bool IsOdd<int64_t>(const int64_t value) throw() {
		return (value & 1ll) == 1ll;
	}

	template<>
	static inline bool IsOdd<int32_t>(const int32_t value) throw() {
		return (value & 1) == 1;
	}

	template<>
	static inline bool IsOdd<int16_t>(const int16_t value) throw() {
		return (value & 1) == 1;
	}

	template<>
	static inline bool IsOdd<int8_t>(const int8_t value) throw() {
		return (value & 1) == 1;
	}

	template<>
	static inline bool IsOdd<float>(const float value) throw() {
		return IsOdd<int64_t>(static_cast<int64_t>(value));
	}

	template<>
	static inline bool IsOdd<double>(const double value) throw() {
		return IsOdd<int64_t>(static_cast<int64_t>(value));
	}

	// IsEven

	template<>
	static inline bool IsEven<uint64_t>(const uint64_t value) throw() {
		return (value & 1ull) == 0ull;
	}

	template<>
	static inline bool IsEven<uint32_t>(const uint32_t value) throw() {
		return (value & 1u) == 0u;
	}

	template<>
	static inline bool IsEven<uint16_t>(const uint16_t value) throw() {
		return (value & 1u) == 0u;
	}

	template<>
	static inline bool IsEven<uint8_t>(const uint8_t value) throw() {
		return (value & 1u) == 0u;
	}

	template<>
	static inline bool IsEven<int64_t>(const int64_t value) throw() {
		return (value & 1ll) == 0ll;
	}

	template<>
	static inline bool IsEven<int32_t>(const int32_t value) throw() {
		return (value & 1) == 0;
	}

	template<>
	static inline bool IsEven<int16_t>(const int16_t value) throw() {
		return (value & 1) == 0;
	}

	template<>
	static inline bool IsEven<int8_t>(const int8_t value) throw() {
		return (value & 1) == 0;
	}

	template<>
	static inline bool IsEven<float>(const float value) throw() {
		return IsEven<int64_t>(static_cast<int64_t>(value));
	}

	template<>
	static inline bool IsEven<double>(const double value) throw() {
		return IsEven<int64_t>(static_cast<int64_t>(value));
	}
}

#endif
