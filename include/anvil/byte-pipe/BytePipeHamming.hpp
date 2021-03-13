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

#ifndef ANVIL_LUTILS_BYTEPIPE_HAMMING_HPP
#define ANVIL_LUTILS_BYTEPIPE_HAMMING_HPP

#include "anvil/byte-pipe/BytePipePacket.hpp"

namespace anvil { namespace BytePipe {

	/*!
		\brief Correct 1-bit errors with Hamming(7,4) encoding
		\details Will throw exception if ReadBytes is called with an input size that would output a number of bits not divisibe by 8.
	*/
	class RawHammingInputPipe final : public InputPipe {
	private:
		InputPipe& _downstream_pipe;
	public:
		RawHammingInputPipe(InputPipe& downstream_pipe);
		virtual ~RawHammingInputPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	/*!
		\brief Correct 1-bit errors with Hamming(7,4) encoding
		\details Will throw exception if ReadBytes is called with an input size that would output a number of bits not divisibe by 8.
	*/
	class RawHammingOutputPipe final : public OutputPipe {
	private:
		OutputPipe& _downstream_pipe;
	public:
		RawHammingOutputPipe(OutputPipe& downstream_pipe);
		virtual ~RawHammingOutputPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

}}

#endif
