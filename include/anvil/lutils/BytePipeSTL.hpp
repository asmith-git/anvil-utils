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

#ifndef ANVIL_LUTILS_BYTEPIPE_STL_HPP
#define ANVIL_LUTILS_BYTEPIPE_STL_HPP

#include <iostream>
#include "BytePipeReader.hpp"
#include "BytePipeWriter.hpp"

namespace anvil { namespace lutils { namespace BytePipe {

	class IStreamPipe final : public InputPipe {
	private:
		std::istream& _stream;
	public:
		IStreamPipe(std::istream& stream);
		virtual ~IStreamPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	class OStreamPipe final : public OutputPipe {
	private:
		std::ostream& _stream;
	public:
		OStreamPipe(std::ostream& stream);
		virtual ~OStreamPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

}}}

#endif
