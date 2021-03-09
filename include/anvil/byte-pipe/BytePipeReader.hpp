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

#ifndef ANVIL_BYTEPIPE_READER_HPP
#define ANVIL_BYTEPIPE_READER_HPP

#include "anvil/byte-pipe/BytePipeCore.hpp"

namespace anvil { namespace BytePipe {
	class InputPipe {
	public:
		virtual ~InputPipe() {}
		virtual uint32_t ReadBytes(void* dst, const uint32_t bytes) = 0;
	};

	enum half : uint16_t {};

	class Parser {
	public:
		Parser() {

		}

		virtual ~Parser() {

		}

		virtual Version GetSupportedVersion() const { 
			return VERSION_1; 
		}

		// Basic functionality

		/*!
			\brief Called when the pipe is about to recieve data
			\details Use this function to initialise variables.
			OnPipeClose will be called when all data is revieved.
			\see OnPipeClose
		*/
		virtual void OnPipeOpen() = 0;

		/*!
			\brief Called after all data has been recieved.
			\details Use this function to clean up variables and free memory.
			\see OnPipeOpen
		*/
		virtual void OnPipeClose() = 0;

		/*!
			\brief Signal that the next value(s) are part of an array.
			\details OnArrayEnd must be called after all values in the array have been parsed.
			The values will be in sequential order, so that the first value is index 0, then index 1, ect.
			\param size The number of values in the array.
			\see OnArrayEnd
		*/
		virtual void OnArrayBegin(const uint32_t size) = 0;

		/*!
			\brief Signal that all values in an array have been parsed.
			\see OnArrayBegin
		*/
		virtual void OnArrayEnd() = 0;

		/*!
			\brief Signal that the next value(s) are part of an object.
			\details OnObjectEnd must be called after all values in the array have been parsed.
			The values are unordered, but OnComponentID must be called before each value to identify it.
			\param size The number of values in the object.
			\see OnArrayEnd
			\see OnComponentID
		*/
		virtual void OnObjectBegin(const uint32_t component_count) = 0;

		/*!
			\brief Signal that all values in an object have been parsed.
			\see OnObjectBegin
		*/
		virtual void OnObjectEnd() = 0;

		/*!
			\brief Specify which value is the next to be parsed.
			Any calls outside of a OnObjectBegin / OnObjectEnd pair should be igored.
			\param id The value's identifier.
			\see OnArrayEnd
			\see OnComponentID
		*/
		virtual void OnComponentID(const uint16_t id) = 0;

		/*!
			\brief Handle a user defined binary structure.
			\details This allows the user to define their own POD (plain old data) structures, which can be 
			handled natively by pipes, this is faster than serialising the structure as an object.
			\param type A 24-bit ID code that describes which structure is being parsed.
			\param bytes The size of the structure in bytes.
			\param data A pointer to the structure.
		*/
		virtual void OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) = 0;

		/*!
			\brief Handle a null value
		*/
		virtual void OnNull() = 0;

		/*!
			\brief Handle a primative value (64-bit floating point)
			\param value The value
		*/
		virtual void OnPrimativeF64(const double value) = 0;

		/*!
			\brief Handle a string value
			\param value The string data, this may not zero-terminated
		*/
		virtual void OnPrimativeString(const char* value, const uint32_t length) = 0;

		/*!
			\brief Handle a primative value (character)
			\param value The value
		*/
		virtual void OnPrimativeC8(const char value) = 0;

		/*!
			\brief Handle a primative value (64-bit unsigned integer)
			\param value The value
		*/
		virtual void OnPrimativeU64(const uint64_t value) { 
			OnPrimativeF64(static_cast<double>(value));
		}

		/*!
			\brief Handle a primative value (64-bit signed integer)
			\param value The value
		*/
		virtual void OnPrimativeS64(const int64_t value) { 
			OnPrimativeF64(static_cast<double>(value));
		}

		/*!
			\brief Handle a primative value (32-bit floating point)
			\param value The value
		*/
		virtual void OnPrimativeF32(const float value) { 
			OnPrimativeF64(value);
		}

		/*!
			\brief Handle a primative value (8-bit unsigned integer)
			\param value The value
		*/
		virtual void OnPrimativeU8(const uint8_t value) { 
			OnPrimativeU64(value);
		}

		/*!
			\brief Handle a primative value (16-bit unsigned integer)
			\param value The value
		*/
		virtual void OnPrimativeU16(const uint16_t value) { 
			OnPrimativeU64(value);
		}

		/*!
			\brief Handle a primative value (32-bit unsigned integer)
			\param value The value
		*/
		virtual void OnPrimativeU32(const uint32_t value) { 
			OnPrimativeU64(value);
		}

		/*!
			\brief Handle a primative value (8-bit signed integer)
			\param value The value
		*/
		virtual void OnPrimativeS8(const int8_t value) { 
			OnPrimativeS64(value);
		}

		/*!
			\brief Handle a primative value (16-bit signed integer)
			\param value The value
		*/
		virtual void OnPrimativeS16(const int16_t value) { 
			OnPrimativeS64(value);
		}

		/*!
			\brief Handle a primative value (32-bit signed integer)
			\param value The value
		*/
		virtual void OnPrimativeS32(const int32_t value) { 
			OnPrimativeS64(value); 
		}

		/*!
			\brief Handle a primative value (16-bit floating point)
			\param value The value
		*/
		virtual void OnPrimativeF16(const half value) { 
			OnPrimativeF32(static_cast<float>(value));  //! \bug half to float conversion not implemented
		}

		// Array Optimisations

		/*!
			\brief Handle an array of primative values (8-bit unsigned integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU8(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayU8(const uint8_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU8(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (16-bit unsigned integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU16(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayU16(const uint16_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU16(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (32-bit unsigned integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU32(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayU32(const uint32_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU32(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (64-bit unsigned integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU64(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayU64(const uint64_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeU64(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (8-bit signed integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS8(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayS8(const int8_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS8(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (16-bit signed integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS16(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayS16(const int16_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS16(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (32-bit signed integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS32(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayS32(const int32_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS32(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (64-bit signed integers)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS64(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayS64(const int64_t* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeS64(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (32-bit floating point)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeF32(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayF32(const float* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeF32(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (64-bit floating point)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeF64(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayF64(const double* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeF64(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (character)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeC8(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayC8(const char* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeC8(src[i]);
			OnArrayEnd();
		}

		/*!
			\brief Handle an array of primative values (16-bit floating point)
			\details This is the same as the following code, but is a special case that could be optimised :
			\code{.cpp}
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeF16(src[i]);
			OnArrayEnd();
			\endcode
			\param src The address of the first value
			\param size The number of values in the array
		*/
		virtual void OnPrimativeArrayF16(const half* src, const uint32_t size) {
			OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) OnPrimativeF16(src[i]);
			OnArrayEnd();
		}

		// Template helpers

		template<class T>
		inline void OnPrimative(const T value);

		template<class T>
		inline void OnPrimativeArray(const T* values, const uint32_t size);

		template<>
		inline void OnPrimative<char>(const char value) {
			OnPrimativeC8(value);
		}

		template<>
		inline void OnPrimativeArray<char>(const char* values, const uint32_t size) {
			OnPrimativeArrayC8(values, size);
		}

		template<>
		inline void OnPrimative<uint8_t>(const uint8_t value) {
			OnPrimativeU8(value);
		}

		template<>
		inline void OnPrimativeArray<uint8_t>(const uint8_t* values, const uint32_t size) {
			OnPrimativeArrayU8(values, size);
		}

		template<>
		inline void OnPrimative<uint16_t>(const uint16_t value) {
			OnPrimativeU16(value);
		}

		template<>
		inline void OnPrimativeArray<uint16_t>(const uint16_t* values, const uint32_t size) {
			OnPrimativeArrayU16(values, size);
		}

		template<>
		inline void OnPrimative<uint32_t>(const uint32_t value) {
			OnPrimativeU32(value);
		}

		template<>
		inline void OnPrimativeArray<uint32_t>(const uint32_t* values, const uint32_t size) {
			OnPrimativeArrayU32(values, size);
		}

		template<>
		inline void OnPrimative<uint64_t>(const uint64_t value) {
			OnPrimativeU64(value);
		}

		template<>
		inline void OnPrimativeArray<uint64_t>(const uint64_t* values, const uint32_t size) {
			OnPrimativeArrayU64(values, size);
		}

		template<>
		inline void OnPrimative<int8_t>(const int8_t value) {
			OnPrimativeS8(value);
		}

		template<>
		inline void OnPrimativeArray<int8_t>(const int8_t* values, const uint32_t size) {
			OnPrimativeArrayS8(values, size);
		}

		template<>
		inline void OnPrimative<int16_t>(const int16_t value) {
			OnPrimativeS16(value);
		}

		template<>
		inline void OnPrimativeArray<int16_t>(const int16_t* values, const uint32_t size) {
			OnPrimativeArrayS16(values, size);
		}

		template<>
		inline void OnPrimative<int32_t>(const int32_t value) {
			OnPrimativeS32(value);
		}

		template<>
		inline void OnPrimativeArray<int32_t>(const int32_t* values, const uint32_t size) {
			OnPrimativeArrayS32(values, size);
		}

		template<>
		inline void OnPrimative<int64_t>(const int64_t value) {
			OnPrimativeS64(value);
		}

		template<>
		inline void OnPrimativeArray<int64_t>(const int64_t* values, const uint32_t size) {
			OnPrimativeArrayS64(values, size);
		}

		template<>
		inline void OnPrimative<half>(const half value) {
			OnPrimativeF16(value);
		}

		template<>
		inline void OnPrimativeArray<half>(const half* values, const uint32_t size) {
			OnPrimativeArrayF16(values, size);
		}

		template<>
		inline void OnPrimative<float>(const float value) {
			OnPrimativeF32(value);
		}

		template<>
		inline void OnPrimativeArray<float>(const float* values, const uint32_t size) {
			OnPrimativeArrayF32(values, size);
		}

		template<>
		inline void OnPrimative<double>(const double value) {
			OnPrimativeF64(value);
		}

		template<>
		inline void OnPrimativeArray<double>(const double* values, const uint32_t size) {
			OnPrimativeArrayF64(values, size);
		}

		// Object helper functions

		template<class T>
		inline void OnPrimative(const uint16_t component_id, const T value) {
			OnComponentID(component_id);
			OnPrimative<T>(value);
		}

		template<class T>
		inline void OnPrimativeArray(const uint16_t component_id, const T* values, const uint32_t size) {
			OnComponentID(component_id);
			OnPrimativeArray<T>(values, size);
		}

	};

	class Reader {
	private:
		Reader(Reader&&) = delete;
		Reader(const Reader&) = delete;
		Reader& operator=(Reader&&) = delete;
		Reader& operator=(const Reader&) = delete;

		InputPipe& _pipe;
	public:
		Reader(InputPipe& pipe);
		~Reader();

		void Read(Parser& dst);
	};

}}

#endif
