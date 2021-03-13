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

#include "anvil/byte-pipe/BytePipeObjects.hpp"

namespace anvil { namespace BytePipe {

	static constexpr const uint8_t g_type_sizes[] = {
		0u,	// TYPE_NULL,
		1u,	// TYPE_C8,
		1u,	// TYPE_U8,
		2u,	// TYPE_U16,
		4u,	// TYPE_U32,
		8u,	// TYPE_U64,
		1u,	// TYPE_S8,
		2u,	// TYPE_S16,
		4u,	// TYPE_S32,
		8u,	// TYPE_S64,
		2u,	// TYPE_F16,
		4u,	// TYPE_F32,
		8u,	// TYPE_F64,
		0u,	// TYPE_STRING,
		0u,	// TYPE_ARRAY,
		0u,	// TYPE_OBJECT,
		1u,	// TYPE_BOOL
	};

	template<class T>
	static inline uint64_t GetRaw(const T value) {
		union {
			uint64_t raw;
			T val;
		};
		if constexpr (sizeof(T) < sizeof(uint64_t)) raw = 0u;
		val = value;
		return raw;
	}

	template<>
	static inline uint64_t GetRaw< uint64_t>(const uint64_t value) {
		return value;
	}

	// PrimativeValue

	bool PrimativeValue::operator==(const PrimativeValue& other) const {
		return type == other.type ? 
			memcmp(&u64, &other.u64, g_type_sizes[type]) == 0 : 
			operator double() == other.operator double();
	}

	bool PrimativeValue::operator!=(const PrimativeValue& other) const {
		return ! operator==(other);
	}

	PrimativeValue::PrimativeValue(Type type, uint64_t raw) :
		u64(raw),
		type(type)
	{}

	PrimativeValue::PrimativeValue() :
		PrimativeValue(TYPE_NULL, 0u)
	{}

	PrimativeValue::PrimativeValue(bool value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(char value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(uint8_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(uint16_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(uint32_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(uint64_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(int8_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(int16_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(int32_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(int64_t value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(half value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(float value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::PrimativeValue(double value) :
		PrimativeValue(GetTypeID<decltype(value)>(), GetRaw<decltype(value)>(value))
	{}

	PrimativeValue::operator bool() const {
		return operator double() > 0.0;
	}

	PrimativeValue::operator char() const {
		if (type == TYPE_C8) {
			return c8;
		} else {
			intptr_t tmp = operator intptr_t();
			if (tmp > 9) tmp = 9;
			else if (tmp < 0) tmp = 0;
			return static_cast<char>('0' + tmp);
		}
	}

	PrimativeValue::operator uint8_t() const {
		if (type == TYPE_U8) {
			return u8;
		} else {
			const uintptr_t tmp = operator uintptr_t();
			return tmp > UINT8_MAX ? UINT8_MAX : static_cast<uint8_t>(tmp);
		}
	}

	PrimativeValue::operator uint16_t() const {
		if (type == TYPE_U16) {
			return u16;
		} else {
			const uintptr_t tmp = operator uintptr_t();
			return tmp > UINT16_MAX ? UINT16_MAX : static_cast<uint16_t>(tmp);
		}
	}

	PrimativeValue::operator uint32_t() const {
		if (type == TYPE_U32) {
			return u8;
		} else {
			const uint64_t tmp = operator uint64_t();
			return tmp > UINT32_MAX ? UINT32_MAX : static_cast<uint32_t>(tmp);
		}
	}

	PrimativeValue::operator uint64_t() const {
		if (type == TYPE_U64) {
			return u64;
		} else {
			const double tmp = operator double();
			return static_cast<uint64_t>(std::round(tmp));
		}
	}

	PrimativeValue::operator int8_t() const {
		if (type == TYPE_S8) {
			return s8;
		} else {
			const intptr_t tmp = operator intptr_t();
			return tmp > INT8_MAX ? INT8_MAX : tmp < INT8_MIN ? INT8_MIN : static_cast<int8_t>(tmp);
		}
	}

	PrimativeValue::operator int16_t() const {
		if (type == TYPE_S16) {
			return s16;
		} else {
			const intptr_t tmp = operator intptr_t();
			return tmp > INT16_MAX ? INT16_MAX : tmp < INT16_MIN ? INT16_MIN : static_cast<int16_t>(tmp);
		}
	}

	PrimativeValue::operator int32_t() const {
		if (type == TYPE_S32) {
			return s32;
		} else {
			const int64_t tmp = operator int64_t();
			return tmp > INT32_MAX ? INT32_MAX : tmp < INT32_MIN ? INT32_MIN : static_cast<int32_t>(tmp);
		}
	}

	PrimativeValue::operator int64_t() const {
		if (type == TYPE_S64) {
			return s64;
		} else {
			const double tmp = operator double();
			return static_cast<uint64_t>(std::round(tmp));
		}
	}

	PrimativeValue::operator half() const {
		if (type == TYPE_F16) {
			return f16;
		} else {
			//! \bug f16 conversion is not implented
			throw std::runtime_error("PrimativeValue::operator half : 16-bit floating point is not implemented");
		}
	}

	PrimativeValue::operator float() const {
		if (type == TYPE_F32) {
			return f32;
		} else {
			return static_cast<float>(operator double());
		}
	}

	PrimativeValue::operator double() const {
		switch (type) {
		case TYPE_NULL:
			return 0.f;
		case TYPE_C8:
			if(c8 >= '0' && c8 <= '9') return static_cast<double>(c8 - '0');
			goto ON_ERROR;
		case TYPE_U8:
			return static_cast<double>(u8);
		case TYPE_U16:
			return static_cast<double>(u16);
		case TYPE_U32:
			return static_cast<double>(u32);
		case TYPE_U64:
			return static_cast<double>(u64);
		case TYPE_S8:
			return static_cast<double>(s8);
		case TYPE_S16:
			return static_cast<double>(s16);
		case TYPE_S32:
			return static_cast<double>(s32);
		case TYPE_S64:
			return static_cast<double>(s64);
		case TYPE_F16:
			return static_cast<double>(f16);
		case TYPE_F32:
			return static_cast<double>(f32);
		case TYPE_F64:
			return static_cast<double>(f64);
		case TYPE_BOOL:
			return b ? 1.0 : 0.0;
		default:
ON_ERROR:
			throw std::runtime_error("PrimativeValue::operator double : Type cannot be converted to double");
		}
	}

#define IS_PRIMATIVE_TYPE(type) (type < TYPE_STRING || type == TYPE_BOOL)

	// Value

	Value::Value() {
		_primative.u64 = 0u;
		_primative.type = TYPE_NULL;
	}

	Value::~Value() {
		SetNull();
	}


	Value::Value(Value&& other) :
		Value()
	{
		Swap(other);
	}

	Value::Value(const Value& other) :
		Value() 
	{
		*this = other;
	}

	Value& Value::operator=(Value&& other) {
		Swap(other);
		return *this;
	}

	Value& Value::operator=(const Value& other) {
		switch (other._primative.type) {
		case TYPE_STRING:
			SetString(static_cast<std::string*>(other._primative.ptr)->c_str());
			break;
		case TYPE_ARRAY:
			{
				SetArray();
				Array& myArray = *static_cast<Array*>(_primative.ptr);
				const Array& otherArray = *static_cast<Array*>(other._primative.ptr);
				myArray = otherArray;
			}
			break;
		case TYPE_OBJECT:
			{
				SetObject();
				Object& myObject = *static_cast<Object*>(_primative.ptr);
				const Object& otherObject = *static_cast<Object*>(other._primative.ptr);
				myObject = otherObject;
			}
		break;
		default:
			SetNull();
			_primative = other._primative;
			break;
		}

		return *this;
	}

	void Value::Swap(Value& other) {
		std::swap(_primative, other._primative);
	}

	Type Value::GetType() const {
		return _primative.type;
	}

	void Value::SetNull() {
		switch (_primative.type) {
		case TYPE_STRING:
			delete static_cast<std::string*>(_primative.ptr);
			break;
		case TYPE_ARRAY:
			delete static_cast<Array*>(_primative.ptr);
			break;
		case TYPE_OBJECT:
			delete static_cast<Object*>(_primative.ptr);
			break;
		}
		_primative.u64 = 0u;
		_primative.type = TYPE_NULL;
	}

	void Value::SetBool(const bool value) {
		SetNull();
		_primative.b = value;
		_primative.type = TYPE_BOOL;
	}

	void Value::SetC8(const char value) {
		SetNull();
		_primative.c8 = value;
		_primative.type = TYPE_C8;
	}

	void Value::SetU8(const uint8_t value) {
		SetNull();
		_primative.u8 = value;
		_primative.type = TYPE_U8;
	}

	void Value::SetU16(const uint16_t value) {
		SetNull();
		_primative.u16 = value;
		_primative.type = TYPE_U16;
	}

	void Value::SetU32(const uint32_t value) {
		SetNull();
		_primative.u32 = value;
		_primative.type = TYPE_U32;
	}

	void Value::SetU64(const uint64_t value) {
		SetNull();
		_primative.u64 = value;
		_primative.type = TYPE_U64;
	}

	void Value::SetS8(const int8_t value) {
		SetNull();
		_primative.s8 = value;
		_primative.type = TYPE_S8;
	}

	void Value::SetS16(const int16_t value) {
		SetNull();
		_primative.s16 = value;
		_primative.type = TYPE_S16;
	}

	void Value::SetS32(const int32_t value) {
		SetNull();
		_primative.s32 = value;
		_primative.type = TYPE_S32;
	}

	void Value::SetS64(const int64_t value) {
		SetNull();
		_primative.s64 = value;
		_primative.type = TYPE_S64;
	}

	void Value::SetF16(const half value) {
		SetNull();
		_primative.f16 = value;
		_primative.type = TYPE_F16;
	}

	void Value::SetF32(const float value) {
		SetNull();
		_primative.f32 = value;
		_primative.type = TYPE_F32;
	}

	void Value::SetF64(const double value) {
		SetNull();
		_primative.f64 = value;
		_primative.type = TYPE_F64;
	}

	void Value::SetString(const char* value) {
		if (_primative.type == TYPE_STRING) {
			if (value == nullptr) {
				static_cast<std::string*>(_primative.ptr)->clear();
			} else {
				*static_cast<std::string*>(_primative.ptr) = value;
			}
		} else {
			SetNull();
			if (value == nullptr) {
				_primative.ptr = new std::string();
			} else{
				_primative.ptr = new std::string(value);
			}
			_primative.type = TYPE_STRING;
		}
	}

	void Value::SetArray() {
		if (_primative.type == TYPE_ARRAY) {
			static_cast<Array*>(_primative.ptr)->clear();
		} else {
			SetNull();
			_primative.ptr = new Array();
			_primative.type = TYPE_ARRAY;
		}
	}

	void Value::AddValue(Value&& value) {
		if (_primative.type != TYPE_ARRAY) throw std::runtime_error("Value::AddValue : Value is not an array");
		static_cast<Array*>(_primative.ptr)->push_back(std::move(value));
	}

	void Value::SetObject() {
		if (_primative.type == TYPE_OBJECT) {
			static_cast<Object*>(_primative.ptr)->clear();
		} else {
			SetNull();
			_primative.ptr = new Object();
			_primative.type = TYPE_OBJECT;
		}
	}

	void Value::AddValue(const ComponentID id, Value&& value) {
		if (_primative.type != TYPE_OBJECT) throw std::runtime_error("Value::AddValue : Value is not an object");
		static_cast<Object*>(_primative.ptr)->emplace(id, std::move(value));
	}

	bool Value::GetBool() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetBool : Value cannot be converted to boolean");
	}

	char Value::GetC8() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetC8 : Value cannot be converted to character");
	}

	uint8_t Value::GetU8() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetU8 : Value cannot be converted to 8-bit unsigned integer");
	}

	uint16_t Value::GetU16() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetU16 : Value cannot be converted to 16-bit unsigned integer");
	}

	uint32_t Value::GetU32() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetU32 : Value cannot be converted to 32-bit unsigned integer");
	}

	uint64_t Value::GetU64() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetU64 : Value cannot be converted to 64-bit unsigned integer");
	}

	int8_t Value::GetS8() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetS8 : Value cannot be converted to 8-bit signed integer");
	}

	int16_t Value::GetS16() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetS16 : Value cannot be converted to 16-bit signed integer");
	}

	int32_t Value::GetS32() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetS32 : Value cannot be converted to 32-bit signed integer");
	}

	int64_t Value::GetS64() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetS64 : Value cannot be converted to 64-bit signed integer");
	}

	half Value::GetF16() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetF16 : Value cannot be converted to 16-bit floating point");
	}

	float Value::GetF32() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetF32 : Value cannot be converted to 32-bit floating point");
	}

	double Value::GetF64() const {
		if (IS_PRIMATIVE_TYPE(_primative.type)) return _primative;
		throw std::runtime_error("Value::GetF64 : Value cannot be converted to 64-bit floating point");
	}

	const char* Value::GetString() {
		if (_primative.type != TYPE_STRING) {
			char buffer[64];
			if (_primative.type == TYPE_C8) {
				buffer[0u] = GetC8();
				buffer[1u] = '\0';
			} else {
				const double f64 = GetF64();
				sprintf(buffer, "%f", f64);
			}
			SetString(buffer);
		}
		return static_cast<std::string*>(_primative.ptr)->c_str();
	}

	Value& Value::GetValue(const uint32_t index) {
		switch (_primative.type) {
		case TYPE_ARRAY:
			{
				Array& myArray = *static_cast<Array*>(_primative.ptr);
				if (index >= myArray.size()) throw std::runtime_error("Value::GetValue : Index out of bounds");
				return myArray[index];
			}
		case TYPE_OBJECT:
			{
				Object& myObject = *static_cast<Object*>(_primative.ptr);
				auto i = myObject.find(index);
				if (i == myObject.end()) throw std::runtime_error("Value::GetValue : No member object with component ID");
				return i->second;
			}
			break;
		default:
			throw std::runtime_error("Value::GetValue : Value is not an array or object");
		}
	}

	ComponentID Value::GetComponentID(const uint32_t index) const {
		switch (_primative.type) {
		case TYPE_OBJECT:
			{
				Object& myObject = *static_cast<Object*>(_primative.ptr);
				if (index >= myObject.size()) throw std::runtime_error("Value::GetValue : Index out of bounds");
				auto i = myObject.begin();
				uint32_t j = index;
				while (j != 0u) {
					++i;
				}
				return i->first;
			}
			break;
		default:
			throw std::runtime_error("Value::GetValue : Value is not an array or object");
		}
	}

	PrimativeValue Value::GetPrimativeValue() const {
		switch (_primative.type) {
		case TYPE_STRING:
		case TYPE_ARRAY:
		case TYPE_OBJECT:
			throw std::runtime_error("Value::GetPrimativeValue : Value is not a numerical type");
			break;
		default:
			return _primative;
		}
	}

	size_t Value::GetSize() const {
		return _primative.type == TYPE_ARRAY ? static_cast<Array*>(_primative.ptr)->size() :
			_primative.type == TYPE_OBJECT ? static_cast<Object*>(_primative.ptr)->size() :
			0u;
	}


}}