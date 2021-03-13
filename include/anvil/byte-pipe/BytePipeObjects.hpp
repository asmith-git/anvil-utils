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

#ifndef ANVIL_BYTEPIPE_OBJECTS_HPP
#define ANVIL_BYTEPIPE_OBJECTS_HPP

#include "anvil/byte-pipe/BytePipeCore.hpp"
#include <vector>
#include <map>

namespace anvil { namespace BytePipe {

	enum Type : uint8_t {
		TYPE_NULL,
		TYPE_C8,
		TYPE_U8,
		TYPE_U16,
		TYPE_U32,
		TYPE_U64,
		TYPE_S8,
		TYPE_S16,
		TYPE_S32,
		TYPE_S64,
		TYPE_F16,
		TYPE_F32,
		TYPE_F64,
		TYPE_STRING,
		TYPE_ARRAY,
		TYPE_OBJECT,
		TYPE_BOOL
	};

	typedef uint16_t ComponentID;

	enum half : uint16_t {};


	template<class T>
	static constexpr Type GetTypeID();

	template<> static constexpr Type GetTypeID<void>() { return TYPE_NULL; }
	template<> static constexpr Type GetTypeID<char>() { return TYPE_C8; }
	template<> static constexpr Type GetTypeID<bool>() { return TYPE_BOOL; }
	template<> static constexpr Type GetTypeID<uint8_t>() { return TYPE_U8; }
	template<> static constexpr Type GetTypeID<uint16_t>() { return TYPE_U16; }
	template<> static constexpr Type GetTypeID<uint32_t>() { return TYPE_U32; }
	template<> static constexpr Type GetTypeID<uint64_t>() { return TYPE_U64; }
	template<> static constexpr Type GetTypeID<int8_t>() { return TYPE_S8; }
	template<> static constexpr Type GetTypeID<int16_t>() { return TYPE_S16; }
	template<> static constexpr Type GetTypeID<int32_t>() { return TYPE_S32; }
	template<> static constexpr Type GetTypeID<int64_t>() { return TYPE_S64; }
	template<> static constexpr Type GetTypeID<half>() { return TYPE_F16; }
	template<> static constexpr Type GetTypeID<float>() { return TYPE_F32; }
	template<> static constexpr Type GetTypeID<double>() { return TYPE_F64; }

	struct PrimativeValue {
		union {
			void* ptr;
			bool b;
			char c8;
			uint8_t u8;
			uint16_t u16;
			uint32_t u32;
			uint64_t u64;
			int8_t s8;
			int16_t s16;
			int32_t s32;
			int64_t s64;
			half f16;
			float f32;
			double f64;
		};

		Type type;

		bool operator==(const PrimativeValue& other) const;
		bool operator!=(const PrimativeValue& other) const;

		PrimativeValue();
		PrimativeValue(bool value);
		PrimativeValue(char value);
		PrimativeValue(uint8_t value);
		PrimativeValue(uint16_t value);
		PrimativeValue(uint32_t value);
		PrimativeValue(uint64_t value);
		PrimativeValue(int8_t value);
		PrimativeValue(int16_t value);
		PrimativeValue(int32_t value);
		PrimativeValue(int64_t value);
		PrimativeValue(half value);
		PrimativeValue(float value);
		PrimativeValue(double value);

		/*!
			\param type The type of \a raw
			\param The value
		*/
		PrimativeValue(Type type, uint64_t raw);

		operator bool() const;
		operator char() const;
		operator uint8_t() const;
		operator uint16_t() const;
		operator uint32_t() const;
		operator uint64_t() const;
		operator int8_t() const;
		operator int16_t() const;
		operator int32_t() const;
		operator int64_t() const;
		operator half() const;
		operator float() const;
		operator double() const;
	};

	class Value {
	private:
		typedef std::vector<Value> Array;
		typedef std::map<ComponentID, Value> Object;
		PrimativeValue _primative;
	public:
		Value();
		Value(Value&&);
		Value(const Value&);
		~Value();

		Value& operator=(Value&&);
		Value& operator=(const Value&);

		void Swap(Value&);

		Type GetType() const;

		/*!
			\brief Set the value to be a null value.
			\details Previous value will be lost.
		*/
		void SetNull();

		/*!
			\brief Set the value to be a boolean.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetBool(const bool value = false);

		/*!
			\brief Set the value to be a character.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetC8(const char value = ' ');

		/*!
			\brief Set the value to be a 8-bit unsigned integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetU8(const uint8_t value = 0u);

		/*!
			\brief Set the value to be a 16-bit unsigned integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetU16(const uint16_t value = 0u);

		/*!
			\brief Set the value to be a 32-bit unsigned integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetU32(const uint32_t value = 0u);

		/*!
			\brief Set the value to be a 64-bit unsigned integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetU64(const uint64_t value = 0u);

		/*!
			\brief Set the value to be a 8-bit signed integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetS8(const int8_t value = 0);

		/*!
			\brief Set the value to be a 16-bit signed integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetS16(const int16_t value = 0);

		/*!
			\brief Set the value to be a 32-bit signed integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetS32(const int32_t value = 0);

		/*!
			\brief Set the value to be a 64-bit signed integer.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetS64(const int64_t value = 0);

		/*!
			\brief Set the value to be a 16-bit floating point.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetF16(const half value = static_cast<half>(0));

		/*!
			\brief Set the value to be a 32-bit floating point.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetF32(const float value = 0.f);

		/*!
			\brief Set the value to be a 64-bit floating point.
			\details Previous value will be lost.
			\param value The value to copy.
		*/
		void SetF64(const double value = 0.0);

		/*!
			\brief Set the value to be a string.
			\details Previous value will be lost.
			\param value The value to copy, nullptr results in an empty string.
		*/
		void SetString(const char* value = nullptr);

		/*!
			\brief Set the value to be an array.
			\details Previous value will be lost.
		*/
		void SetArray();

		/*!
			\brief Append a value to the end of the array.
			\details Throws exception is value is not an array.
			\param value The value to add.
		*/
		void AddValue(Value&& value);

		/*!
			\brief Set the value to be an object.
			\details Previous value will be lost.
		*/
		void SetObject();

		/*!
			\brief Add a member value to an object.
			\details Throws exception is value is not an object.
			If the component ID already exists the previous value will be overwritten.
			\param id The component ID of the value.
			\param value The value to add.
		*/
		void AddValue(const ComponentID id, Value&& value);

		bool GetBool() const;
		char GetC8() const;
		uint8_t GetU8() const;
		uint16_t GetU16() const;
		uint32_t GetU32() const;
		uint64_t GetU64() const;
		int8_t GetS8() const;
		int16_t GetS16() const;
		int32_t GetS32() const;
		int64_t GetS64() const;
		half GetF16() const;
		float GetF32() const;
		double GetF64() const;
		const char* GetString();

		/*!
			\brief Get a child value of an array or object.
			\details Throws an exception if the index is out of bounds or the component ID doesn't exist.
			\param index The index in an array or the componend ID of an object.
			\return The value at the location.
		*/
		Value& GetValue(const uint32_t index);

		/*!
			\brief Get component ID at a specific index.
			\details Throws an exception if the index is out of bounds.
			\param index The index of the member (eg. 0 = First member, 1 = second member, ect).
			\return The component ID.
		*/
		ComponentID GetComponentID(const uint32_t index) const;

		/*!
			\brief Return the value as a primative
			\detail Throws an exception if the type is not numerical.
		*/
		PrimativeValue GetPrimativeValue() const;

		/*!
			\brief Get the number of child values in an array or object.
			\detail Zero will be returned if the value is not an array or object.
		*/
		size_t GetSize() const;
	};

}}

#endif
