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

#include "anvil/lutils/BytePipeWriter.hpp"
#include <cstddef>

namespace anvil { namespace lutils { namespace BytePipe {

	// Header definitions
#pragma pack(push, 1)
	struct PipeHeader {
		uint8_t version;
	};

	struct ValueHeader {
		uint8_t id;
		union {
			struct {
				uint32_t size;
			} array_v1;

			struct {
				uint32_t size;
				uint8_t secondary_id;
			} array_v2;

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
		};
	};

#define primative_v3 primative_v1
#pragma pack(pop)

	// Compile-time error checks

	enum {
		ID_ID_BYTES = sizeof(ValueHeader::id),
	};

	static_assert(sizeof(PipeHeader) == 1u, "PipeHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader) == 9u, "ValueHeader was not packed correctly by compiler");
	static_assert(offsetof(ValueHeader, primative_v1.u8) == ID_ID_BYTES, "ValueHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader::array_v2) == 5u, "ValueHeader was not packed correctly by compiler");

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
		ID_OBJECT,
		ID_C8,
		ID_F16
	};

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

		// Inherited from ParserV3

		void OnPrimativeC8(const char value) final {
			// Decompose into V2 calls
			const char tmp[2u] = { value, '\0' };
			OnPrimativeString(tmp, 1u);
		}

		void OnPrimativeF16(const half value) final {
			// Decompose into V2 calls
			_parser.OnPrimativeF32(value); //! \bug half to float conversion not implemented
		}

		void OnPrimativeArrayC8(const char* src, const uint32_t size) final {
			char* const tmp = static_cast<char*>(operator new(size + 1u));
			try {
				memcpy(tmp, src, size);
				tmp[size] = '\0';
				OnPrimativeString(tmp, size);
			} catch (...) {
				operator delete(tmp);
				throw;
			}
			operator delete(tmp);
		}

		void OnPrimativeArrayF16(const half* src, const uint32_t size) final {
			// Decompose into V2 calls
			_parser.OnArrayBegin(size);
			const half* const end = src + size;
			while (src != end) {
				_parser.OnPrimativeF32(*src); //! \bug half to float conversion not implemented
				++src;
			}
			_parser.OnArrayEnd();
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
		if (bytesWritten != bytes) throw std::runtime_error("Failed to write to pipe");
	}

	Writer::State Writer::GetCurrentState() const {
		return _state_stack.empty() ? _default_state : _state_stack.back();
	}

	void Writer::OnPipeOpen() {
		if (_default_state != STATE_CLOSED) throw std::runtime_error("BytePipe was already open");
		_default_state = STATE_NORMAL;

		PipeHeader header;
		header.version = GetSupportedVersion();
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
		if (_version == VERSION_1) {
			Write(&header, sizeof(ValueHeader::array_v1) + 1u);
		} else {
			header.array_v2.secondary_id = ID_NULL;
			Write(&header, sizeof(ValueHeader::array_v2) + 1u);
		}
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

	void Writer::_OnPrimative(const uint64_t value, uint32_t bytes, const uint8_t id) {
		ValueHeader header;
		header.id = id;
		header.primative_v1.u64 = value;
		Write(&header, bytes + 1u);
	}

	void Writer::OnPrimativeU8(const uint8_t value) {
		union { uint64_t u64; uint8_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 1u, ID_U8);
	}

	void Writer::OnPrimativeU16(const uint16_t value) {
		union { uint64_t u64; uint16_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 2u, ID_U16);
	}

	void Writer::OnPrimativeU32(const uint32_t value) {
		union { uint64_t u64; uint32_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, ID_U32);
	}

	void Writer::OnPrimativeU64(const uint64_t value) {
		_OnPrimative(value, 8u, ID_U64);
	}

	void Writer::OnPrimativeS8(const int8_t value) {
		union { uint64_t u64; int8_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 1u, ID_S8);
	}

	void Writer::OnPrimativeS16(const int16_t value) {
		union { uint64_t u64; int16_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 2u, ID_S16);
	}

	void Writer::OnPrimativeS32(const int32_t value) {
		union { uint64_t u64; int32_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, ID_S32);
	}

	void Writer::OnPrimativeS64(const int64_t value) {
		union { uint64_t u64; int64_t val; };
		val = value;
		_OnPrimative(u64, 8u, ID_S64);
	}

	void Writer::OnPrimativeF32(const float value) {
		union { uint64_t u64; float val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, ID_F32);
	}

	void Writer::OnPrimativeF64(const double value) {
		union { uint64_t u64; double val; };
		val = value;
		_OnPrimative(u64, 8u, ID_F64);
	}

	void Writer::OnPrimativeString(const char* value, const uint32_t length) {
		ValueHeader header;
		header.id = ID_STRING;
		header.string_v1.length = length;
		Write(&header, sizeof(ValueHeader::string_v1) + 1u);
		Write(value, length);
	}

	void Writer::_OnPrimativeArray(const void* ptr, const uint32_t size, const uint8_t id, const uint32_t element_bytes) {
		ValueHeader header;
		header.id = ID_ARRAY;
		header.array_v2.size = size;
		header.array_v2.secondary_id = id;
		Write(&header, sizeof(ValueHeader::array_v2) + 1u);
		__assume(element_bytes <= 8u);
		Write(ptr, size * element_bytes);
	}

	void Writer::OnPrimativeArrayU8(const uint8_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU8(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_U8, sizeof(uint8_t));
		}
	}

	void Writer::OnPrimativeArrayU16(const uint16_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU16(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_U16, sizeof(uint16_t));
		}
	}

	void Writer::OnPrimativeArrayU32(const uint32_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU32(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_U32, sizeof(uint32_t));
		}
	}

	void Writer::OnPrimativeArrayU64(const uint64_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayU64(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_U64, sizeof(uint32_t));
		}
	}

	void Writer::OnPrimativeArrayS8(const int8_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS8(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_S8, sizeof(int8_t));
		}
	}

	void Writer::OnPrimativeArrayS16(const int16_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS16(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_S16, sizeof(int16_t));
		}
	}

	void Writer::OnPrimativeArrayS32(const int32_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS32(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_S32, sizeof(int32_t));
		}
	}

	void Writer::OnPrimativeArrayS64(const int64_t* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayS64(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_S64, sizeof(int64_t));
		}
	}

	void Writer::OnPrimativeArrayF32(const float* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayF32(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_F32, sizeof(float));
		}
	}

	void Writer::OnPrimativeArrayF64(const double* ptr, const uint32_t size) {
		if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			parser.OnPrimativeArrayF64(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_F64, sizeof(double));
		}
	}

	void Writer::OnPrimativeArrayC8(const char* ptr, const uint32_t size) {
		if (_version == VERSION_2) {
			ParserV3ToV2Converter parser(*this);
			parser.OnPrimativeArrayC8(ptr, size);
		} else if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			ParserV3ToV2Converter parser2(parser);
			parser2.OnPrimativeArrayC8(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_C8, sizeof(char));
		}
	}

	void Writer::OnPrimativeArrayF16(const half* ptr, const uint32_t size) {
		if (_version == VERSION_2) {
			ParserV3ToV2Converter parser(*this);
			parser.OnPrimativeArrayF16(ptr, size);
		} else if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			ParserV3ToV2Converter parser2(parser);
			parser2.OnPrimativeArrayF16(ptr, size);
		} else {
			_OnPrimativeArray(ptr, size, ID_F16, sizeof(half));
		}
	}

	void Writer::OnPrimativeC8(const char value) {
		if (_version == VERSION_2) {
			ParserV3ToV2Converter parser(*this);
			parser.OnPrimativeC8(value);
		} else if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			ParserV3ToV2Converter parser2(parser);
			parser2.OnPrimativeC8(value);
		} else {
			union { uint64_t u64; char val; };
			u64 = 0u;
			val = value;
			_OnPrimative(u64, 1u, ID_C8);
		}
	}

	void Writer::OnPrimativeF16(const half value) {
		if (_version == VERSION_2) {
			ParserV3ToV2Converter parser(*this);
			parser.OnPrimativeF16(value);
		} else if (_version == VERSION_1) {
			ParserV2ToV1Converter parser(*this);
			ParserV3ToV2Converter parser2(parser);
			parser2.OnPrimativeF16(value);
		} else {
			union { uint64_t u64; half val; };
			u64 = 0u;
			val = value;
			_OnPrimative(u64, 2u, ID_F16);
		}
	}

	// Reader

	static void Read(InputPipe& pipe, void* dst, const uint32_t bytes) {
		const uint32_t bytesRead = pipe.ReadBytes(dst, bytes);
		if (bytesRead != bytes) throw std::runtime_error("Failed to read from pipe");
	}

	static void ReadGeneric(ValueHeader& header, InputPipe& pipe, ParserV3& parser, const Version version);
	static void ReadArray(ValueHeader& header, InputPipe& pipe, ParserV3& parser, const Version version);

	static void ReadObject(ValueHeader& header, InputPipe& pipe, ParserV3& parser, const Version version) {
		const uint32_t size = header.object_v1.components;
		parser.OnObjectBegin(size);
		uint16_t component_id;
		for (uint32_t i = 0u; i < size; ++i) {
			Read(pipe, reinterpret_cast<char*>(&component_id), sizeof(component_id));
			parser.OnComponentID(component_id);
			Read(pipe, &header, 1u);
			ReadGeneric(header, pipe, parser, version);
		}
		parser.OnObjectEnd();
	}

	void ReadArray(ValueHeader& header, InputPipe& pipe, ParserV3& parser, const Version version) {
		const uint32_t size = header.array_v1.size;
		
		if (version == VERSION_1) {
	VERSION_1_ARRAY:
			parser.OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) {
				Read(pipe, &header, 1u);
				ReadGeneric(header, pipe, parser, version);
			}
			parser.OnArrayEnd();
		} else {
			uint32_t bytes = 0u;
			void* buffer = nullptr;

			switch (header.array_v2.secondary_id) {
			case ID_U8:
				bytes = size * sizeof(uint8_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayU8(static_cast<uint8_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_U16:
				bytes = size * sizeof(uint16_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayU16(static_cast<uint16_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_U32:
				bytes = size * sizeof(uint32_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayU32(static_cast<uint32_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_U64:
				bytes = size * sizeof(uint64_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayU64(static_cast<uint64_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_S8:
				bytes = size * sizeof(int8_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayS8(static_cast<int8_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_S16:
				bytes = size * sizeof(int16_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayS16(static_cast<int16_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_S32:
				bytes = size * sizeof(int32_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayS32(static_cast<int32_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_S64:
				bytes = size * sizeof(int64_t);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayS64(static_cast<int64_t*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_F32:
				bytes = size * sizeof(float);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayF32(static_cast<float*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_F64:
				bytes = size * sizeof(double);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayF64(static_cast<double*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_C8:
				bytes = size * sizeof(char);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayC8(static_cast<char*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			case ID_F16:
				bytes = size * sizeof(half);
				buffer = operator new(bytes);
				try {
					Read(pipe, static_cast<char*>(buffer), bytes);
					parser.OnPrimativeArrayF16(static_cast<half*>(buffer), size);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				break;
			default:
				goto VERSION_1_ARRAY;
			}

			operator delete(buffer);
		}

	}

	void ReadGeneric(ValueHeader& header, InputPipe& pipe, ParserV3& parser, const Version version) {

		switch (header.id) {
		case ID_NULL:
			break;
		case ID_U8:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(uint8_t));
			parser.OnPrimativeU8(header.primative_v1.u8);
			break;
		case ID_U16:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(uint16_t));
			parser.OnPrimativeU16(header.primative_v1.u16);
			break;
		case ID_U32:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(uint32_t));
			parser.OnPrimativeU32(header.primative_v1.u16);
			break;
		case ID_U64:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(uint64_t));
			parser.OnPrimativeU64(header.primative_v1.u64);
			break;
		case ID_S8:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(int8_t));
			parser.OnPrimativeS8(header.primative_v1.s8);
			break;
		case ID_S16:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(int32_t));
			parser.OnPrimativeS16(header.primative_v1.s16);
			break;
		case ID_S32:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(int32_t));
			parser.OnPrimativeS32(header.primative_v1.s16);
			break;
		case ID_S64:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(int64_t));
			parser.OnPrimativeS64(header.primative_v1.s64);
			break;
		case ID_F32:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(float));
			parser.OnPrimativeF32(header.primative_v1.f32);
			break;
		case ID_F64:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v1), sizeof(double));
			parser.OnPrimativeF64(header.primative_v1.f64);
			break;
		case ID_STRING:
			Read(pipe, reinterpret_cast<char*>(&header.string_v1), sizeof(header.string_v1));
			{
				const uint32_t len = header.string_v1.length;
				char* const buffer = static_cast<char*>(operator new(len + 1u));
				try {
					Read(pipe, buffer, len);
					buffer[len] = '\0';
					parser.OnPrimativeString(buffer, len);
				} catch (...) {
					operator delete(buffer);
					throw;
				}
				operator delete(buffer);
			}
			break;
		case ID_ARRAY:
			Read(pipe, reinterpret_cast<char*>(&header.array_v2), version == VERSION_1 ? sizeof(header.array_v1) : sizeof(header.array_v2));
			ReadArray(header, pipe, parser, version);
			break;
		case ID_OBJECT:
			Read(pipe, reinterpret_cast<char*>(&header.object_v1), sizeof(header.object_v1));
			ReadObject(header, pipe, parser, version);
			break;
		case ID_C8:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v3), sizeof(char));
			parser.OnPrimativeC8(header.primative_v3.c8);
			break;
		case ID_F16:
			Read(pipe, reinterpret_cast<char*>(&header.primative_v3), sizeof(char));
			parser.OnPrimativeF16(header.primative_v3.f16);
			break;
		}
	}

	Reader::Reader(InputPipe& pipe) :
		_pipe(pipe)
	{}

	Reader::~Reader() {

	}

	void Reader::Read(Parser& dst) {
		if (dst.GetSupportedVersion() == VERSION_2) {
			ParserV3ToV2Converter parser(static_cast<ParserV2&>(dst));
			Read(parser);
		} else if (dst.GetSupportedVersion() == VERSION_1) {
			ParserV2ToV1Converter parser(static_cast<ParserV1&>(dst));
			Read(parser);
		} else {
			PipeHeader pipeHeader;
			BytePipe::Read(_pipe, &pipeHeader, sizeof(PipeHeader));

			if (pipeHeader.version > VERSION_3) throw std::runtime_error("BytePipe version not supported");
			const Version version = static_cast<Version>(pipeHeader.version);

			ValueHeader valueHeader;
			BytePipe::Read(_pipe, &valueHeader.id, 1u);
			while (valueHeader.id != ID_NULL) {
				ReadGeneric(valueHeader, _pipe, static_cast<ParserV3&>(dst), version);
				BytePipe::Read(_pipe, &valueHeader.id, 1u);
			}
		}
	}
}}}