//Copyright 2020 Adam G. Smith
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

#ifndef ANVIL_LUTILS_ASM_FLOAT32_HPP
#define ANVIL_LUTILS_ASM_FLOAT32_HPP

#include "anvil/lutils/asm/ASM_Base.hpp"

namespace anvil { namespace lutils { namespace experimental {

	// Default Implementations
	template<class T, InstructionSets IS = MinInstructionSet<T>::value>
	struct BlendRT;

	template<const uint64_t MASK, class T, InstructionSets IS = MinInstructionSet<T>::value>
	struct Blend {
	private:
		const BlendRT<T, IS> _blend;
	public:
		Blend() :
			_blend(MASK)
		{}

		inline T operator()(const T src, const T other) const throw() {
			return _blend(src, other);
		}
	};

	template<class T, const uint64_t MASK, InstructionSets IS>
	struct Blend<MASK, std::pair<T, T>, IS> {

		inline std::pair<T, T> operator()(const std::pair<T, T>& src, const std::pair<T, T>& other) const throw() {
			enum : uint64_t { MASK2 = MASK >> VectorLength<T>::value };
			return {
				Blend<MASK, T, IS>()(src.first, other.first),
				Blend<MASK2, T, IS>()(src.second, other.second)
			};
		}
	};

	template<class T, InstructionSets IS>
	struct BlendRT<std::pair<T, T>, IS> {
	private:
		const BlendRT<T, IS> _lhs;
		const BlendRT<T, IS> _rhs;
	public:
		BlendRT(uint64_t mask) :
			_lhs(mask),
			_rhs(mask >> VectorLength<T>::value)
		{}

		inline std::pair<T, T> operator()(const std::pair<T, T>& src, const std::pair<T, T>& other) const throw() {
			return {
				_lhs(src.first, other.first),
				_rhs(src.second, other.second)
			};
		}
	};

	template<class T, size_t S, const uint64_t MASK, InstructionSets IS>
	struct Blend<MASK, std::array<T, S>, IS> {
	private:
		template<size_t I>
		static inline void Execute(const std::array<T, S>& src, const std::array<T, S>& other, std::array<T, S>& out) throw() {
			enum : uint64_t { MASK2 = MASK >> (VectorLength<T>::value * I) };
			out[I] = Blend<MASK2, T, IS>()(src[I], other[I]);
			Execute<I + 1u>(src, other, out);
		}

		template<>
		static inline void Execute<S>(const std::array<T, S>& src, const std::array<T, S>& other, std::array<T, S>& out) throw() {
			// Do nothing
		}
	public:
		std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& other) const throw() {
			std::array<T, S> tmp;
			Execute<0u>(src, other, tmp);
			return tmp;
		}
	};

	template<class T, size_t S, InstructionSets IS>
	struct BlendRT<std::array<T, S>, IS> {
	private:
		enum {
			IS_CPP_PRIMATIVE = std::is_integral<T>::value || std::is_floating_point<T>::value
		};

		typedef BlendRT<T, IS> BlendFn;
		union {
			uint8_t _blends[IS_CPP_PRIMATIVE ? 1u : sizeof(BlendFn) * S];
			uint64_t _mask;
		};
	public:
		BlendRT(uint64_t mask) {
			if constexpr (IS_CPP_PRIMATIVE) {
				_mask = mask;
			} else {
				BlendFn* const blends = reinterpret_cast<BlendFn*>(_blends);
				for (size_t i = 0u; i < S; ++i) {
					new(blends + i) BlendFn(mask);
					mask >>= VectorLength<T>::value;
				}
			}
		}

		~BlendRT() {
			if constexpr(! (std::is_trivially_destructible<BlendFn>::value || IS_CPP_PRIMATIVE)) {
				BlendFn* const blends = reinterpret_cast<BlendFn*>(_blends);
				for (size_t i = 0u; i < S; ++i) {
					blends[i].~BlendRT();
				}
			}
		}

		std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& other) const throw() {
			std::array<T, S> tmp;
			if constexpr (IS_CPP_PRIMATIVE) {
				uint64_t mask = _mask;
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = BlendFn(mask)(src[i], other[i]);
					mask >>= VectorLength<T>::value;
				}
			} else {
				const BlendFn* const blends = reinterpret_cast<const BlendFn*>(_blends);
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = blends[i](src[i], other[i]);
				}
			}
			return tmp;
		}
	};

	// Primative Implementations

	template<const uint64_t MASK, InstructionSets IS>
	struct Blend<MASK, float, IS> {
		inline float operator()(const float src, const float other) const throw() {
			enum : uint64_t { MASK_BOUND = MASK & 1ull };

			if constexpr (MASK_BOUND == 1u) {
				return other;
			} else {
				return src;
			}
		}
	};

	template<InstructionSets IS>
	struct BlendRT<float, IS> {
	private:
		const bool _condition;
	public:
		BlendRT(uint64_t mask) :
			_condition((mask & 1ull) == 0u)
		{}

		inline float operator()(const float src, const float other) const throw() {
			return _condition ? src : other;
		}
	};


	// x86 Implementation

#if ANVIL_EXPERIMENTAL_X86
	namespace detail {

		template<uint64_t MASK, size_t S>
		struct MaskHelper32;

		template<uint64_t MASK>
		struct MaskHelper32<MASK, 4u> {
			enum : int {
				M0 = (MASK & 1ull) == 0ull ? 0 : -1,
				M1 = (MASK & 2ull) == 0ull ? 0 : -1,
				M2 = (MASK & 4ull) == 0ull ? 0 : -1,
				M3 = (MASK & 8ull) == 0ull ? 0 : -1
			};

			static constexpr const int32_t g_mask[4u] = { M0, M1, M2, M3 };
		};

		static constexpr bool UsingSSE41(InstructionSets is) {
			enum : InstructionSets {
				MASK = ~(ANVIL_SSE | ANVIL_SSE2 | ANVIL_SSE3 | ANVIL_SSSE3)
			};
			return (is & MASK) != 0ull;
		}

		

		template<uint64_t MASK, InstructionSets IS, bool = UsingSSE41(IS)>
		struct BlendF32SSE;

		template<uint64_t MASK, InstructionSets IS>
		struct BlendF32SSE<MASK, IS, false> {
		private:
			enum : uint64_t { MASK2 = MASK & DefaultMask<__m128>::value };
			const __m128 _mask;
		public:
			BlendF32SSE() :
				_mask(_mm_loadu_ps(reinterpret_cast<const float*>(MaskHelper32<MASK2, 4u>::g_mask)))
			{}

			inline __m128 operator()(const __m128 src, const __m128 other) const throw() {
				return _mm_or_ps(_mm_andnot_ps(_mask, src), _mm_and_ps(_mask, other));
			}
		};

		template<uint64_t MASK, InstructionSets IS>
		struct BlendF32SSE<MASK, IS, true> {
			inline __m128 operator()(const __m128 src, const __m128 other) const throw() {
				enum : int { MASK2 = static_cast<int>(MASK & DefaultMask<__m128>::value) };
				return _mm_blend_ps(src, other, MASK2);
			}
		};
	}
#endif
	

	template<const uint64_t MASK, InstructionSets IS>
	struct Blend<MASK, __m128, IS> {
	private:
		const detail::BlendF32SSE<MASK, IS> _blend;
	public:
		inline __m128 operator()(const __m128 src, const __m128 other) const throw() {
			return _blend(src, other);
		}
	};

}}}

#endif