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

#ifndef ANVILBYTEPIPE_WRITER_HPP
#define ANVILBYTEPIPE_WRITER_HPP

#include <vector>
#include "anvil/byte-pipe/BytePipeReader.hpp"

namespace anvil { namespace BytePipe {

	/*!
		\author Adam Smtih
		\date September 2019
		\brief An output stream for binary data.
		\see InputPipe
	*/
	class OutputPipe {
	public:
		virtual ~OutputPipe() {}
		virtual uint32_t WriteBytes(const void* src, const uint32_t bytes) = 0;
		virtual void Flush() = 0;
	};

	/*!
		\author Adam Smtih
		\date September 2019
		\brief Writes binary serialised data into an OutputPipe
		\see Reader
	*/
	class Writer final : public Parser {
	private:
		Writer(Writer&&) = delete;
		Writer(const Writer&) = delete;
		Writer& operator=(Writer&&) = delete;
		Writer& operator=(const Writer&) = delete;

		enum State : uint8_t {
			STATE_CLOSED,
			STATE_NORMAL,
			STATE_ARRAY,
			STATE_OBJECT
		};

		OutputPipe& _pipe;
		std::vector<State> _state_stack;
		State _default_state;
		Version _version;

		State GetCurrentState() const;
		void Write(const void* src, const uint32_t bytes);
		void _OnPrimative(const uint64_t value, const uint32_t bytes, const uint8_t id);
		void _OnPrimativeArray(const void* ptr, const uint32_t size, const uint8_t id);
	public:
		Writer(OutputPipe& pipe);
		Writer(OutputPipe& pipe, Version version);
		virtual ~Writer();

		// Inherited from Parser

		void OnPipeOpen() final;
		void OnPipeClose() final;
		void OnArrayBegin(const uint32_t size)  final;
		void OnArrayEnd() final;
		void OnObjectBegin(const uint32_t component_count) final;
		void OnObjectEnd() final;
		void OnComponentID(const uint16_t id) final;
		void OnNull() final;
		void OnPrimativeF64(const double value) final;
		void OnPrimativeString(const char* value, const uint32_t length) final;
		void OnPrimativeU64(const uint64_t value) final;
		void OnPrimativeS64(const int64_t value) final;
		void OnPrimativeF32(const float value) final;
		void OnPrimativeU8(const uint8_t value) final;
		void OnPrimativeU16(const uint16_t value) final;
		void OnPrimativeU32(const uint32_t value) final;
		void OnPrimativeS8(const int8_t value) final;
		void OnPrimativeS16(const int16_t value) final;
		void OnPrimativeS32(const int32_t value) final;
		void OnPrimativeC8(const char value) final;
		void OnPrimativeF16(const half value) final;

		void OnPrimativeArrayU8(const uint8_t* src, const uint32_t size) final;
		void OnPrimativeArrayU16(const uint16_t* src, const uint32_t size) final;
		void OnPrimativeArrayU32(const uint32_t* src, const uint32_t size) final;
		void OnPrimativeArrayU64(const uint64_t* src, const uint32_t size) final;
		void OnPrimativeArrayS8(const int8_t* src, const uint32_t size) final;
		void OnPrimativeArrayS16(const int16_t* src, const uint32_t size) final;
		void OnPrimativeArrayS32(const int32_t* src, const uint32_t size) final;
		void OnPrimativeArrayS64(const int64_t* src, const uint32_t size) final;
		void OnPrimativeArrayF32(const float* src, const uint32_t size) final;
		void OnPrimativeArrayF64(const double* src, const uint32_t size) final;
		void OnPrimativeArrayC8(const char* src, const uint32_t size) final;
		void OnPrimativeArrayF16(const half* src, const uint32_t size) final;

		void OnUserPOD(const uint32_t type, const uint32_t bytes, const void* data) final;
	};

}}

#endif
