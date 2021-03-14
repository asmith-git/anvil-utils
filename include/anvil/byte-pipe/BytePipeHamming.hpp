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
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details The hamming encoded data size must be divisible by 8 bits otherwise an exception will be thrown.
		\see RawHammingOutputPipe
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
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details The hamming encoded data size must be divisible by 8 bits otherwise an exception will be thrown.
		\see RawHammingInputPipe
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

	/*!
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details Uses a PacketInputPipe to guarantee fixed-size memory blocks.
		This adds some additional overhead, use RawHammingInputPipe to avoid.
		\see HammingOutputPipe
	*/
	class HammingInputPipe final : public InputPipe {
	private:
		PacketInputPipe _packet_pipe;
		RawHammingInputPipe _hamming_pipe;
	public:
		HammingInputPipe(InputPipe& downstream_pipe);
		virtual ~HammingInputPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	/*!
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details Uses a PacketInputPipe to guarantee fixed-size memory blocks.
		This adds some additional overhead, use RawHammingOutputPipe to avoid.
		\see HammingInputPipe
	*/
	class HammingOutputPipe final : public OutputPipe {
	private:
		RawHammingOutputPipe _hamming_pipe;
		PacketOutputPipe _packet_pipe;
	public:
		HammingOutputPipe(OutputPipe& downstream_pipe);
		virtual ~HammingOutputPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

}}

#endif
