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

#ifndef ANVIL_LUTILS_BYTEPIPE_RLE_HPP
#define ANVIL_LUTILS_BYTEPIPE_RLE_HPP

#include <iostream>
#include "anvil/byte-pipe/BytePipeReader.hpp"
#include "anvil/byte-pipe/BytePipeWriter.hpp"

namespace anvil { namespace BytePipe {

	/*!
		\page RLE (Run-length encoding)
		\details
		The RLE encoder / decoder pipes have two template types, LengthWord and DataWord.
		DataWord controls the size of the words that the RLE algorithm looks at, this can be 1, 2, 4 or 8 bytes.
		Length word determines the maximum number of words that can be in each RLE block.
		The last bit of the length word is reserved to signal which mode the block in.
		A 1 signals that the block only stores one word, but in the original data there
		were N copies of it in total.
		A 0 signals that there are N words in the block that do not have any special
		encoding.
	*/

	template<class LengthWord = uint16_t, class DataWord = uint8_t>
	class RLEEncoderPipe final : public OutputPipe {
	private:
		static_assert(std::is_unsigned<LengthWord>::value, "LengthWord must be an unsigned integer");
		static_assert(std::is_unsigned<DataWord>::value, "DataWord must be an unsigned integer");

		enum : LengthWord {
			RLE_FLAG = 1 << (sizeof(LengthWord) * 8 - 1),
			MAX_RLE_LENGTH = static_cast<LengthWord>(-1) >> 1
		};

		OutputPipe& _output;
		DataWord* _buffer;
		DataWord _current_word;
		LengthWord _length;
		bool _rle_mode;

		bool _Flush() {
			if (_length > 0u) {
				LengthWord len = _length;
				if (_rle_mode) {
					// Write the block
					len |= RLE_FLAG;

					// Combine the write into one call
					uint8_t mem[sizeof(LengthWord) + sizeof(DataWord)];
					*reinterpret_cast<LengthWord*>(mem) = len;
					*reinterpret_cast<DataWord*>(mem + sizeof(LengthWord)) = _current_word;
					_output.WriteBytes(mem, sizeof(LengthWord) + sizeof(DataWord));
				} else {
					// Write the buffer
					_output.WriteBytes(&len, sizeof(LengthWord));
					_output.WriteBytes(_buffer, _length * sizeof(DataWord));
				}

				// Reset the encoder state
				_current_word = 0u;
				_length = 0u;
				_rle_mode = false;
				return true;
			}

			return false;
		}

		void WriteWordRLE(const DataWord word) {
			// If the current RLE block is full then flush the data
			if (_length == MAX_RLE_LENGTH) {
NEW_BLOCK:
				_Flush();
				// Flush the current block and write as aif it was a non-repeating block
				WriteWordNonRLE(word);
				return;
			}
			
			if (_length == 0u) {
				// Start a new repeating block
				_current_word = word;
				_length = 1u;
				_rle_mode = true;
			} else if (word == _current_word) {
				// Add the word to the repeat
				++_length;
			} else {
				goto NEW_BLOCK;
			}
		}

		void WriteWordNonRLE(const DataWord word) {
			// If the current RLE block is full then flush the data
			if (_length == MAX_RLE_LENGTH) _Flush();

			if (_length > 0u) {
				// If the word is the same as the last word in the buffer
				if (_buffer[_length - 1u] == word) {
					// Flush the current block except for the last word
					--_length;
					_Flush();

					// Start a new RLE block
					_current_word = word;
					_length = 2u;
					_rle_mode = true;
					return;
				}
			}

			// Add word to buffer
			_buffer[_length++] = word;
		}

		void WriteWord(const DataWord word) {
			// If the current block is repeating
			if (_rle_mode) {
				WriteWordRLE(word);
			} else {
				WriteWordNonRLE(word);
			}
		}

		void WriteWord4(const uint32_t word) {
			if ANVIL_CONSTEXPR (std::is_same<DataWord, uint8_t>::value) {

				const uint32_t w1 = word & 255u;
				const uint32_t w2 = (word >> 8u) & 255u;
				const uint32_t w3 = (word >> 16u) & 255u;
				const uint32_t w4 = word >> 24u;


				// If all words are the same
				if (((w1 ^ w2) | (w1 ^ w3) | (w1 ^ w4)) == 0u) {
					// If the currently block isn't repeating or is repeating a different word then flush it
					if (_length > 0u) {
						if (_rle_mode) {
							if (w1 != _current_word) {
							FLUSH:
								// Flush the current block
								_Flush();
							}
						} else {
							goto FLUSH;
						}
					}

					_rle_mode = true;
					_current_word = static_cast<DataWord>(w1);
					if (_length < MAX_RLE_LENGTH - 4u) {
						// Add all 4 words to the block at once
						_length += 4u;
					} else {
						// Add the words individually
						WriteWordRLE(static_cast<DataWord>(w1));
						WriteWordRLE(static_cast<DataWord>(w2));
						WriteWordRLE(static_cast<DataWord>(w3));
						WriteWordRLE(static_cast<DataWord>(w4));
					}
					return;
				}

				// Add the words individually
				WriteWord(static_cast<DataWord>(w1));
				WriteWord(static_cast<DataWord>(w2));
				WriteWord(static_cast<DataWord>(w3));
				WriteWord(static_cast<DataWord>(w4));
			} else {
				throw std::runtime_error("RLEEncoderPipe::WriteWord4 : Only implemented for 1 byte words");
			}
		}

		void WriteWord8(const uint64_t word) {
			if ANVIL_CONSTEXPR (std::is_same<DataWord, uint8_t>::value) {

				const uint64_t w1 = word & 255ull;
				const uint64_t w2 = (word >> 8ull) & 255ull;
				const uint64_t w3 = (word >> 16ull) & 255ull;
				const uint64_t w4 = (word >> 24ull) & 255ull;
				const uint64_t w5 = (word >> 32ull) & 255ull;
				const uint64_t w6 = (word >> 40ull) & 255ull;
				const uint64_t w7 = (word >> 48ull) & 255ull;
				const uint64_t w8 = word >> 56ull;

				// If all words are the same
				if (((w1 ^ w2) | (w1 ^ w3) | (w1 ^ w4) | (w1 ^ w5) | (w1 ^ w6) | (w1 ^ w7) | (w1 ^ w8)) == 0u) {
					// If the currently block isn't repeating or is repeating a different word then flush it
					if (_length > 0u) {
						if (_rle_mode) {
							if (w1 != _current_word) {
							FLUSH:
								// Flush the current block
								_Flush();
							}
						} else {
							goto FLUSH;
						}
					}

					_rle_mode = true;
					_current_word = static_cast<DataWord>(w1);
					if (_length < MAX_RLE_LENGTH - 8u) {
						// Add all 4 words to the block at once
						_length += 8u;
					} else {
						// Add the words individually
						WriteWordRLE(static_cast<DataWord>(w1));
						WriteWordRLE(static_cast<DataWord>(w2));
						WriteWordRLE(static_cast<DataWord>(w3));
						WriteWordRLE(static_cast<DataWord>(w4));
						WriteWordRLE(static_cast<DataWord>(w5));
						WriteWordRLE(static_cast<DataWord>(w6));
						WriteWordRLE(static_cast<DataWord>(w7));
						WriteWordRLE(static_cast<DataWord>(w8));
					}
					return;
				}

				// Add the words individually
				WriteWord(static_cast<DataWord>(w1));
				WriteWord(static_cast<DataWord>(w2));
				WriteWord(static_cast<DataWord>(w3));
				WriteWord(static_cast<DataWord>(w4));
				WriteWord(static_cast<DataWord>(w5));
				WriteWord(static_cast<DataWord>(w6));
				WriteWord(static_cast<DataWord>(w7));
				WriteWord(static_cast<DataWord>(w8));
			} else {
				throw std::runtime_error("RLEEncoderPipe::WriteWord8 : Only implemented for 1 byte words");
			}
		}
	public:
		RLEEncoderPipe(OutputPipe& output) :
			_output(output),
			_buffer(new DataWord[MAX_RLE_LENGTH]),
			_current_word(0u),
			_length(0u),
			_rle_mode(false)
		{}

		~RLEEncoderPipe() {
			if (_Flush()) _output.Flush();
			delete[] _buffer;
			_buffer = nullptr;
		}


		uint32_t WriteBytes(const void* src, const uint32_t bytes) final {
			uint32_t words = bytes / sizeof(DataWord);
			if (words * sizeof(DataWord) != bytes) throw std::runtime_error("RLEEncoderPipe::WriteBytes : Byte count is not divisible by the word size");

			const DataWord* wordPtr = static_cast<const DataWord*>(src);


			// Optimise memory reads for 1 byte data
			if ANVIL_CONSTEXPR (std::is_same<DataWord, uint8_t>::value) {
				if ANVIL_CONSTEXPR (sizeof(intptr_t) >= 8u) {
					while (words >= 8u) {
						WriteWord8(*reinterpret_cast<const uint64_t*>(wordPtr));
						wordPtr += 8u;
						words -= 8u;
					}
				} else {
					while (words >= 4u) {
						WriteWord4(*reinterpret_cast<const uint32_t*>(wordPtr));
						wordPtr += 4u;
						words -= 4u;
					}
				}
			}

			for (uint32_t i = 0; i < words; ++i) {
				WriteWord(wordPtr[i]);
			}

			return bytes;
		}

		void Flush() final {
			if (_Flush()) _output.Flush();
		}
	};

	

	template<class LengthWord = uint16_t, class DataWord = uint8_t>
	class RLEDecoderPipe final : public InputPipe {
	private:
		static_assert(std::is_unsigned<LengthWord>::value, "LengthWord must be an unsigned integer");
		static_assert(std::is_unsigned<DataWord>::value, "DataWord must be an unsigned integer");

		enum : LengthWord {
			RLE_FLAG = 1 << (sizeof(LengthWord) * 8 - 1),
			MAX_RLE_LENGTH = static_cast<LengthWord>(-1) >> 1
		};

		InputPipe& _input;
		DataWord* _buffer;
		LengthWord _length;
		DataWord _repeat_word;
		bool _rle_mode;

		void ReadNextBlock() {
			// Read the length of the block
			LengthWord len = 0u;
			uint32_t bytes_read = _input.ReadBytes(&len, sizeof(LengthWord));
			if (bytes_read != sizeof(LengthWord)) throw std::runtime_error("RLEDecoderPipe::ReadNextBlock : Failed to read block length");

			// If the block is repeated word
			if (len & RLE_FLAG) {
				len &= ~RLE_FLAG;
				_length = len;

				_rle_mode = true;
				_repeat_word = 0u;
				
				// Read the word
				bytes_read = _input.ReadBytes(&_repeat_word, sizeof(DataWord));
				if (bytes_read != sizeof(DataWord)) throw std::runtime_error("RLEDecoderPipe::ReadNextBlock : Failed to repeated word");
			} else {
				_length = len;
				_rle_mode = false;
				bytes_read = _input.ReadBytes(_buffer, _length * sizeof(DataWord));
				if (bytes_read != _length * sizeof(DataWord)) throw std::runtime_error("RLEDecoderPipe::ReadNextBlock : Failed to non-repeating words");
			}
		}
	public:
		RLEDecoderPipe(InputPipe& input) :
			_input(input),
			_buffer(new DataWord[MAX_RLE_LENGTH]),
			_repeat_word(0u),
			_length(0u),
			_rle_mode(false)
		{}

		~RLEDecoderPipe() {
			delete[] _buffer;
			_buffer = nullptr;
		}


		uint32_t ReadBytes(void* dst, const uint32_t bytes) final{
			uint32_t words = bytes / sizeof(DataWord);
			if (words * sizeof(DataWord) != bytes) throw std::runtime_error("RLEDecoderPipe::ReadBytes : Byte count is not divisible by the word size");

			DataWord* wordPtr = static_cast<DataWord*>(dst);
			uint32_t wordsToRead = 0u;

			while (words != 0u) {
				if (_length == 0u) ReadNextBlock();
				wordsToRead = words < _length ? words : _length;

				if (_rle_mode) {
					for (uint32_t i = 0u; i < wordsToRead; ++i) wordPtr[i] = _repeat_word;
				} else {
					memcpy(wordPtr, _buffer, sizeof(DataWord) * wordsToRead);
				}

				_length -= wordsToRead;
				words -= wordsToRead;
				wordPtr += wordsToRead;
			}

			return bytes;
		}
	};

}}

#endif
