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
			} array_v1;

			struct {
				uint32_t components;
			} object_v1;

			struct {
				uint32_t length;
			} string_v1;

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
			} primative_v1;
		};
	};
#pragma pack(pop)

	// Compile-time error checks

	enum {
		ID_ID_BYTES = sizeof(ValueHeader::id),
	};

	static_assert(sizeof(PipeHeader) == 1u, "PipeHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader) == 9u, "ValueHeader was not packed correctly by compiler");
	static_assert(offsetof(ValueHeader, primative_v1.u8) == ID_ID_BYTES, "ValueHeader was not packed correctly by compiler");

	// Misc

	enum : uint8_t {
		ID_NULL,
		ID_U8,
		ID_U16,
		ID_U32,
		ID_U64,
		ID_S8,
		ID_S16,
		ID_S32,
		ID_S64,
		ID_F32,
		ID_F64,
		ID_STRING,
		ID_ARRAY,
		ID_OBJECT
	};

	// Writer

	Writer::Writer(OutputPipe& pipe) :
		_pipe(pipe),
		_default_state(STATE_CLOSED)
	{}

	Writer::~Writer() {
		_pipe.Flush();
	}

	void Writer::Write(const void* src, const uint32_t bytes) {
		const uint32_t bytesWritten = _pipe.WriteBytes(src, bytes);
		if (bytesWritten != bytes) throw std::runtime_error("Failed to write to pipe");
	}

	Writer::State Writer::GetCurrentState() const {
		return _state_stack.empty() ? _default_state : _state_stack.back();
	}

	void Writer::OnPipeOpen() {
		if (_default_state != STATE_CLOSED) throw std::runtime_error("BytePipe was already open");
		_default_state = STATE_NORMAL;

		PipeHeader header;
		header.id = GetSupportedVersion();
		Write(&header, 1u);
	}

	void Writer::OnPipeClose() {
		if (_default_state != STATE_NORMAL) throw std::runtime_error("BytePipe was already closed");
		_default_state = STATE_CLOSED;

		uint8_t terminator = 0u;
		Write(&terminator, 1u);
	}

	void Writer::OnArrayBegin(const uint32_t size) {
		_state_stack.push_back(STATE_ARRAY);

		ValueHeader header;
		header.id = ID_ARRAY;
		header.array_v1.size = size;
		Write(&header, sizeof(ValueHeader::array_v1) + 1u);
	}

	void Writer::OnArrayEnd() {
		if (GetCurrentState() != STATE_ARRAY) throw std::runtime_error("BytePipe was not in array mode");
		_state_stack.pop_back();
	}

	void Writer::OnObjectBegin(const uint32_t components) {
		_state_stack.push_back(STATE_OBJECT);

		ValueHeader header;
		header.id = ID_OBJECT;
		header.object_v1.components = components;
		Write(&header, sizeof(ValueHeader::object_v1) + 1u);
	}

	void Writer::OnObjectEnd() {
		if (GetCurrentState() != ID_OBJECT) throw std::runtime_error("BytePipe was not in object mode");
		_state_stack.pop_back();
	}

	void Writer::OnComponentID(const uint16_t id) {
		if (GetCurrentState() != ID_OBJECT) throw std::runtime_error("BytePipe was not in object mode");
		Write(&id, 2u);
	}

	void Writer::OnPrimativeU8(const uint8_t value) {
		ValueHeader header;
		header.primative_v1.u8 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.u8);
		} else {
			header.id = ID_U8;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeU16(const uint16_t value) {
		ValueHeader header;
		header.primative_v1.u16 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.u16);
		} else {
			header.id = ID_U16;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeU32(const uint32_t value) {
		ValueHeader header;
		header.primative_v1.u32 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.u32);
		} else {
			header.id = ID_U32;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeU64(const uint64_t value) {
		ValueHeader header;
		header.primative_v1.u64 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.u64);
		} else {
			header.id = ID_U64;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeS8(const int8_t value) {
		ValueHeader header;
		header.primative_v1.s8 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.s8);
		} else {
			header.id = ID_S8;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeS16(const int16_t value) {
		ValueHeader header;
		header.primative_v1.s16 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.s16);
		} else {
			header.id = ID_S16;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeS32(const int32_t value) {
		ValueHeader header;
		header.primative_v1.s32 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.s32);
		} else {
			header.id = ID_S32;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeS64(const int64_t value) {
		ValueHeader header;
		header.primative_v1.s64 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.s64);
		} else {
			header.id = ID_S64;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeF32(const float value) {
		ValueHeader header;
		header.primative_v1.f32 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.f32);
		} else {
			header.id = ID_F32;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeF64(const double value) {
		ValueHeader header;
		header.primative_v1.f64 = value;

		uint32_t bytes = sizeof(value);
		const void* ptr = &header;
		if (GetCurrentState() == STATE_ARRAY) {
			ptr = static_cast<const uint8_t*>(ptr) + offsetof(ValueHeader, primative_v1.f64);
		} else {
			header.id = ID_F64;
			bytes += ID_ID_BYTES;
		}

		Write(ptr, bytes);
	}

	void Writer::OnPrimativeString(const char* value, const uint32_t length) {
		ValueHeader header;
		header.id = ID_STRING;
		header.string_v1.length = length;
		Write(&header, sizeof(ValueHeader::string_v1) + 1u);
		Write(value, length);
	}
}}}