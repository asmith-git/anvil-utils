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

	// Example block sizes (in bytes) for Hamming (7,4) = {16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200}
	// Example block sizes (in bytes) for Hamming (15,11) = {88, 176, 264, 352, 440, 528}
	// Example block sizes (in bytes) for Hamming (31,26) = {104, 208, 312, 416, 520, 624, 728, 832, 936, 1040, 1144, 1248}

	/*!
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details The number of bits sent to ReadBytes() must be divisble by 4 otherwise an exception will be thrown.
		\see RawHamming74OutputPipe
	*/
	class RawHamming74InputPipe final : public InputPipe {
	private:
		InputPipe& _downstream_pipe;
	public:
		RawHamming74InputPipe(InputPipe& downstream_pipe);
		virtual ~RawHamming74InputPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	/*!
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details The number of bits sent to WriteBytes() must be divisble by 4 otherwise an exception will be thrown.
		\see RawHamming74InputPipe
	*/
	class RawHamming74OutputPipe final : public OutputPipe {
	private:
		OutputPipe& _downstream_pipe;
	public:
		RawHamming74OutputPipe(OutputPipe& downstream_pipe);
		virtual ~RawHamming74OutputPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

	/*!
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details Uses a PacketInputPipe to guarantee fixed-size memory blocks.
		This adds some additional overhead, use RawHamming74InputPipe to avoid.
		\see Hamming74OutputPipe
	*/
	class Hamming74InputPipe final : public InputPipe {
	private:
		PacketInputPipe _packet_pipe;
		RawHamming74InputPipe _hamming_pipe;
	public:
		Hamming74InputPipe(InputPipe& downstream_pipe);
		virtual ~Hamming74InputPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	/*!
		\brief Data is encoded with Hamming(7,4) to correct single bit errors.
		\details Uses a PacketInputPipe to guarantee fixed-size memory blocks.
		This adds some additional overhead, use RawHamming74OutputPipe to avoid.
		\see Hamming74InputPipe
	*/
	class Hamming74OutputPipe final : public OutputPipe {
	private:
		RawHamming74OutputPipe _hamming_pipe;
		PacketOutputPipe _packet_pipe;
	public:
		Hamming74OutputPipe(OutputPipe& downstream_pipe, uint32_t block_size = 256);
		virtual ~Hamming74OutputPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

	/*!
		\brief Data is encoded with extended Hamming(15,11) to correct single bit errors and detect two bit errors.
		\details The number of bits sent to WriteBytes() must be divisble by 11 otherwise an exception will be thrown.
		\see RawHamming1511InputPipe
	*/
	class RawHamming1511OutputPipe final : public OutputPipe {
	private:
		OutputPipe& _downstream_pipe;
	public:
		RawHamming1511OutputPipe(OutputPipe& downstream_pipe);
		virtual ~RawHamming1511OutputPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

	/*!
		\brief Data is encoded with extended Hamming(15,11) to correct single bit errors and detect two bit errors.
		\details The number of bits sent to ReadBytes() must be divisble by 11 otherwise an exception will be thrown.
		\see RawHamming1511OutputPipe
	*/
	class RawHamming1511InputPipe final : public InputPipe {
	private:
		InputPipe& _downstream_pipe;
	public:
		RawHamming1511InputPipe(InputPipe& downstream_pipe);
		virtual ~RawHamming1511InputPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	/*!
		\brief Data is encoded with extended Hamming(15,11) to correct single bit errors and detect two bit errors.
		\details Uses a PacketInputPipe to guarantee fixed-size memory blocks.
		This adds some additional overhead, use RawHamming1511InputPipe to avoid.
		\see Hamming1511OutputPipe
	*/
	class Hamming1511InputPipe final : public InputPipe {
	private:
		PacketInputPipe _packet_pipe;
		RawHamming1511InputPipe _hamming_pipe;
	public:
		Hamming1511InputPipe(InputPipe& downstream_pipe);
		virtual ~Hamming1511InputPipe();
		uint32_t ReadBytes(void* dst, const uint32_t bytes) final;
	};

	/*!
		\brief Data is encoded with extended Hamming(15,11) to correct single bit errors and detect two bit errors.
		\details Uses a PacketInputPipe to guarantee fixed-size memory blocks.
		This adds some additional overhead, use RawHamming1511OutputPipe to avoid.
		\see Hamming1511InputPipe
	*/
	class Hamming1511OutputPipe final : public OutputPipe {
	private:
		RawHamming1511OutputPipe _hamming_pipe;
		PacketOutputPipe _packet_pipe;
	public:
		Hamming1511OutputPipe(OutputPipe& downstream_pipe, uint32_t block_size = 264);
		virtual ~Hamming1511OutputPipe();
		uint32_t WriteBytes(const void* src, const uint32_t bytes) final;
		void Flush() final;
	};

}}

#endif
