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

#ifndef ANVIL_LUTILS_ASM_ADD_HPP
#define ANVIL_LUTILS_ASM_ADD_HPP

#include "anvil/lutils/asm/ASM_Base.hpp"
#include "anvil/lutils/asm/ASM_Blend.hpp"

namespace anvil { namespace lutils { namespace experimental {

	// Default Implementations
	template<class T, InstructionSets IS = MinInstructionSet<T>::value>
	struct AddRT;

	template<const uint64_t MASK, class T, InstructionSets IS = MinInstructionSet<T>::value>
	struct Add {
	private:
		const AddRT<T, IS> _add;
	public:
		Add() :
			_add(MASK)
		{}

		inline T operator()(const T src, const T lhs, const T rhs) const throw() {
			return _add(src, lhs, rhs);
		}
	};

	template<class T, const uint64_t MASK, InstructionSets IS>
	struct Add<MASK, std::pair<T, T>, IS> {
	private:
		enum : uint64_t { MASK2 = MASK >> VectorLength<T>::value };
		const Add<MASK2, T, IS> _lhs;
		const Add<MASK2, T, IS> _rhs;
	public:
		inline std::pair<T, T> operator()(const std::pair<T, T>& src, const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const throw() {
			return {
				_lhs(src.first, lhs.first, rhs.first),
				_rhs(src.second, lhs.second, rhs.second)
			};
		}
	};

	template<class T, InstructionSets IS>
	struct AddRT<std::pair<T, T>, IS> {
	private:
		const AddRT<T, IS> _lhs;
		const AddRT<T, IS> _rhs;
	public:
		AddRT(uint64_t mask) :
			_lhs(mask),
			_rhs(mask >> VectorLength<T>::value)
		{}

		inline std::pair<T, T> operator()(const std::pair<T, T>& src, const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const throw() {
			return {
				_lhs(src.first, lhs.first, rhs.first),
				_rhs(src.second, lhs.second, rhs.second)
			};
		}
	};

	template<class T, size_t S, const uint64_t MASK, InstructionSets IS>
	struct Add<MASK, std::array<T, S>, IS> {
	private:
		template<size_t I>
		static inline void Execute(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs, std::array<T, S>& out) throw() {
			enum : uint64_t { MASK2 = MASK >> (VectorLength<T>::value * I) };
			out[I] = Add<MASK2, T, IS>()(src[I], lhs[I], rhs[I]);
			Execute<I + 1u>(src, lhs, rhs, out);
		}

		template<>
		static inline void Execute<S>(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs, std::array<T, S>& out) throw() {
			// Do nothing
		}
	public:
		std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs) const throw() {
			std::array<T, S> tmp;
			Execute<0u>(src, lhs, rhs, tmp);
			return tmp;
		}
	};

	template<class T, size_t S, InstructionSets IS>
	struct AddRT<std::array<T, S>, IS> {
	private:
		enum {
			IS_CPP_PRIMATIVE = std::is_integral<T>::value || std::is_floating_point<T>::value
		};

		typedef AddRT<T, IS> AddFn;
		union {
			uint8_t _adds[IS_CPP_PRIMATIVE ? 1u : sizeof(AddFn) * S];
			uint64_t _mask;
		};
	public:
		AddRT(uint64_t mask) {
			if constexpr (IS_CPP_PRIMATIVE) {
				_mask = mask;
			} else {
				AddFn* const blends = reinterpret_cast<AddFn*>(_adds);
				for (size_t i = 0u; i < S; ++i) {
					new(blends + i) AddFn(mask);
					mask >>= VectorLength<T>::value;
				}
			}
		}

		~AddRT() {
			if constexpr(! (std::is_trivially_destructible<AddFn>::value || IS_CPP_PRIMATIVE)) {
				AddFn* const blends = reinterpret_cast<AddFn*>(_adds);
				for (size_t i = 0u; i < S; ++i) {
					blends[i].~AddRT();
				}
			}
		}

		std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs) const throw() {
			std::array<T, S> tmp;
			if constexpr (IS_CPP_PRIMATIVE) {
				uint64_t mask = _mask;
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = AddFn(mask)(src[i], lhs[i], rhs[i]);
					mask >>= VectorLength<T>::value;
				}
			} else {
				const AddFn* const blends = reinterpret_cast<const AddFn*>(_adds);
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = blends[i](src[i], lhs[i], rhs[i]);
				}
			}
			return tmp;
		}
	};

	// Primative Implementations

	template<const uint64_t MASK, InstructionSets IS>
	struct Add<MASK, float, IS> {
		inline float operator()(const float src, const float lhs, const float rhs) const throw() {
			enum : uint64_t { MASK_BOUND = MASK & 1ull };

			if constexpr (MASK_BOUND == 0u) {
				return src;
			} else {
				return lhs + rhs;
			}
		}
	};

	template<InstructionSets IS>
	struct AddRT<float, IS> {
	private:
		const bool _condition;
	public:
		AddRT(uint64_t mask) :
			_condition((mask & 1ull) == 0u)
		{}

		inline float operator()(const float src, const float lhs, const float rhs) const throw() {
			return _condition ? src : lhs + rhs;
		}
	};

	// x86 Implementation

#if ANVIL_EXPERIMENTAL_X86
	//! \todo Optimise for AVX512

	template<const uint64_t MASK, InstructionSets IS>
	struct Add<MASK, __m128, IS> {
	private:
		Blend<MASK, __m128, IS> _blend;
	public:
		inline __m128 operator()(const __m128 src, const __m128 lhs, const __m128 rhs) const throw() {
			return _blend(src, _mm_add_ps(lhs, rhs));
		}
	};

	template<InstructionSets IS>
	struct AddRT<__m128, IS> {
	private:
		BlendRT<__m128, IS> _blend;
	public:
		AddRT(uint64_t mask) :
			_blend(mask)
		{}

		inline __m128 operator()(const __m128 src, const __m128 lhs, const __m128 rhs) const throw() {
			return _blend(src, _mm_add_ps(lhs, rhs));
		}
	};
#endif

}}}

#endif