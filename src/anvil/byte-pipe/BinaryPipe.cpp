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
#include "anvil/byte-pipe/BytePipeEndian.hpp"

#ifdef ANVIL_DISABLE_LUTILS
	#ifndef ANVIL_CONTRACT
		#define ANVIL_CONTRACT(condition, msg) if(!(condition)) throw std::runtime_error(msg)
	#endif

	#ifndef ANVIL_ASSUME
		#ifdef _MSVC_LANG
			#ifdef _DEBUG
				#define ANVIL_ASSUME(condition) ANVIL_CONTRACT(condition, "Anvil Byte Pipe (Debug) : Assumption is incorrect")
			#else
					#define ANVIL_ASSUME(condition) __assume(condition)
			#endif
		#else
			#define ANVIL_ASSUME(condition)
		#endif
	#endif
#else
	#include "anvil/lutils/Assert.hpp"
#endif

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
		SID_F16,
		SID_B
	};

	// Header definitions
#pragma pack(push, 1)
	struct PipeHeaderV1 {
		uint8_t version;
	};

	struct PipeHeaderV2 {
		uint8_t version;
		struct {
			uint8_t little_endian : 1u;
			uint8_t reserved_flag1 : 1u;
			uint8_t reserved_flag2 : 1u;
			uint8_t reserved_flag3 : 1u;
			uint8_t reserved_flag4 : 1u;
			uint8_t reserved_flag5 : 1u;
			uint8_t reserved_flag6 : 1u;
			uint8_t reserved_flag7 : 1u;
		};
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

			union ValueHeaderPrimative {
				bool b;
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

	static_assert(sizeof(PipeHeaderV1) == 1u, "PipeHeaderV1 was not packed correctly by compiler");
	static_assert(sizeof(PipeHeaderV2) == 2u, "PipeHeaderV2 was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader) == 9u, "ValueHeader was not packed correctly by compiler");
	static_assert(sizeof(ValueHeader::user_pod) == 6u, "ValueHeader was not packed correctly by compiler");
	static_assert(offsetof(ValueHeader, primative_v1.u8) == 1u, "ValueHeader was not packed correctly by compiler");

	// Helper functions

	namespace detail {
		static void CallOnPrimativeU8(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeU8(header.u8);
		}

		static void CallOnPrimativeU16(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeU16(header.u16);
		}

		static void CallOnPrimativeU32(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeU32(header.u32);
		}

		static void CallOnPrimativeU64(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeU64(header.u64);
		}

		static void CallOnPrimativeS8(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeS8(header.s8);
		}

		static void CallOnPrimativeS16(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeS16(header.s16);
		}

		static void CallOnPrimativeS32(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeS32(header.s32);
		}

		static void CallOnPrimativeS64(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeS64(header.s64);
		}

		static void CallOnPrimativeF16(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeF16(header.f16);
		}

		static void CallOnPrimativeF32(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeF32(header.f32);
		}

		static void CallOnPrimativeF64(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeF64(header.f64);
		}

		static void CallOnPrimativeC8(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeC8(header.c8);
		}

		static void CallOnNull(Parser& parser, const PrimativeValue& header) {
			parser.OnNull();
		}

		static void CallOnPrimativeB(Parser& parser, const PrimativeValue& header) {
			parser.OnPrimativeBool(header.b);
		}
	}

	template<class T, class U>
	static inline uint64_t GetRaw(const T value) {
		union {
			U raw;
			T val;
		};
		if ANVIL_CONSTEXPR (sizeof(T) < sizeof(U)) raw = 0u;
		val = value;
		return raw;
	}

	template<>
	static inline uint64_t GetRaw<uint64_t, uint64_t>(const uint64_t value) {
		return value;
	}

	template<>
	static inline uint64_t GetRaw<uint32_t, uint32_t>(const uint32_t value) {
		return value;
	}

	// Helper arrays

	static ANVIL_CONSTEXPR const uint8_t g_secondary_type_sizes[] = {
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
		2u, // SID_F16,
		1u // SID_B
	};

	// Convert binary primative type ID to value type
	static ANVIL_CONSTEXPR const Type g_sid_2_object_type[] = {
		TYPE_NULL, // SID_NULL
		TYPE_U8, // SID_U8
		TYPE_U16, // SID_U16
		TYPE_U32, // SID_U32
		TYPE_U64, // SID_U64
		TYPE_S8, // SID_S8
		TYPE_S16, // SID_S16
		TYPE_S32, // SID_S32
		TYPE_S64, // SID_S64
		TYPE_F32, // SID_F32
		TYPE_F32, // SID_F64
		TYPE_C8, // SID_C8
		TYPE_F16, // SID_F16,
		TYPE_BOOL // SID_B
	};

	// Convert Value type to binary primative type ID
	static ANVIL_CONSTEXPR const SecondaryID g_object_type_2_sid[] = {
		SID_NULL, // TYPE_NULL
		SID_C8, // TYPE_C8
		SID_U8, // TYPE_U8
		SID_U16, // TYPE_U16
		SID_U32, // TYPE_U32
		SID_U64, // TYPE_U64
		SID_S8, // TYPE_S8
		SID_S16, // TYPE_S16
		SID_S32, // TYPE_S32
		SID_S64, // TYPE_S64
		SID_F16, // TYPE_F16
		SID_F32, // TYPE_F32
		SID_F64, // TYPE_F64
		static_cast<SecondaryID>(255), // TYPE_STRING
		static_cast<SecondaryID>(255), // TYPE_ARRAY
		static_cast<SecondaryID>(255), // TYPE_OBJECT
		SID_B, // TYPE_BOOL
	};

	typedef void(Parser::*PrimativeArrayCallback)(const void* ptr, const uint32_t size);
	static ANVIL_CONSTEXPR const PrimativeArrayCallback g_primative_array_callbacks[] = {
		nullptr,														// SID_NULL
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayU8),	// SID_U8
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayU16),	// SID_U16
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayU32),	// SID_U32
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayU64),	// SID_U64
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayS8),	// SID_S8
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayS16),	// SID_S16
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayS32),	// SID_S32
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayS64),	// SID_S64
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayF32),	// SID_F32
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayF64),	// SID_F64
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayC8),	// SID_C8
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayF16),	// SID_F16
		reinterpret_cast<PrimativeArrayCallback>(&Parser::OnPrimativeArrayBool)	// SID_B
	};


	typedef void(*PrimativeCallback)(Parser& parser, const PrimativeValue& header);
	static ANVIL_CONSTEXPR const PrimativeCallback g_primative_callbacks[] = {
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
		detail::CallOnPrimativeF16,	// SID_F16
		detail::CallOnPrimativeB	// SID_B
	};

	template<class T>
	static ANVIL_CONSTEXPR SecondaryID GetSecondaryID();

	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<void>() { return SID_NULL; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<char>() { return SID_C8; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<bool>() { return SID_B; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<uint8_t>() { return SID_U8; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<uint16_t>() { return SID_U16; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<uint32_t>() { return SID_U32; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<uint64_t>() { return SID_U64; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<int8_t>() { return SID_S8; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<int16_t>() { return SID_S16; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<int32_t>() { return SID_S32; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<int64_t>() { return SID_S64; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<half>() { return SID_F16; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<float>() { return SID_F32; }
	template<> static ANVIL_CONSTEXPR SecondaryID GetSecondaryID<double>() { return SID_F64; }

	// Writer

	Writer::Writer(OutputPipe& pipe, Version version, bool swap_byte_order) :
		_pipe(pipe),
		_default_state(STATE_CLOSED),
		_version(version),
		_swap_byte_order(swap_byte_order)
	{
		// Check for invalid settings
		if (_version == VERSION_1 && GetEndianness() == ENDIAN_BIG) throw std::runtime_error("Writer::Writer : Writing to big endian requires version 2 or higher");
	}

	Writer::Writer(OutputPipe& pipe, Version version) :
		Writer(pipe, version, false)
	{}

	Writer::Writer(OutputPipe& pipe) :
		Writer(pipe, VERSION_2)
	{}


	Writer::Writer(OutputPipe& pipe, Version version, Endianness endianness) :
		Writer(pipe, version, GetEndianness() != endianness)
	{}

	Writer::~Writer() {
		_pipe.Flush();
	}

	Endianness Writer::GetEndianness() const {
		const Endianness e = GetEndianness();
		return _swap_byte_order ? (e == ENDIAN_LITTLE ? ENDIAN_BIG : ENDIAN_LITTLE) : e;
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

		union {
			PipeHeaderV1 header_v1;
			PipeHeaderV2 header_v2;
		};
		uint32_t size;

		header_v1.version = _version;
		if (_version > VERSION_1) {
			const Endianness e = GetEndianness();

			header_v2.little_endian = (_swap_byte_order ? e != ENDIAN_LITTLE : e == ENDIAN_LITTLE) ? 1u : 0u;
			header_v2.reserved_flag1 = 0u;
			header_v2.reserved_flag2 = 0u;
			header_v2.reserved_flag3 = 0u;
			header_v2.reserved_flag4 = 0u;
			header_v2.reserved_flag5 = 0u;
			header_v2.reserved_flag6 = 0u;
			header_v2.reserved_flag7 = 0u;
			size = sizeof(PipeHeaderV2);
		}else {
			size = sizeof(PipeHeaderV1);
		}
		Write(&header_v1.version, size);
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

	void Writer::_OnPrimative32(uint32_t value, const uint8_t id) {
		const uint32_t bytes = g_secondary_type_sizes[id];
		ANVIL_ASSUME(bytes <= 4u);

		ValueHeader header;
		header.primary_id = PID_PRIMATIVE;
		header.secondary_id = id;
		header.primative_v1.u32 = value;
		if (_swap_byte_order && bytes > 1u) {
			if (bytes == 2u) {
				header.primative_v1.u16 = SwapByteOrder(header.primative_v1.u16);
			} else if (bytes == 4u) {
				header.primative_v1.u32 = SwapByteOrder(header.primative_v1.u32);
			} else {
				throw std::runtime_error("Writer::_OnPrimative32 : Cannot swap byte order");
			}
		}
		Write(&header, bytes + 1u);
	}

	void Writer::_OnPrimative64(uint64_t value, const uint8_t id) {
		const uint32_t bytes = g_secondary_type_sizes[id];
		ANVIL_ASSUME(bytes <= 8u);

		ValueHeader header;
		header.primary_id = PID_PRIMATIVE;
		header.secondary_id = id;
		header.primative_v1.u64 = value;
		if (_swap_byte_order && bytes > 1u) {
			if (bytes == 2u) {
				header.primative_v1.u16 = SwapByteOrder(header.primative_v1.u16);
			} else if (bytes == 4u) {
				header.primative_v1.u32 = SwapByteOrder(header.primative_v1.u32);
			} else if (bytes == 8u) {
				header.primative_v1.u64 = SwapByteOrder(header.primative_v1.u64);
			} else {
				throw std::runtime_error("Writer::_OnPrimative64 : Cannot swap byte order");
			}
		}
		Write(&header, bytes + 1u);
	}

	void Writer::OnPrimativeBool(const bool value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeU8(const uint8_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeU16(const uint16_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeU32(const uint32_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeU64(const uint64_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative64(GetRaw<T, uint64_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeS8(const int8_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeS16(const int16_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeS32(const int32_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeS64(const int64_t value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative64(GetRaw<T, uint64_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeF32(const float value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeF64(const double value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative64(GetRaw<T, uint64_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeC8(const char value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
	}

	void Writer::OnPrimativeF16(const half value) {
		typedef std::remove_const<decltype(value)>::type T;
		_OnPrimative32(GetRaw<T, uint32_t>(value), GetSecondaryID<T>());
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
		if (_swap_byte_order && element_bytes > 1u) {
			// Allocate temporary storage
			void* buffer = _alloca(size * element_bytes);

			// Copy and swap byte order
			if (element_bytes == 2u) {
				typedef uint16_t T;
				T* buffer2 = static_cast<T*>(buffer);
				for (uint32_t i = 0u; i < size; ++i) buffer2[i] = SwapByteOrder(static_cast<const T*>(ptr)[i]);
			} else if (element_bytes == 4u) {
				typedef uint32_t T;
				T* buffer2 = static_cast<T*>(buffer);
				for (uint32_t i = 0u; i < size; ++i) buffer2[i] = SwapByteOrder(static_cast<const T*>(ptr)[i]);
			} else if (element_bytes == 8u) {
				typedef uint64_t T;
				T* buffer2 = static_cast<T*>(buffer);
				for (uint32_t i = 0u; i < size; ++i) buffer2[i] = SwapByteOrder(static_cast<const T*>(ptr)[i]);
			} else {
				throw std::runtime_error("Writer::_OnPrimativeArray : Cannot swap byte order");
			}

			// Write the swapped bytes
			Write(buffer, size * element_bytes);
		} else {
			Write(ptr, size * element_bytes);
		}
	}

	void Writer::OnPrimativeArrayBool(const bool* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayU8(const uint8_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayU16(const uint16_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayU32(const uint32_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayU64(const uint64_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayS8(const int8_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayS16(const int16_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayS32(const int32_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayS64(const int64_t* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayF32(const float* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayF64(const double* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayC8(const char* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
	}

	void Writer::OnPrimativeArrayF16(const half* ptr, const uint32_t size) {
		typedef std::remove_const<std::remove_pointer<decltype(ptr)>::type>::type T;
		_OnPrimativeArray(ptr, size, GetSecondaryID<T>());
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

	class ReadHelper {
	private:
		InputPipe& _pipe;
		Parser& _parser;
		void* _mem;
		uint32_t _mem_bytes;
		bool _swap_byte_order;

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

			if (_swap_byte_order && bytes > 1u) {
				if (bytes == 2u) {
					header.primative_v1.u16 = SwapByteOrder(header.primative_v1.u16);
				} else if (bytes == 4u) {
					header.primative_v1.u32 = SwapByteOrder(header.primative_v1.u32);
				} else if (bytes == 8u) {
					header.primative_v1.u64 = SwapByteOrder(header.primative_v1.u64);
				} else {
					throw std::runtime_error("ReadHelper::ReadPrimative : Cannot swap byte order");
				}
			}

			// Output the value
			PrimativeValue tmp(g_sid_2_object_type[id], header.primative_v1.u64);
			_parser.OnValue(tmp);
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
					//! \bug Doesn't know how to swap the byte order of a POD

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
				ANVIL_CONTRACT(id <= SID_B, "Unknown secondary type ID");

				const uint32_t size = header.array_v1.size;
				const uint32_t element_bytes = g_secondary_type_sizes[id];
				const uint32_t bytes = element_bytes * size;
				void* buffer = buffer = AllocateMemory(bytes);
				ReadFromPipe(_pipe, buffer, bytes);
				if (_swap_byte_order && element_bytes > 1u) {
					if (bytes == 2u) {
						typedef uint16_t T;
						T* buffer2 = static_cast<T*>(buffer);
						for (uint32_t i = 0u; i < size; ++i) buffer2[i] = SwapByteOrder(buffer2[i]);
					} else if (bytes == 4u) {
						typedef uint32_t T;
						T* buffer2 = static_cast<T*>(buffer);
						for (uint32_t i = 0u; i < size; ++i) buffer2[i] = SwapByteOrder(buffer2[i]);
					} else if (bytes == 8u) {
						typedef uint64_t T;
						T* buffer2 = static_cast<T*>(buffer);
						for (uint32_t i = 0u; i < size; ++i) buffer2[i] = SwapByteOrder(buffer2[i]);
					} else {
						throw std::runtime_error("ReadHelper::ReadArray : Cannot swap byte order");
					}
				}
				(_parser.*g_primative_array_callbacks[id])(buffer, size);
			}
		}
	public:
		ValueHeader header;

		ReadHelper(InputPipe& pipe, Parser& parser, Version version, const bool swap_byte_order) :
			_pipe(pipe),
			_parser(parser),
			_mem(nullptr),
			_mem_bytes(0u),
			_swap_byte_order(swap_byte_order)
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
		// Read the version from the header
		union {
			PipeHeaderV1 header_v1;
			PipeHeaderV2 header_v2;
		};
		ReadFromPipe(_pipe, &header_v1, sizeof(PipeHeaderV1));

		// Check for unsupported version
		if (header_v1.version > VERSION_2) throw std::runtime_error("Reader::Read : BytePipe version not supported");

		// Read additional header data
		bool swap_byte_order;
		const Endianness e = GetEndianness();
		if (header_v1.version == VERSION_1) {
			// Version 1 only supports little endian
			swap_byte_order = e != ENDIAN_LITTLE;
		} else {
			// Read the version 2 header info
			ReadFromPipe(_pipe, reinterpret_cast<uint8_t*>(&header_v2) + sizeof(PipeHeaderV1), sizeof(PipeHeaderV2) - sizeof(PipeHeaderV1));
			swap_byte_order = e != (header_v2.little_endian ? ENDIAN_LITTLE : ENDIAN_BIG);

			// These header options are not defined yet
			if(header_v2.reserved_flag1 || header_v2.reserved_flag2 || header_v2.reserved_flag3 || header_v2.reserved_flag4 || header_v2.reserved_flag5 ||
				header_v2.reserved_flag5 || header_v2.reserved_flag6 || header_v2.reserved_flag7)
				throw std::runtime_error("Reader::Read : BytePipe version not supported");
		}


		// Select correct reader for pipe version
		ReadHelper helper(_pipe, dst, static_cast<Version>(header_v1.version), swap_byte_order);
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

	void ValueParser::OnPrimativeBool(const bool value) {
		NextValue().SetBool(value);
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
		const SecondaryID id = g_object_type_2_sid[value.type];
		ANVIL_CONTRACT(id <= SID_B, "PrimativeCallbackHelper : Unknown primative type");
		g_primative_callbacks[id](*this, value);
	}

}}