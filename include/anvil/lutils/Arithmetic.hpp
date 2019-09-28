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

	template<class T>
	static T RoundUpOdd(const T) throw();

	template<class T>
	static T RoundDownOdd(const T) throw();

	template<class T>
	static T RoundUpEven(const T) throw();

	template<class T>
	static T RoundDownEven(const T) throw();

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

	// RoundUpOdd

	template<>
	static inline uint64_t RoundUpOdd<uint64_t>(const uint64_t value) throw() {
		return value | 1ull;
	}

	template<>
	static inline uint32_t RoundUpOdd<uint32_t>(const uint32_t value) throw() {
		return value | 1u;
	}

	template<>
	static inline uint16_t RoundUpOdd<uint16_t>(const uint16_t value) throw() {
		return value | 1u;
	}

	template<>
	static inline uint8_t RoundUpOdd<uint8_t>(const uint8_t value) throw() {
		return value | 1u;
	}

	template<>
	static inline int64_t RoundUpOdd<int64_t>(const int64_t value) throw() {
		return value | 1ll;
	}

	template<>
	static inline int32_t RoundUpOdd<int32_t>(const int32_t value) throw() {
		return value | 1;
	}

	template<>
	static inline int16_t RoundUpOdd<int16_t>(const int16_t value) throw() {
		return value | 1;
	}

	template<>
	static inline int8_t RoundUpOdd<int8_t>(const int8_t value) throw() {
		return value | 1;
	}

	template<>
	static inline float RoundUpOdd<float>(const float value) throw() {
		return static_cast<float>(RoundUpOdd(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundUpOdd<double>(const double value) throw() {
		return static_cast<double>(RoundUpOdd(static_cast<int64_t>(value)));
	}

	// RoundDownOdd

	template<>
	static inline uint64_t RoundDownOdd<uint64_t>(const uint64_t value) throw() {
		enum : uint64_t { MASK = ~1ull };
		return value & MASK;
	}

	template<>
	static inline uint32_t RoundDownOdd<uint32_t>(const uint32_t value) throw() {
		enum : uint32_t { MASK = ~1u };
		return value & MASK;
	}

	template<>
	static inline uint16_t RoundDownOdd<uint16_t>(const uint16_t value) throw() {
		enum : uint16_t { MASK = ~1u };
		return value & MASK;
	}

	template<>
	static inline uint8_t RoundDownOdd<uint8_t>(const uint8_t value) throw() {
		enum : uint8_t { MASK = ~1u };
		return value & MASK;
	}

	template<>
	static inline int64_t RoundDownOdd<int64_t>(const int64_t value) throw() {
		enum : int64_t { MASK = ~1ll };
		return value & MASK;
	}

	template<>
	static inline int32_t RoundDownOdd<int32_t>(const int32_t value) throw() {
		enum : int32_t { MASK = ~1 };
		return value & MASK;
	}

	template<>
	static inline int16_t RoundDownOdd<int16_t>(const int16_t value) throw() {
		enum : int16_t { MASK = ~1 };
		return value & MASK;
	}

	template<>
	static inline int8_t RoundDownOdd<int8_t>(const int8_t value) throw() {
		enum : int8_t { MASK = ~1 };
		return value & MASK;
	}

	template<>
	static inline float RoundDownOdd<float>(const float value) throw() {
		return static_cast<float>(RoundDownOdd(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundDownOdd<double>(const double value) throw() {
		return static_cast<double>(RoundDownOdd(static_cast<int64_t>(value)));
	}

	// RoundUpEven

	template<>
	static inline uint64_t RoundUpEven<uint64_t>(const uint64_t value) throw() {
		return value + (value & 1ull);
	}

	template<>
	static inline uint32_t RoundUpEven<uint32_t>(const uint32_t value) throw() {
		return value + (value & 1u);
	}

	template<>
	static inline uint16_t RoundUpEven<uint16_t>(const uint16_t value) throw() {
		return value + (value & 1u);
	}

	template<>
	static inline uint8_t RoundUpEven<uint8_t>(const uint8_t value) throw() {
		return value + (value & 1u);
	}

	template<>
	static inline int64_t RoundUpEven<int64_t>(const int64_t value) throw() {
		return value + (value & 1ll);
	}

	template<>
	static inline int32_t RoundUpEven<int32_t>(const int32_t value) throw() {
		return value + (value & 1);
	}

	template<>
	static inline int16_t RoundUpEven<int16_t>(const int16_t value) throw() {
		return value + (value & 1);
	}

	template<>
	static inline int8_t RoundUpEven<int8_t>(const int8_t value) throw() {
		return value + (value & 1);
	}

	template<>
	static inline float RoundUpEven<float>(const float value) throw() {
		return static_cast<float>(RoundUpEven(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundUpEven<double>(const double value) throw() {
		return static_cast<double>(RoundUpEven(static_cast<int64_t>(value)));
	}

	// RoundDownEven

	template<>
	static inline uint64_t RoundDownEven<uint64_t>(const uint64_t value) throw() {
		return value - (value & 1ull);
	}

	template<>
	static inline uint32_t RoundDownEven<uint32_t>(const uint32_t value) throw() {
		return value - (value & 1u);
	}

	template<>
	static inline uint16_t RoundDownEven<uint16_t>(const uint16_t value) throw() {
		return value - (value & 1u);
	}

	template<>
	static inline uint8_t RoundDownEven<uint8_t>(const uint8_t value) throw() {
		return value - (value & 1u);
	}

	template<>
	static inline int64_t RoundDownEven<int64_t>(const int64_t value) throw() {
		return value - (value & 1ll);
	}

	template<>
	static inline int32_t RoundDownEven<int32_t>(const int32_t value) throw() {
		return value - (value & 1);
	}

	template<>
	static inline int16_t RoundDownEven<int16_t>(const int16_t value) throw() {
		return value - (value & 1);
	}

	template<>
	static inline int8_t RoundDownEven<int8_t>(const int8_t value) throw() {
		return value - (value & 1);
	}

	template<>
	static inline float RoundDownEven<float>(const float value) throw() {
		return static_cast<float>(RoundDownEven(static_cast<int64_t>(value)));
	}

	template<>
	static inline double RoundDownEven<double>(const double value) throw() {
		return static_cast<double>(RoundDownEven(static_cast<int64_t>(value)));
	}
}

#endif
