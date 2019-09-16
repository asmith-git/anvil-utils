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

#include "anvil/lutils/BytePipe.hpp"
#include <cstddef>

namespace anvil { namespace lutils { namespace BytePipe {

	// Header definitions
#pragma pack(push, 1)
	struct PipeHeader {
		uint8_t id;
	};

	struct ValueHeader {
		uint8_t id;
		union {
			struct {
				uint32_t size;
			} array_;

			struct {
				uint32_t components;
			} object;

			struct {
				uint32_t length;
			} string;

			union {
				uint8_t u8;
				uint16_t u16;
				uint32_t u32;
				uint64_t u64;
				int8_t s8;
				int16_t s16;
				int32_t s32;
				int64_t s64;
				float f32;
				double f64;
			} primative;
		};
	};
#pragma pack(pop)

	// Compile-time error checks

	enum {
		HEADER_ID_BYTES = sizeof(ValueHeader::id),
	};

	static_assert(sizeof(PipeHeader) == 1u, "PipeHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader) == 9u, "ValueHeader was not packed correctly by compiler");
	static_assert(offsetof(ValueHeader, primative.u8) == HEADER_ID_BYTES, "ValueHeader was not packed correctly by compiler");
}}}