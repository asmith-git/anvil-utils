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

#include "anvil/byte-pipe/BytePipeJSON.hpp"

namespace anvil { namespace BytePipe {

	static char ToHex(uint32_t nybble) {
		return nybble <= 9 ?
			'0' + nybble :
			'A' + (nybble - 9);
	}

	static inline void ToHex(uint32_t byte, char* out) {
		out[0u] = ToHex(byte & 15u);
		out[1u] = ToHex(byte >> 4u);
	}

	JsonWriter::JsonWriter() {

	}

	JsonWriter::~JsonWriter() {

	}

	const std::string& JsonWriter::GetJSON() const {
		return _out;
	}

	// Inherited from Parser

	void JsonWriter::OnPipeOpen() {
		// Reset object state
		_out.clear();
	}

	void JsonWriter::OnPipeClose() {
		// Do nothing
	}

	void JsonWriter::OnArrayBegin(const uint32_t size) {
		_out += '[';
	}

	void JsonWriter::OnArrayEnd() {
		if (_out.back() == ',') _out.pop_back();
		_out += ']';
	}

	void JsonWriter::OnObjectBegin(const uint32_t component_count) {
		_out += '{';
	}

	void JsonWriter::OnObjectEnd() {
		if (_out.back() == ',') _out.pop_back();
		_out += '}';
	}

	void JsonWriter::OnComponentID(const ComponentID id) {
		_out += '"' + std::to_string(id) + "\":";
	}


	void JsonWriter::AddValue(Type type, std::string&& val) {
		if (_out.empty()) {
			_out.swap(val);
		} else {
			_out += val;
		}
		_out += ",";
	}

	void JsonWriter::OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) {
		// Format the POD as an object, a POD is identified by containg the member __ANVIL_POD with the value 123456789
		std::string value = "{\"__ANVIL_POD\":123456789,\"type\":" + std::to_string(type) + ",\data\":\"";

		// Store the binary data as hexidecimal
		char buffer[3u] = "??";
		for (uint32_t i = 0u; i < bytes; ++i) {
			ToHex(reinterpret_cast<const uint8_t*>(data)[i], buffer);
			value += buffer;
		}
		value += "\"}";
		
		// Add the value
		AddValue(TYPE_OBJECT, std::move(value));
	}

	void JsonWriter::OnNull() {
		AddValue(TYPE_NULL, "null");
	}

	void JsonWriter::OnPrimativeF64(const double value) {
		AddValue(TYPE_F64, std::to_string(value));
	}

	void JsonWriter::OnPrimativeString(const char* value, const uint32_t length) {
		AddValue(TYPE_STRING, std::string(value, value + length));
	}

	void JsonWriter::OnPrimativeBool(const bool value) {
		AddValue(TYPE_BOOL, value ? "True" : "False");
	}

	void JsonWriter::OnPrimativeC8(const char value) {
		OnPrimativeString(&value, 1u);
	}

	void JsonWriter::OnPrimativeU64(const uint64_t value) {
		AddValue(TYPE_U64, std::to_string(value));
	}

	void JsonWriter::OnPrimativeS64(const int64_t value) {
		AddValue(TYPE_S64, std::to_string(value));
	}

	void JsonWriter::OnPrimativeF32(const float value) {
		AddValue(TYPE_F64, std::to_string(value));
	}

	void JsonWriter::OnPrimativeU8(const uint8_t value) {
		AddValue(TYPE_U8, std::to_string(value));
	}

	void JsonWriter::OnPrimativeU16(const uint16_t value) {
		AddValue(TYPE_U16, std::to_string(value));
	}

	void JsonWriter::OnPrimativeU32(const uint32_t value) {
		AddValue(TYPE_U32, std::to_string(value));
	}

	void JsonWriter::OnPrimativeS8(const int8_t value) {
		AddValue(TYPE_S8, std::to_string(value));
	}

	void JsonWriter::OnPrimativeS16(const int16_t value) {
		AddValue(TYPE_S16, std::to_string(value));
	}

	void JsonWriter::OnPrimativeS32(const int32_t value) {
		AddValue(TYPE_S32, std::to_string(value));
	}

}}