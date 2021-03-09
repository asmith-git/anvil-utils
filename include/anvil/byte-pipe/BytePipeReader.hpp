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
		Parser() {}
		virtual ~Parser() {}
		virtual Version GetSupportedVersion() const { return VERSION_1; }

		// Basic functionality

		virtual void OnPipeOpen() = 0;
		virtual void OnPipeClose() = 0;

		virtual void OnArrayBegin(const uint32_t size) = 0;
		virtual void OnArrayEnd() = 0;

		virtual void OnObjectBegin(const uint32_t component_count) = 0;
		virtual void OnObjectEnd() = 0;
		virtual void OnComponentID(const uint16_t id) = 0;

		virtual void OnNull() = 0;

		virtual void OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) = 0;

		virtual void OnPrimativeF64(const double value) = 0;
		virtual void OnPrimativeString(const char* value, const uint32_t length) = 0;
		virtual void OnPrimativeC8(const char value) = 0;

		virtual void OnPrimativeU64(const uint64_t value) { OnPrimativeF64(static_cast<double>(value)); }
		virtual void OnPrimativeS64(const int64_t value) { OnPrimativeF64(static_cast<double>(value)); }
		virtual void OnPrimativeF32(const float value) { OnPrimativeF64(value); }
		virtual void OnPrimativeU8(const uint8_t value) { OnPrimativeU64(value); }
		virtual void OnPrimativeU16(const uint16_t value) { OnPrimativeU64(value); }
		virtual void OnPrimativeU32(const uint32_t value) { OnPrimativeU64(value); }
		virtual void OnPrimativeS8(const int8_t value) { OnPrimativeS64(value); }
		virtual void OnPrimativeS16(const int16_t value) { OnPrimativeS64(value); }
		virtual void OnPrimativeS32(const int32_t value) { OnPrimativeS64(value); }
		virtual void OnPrimativeF16(const half value) { OnPrimativeF32(static_cast<float>(value)); } //! \bug half to float conversion not implemented

		// Array Optimisations

		virtual void OnPrimativeArrayU8(const uint8_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeU8(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayU16(const uint16_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeU16(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayU32(const uint32_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeU32(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayU64(const uint64_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeU64(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayS8(const int8_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeS8(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayS16(const int16_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeS16(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayS32(const int32_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeS32(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayS64(const int64_t* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeS64(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayF32(const float* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeF32(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayF64(const double* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeF64(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayC8(const char* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeC8(src[i]);
			Parser::OnArrayEnd();
		}

		virtual void OnPrimativeArrayF16(const half* src, const uint32_t size) {
			Parser::OnArrayBegin(size);
			for (uint32_t i = 0u; i < size; ++i) Parser::OnPrimativeF16(src[i]);
			Parser::OnArrayEnd();
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
