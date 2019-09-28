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

#include <cstddef>
#include "anvil/byte-pipe/BytePipeWriter.hpp"
#include "anvil/lutils/Assert.hpp"

namespace anvil { namespace BytePipe {

	enum PrimaryID : uint8_t {
		ID1_NULL,
		ID1_PRIMATIVE,
		ID1_STRING,
		ID1_ARRAY,
		ID1_OBJECT,
		ID1_USER_POD
	};

	enum SecondaryID : uint8_t {
		ID2_NULL,
		ID2_U8,
		ID2_U16,
		ID2_U32,
		ID2_U64,
		ID2_S8,
		ID2_S16,
		ID2_S32,
		ID2_S64,
		ID2_F32,
		ID2_F64,
		ID2_C8,
		ID2_F16
	};

	// Header definitions
#pragma pack(push, 1)
	struct PipeHeader {
		uint8_t version;
	};

	struct ValueHeader {
		union {
			struct {
				uint8_t primary_id : 4;
				uint8_t secondary_id : 4;
			};
			uint8_t id_union;
		};
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
				char c8;
				half f16;
			} primative_v1;

			struct {
				uint16_t extended_secondary_id;
				uint32_t bytes;
			} user_pod;
		};
	};
#pragma pack(pop)

	// Compile-time error checks

	static_assert(sizeof(PipeHeader) == 1u, "PipeHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader) == 9u, "ValueHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader::user_pod) == 6u, "ValueHeader was not packed correctly by compiler");
	static_assert(offsetof(ValueHeader, primative_v1.u8) == 1u, "ValueHeader was not packed correctly by compiler");

	// Misc

	class ParserV2ToV1Converter final : public ParserV2 {
	private:
		ParserV1& _parser;
	public:
		ParserV2ToV1Converter(ParserV1& parser) :
			_parser(parser)
		{}

		virtual ~ParserV2ToV1Converter() {

		}

		// Inherited from ParserV1
		
		void OnPipeOpen() final {
			_parser.OnPipeOpen();
		}

		void OnPipeClose() final {
			_parser.OnPipeClose();
		}

		void OnArrayBegin(const uint32_t size) final {
			_parser.OnArrayBegin(size);
		}

		void OnArrayEnd() final {
			_parser.OnArrayEnd();
		}

		void OnObjectBegin(const uint32_t component_count) final {
			_parser.OnObjectBegin(component_count);
		}

		void OnObjectEnd() final {
			_parser.OnObjectEnd();
		}

		void OnComponentID(const uint16_t id) final {
			_parser.OnComponentID(id);
		}

		void OnNull() final {
			_parser.OnNull();
		}

		void OnPrimativeF64(const double value) final {
			_parser.OnPrimativeF64(value);
		}

		void OnPrimativeString(const char* value, const uint32_t length) final {
			_parser.OnPrimativeString(value, length);
		}

		void OnPrimativeU64(const uint64_t value) final {
			_parser.OnPrimativeU64(value);
		}

		void OnPrimativeS64(const int64_t value) final {
			_parser.OnPrimativeS64(value);
		}

		void OnPrimativeF32(const float value) final {
			_parser.OnPrimativeF32(value);
		}

		void OnPrimativeU8(const uint8_t value) final {
			_parser.OnPrimativeU8(value);
		}

		void OnPrimativeU16(const uint16_t value) final {
			_parser.OnPrimativeU16(value);
		}

		void OnPrimativeU32(const uint32_t value) final {
			_parser.OnPrimativeU32(value);
		}

		void OnPrimativeS8(const int8_t value) final {
			_parser.OnPrimativeS8(value);
		}

		void OnPrimativeS16(const int16_t value) final {
			_parser.OnPrimativeS16(value);
		}

		void OnPrimativeS32(const int32_t value) final {
			_parser.OnPrimativeS32(value);
		}

		void OnPrimativeC8(const char value) final {
			_parser.OnPrimativeC8(value);
		}

		void OnPrimativeF16(const half value) final {
			_parser.OnPrimativeF16(value);
		}

		// Inherited from ParserV2

		void OnPrimativeArrayU8(const uint8_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const uint8_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeU8(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayU16(const uint16_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const uint16_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeU16(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayU32(const uint32_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const uint32_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeU32(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayU64(const uint64_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const uint64_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeU64(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayS8(const int8_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const int8_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeS8(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayS16(const int16_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const int16_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeS16(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayS32(const int32_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const int32_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeS32(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayS64(const int64_t* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const int64_t* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeS64(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayF32(const float* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const float* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeF32(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayF64(const double* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const double* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeF64(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayC8(const char* src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const char* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeC8(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}

		void OnPrimativeArrayF16(const half *src, const uint32_t size) final {
			// Decompose into V1 calls
			_parser.OnArrayBegin(size);
			const half* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeF16(*src);
				++src;
			}
			_parser.OnArrayEnd();
		}
	};



	class ParserV3ToV2Converter final : public ParserV3 {
	private:
		ParserV2& _parser;
	public:
		ParserV3ToV2Converter(ParserV2& parser) :
			_parser(parser)
		{}

		virtual ~ParserV3ToV2Converter() {

		}

		// Inherited from ParserV1

		void OnPipeOpen() final {
			_parser.OnPipeOpen();
		}

		void OnPipeClose() final {
			_parser.OnPipeClose();
		}

		void OnArrayBegin(const uint32_t size) final {
			_parser.OnArrayBegin(size);
		}

		void OnArrayEnd() final {
			_parser.OnArrayEnd();
		}

		void OnObjectBegin(const uint32_t component_count) final {
			_parser.OnObjectBegin(component_count);
		}

		void OnObjectEnd() final {
			_parser.OnObjectEnd();
		}

		void OnComponentID(const uint16_t id) final {
			_parser.OnComponentID(id);
		}

		void OnNull() final {
			_parser.OnNull();
		}

		void OnPrimativeF64(const double value) final {
			_parser.OnPrimativeF64(value);
		}

		void OnPrimativeString(const char* value, const uint32_t length) final {
			_parser.OnPrimativeString(value, length);
		}

		void OnPrimativeU64(const uint64_t value) final {
			_parser.OnPrimativeU64(value);
		}

		void OnPrimativeS64(const int64_t value) final {
			_parser.OnPrimativeS64(value);
		}

		void OnPrimativeF32(const float value) final {
			_parser.OnPrimativeF32(value);
		}

		void OnPrimativeU8(const uint8_t value) final {
			_parser.OnPrimativeU8(value);
		}

		void OnPrimativeU16(const uint16_t value) final {
			_parser.OnPrimativeU16(value);
		}

		void OnPrimativeU32(const uint32_t value) final {
			_parser.OnPrimativeU32(value);
		}

		void OnPrimativeS8(const int8_t value) final {
			_parser.OnPrimativeS8(value);
		}

		void OnPrimativeS16(const int16_t value) final {
			_parser.OnPrimativeS16(value);
		}

		void OnPrimativeS32(const int32_t value) final {
			_parser.OnPrimativeS32(value);
		}

		void OnPrimativeC8(const char value) final {
			_parser.OnPrimativeC8(value);
		}

		void OnPrimativeF16(const half value) final {
			_parser.OnPrimativeF16(value);
		}

		// Inherited from ParserV2

		void OnPrimativeArrayU8(const uint8_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayU8(src, size);
		}

		void OnPrimativeArrayU16(const uint16_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayU16(src, size);
		}

		void OnPrimativeArrayU32(const uint32_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayU32(src, size);
		}

		void OnPrimativeArrayU64(const uint64_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayU64(src, size);
		}

		void OnPrimativeArrayS8(const int8_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayS8(src, size);
		}

		void OnPrimativeArrayS16(const int16_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayS16(src, size);
		}

		void OnPrimativeArrayS32(const int32_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayS32(src, size);
		}

		void OnPrimativeArrayS64(const int64_t* src, const uint32_t size) final {
			_parser.OnPrimativeArrayS64(src, size);
		}

		void OnPrimativeArrayF32(const float* src, const uint32_t size) final {
			_parser.OnPrimativeArrayF32(src, size);
		}

		void OnPrimativeArrayF64(const double* src, const uint32_t size) final {
			_parser.OnPrimativeArrayF64(src, size);
		}

		void OnPrimativeArrayC8(const char* src, const uint32_t size) final {
			_parser.OnPrimativeArrayC8(src, size);
		}

		void OnPrimativeArrayF16(const half *src, const uint32_t size) final {
			_parser.OnPrimativeArrayF16(src, size);
		}

		// Inherited from ParserV3
		
		void OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) final {
			// Decompose to version 2 calls
			_parser.OnPrimativeArrayU8(static_cast<const uint8_t*>(data), bytes);
		}
	};

	// Writer

	Writer::Writer(OutputPipe& pipe, Version version) :
		_pipe(pipe),
		_default_state(STATE_CLOSED),
		_version(version)
	{}

	Writer::Writer(OutputPipe& pipe) :
		Writer(pipe, VERSION_3)
	{}

	Writer::~Writer() {
		_pipe.Flush();
	}

	void Writer::Write(const void* src, const uint32_t bytes) {
		const uint32_t bytesWritten = _pipe.WriteBytes(src, bytes);
		ANVIL_CONTRACT(bytesWritten == bytes, "Failed to write to pipe");
	}

	Writer::State Writer::GetCurrentState() const {
		return _state_stack.empty() ? _default_state : _state_stack.back();
	}

	void Writer::OnPipeOpen() {
		ANVIL_CONTRACT(_default_state == STATE_CLOSED, "BytePipe was already open");
		_default_state = STATE_NORMAL;

		PipeHeader header;
		header.version = GetSupportedVersion();
		Write(&header, 1u);
	}

	void Writer::OnPipeClose() {
		ANVIL_CONTRACT(_default_state == STATE_NORMAL, "BytePipe was already closed");
		_default_state = STATE_CLOSED;

		uint8_t terminator = 0u;
		Write(&terminator, 1u);
	}

	void Writer::OnArrayBegin(const uint32_t size) {
		_state_stack.push_back(STATE_ARRAY);

		ValueHeader header;
		header.primary_id = ID1_ARRAY;
		header.secondary_id = ID2_NULL;
		header.array_v1.size = size;
		Write(&header, sizeof(ValueHeader::array_v1) + 1u);
	}

	void Writer::OnArrayEnd() {
		ANVIL_CONTRACT(GetCurrentState() == STATE_ARRAY, "BytePipe was not in array mode");
		_state_stack.pop_back();
	}

	void Writer::OnObjectBegin(const uint32_t components) {
		_state_stack.push_back(STATE_OBJECT);

		ValueHeader header;
		header.primary_id = ID1_OBJECT;
		header.secondary_id = ID2_NULL;
		header.object_v1.components = components;
		Write(&header, sizeof(ValueHeader::object_v1) + 1u);
	}

	void Writer::OnObjectEnd() {
		ANVIL_CONTRACT(GetCurrentState() == ID1_OBJECT, "BytePipe was not in object mode");
		_state_stack.pop_back();
	}

	void Writer::OnComponentID(const uint16_t id) {
		ANVIL_CONTRACT(GetCurrentState() == ID1_OBJECT, "BytePipe was not in object mode");
		Write(&id, 2u);
	}

	void Writer::OnNull() {
		ValueHeader header;
		header.primary_id = ID1_PRIMATIVE;
		header.secondary_id = ID2_NULL;
		Write(&header, 1u);
	}

	void Writer::_OnPrimative(const uint64_t value, uint32_t bytes, const uint8_t id) {
		ValueHeader header;
		header.primary_id = ID1_PRIMATIVE;
		header.secondary_id = id;
		header.primative_v1.u64 = value;
		Write(&header, bytes + 1u);
	}

	void Writer::OnPrimativeU8(const uint8_t value) {
		union { uint64_t u64; uint8_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 1u, ID2_U8);
	}

	void Writer::OnPrimativeU16(const uint16_t value) {
		union { uint64_t u64; uint16_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 2u, ID2_U16);
	}

	void Writer::OnPrimativeU32(const uint32_t value) {
		union { uint64_t u64; uint32_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, ID2_U32);
	}

	void Writer::OnPrimativeU64(const uint64_t value) {
		_OnPrimative(value, 8u, ID2_U64);
	}

	void Writer::OnPrimativeS8(const int8_t value) {
		union { uint64_t u64; int8_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 1u, ID2_S8);
	}

	void Writer::OnPrimativeS16(const int16_t value) {
		union { uint64_t u64; int16_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 2u, ID2_S16);
	}

	void Writer::OnPrimativeS32(const int32_t value) {
		union { uint64_t u64; int32_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, ID2_S32);
	}

	void Writer::OnPrimativeS64(const int64_t value) {
		union { uint64_t u64; int64_t val; };
		val = value;
		_OnPrimative(u64, 8u, ID2_S64);
	}

	void Writer::OnPrimativeF32(const float value) {
		union { uint64_t u64; float val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, ID2_F32);
	}

	void Writer::OnPrimativeF64(const double value) {
		union { uint64_t u64; double val; };
		val = value;
		_OnPrimative(u64, 8u, ID2_F64);
	}

	void Writer::OnPrimativeC8(const char value) {
		union { uint64_t u64; char val; };
		val = value;
		_OnPrimative(u64, 1u, ID2_C8);
	}

	void Writer::OnPrimativeF16(const half value) {
		union { uint64_t u64; half val; };
		val = value;
		_OnPrimative(u64, 2u, ID2_F16);
	}

	void Writer::OnPrimativeString(const char* value, const uint32_t length) {
		ValueHeader header;
		header.primary_id = ID1_STRING;
		header.secondary_id = ID2_C8;
		header.string_v1.length = length;
		Write(&header, sizeof(ValueHeader::string_v1) + 1u);
		Write(value, length);
	}

	void Writer::_OnPrimativeArray(const void* ptr, const uint32_t size, const uint8_t id, const uint32_t element_bytes) {
		ValueHeader header;
		header.primary_id = ID1_ARRAY;
		header.secondary_id = id;
		header.array_v1.size = size;
		Write(&header, sizeof(ValueHeader::array_v1) + 1u);
		ANVIL_ASSUME(element_bytes <= 8u);
		Write(ptr, size * element_bytes);
	}

	void Writer::OnPrimativeArrayU8(const uint8_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU8(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_U8, sizeof(uint8_t));
		}
	}

	void Writer::OnPrimativeArrayU16(const uint16_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU16(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_U16, sizeof(uint16_t));
		}
	}

	void Writer::OnPrimativeArrayU32(const uint32_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU32(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_U32, sizeof(uint32_t));
		}
	}

	void Writer::OnPrimativeArrayU64(const uint64_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU64(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_U64, sizeof(uint32_t));
		}
	}

	void Writer::OnPrimativeArrayS8(const int8_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS8(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_S8, sizeof(int8_t));
		}
	}

	void Writer::OnPrimativeArrayS16(const int16_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS16(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_S16, sizeof(int16_t));
		}
	}

	void Writer::OnPrimativeArrayS32(const int32_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS32(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_S32, sizeof(int32_t));
		}
	}

	void Writer::OnPrimativeArrayS64(const int64_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS64(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_S64, sizeof(int64_t));
		}
	}

	void Writer::OnPrimativeArrayF32(const float* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayF32(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_F32, sizeof(float));
		}
	}

	void Writer::OnPrimativeArrayF64(const double* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayF64(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_F64, sizeof(double));
		}
	}

	void Writer::OnPrimativeArrayC8(const char* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayC8(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_C8, sizeof(char));
		}
	}

	void Writer::OnPrimativeArrayF16(const half* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayF16(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID2_F16, sizeof(half));
		}
	}

	void Writer::OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) {
		ANVIL_CONTRACT(type <= 1048575u, "Type must be <= 1048575u");
		ValueHeader header;
		header.primary_id = ID1_USER_POD;
		header.secondary_id = type & 15u;
		header.user_pod.extended_secondary_id = static_cast<uint16_t>(type >> 4u);
		header.user_pod.bytes = bytes;
		Write(&header, sizeof(ValueHeader::user_pod) + 1u);
		Write(data, bytes);
	}

	// Reader

	static inline void ReadFromPipe(InputPipe& pipe, void* dst, const uint32_t bytes) {
		const uint32_t bytesRead = pipe.ReadBytes(dst, bytes);
		ANVIL_CONTRACT(bytesRead == bytes, "Failed to read from pipe");
	}

	class ReadHelper {
	private:
		InputPipe& _pipe;
		Parser& _parser;
		void (ReadHelper::*_read_generic)();
		void (ReadHelper::*_read_primative)();
		void (ReadHelper::*_read_array)();
		void (ReadHelper::*_read_object)();
		void* _mem;
		uint32_t _mem_bytes;

		void* AllocateMemory(const uint32_t bytes) {
			if (_mem_bytes < bytes) {
				if (_mem) operator delete(_mem);
				_mem = operator new(bytes);
				ANVIL_CONTRACT(_mem != nullptr, "Failed to allocate memory");
			}
			return _mem;
		}

		inline void ReadGeneric() {
			(this->*_read_generic)();
		}

		inline void ReadPrimative() {
			(this->*_read_primative)();
		}

		inline void ReadArray() {
			(this->*_read_array)();
		}

		inline void ReadObject() {
			(this->*_read_object)();
		}

		void ReadPrimativeV1() {
			ParserV1& parser = static_cast<ParserV1&>(_parser);

			switch (header.primary_id) {
			case ID2_NULL:
				parser.OnNull();
				break;
			case ID2_U8:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(uint8_t));
				parser.OnPrimativeU8(header.primative_v1.u8);
				break;
			case ID2_U16:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(uint16_t));
				parser.OnPrimativeU16(header.primative_v1.u16);
				break;
			case ID2_U32:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(uint32_t));
				parser.OnPrimativeU32(header.primative_v1.u16);
				break;
			case ID2_U64:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(uint64_t));
				parser.OnPrimativeU64(header.primative_v1.u64);
				break;
			case ID2_S8:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(int8_t));
				parser.OnPrimativeS8(header.primative_v1.s8);
				break;
			case ID2_S16:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(int32_t));
				parser.OnPrimativeS16(header.primative_v1.s16);
				break;
			case ID2_S32:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(int32_t));
				parser.OnPrimativeS32(header.primative_v1.s16);
				break;
			case ID2_S64:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(int64_t));
				parser.OnPrimativeS64(header.primative_v1.s64);
				break;
			case ID2_F32:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(float));
				parser.OnPrimativeF32(header.primative_v1.f32);
				break;
			case ID2_F64:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(double));
				parser.OnPrimativeF64(header.primative_v1.f64);
				break;
			case ID2_C8:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(char));
				parser.OnPrimativeC8(header.primative_v1.c8);
				break;
			case ID2_F16:
				ReadFromPipe(_pipe, &header.primative_v1, sizeof(half));
				parser.OnPrimativeF16(header.primative_v1.f16);
				break;
			default:
				ANVIL_CONTRACT(false, "Invalid value ID");
				break;
			}
		}

		void ReadGenericV1() {
			ParserV1& parser = static_cast<ParserV1&>(_parser);

			switch (header.primary_id) {
			case ID1_NULL:
				break;
			case ID1_STRING:
				ANVIL_CONTRACT(header.secondary_id == ID2_C8, "String subtype was not char");
				ReadFromPipe(_pipe, &header.string_v1, sizeof(header.string_v1));
				{
					const uint32_t len = header.string_v1.length;
					char* const buffer = static_cast<char*>(AllocateMemory(len + 1u));
					ReadFromPipe(_pipe, buffer, len);
					buffer[len] = '\0';
					parser.OnPrimativeString(buffer, len);
				}
				break;
			case ID1_ARRAY:
				ReadFromPipe(_pipe, &header.array_v1, sizeof(header.array_v1));
				ReadArray();
				break;
			case ID1_OBJECT:
				ReadFromPipe(_pipe, &header.object_v1, sizeof(header.object_v1));
				ReadObject();
				break;
			default:
				ReadPrimative();
				break;
			}
		}

		void ReadArrayV1() {
			ParserV1& parser = static_cast<ParserV1&>(_parser);

			const uint32_t size = header.array_v1.size;
			parser.OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) {
				ReadFromPipe(_pipe, &header, 1u);
				ReadGeneric();
			}
			parser.OnArrayEnd();
		}

		void ReadObjectV1() {
			ParserV1& parser = static_cast<ParserV1&>(_parser);

			const uint32_t size = header.object_v1.components;
			parser.OnObjectBegin(size);
			uint16_t component_id;
			for (uint32_t i = 0u; i < size; ++i) {
				ReadFromPipe(_pipe, &component_id, sizeof(component_id));
				parser.OnComponentID(component_id);
				ReadFromPipe(_pipe, &header, 1u);
				ReadGeneric();
			}
			parser.OnObjectEnd();
		}

		void ReadArrayV2() {
			ParserV2& parser = static_cast<ParserV2&>(_parser);

			const uint32_t id = header.secondary_id;
			if (id == ID2_NULL) {
				ReadArrayV1();
			} else {
				ANVIL_CONTRACT(id <= ID2_F16, "Unknown secondary type ID");

				const uint32_t size = header.array_v1.size;
				uint32_t bytes = 0u;
				void* buffer = nullptr;
				typedef void(ParserV2::*ParserCallback)(const void* ptr, const uint32_t size);
				ParserCallback callback = nullptr;

				// 0 indexed jump table
				switch (id - 1u) {
				case ID2_U8 - 1u:
					bytes = sizeof(uint8_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayU8);
					break;
				case ID2_U16 - 1u:
					bytes = sizeof(uint16_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayU16);
					break;
				case ID2_U32 - 1u:
					bytes = sizeof(uint32_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayU32);
					break;
				case ID2_U64 - 1u:
					bytes = sizeof(uint64_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayU64);
					break;
				case ID2_S8 - 1u:
					bytes = sizeof(int8_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayS8);
					break;
				case ID2_S16 - 1u:
					bytes = sizeof(int16_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayS16);
					break;
				case ID2_S32 - 1u:
					bytes = sizeof(int32_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayS32);
					break;
				case ID2_S64 - 1u:
					bytes = sizeof(int64_t);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayS64);
					break;
				case ID2_F32 - 1u:
					bytes = sizeof(float);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayF32);
					break;
				case ID2_F64 - 1u:
					bytes = sizeof(double);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayF64);
					break;
				case ID2_C8 - 1u:
					bytes = sizeof(char);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayC8);
					break;
				case ID2_F16 - 1u:
					bytes = sizeof(half);
					callback = reinterpret_cast<ParserCallback>(&ParserV2::OnPrimativeArrayF16);
					break;
				default:
					ANVIL_ASSUME_IMPOSSIBLE;
					break;
				}

				bytes *= size;
				buffer = AllocateMemory(bytes);
				ReadFromPipe(_pipe, buffer, bytes);
				(parser.*callback)(buffer, size);
			}
		}

		void ReadGenericV3() {
			ParserV3& parser = static_cast<ParserV3&>(_parser);
			if (header.primary_id == ID1_USER_POD) {
				uint32_t id = header.user_pod.extended_secondary_id;
				id <<= 4u;
				id |= header.secondary_id;
				void* mem = AllocateMemory(header.user_pod.bytes);
				ReadFromPipe(_pipe, mem, header.user_pod.bytes);
				parser.OnUserPOD(id, header.user_pod.bytes, mem);
			} else {
				ReadGenericV1();
			}
		}
	public:
		ValueHeader header;

		ReadHelper(InputPipe& pipe, Parser& parser, Version version) :
			_pipe(pipe),
			_parser(parser),
			_mem(nullptr),
			_mem_bytes(0u)
		{
			_read_generic = &ReadHelper::ReadGenericV1;
			_read_primative = &ReadHelper::ReadPrimativeV1;
			_read_array = &ReadHelper::ReadArrayV1;
			_read_object = &ReadHelper::ReadObjectV1;

			if (version >= VERSION_2) {
				_read_array = &ReadHelper::ReadArrayV2;
				if (version >= VERSION_3) {
					_read_generic = &ReadHelper::ReadGenericV3;
				}
			}
		}

		~ReadHelper() {
			if (_mem) operator delete(_mem);
		}

		void Read() {
			// Continue with read
			ReadFromPipe(_pipe, &header.id_union, 1u);
			while (header.id_union != ID1_NULL) {
				ReadGeneric();
				ReadFromPipe(_pipe, &header.id_union, 1u);
			}
		}
	};

	Reader::Reader(InputPipe& pipe) :
		_pipe(pipe)
	{}

	Reader::~Reader() {

	}

	static void Read2(Parser& dst, InputPipe& pipe, const Version pipe_version) {
		const Version parser_version = dst.GetSupportedVersion();
		if (parser_version < parser_version) {
			// Upgrade the parser by one version and recursively call this function again
			switch (parser_version) {
			case VERSION_1: {
				ParserV2ToV1Converter converter(static_cast<ParserV1&>(dst));
				Read2(converter, pipe, pipe_version);
				} break;
			case VERSION_2: {
				ParserV3ToV2Converter converter(static_cast<ParserV2&>(dst));
				Read2(converter, pipe, pipe_version);
				} break;
			case VERSION_3:
				ANVIL_ASSUME_IMPOSSIBLE;
				break;
			}
		} else {
	READ_WITHOUT_UPGRADE:
			ReadHelper helper(pipe, dst, pipe_version);
			helper.Read();
		}
	}

	void Reader::Read(Parser& dst) {
		PipeHeader pipeHeader;
		ReadFromPipe(_pipe, &pipeHeader, sizeof(PipeHeader));

		ANVIL_CONTRACT(pipeHeader.version <= VERSION_3, "BytePipe version not supported");
		const Version version = static_cast<Version>(pipeHeader.version);

		// Select correct reader for pipe version
		Read2(dst, _pipe, version);
	}
}}