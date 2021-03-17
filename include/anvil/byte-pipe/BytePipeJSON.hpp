//Copyright 2021 Adam G. Smith
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

#ifndef ANVIL_LUTILS_BYTEPIPE_JSON_HPP
#define ANVIL_LUTILS_BYTEPIPE_JSON_HPP

#include <vector>
#include <string>
#include "anvil/byte-pipe/BytePipeReader.hpp"

namespace anvil { namespace BytePipe {

	/*!
		\author Adam Smith
		\date March 2021
		\brief Converts a BytePipe serialisation into a JSON string
	*/
	class JsonWriter final : public Parser {
	private:
		mutable std::string _out;
	private:
		void AddValue(const std::string& val);
		void AddValueC(const char* val);
	public:
		JsonWriter();
		virtual ~JsonWriter();

		const std::string& GetJSON() const;

		// Inherited from Parser

		void OnPipeOpen() final;
		void OnPipeClose() final;
		void OnArrayBegin(const uint32_t size) final;
		void OnArrayEnd() final;
		void OnObjectBegin(const uint32_t component_count) final;
		void OnObjectEnd() final;
		void OnComponentID(const ComponentID id)  final;
		void OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) final;
		void OnNull() final;
		void OnPrimativeF64(const double value) final;
		void OnPrimativeString(const char* value, const uint32_t length) final;
		void OnPrimativeBool(const bool value) final;
		void OnPrimativeC8(const char value) final;
		void OnPrimativeU64(const uint64_t value) final;
		void OnPrimativeS64(const int64_t value) final;
		void OnPrimativeF32(const float value) final;
		void OnPrimativeU8(const uint8_t value) final;
		void OnPrimativeU16(const uint16_t value) final;
		void OnPrimativeU32(const uint32_t value) final;
		void OnPrimativeS8(const int8_t value) final;
		void OnPrimativeS16(const int16_t value) final;
		void OnPrimativeS32(const int32_t value) final;
	};

}}

#endif
