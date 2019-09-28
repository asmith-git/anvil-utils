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

#include "anvil/lutils/BytePipeSTL.hpp"

namespace anvil { namespace lutils { namespace BytePipe {
	// IStreamPipe
	
	IStreamPipe::IStreamPipe(std::istream& stream) :
		_stream(stream)
	{}

	IStreamPipe::~IStreamPipe() {

	}

	uint32_t IStreamPipe::ReadBytes(void* dst, const uint32_t bytes) {
		_stream.read(static_cast<char*>(dst), bytes);
		return static_cast<uint32_t>(_stream.gcount());
	}


	// OStreamPipe

	OStreamPipe::OStreamPipe(std::ostream& stream) :
		_stream(stream)
	{}

	OStreamPipe::~OStreamPipe() {
		_stream.flush();
	}

	uint32_t OStreamPipe::WriteBytes(const void* src, const uint32_t bytes) {
		const auto pos = _stream.tellp();
		_stream.write(static_cast<const char*>(src), bytes);
		return static_cast<uint32_t>(_stream.tellp() - pos);
	}

	void OStreamPipe::Flush() {
		_stream.flush();
	}

}}}