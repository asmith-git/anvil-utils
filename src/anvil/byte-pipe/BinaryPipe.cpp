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
		PID_NULL,
		PID_PRIMATIVE,
		PID_STRING,
		PID_ARRAY,
		PID_OBJECT,
		PID_USER_POD
	};

	enum SecondaryID : uint8_t {
		SID_NULL,
		SID_U8,
		SID_U16,
		SID_U32,
		SID_U64,
		SID_S8,
		SID_S16,
		SID_S32,
		SID_S64,
		SID_F32,
		SID_F64,
		SID_C8,
		SID_F16
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

	// Helper arrays

	static constexpr const uint8_t g_secondary_type_sizes[] = {
		0u, // SID_NULL
		1u, // SID_U8
		2u, // SID_U16
		4u, // SID_U32
		8u, // SID_U64
		1u, // SID_S8
		2u, // SID_S16
		4u, // SID_S32
		8u, // SID_S64
		4u, // SID_F32
		8u, // SID_F64
		1u, // SID_C8
		2u // SID_F16
	};

	// Writer

	Writer::Writer(OutputPipe& pipe, Version version) :
		_pipe(pipe),
		_default_state(STATE_CLOSED),
		_version(version)
	{}

	Writer::Writer(OutputPipe& pipe) :
		Writer(pipe, VERSION_1)
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
		header.primary_id = PID_ARRAY;
		header.secondary_id = SID_NULL;
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
		header.primary_id = PID_OBJECT;
		header.secondary_id = SID_NULL;
		header.object_v1.components = components;
		Write(&header, sizeof(ValueHeader::object_v1) + 1u);
	}

	void Writer::OnObjectEnd() {
		ANVIL_CONTRACT(GetCurrentState() == PID_OBJECT, "BytePipe was not in object mode");
		_state_stack.pop_back();
	}

	void Writer::OnComponentID(const uint16_t id) {
		ANVIL_CONTRACT(GetCurrentState() == PID_OBJECT, "BytePipe was not in object mode");
		Write(&id, 2u);
	}

	void Writer::OnNull() {
		ValueHeader header;
		header.primary_id = PID_PRIMATIVE;
		header.secondary_id = SID_NULL;
		Write(&header, 1u);
	}

	void Writer::_OnPrimative(const uint64_t value, uint32_t bytes, const uint8_t id) {
		ValueHeader header;
		header.primary_id = PID_PRIMATIVE;
		header.secondary_id = id;
		header.primative_v1.u64 = value;
		Write(&header, bytes + 1u);
	}

	void Writer::OnPrimativeU8(const uint8_t value) {
		union { uint64_t u64; uint8_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 1u, SID_U8);
	}

	void Writer::OnPrimativeU16(const uint16_t value) {
		union { uint64_t u64; uint16_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 2u, SID_U16);
	}

	void Writer::OnPrimativeU32(const uint32_t value) {
		union { uint64_t u64; uint32_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, SID_U32);
	}

	void Writer::OnPrimativeU64(const uint64_t value) {
		_OnPrimative(value, 8u, SID_U64);
	}

	void Writer::OnPrimativeS8(const int8_t value) {
		union { uint64_t u64; int8_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 1u, SID_S8);
	}

	void Writer::OnPrimativeS16(const int16_t value) {
		union { uint64_t u64; int16_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 2u, SID_S16);
	}

	void Writer::OnPrimativeS32(const int32_t value) {
		union { uint64_t u64; int32_t val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, SID_S32);
	}

	void Writer::OnPrimativeS64(const int64_t value) {
		union { uint64_t u64; int64_t val; };
		val = value;
		_OnPrimative(u64, 8u, SID_S64);
	}

	void Writer::OnPrimativeF32(const float value) {
		union { uint64_t u64; float val; };
		u64 = 0u;
		val = value;
		_OnPrimative(u64, 4u, SID_F32);
	}

	void Writer::OnPrimativeF64(const double value) {
		union { uint64_t u64; double val; };
		val = value;
		_OnPrimative(u64, 8u, SID_F64);
	}

	void Writer::OnPrimativeC8(const char value) {
		union { uint64_t u64; char val; };
		val = value;
		_OnPrimative(u64, 1u, SID_C8);
	}

	void Writer::OnPrimativeF16(const half value) {
		union { uint64_t u64; half val; };
		val = value;
		_OnPrimative(u64, 2u, SID_F16);
	}

	void Writer::OnPrimativeString(const char* value, const uint32_t length) {
		ValueHeader header;
		header.primary_id = PID_STRING;
		header.secondary_id = SID_C8;
		header.string_v1.length = length;
		Write(&header, sizeof(ValueHeader::string_v1) + 1u);
		Write(value, length);
	}

	void Writer::_OnPrimativeArray(const void* ptr, const uint32_t size, const uint8_t id) {
		ValueHeader header;
		header.primary_id = PID_ARRAY;
		header.secondary_id = id;
		header.array_v1.size = size;
		Write(&header, sizeof(ValueHeader::array_v1) + 1u);
		const uint32_t element_bytes = g_secondary_type_sizes[id];
		ANVIL_ASSUME(element_bytes <= 8u);
		Write(ptr, size * element_bytes);
	}

	void Writer::OnPrimativeArrayU8(const uint8_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_U8);
	}

	void Writer::OnPrimativeArrayU16(const uint16_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_U16);
	}

	void Writer::OnPrimativeArrayU32(const uint32_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_U32);
	}

	void Writer::OnPrimativeArrayU64(const uint64_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_U64);
	}

	void Writer::OnPrimativeArrayS8(const int8_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_S8);
	}

	void Writer::OnPrimativeArrayS16(const int16_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_S16);
	}

	void Writer::OnPrimativeArrayS32(const int32_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_S32);
	}

	void Writer::OnPrimativeArrayS64(const int64_t* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_S64);
	}

	void Writer::OnPrimativeArrayF32(const float* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_F32);
	}

	void Writer::OnPrimativeArrayF64(const double* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_F64);
	}

	void Writer::OnPrimativeArrayC8(const char* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_C8);
	}

	void Writer::OnPrimativeArrayF16(const half* ptr, const uint32_t size) {
		_OnPrimativeArray(ptr, size, SID_F16);
	}

	void Writer::OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) {
		ANVIL_CONTRACT(type <= 1048575u, "Type must be <= 1048575u");
		ValueHeader header;
		header.primary_id = PID_USER_POD;
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

	namespace detail {
		static void CallOnPrimativeU8(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeU8(header.primative_v1.u8);
		}

		static void CallOnPrimativeU16(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeU16(header.primative_v1.u16);
		}

		static void CallOnPrimativeU32(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeU32(header.primative_v1.u32);
		}

		static void CallOnPrimativeU64(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeU64(header.primative_v1.u64);
		}

		static void CallOnPrimativeS8(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeS8(header.primative_v1.s8);
		}

		static void CallOnPrimativeS16(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeS16(header.primative_v1.s16);
		}

		static void CallOnPrimativeS32(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeS32(header.primative_v1.s32);
		}

		static void CallOnPrimativeS64(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeS64(header.primative_v1.s64);
		}

		static void CallOnPrimativeF16(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeF16(header.primative_v1.f16);
		}

		static void CallOnPrimativeF32(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeF32(header.primative_v1.f32);
		}

		static void CallOnPrimativeF64(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeF64(header.primative_v1.f64);
		}

		static void CallOnPrimativeC8(Parser& parser, const ValueHeader& header) {
			parser.OnPrimativeC8(header.primative_v1.c8);
		}

		static void CallOnNull(Parser& parser, const ValueHeader& header) {
			parser.OnNull();
		}
	}

	class ReadHelper {
	private:
		InputPipe& _pipe;
		Parser& _parser;
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

		void ReadObject() {
			const uint32_t size = header.object_v1.components;
			_parser.OnObjectBegin(size);
			uint16_t component_id;
			for (uint32_t i = 0u; i < size; ++i) {
				ReadFromPipe(_pipe, &component_id, sizeof(component_id));
				_parser.OnComponentID(component_id);
				ReadFromPipe(_pipe, &header, 1u);
				ReadGeneric();
			}
			_parser.OnObjectEnd();
		}

		inline void ReadPrimative() {
			const uint32_t id = header.primary_id;

			// Read primative value
			const uint32_t bytes = g_secondary_type_sizes[id];
			if(bytes > 0u) ReadFromPipe(_pipe, &header.primative_v1, g_secondary_type_sizes[id]);

			// Output the primative value
			typedef void(*OnPrimativeCallback)(Parser& parser, const ValueHeader& header);
			static constexpr const OnPrimativeCallback g_on_primative_callbacks[] = {
				detail::CallOnNull,			// SID_NULL
				detail::CallOnPrimativeU8,	// SID_U8
				detail::CallOnPrimativeU16,	// SID_U16
				detail::CallOnPrimativeU32,	// SID_U32
				detail::CallOnPrimativeU64,	// SID_U64
				detail::CallOnPrimativeS8,	// SID_S8
				detail::CallOnPrimativeS16,	// SID_S16
				detail::CallOnPrimativeS32,	// SID_S32
				detail::CallOnPrimativeS64,	// SID_S64
				detail::CallOnPrimativeF32,	// SID_F32
				detail::CallOnPrimativeF64,	// SID_F64
				detail::CallOnPrimativeC8,	// SID_C8
				detail::CallOnPrimativeF16	// SID_F16
			};

			g_on_primative_callbacks[id](_parser, header);
		}

		void ReadGeneric() {
			switch (header.primary_id) {
			case PID_NULL:
				_parser.OnNull();
				break;
			case PID_STRING:
				ANVIL_CONTRACT(header.secondary_id == SID_C8, "String subtype was not char");
				ReadFromPipe(_pipe, &header.string_v1, sizeof(header.string_v1));
				{
					const uint32_t len = header.string_v1.length;
					char* const buffer = static_cast<char*>(AllocateMemory(len + 1u));
					ReadFromPipe(_pipe, buffer, len);
					buffer[len] = '\0';
					_parser.OnPrimativeString(buffer, len);
				}
				break;
			case PID_ARRAY:
				ReadFromPipe(_pipe, &header.array_v1, sizeof(header.array_v1));
				ReadArray();
				break;
			case PID_OBJECT:
				ReadFromPipe(_pipe, &header.object_v1, sizeof(header.object_v1));
				ReadObject();
				break;
			case PID_USER_POD:
				{
					// Construct the user POD ID number
					uint32_t id = header.user_pod.extended_secondary_id;
					id <<= 4u;
					id |= header.secondary_id;

					// Read the POD from the input pipe
					void* mem = AllocateMemory(header.user_pod.bytes);
					ReadFromPipe(_pipe, mem, header.user_pod.bytes);

					// Output the POD
					_parser.OnUserPOD(id, header.user_pod.bytes, mem);
				}
				break;
			default:
				ReadPrimative();
				break;
			}
		}

		void ReadArray() {
			const uint32_t id = header.secondary_id;
			// If the array contains generic values
			if (id == SID_NULL) {
				const uint32_t size = header.array_v1.size;
				_parser.OnArrayBegin(size);
				for (uint32_t i = 0u; i < size; ++i) {
					ReadFromPipe(_pipe, &header, 1u);
					ReadGeneric();
				}
				_parser.OnArrayEnd();

			// The array contains primatives of the same type
			} else {
				ANVIL_CONTRACT(id <= SID_F16, "Unknown secondary type ID");

				typedef void(Parser::*ParserCallback)(const void* ptr, const uint32_t size);
				static constexpr const ParserCallback g_callbacks[] = {
					nullptr,														// SID_NULL
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayU8),	// SID_U8
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayU16),	// SID_U16
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayU32),	// SID_U32
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayU64),	// SID_U64
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayS8),	// SID_S8
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayS16),	// SID_S16
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayS32),	// SID_S32
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayS64),	// SID_S64
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayF32),	// SID_F32
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayF64),	// SID_F64
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayC8),	// SID_C8
					reinterpret_cast<ParserCallback>(&Parser::OnPrimativeArrayF16)	// SID_F16
				};

				const uint32_t size = header.array_v1.size;
				const uint32_t bytes = g_secondary_type_sizes[id] * size;
				void* buffer = buffer = AllocateMemory(bytes);
				ReadFromPipe(_pipe, buffer, bytes);
				(_parser.*g_callbacks[id])(buffer, size);
			}
		}
	public:
		ValueHeader header;

		ReadHelper(InputPipe& pipe, Parser& parser, Version version) :
			_pipe(pipe),
			_parser(parser),
			_mem(nullptr),
			_mem_bytes(0u)
		{}

		~ReadHelper() {
			if (_mem) operator delete(_mem);
		}

		void Read() {
			// Continue with read
			ReadFromPipe(_pipe, &header.id_union, 1u);
			while (header.id_union != PID_NULL) {
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

	void Reader::Read(Parser& dst) {
		PipeHeader pipeHeader;
		ReadFromPipe(_pipe, &pipeHeader, sizeof(PipeHeader));

		ANVIL_CONTRACT(pipeHeader.version <= VERSION_1, "BytePipe version not supported");
		const Version version = static_cast<Version>(pipeHeader.version);

		// Select correct reader for pipe version
		ReadHelper helper(_pipe, dst, version);
		helper.Read();
	}

	// ValueParser

	ValueParser::ValueParser() {
		
	}

	ValueParser::~ValueParser() {
		
	}

	Value& ValueParser::GetValue() {
		return _root;
	}

	void ValueParser::OnPipeOpen() {
		_value_stack.clear();
		_root.SetNull();
	}

	void ValueParser::OnPipeClose() {
		_value_stack.clear();
		_root.SetNull();
	}

	void ValueParser::OnArrayBegin(const uint32_t size) {
		Value& val = NextValue();
		val.SetArray();
		_value_stack.push_back(&val);
	}

	void ValueParser::OnArrayEnd() {
		_value_stack.pop_back();
	}

	void ValueParser::OnObjectBegin(const uint32_t component_count) {
		Value& val = NextValue();
		val.SetObject();
		_value_stack.push_back(&val);
	}

	void ValueParser::OnObjectEnd() {
		_value_stack.pop_back();
	}

	void ValueParser::OnComponentID(const ComponentID id) {
		_component_id = id;
	}

	void ValueParser::OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) {
		throw std::runtime_error("ValueParser::OnUserPOD : Pods not supported");
	}

	void ValueParser::OnNull() {
		NextValue().SetNull();
	}

	void ValueParser::OnPrimativeF64(const double value) {
		NextValue().SetF64(value);
	}

	void ValueParser::OnPrimativeString(const char* value, const uint32_t length) {
		// Zero terminate string
		char* str = static_cast<char*>(operator new(length + 1));
		memcpy(str, value, length);
		str[length] = '\0';

		try {
			Value& val = NextValue();
			val.SetString(str);
		} catch (...) {
			operator delete(str);
			std::rethrow_exception(std::current_exception());
		}
		operator delete(str);
	}

	void ValueParser::OnPrimativeC8(const char value) {
		NextValue().SetC8(value);
	}

	void ValueParser::OnPrimativeU64(const uint64_t value) {
		NextValue().SetU64(value);
	}

	void ValueParser::OnPrimativeS64(const int64_t value) {
		NextValue().SetS64(value);
	}

	void ValueParser::OnPrimativeF32(const float value) {
		NextValue().SetF32(value);
	}

	void ValueParser::OnPrimativeU8(const uint8_t value) {
		NextValue().SetU8(value);
	}

	void ValueParser::OnPrimativeU16(const uint16_t value) {
		NextValue().SetU16(value);
	}

	void ValueParser::OnPrimativeU32(const uint32_t value) {
		NextValue().SetU32(value);
	}

	void ValueParser::OnPrimativeS8(const int8_t value) {
		NextValue().SetS8(value);
	}

	void ValueParser::OnPrimativeS16(const int16_t value) {
		NextValue().SetS16(value);
	}

	void ValueParser::OnPrimativeS32(const int32_t value) {
		NextValue().SetS32(value);
	}

	void ValueParser::OnPrimativeF16(const half value) {
		NextValue().SetF16(value);
	}

	Value& ValueParser::CurrentValue() {
		return _value_stack.empty() ? _root : *_value_stack.back();
	}

	Value& ValueParser::NextValue() {
		Value& val = CurrentValue();
		switch (val.GetType()) {
		case TYPE_ARRAY:
			{
				Value tmp;
				val.AddValue(std::move(tmp));
				return val.GetValue(val.GetSize() - 1);
			}
			break;
		case TYPE_OBJECT:
			{
				Value tmp;
				val.AddValue(_component_id, std::move(tmp));
				return val.GetValue(_component_id);
			}
			break;
		default:
			return val;
		}
	}

	// Parser

	void Parser::OnValue(const Value& value) {
		switch (value.GetType()) {
		case TYPE_STRING:
			{
				const char* str = const_cast<Value&>(value).GetString();
				OnPrimativeString(str, strlen(str));
			}
			break;
		case TYPE_ARRAY:
			{
				const size_t size = value.GetSize();
				OnArrayBegin(size);
				for (size_t i = 0u; i < size; ++i) {
					OnValue(const_cast<Value&>(value).GetValue(i));
				}
				OnArrayEnd();
			}
			break;
		case TYPE_OBJECT:
			{
				const size_t size = value.GetSize();
				OnObjectBegin(size);
				for (size_t i = 0u; i < size; ++i) {
					const ComponentID id = value.GetComponentID(i);
					OnComponentID(id);
					OnValue(const_cast<Value&>(value).GetValue(id));
				}
				OnObjectEnd();
			}
			break;
		default:
			OnValue(value.GetPrimativeValue());
			break;
		}
	}

	void Parser::OnValue(const PrimativeValue& value) {
		switch (value.type) {
		case TYPE_NULL:
			OnNull();
			break;
		case TYPE_U8:
			OnPrimativeU8(value.u8);
			break;
		case TYPE_U16:
			OnPrimativeU16(value.u16);
			break;
		case TYPE_U32:
			OnPrimativeU32(value.u32);
			break;
		case TYPE_U64:
			OnPrimativeU64(value.u64);
			break;
		case TYPE_S8:
			OnPrimativeS8(value.u8);
			break;
		case TYPE_S16:
			OnPrimativeS16(value.s16);
			break;
		case TYPE_S32:
			OnPrimativeS32(value.s32);
			break;
		case TYPE_S64:
			OnPrimativeS64(value.s64);
			break;
		case TYPE_F16:
			OnPrimativeF16(value.f16);
			break;
		case TYPE_F32:
			OnPrimativeF32(value.f32);
			break;
		case TYPE_F64:
			OnPrimativeF64(value.f64);
			break;
		default:
			throw std::runtime_error("Writer::OnValue : Value is not a primative type");
		}
	}

}}