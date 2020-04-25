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

#ifndef ANVIL_LUTILS_ASM_OPERATOR2_HPP
#define ANVIL_LUTILS_ASM_OPERATOR2_HPP

#include "anvil/lutils/asm/ASM_Base.hpp"
#include "anvil/lutils/asm/ASM_Blend.hpp"

namespace anvil { namespace lutils { namespace experimental {

	enum Operator {
		OPERATOR_ADD
	};

	// Default Implementations

	template<Operator OP, class T, InstructionSets IS = MinInstructionSet<T>::value>
	struct Operator2Primative;

	template<Operator OP, class T, const uint64_t MASK = DefaultMask<T>::value, InstructionSets IS = MinInstructionSet<T>::value>
	struct Operator2 {
	private:
		const Blend<T, MASK, IS> _blend;
		const Operator2Primative<OP, T, IS> _op;
	public:
		inline T operator()(const T src, const T lhs, const T rhs) const throw() {
			enum : uint64_t { MASK_BOUND = MASK & DefaultMask<T>::value };
			if constexpr (MASK_BOUND == 0u) {
				return src;
			} else if constexpr (MASK_BOUND == DefaultMask<T>::value) {
				return _op(lhs, rhs);
			} else if constexpr(Operator2Primative<OP, T, IS>::optimised_blend_ct) {
				return _op.OptimisedBlendCT<MASK_BOUND>(src, lhs, rhs);
			} else {
				return _blend(src, _op(lhs, rhs));
			}
		}
	};

	template<Operator OP, class T, InstructionSets IS = MinInstructionSet<T>::value>
	struct Operator2RT {
	private:
		const BlendRT<T, IS> _blend;
		const Operator2Primative<OP, T, IS> _op;
		const uint64_t _mask;
	public:
		Operator2RT(const uint64_t mask) :
			_blend(mask),
			_mask(mask)
		{}

		inline T operator()(const T src, const T lhs, const T rhs) const throw() {
			if constexpr(Operator2Primative<OP, T, IS>::optimised_blend_rt) {
				return _op.OptimisedBlendRT(src, lhs, rhs, _mask);
			} else {
				return _blend(src, _op(lhs, rhs));
			}
		}
	};

	template<Operator OP, class T, const uint64_t MASK, InstructionSets IS>
	struct Operator2<OP, std::pair<T, T>, MASK, IS> {
	private:
		enum : uint64_t { MASK2 = MASK >> VectorLength<T>::value };
		const Operator2<OP, T, MASK, IS> _lhs;
		const Operator2<OP, T, MASK2, IS> _rhs;
	public:
		inline std::pair<T, T> operator()(const std::pair<T, T>& src, const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const throw() {
			return {
				_lhs(src.first, lhs.first, rhs.first),
				_rhs(src.second, lhs.second, rhs.second)
			};
		}
	};

	template<Operator OP, class T, InstructionSets IS>
	struct Operator2RT<OP, std::pair<T, T>, IS> {
	private:
		const Operator2RT<OP, T, IS> _lhs;
		const Operator2RT<OP, T, IS> _rhs;
	public:
		Operator2RT(const uint64_t mask) :
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

	template<Operator OP, class T, size_t S, const uint64_t MASK, InstructionSets IS>
	struct Operator2<OP, std::array<T, S>, MASK, IS> {
	private:
		template<size_t I>
		static inline void Execute(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs, std::array<T, S>& out) throw() {
			enum : uint64_t { MASK2 = MASK >> (VectorLength<T>::value * I) };
			out[I] = Operator2<OP, T, MASK2, IS>()(src[I], lhs[I], rhs[I]);
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

	template<Operator OP, class T, size_t S, InstructionSets IS>
	struct Operator2RT<OP, std::array<T, S>, IS> {
	private:
		enum {
			IS_CPP_PRIMATIVE = std::is_integral<T>::value || std::is_floating_point<T>::value
		};

		typedef Operator2RT<OP, T, IS> Operator2Fn;
		union {
			uint8_t _operators[IS_CPP_PRIMATIVE ? 1u : sizeof(Operator2Fn) * S];
			uint64_t _mask;
		};
	public:
		Operator2RT(uint64_t mask) {
			if constexpr (IS_CPP_PRIMATIVE) {
				_mask = mask;
			} else {
				Operator2Fn* const blends = reinterpret_cast<Operator2Fn*>(_operators);
				for (size_t i = 0u; i < S; ++i) {
					new(blends + i) Operator2Fn(mask);
					mask >>= VectorLength<T>::value;
				}
			}
		}

		~Operator2RT() {
			if constexpr(! (std::is_trivially_destructible<Operator2Fn>::value || IS_CPP_PRIMATIVE)) {
				Operator2Fn* const blends = reinterpret_cast<Operator2Fn*>(_operators);
				for (size_t i = 0u; i < S; ++i) {
					blends[i].~Operator2RT();
				}
			}
		}

		std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs) const throw() {
			std::array<T, S> tmp;
			if constexpr (IS_CPP_PRIMATIVE) {
				uint64_t mask = _mask;
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = Operator2Fn(mask)(src[i], lhs[i], rhs[i]);
					mask >>= VectorLength<T>::value;
				}
			} else {
				const Operator2Fn* const blends = reinterpret_cast<const Operator2Fn*>(_operators);
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = blends[i](src[i], lhs[i], rhs[i]);
				}
			}
			return tmp;
		}
	};

}}}

#endif