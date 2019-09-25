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

#ifndef ANVIL_LUTILS_POD_ALGORITHM_HPP
#define ANVIL_LUTILS_POD_ALGORITHM_HPP

#include <type_traits>
#include "anvil/lutils/Alignment.hpp"

namespace anvil { namespace lutils {

	//template<size_t BYTES>
	//static inline void FastMemcpy_avx512(void* const dst, const void* src) throw() {
	//	static_assert((BYTES % 64u) == 0, "Memory block must be a multiple of 64u bytes");
	//	__m512 xmm0;
	//	for (size_t i = 0u; i < BYTES; i += 64u) {
	//		xmm0 = _mm512_loadu_ps(reinterpret_cast<const float*>(static_cast<const uint8_t*>(src) + i));
	//		_mm512_storeu_ps(reinterpret_cast<float*>(static_cast<uint8_t*>(dst) + i), xmm0);
	//	}
	//}

	//template<size_t BYTES>
	//static inline void FastMemcpy_avx(void* const dst, const void* src) throw() {
	//	static_assert((BYTES % 32u) == 0, "Memory block must be a multiple of 32 bytes");
	//	__m256 xmm0;
	//	for (size_t i = 0u; i < BYTES; i += 32u) {
	//		xmm0 = _mm256_loadu_ps(reinterpret_cast<const float*>(static_cast<const uint8_t*>(src) + i));
	//		_mm256_storeu_ps(reinterpret_cast<float*>(static_cast<uint8_t*>(dst) + i), xmm0);
	//	}
	//}

	//template<size_t BYTES>
	//static inline void FastMemcpy_sse(void* const dst, const void* src) throw() {
	//	static_assert((BYTES % 16u) == 0, "Memory block must be a multiple of 16 bytes");
	//	__m128 xmm0;
	//	for (size_t i = 0u; i < BYTES; i += 16u) {
	//		xmm0 = _mm_loadu_ps(reinterpret_cast<const float*>(static_cast<const uint8_t*>(src) + i));
	//		_mm_storeu_ps(reinterpret_cast<float*>(static_cast<uint8_t*>(dst) + i), xmm0);
	//	}
	//}

	template<size_t BYTES>
	static inline void FastMemcpy(void* const dst, const void* src) throw() {
		//if constexpr ((BYTES % 64u) == 0) {
		//	FastMemcpy_avx512(dst, src);
		//} else if constexpr ((BYTES % 32u) == 0) {
		//	FastMemcpy_avx(dst, src);
		//} else if constexpr ((BYTES % 16u) == 0) {
		//	FastMemcpy_sse(dst, src);
		//} else {
			std::memcpy(dst, src, BYTES);
		//}
	}
	
	template<class F, class T>
	static void transform(T* begin, const T* const end, const F& unary_op) {
		while (begin != end) {
			unary_op(*begin);
			++begin;
		}
	}

	template<class F, class T>
	static void transform(const T* input_begin, const T* const input_end, T* output_begin, const F& unary_op) {
		const uint32_t count = static_cast<uint32_t>(input_end - input_begin);
		if (input_begin == output_begin) {
			// Call the in-place transformation algorithm
			transform<F, T>(output_begin, output_begin + count, unary_op);
		} else {
			enum : uint32_t { TARGET_CACHE_BLOCK = CACHE_LINE_BYTES };
			// If working on a small POD type
			if constexpr (std::is_trivially_copyable<T>::value && sizeof(T) < TARGET_CACHE_BLOCK) {
				// Divide the work into blocks of 4 KiB
				enum : uint32_t {
					CACHE_BLOCK_COUNT = TARGET_CACHE_BLOCK / sizeof(T),
					ACTUAL_CACHE_BLOCK = CACHE_BLOCK_COUNT * sizeof(T)
				};

				// Process the alligned blocks
				const uint32_t cacheAligned = count / CACHE_BLOCK_COUNT;
				for (uint32_t i = 0u; i < cacheAligned; ++i) {
					FastMemcpy<ACTUAL_CACHE_BLOCK>(output_begin, input_begin);
					T* const output_end = output_begin + CACHE_BLOCK_COUNT;
					transform<F, T>(output_begin, output_end, unary_op);
					output_begin = output_end;
				}

				// Process a remaining unaligned block if one exists
				const uint32_t cacheUnaligned = count % CACHE_BLOCK_COUNT;
				if (cacheUnaligned != 0u) {
					std::memcpy(output_begin, input_begin, cacheUnaligned * sizeof(T));
					transform<F, T>(output_begin, output_begin + cacheUnaligned, unary_op);
				}
			} else {
				// Process one item at a time with a copy
				for (uint32_t i = 0u; i < count; ++i) {
					output_begin[i] = input_begin[i];
					unary_op(output_begin[i]);
				}
			}
		}
	}
}}

#endif
