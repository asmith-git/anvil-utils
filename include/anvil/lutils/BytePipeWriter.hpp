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

#ifndef ANVIL_LUTILS_BYTEPIPE_WRITER_HPP
#define ANVIL_LUTILS_BYTEPIPE_WRITER_HPP

#include <vector>
#include "BytePipeReader.hpp"

namespace anvil { namespace lutils { namespace BytePipe {

	class OutputPipe {
	public:
		virtual ~OutputPipe() {}
		virtual uint32_t WriteBytes(const void* src, const uint32_t bytes) = 0;
		virtual void Flush() = 0;
	};

	class Writer final : public ParserV1 {
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

		State GetCurrentState() const;
		void Write(const void* src, const uint32_t bytes);
	public:
		Writer(OutputPipe& pipe);
		virtual ~Writer();

		// Inherited from ParserV1

		void OnPipeOpen() final;
		void OnPipeClose() final;
		void OnArrayBegin(const uint32_t size)  final;
		void OnArrayEnd() final;
		void OnObjectBegin(const uint32_t component_count) final;
		void OnObjectEnd() final;
		void OnComponentID(const uint16_t id) final;
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
	};

}}}

#endif
