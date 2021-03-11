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

	bool Value::GetBool() {
		return GetS64() != 0.0;
	}

	char Value::GetC8() {
		return static_cast<char>(GetS64());
	}

	uint8_t Value::GetU8() {
		return static_cast<uint8_t>(GetU64());
	}

	uint16_t Value::GetU16() {
		return static_cast<uint16_t>(GetU64());
	}

	uint32_t Value::GetU32() {
		return static_cast<uint32_t>(GetU64());
	}

	uint64_t Value::GetU64() {
		return static_cast<uint64_t>(GetF64());
	}

	int8_t Value::GetS8() {
		return static_cast<int8_t>(GetS64());
	}

	int16_t Value::GetS16() {
		return static_cast<int16_t>(GetS64());
	}

	int32_t Value::GetS32() {
		return static_cast<int64_t>(GetS64());
	}

	int64_t Value::GetS64() {
		return static_cast<int64_t>(GetF64());
	}

	half Value::GetF16() {
		return static_cast<half>(static_cast<int>(GetF32())); //! \buf Float to half conversion not implemented
	}

	float Value::GetF32() {
		return static_cast<float>(GetF64());
	}

	double Value::GetF64() {
		switch (_primative.type) {
		case TYPE_U8:
			_primative.f64 = static_cast<double>(_primative.u8);
			break;
		case TYPE_U16:
			_primative.f64 = static_cast<double>(_primative.u16);
			break;
		case TYPE_U32:
			_primative.f64 = static_cast<double>(_primative.u32);
			break;
		case TYPE_U64:
			_primative.f64 = static_cast<double>(_primative.u64);
			break;
		case TYPE_S8:
			_primative.f64 = static_cast<double>(_primative.s8);
			break;
		case TYPE_S16:
			_primative.f64 = static_cast<double>(_primative.s16);
			break;
		case TYPE_S32:
			_primative.f64 = static_cast<double>(_primative.s32);
			break;
		case TYPE_S64:
			_primative.f64 = static_cast<double>(_primative.s64);
			break;
		case TYPE_F16:
			//! \bug Double to half conversion not implemented
			_primative.f64 = static_cast<double>(_primative.f16);
			break;
		case TYPE_F32:
			_primative.f64 = static_cast<double>(_primative.f32);
			break;
		case TYPE_F64:
			break;
		case TYPE_BOOL:
			_primative.f64 = _primative.b ? 1.0 : 0.0;
			break;
		default:
			throw std::runtime_error("Value::GetF64 : Value is not a numerical type");
		}

		_primative.type = TYPE_F64;
		return _primative.f64;
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