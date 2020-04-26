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

	template<Operator OP, class T, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	struct Operator2Primative;

	// Default

	namespace detail {
		template<class OP, class T, uint64_t MASK, InstructionSets IS, bool OPTIMISED = OP::optimised_blend_ct>
		struct Operator2_;

		template<class OP, class T, uint64_t MASK, InstructionSets IS>
		struct Operator2_<OP, T, MASK, IS, false> {
		private:
			const Blend<T, MASK, IS> _blend;
		public:
			const OP op;

			inline T operator()(const T src, const T lhs, const T rhs) const throw() {
				return _blend(src, op(lhs, rhs));
			}
		};

		template<class OP, class T, uint64_t MASK, InstructionSets IS>
		struct Operator2_<OP, T, MASK, IS, true> {
			const OP op;

			inline T operator()(const T src, const T lhs, const T rhs) const throw() {
				return op.OptimisedBlendCT<MASK>(src, lhs, rhs);
			}
		};
	}

	template<Operator OP, class T, const uint64_t MASK = DefaultMask<T>::value, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	struct Operator2 {
	private:
		detail::Operator2_<Operator2Primative<OP, T, IS>, T, MASK, IS> _op;
	public:
		inline T operator()(const T src, const T lhs, const T rhs) const throw() {
			enum : uint64_t { MASK_BOUND = MASK & DefaultMask<T>::value };
			if constexpr (MASK_BOUND == 0u) {
				return src;
			} else if constexpr (MASK_BOUND == DefaultMask<T>::value) {
				return _op.op(lhs, rhs);
			} else {
				return _op(src, lhs, rhs);
			}

		}
	};

	namespace detail {
		template<class OP, class T, InstructionSets IS, bool OPTIMISED = OP::optimised_blend_rt>
		struct Operator2RT_;

		template<class OP, class T, InstructionSets IS>
		struct Operator2RT_<OP, T, IS, false> {
		private:
			const BlendRT<T, IS> _blend;
			const OP _op;
		public:
			Operator2RT_(const uint64_t mask) :
				_blend(mask)
			{}

			inline T operator()(const T src, const T lhs, const T rhs) const throw() {
				return _blend(src, _op(lhs, rhs));
			}
		};

		template<class OP, class T, InstructionSets IS>
		struct Operator2RT_<OP, T, IS, true> {
		private:
			const OP _op;
			const uint64_t _mask;
		public:
			Operator2RT_(const uint64_t mask) :
				_mask(mask)
			{}

			inline T operator()(const T src, const T lhs, const T rhs) const throw() {
				return _op.OptimisedBlendRT(src, lhs, rhs, _mask);
			}
		};
	}

	template<Operator OP, class T, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	struct Operator2RT {
	private:
		detail::Operator2RT_<Operator2Primative<OP, T, IS>, T, IS> _op;
	public:
		Operator2RT(const uint64_t mask) :
			_op(mask)
		{}

		inline T operator()(const T src, const T lhs, const T rhs) const throw() {
			return _op(src, lhs, rhs);
		}
	};

	// std::pair

	template<Operator OP, class T, const uint64_t MASK, InstructionSets IS>
	struct Operator2<OP, std::pair<T, T>, MASK, IS> {
	private:
		enum : uint64_t { MASK2 = MASK >> VectorTypeProperties<T>::length };
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
			_rhs(mask >> VectorTypeProperties<T>::length)
		{}

		inline std::pair<T, T> operator()(const std::pair<T, T>& src, const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const throw() {
			return {
				_lhs(src.first, lhs.first, rhs.first),
				_rhs(src.second, lhs.second, rhs.second)
			};
		}
	};

	// std::array

	template<Operator OP, class T, size_t S, const uint64_t MASK, InstructionSets IS>
	struct Operator2<OP, std::array<T, S>, MASK, IS> {
	private:
		template<size_t I>
		static inline void Execute(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs, std::array<T, S>& out) throw() {
			enum : uint64_t { MASK2 = MASK >> (VectorTypeProperties<T>::length * I) };
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

	namespace detail {
		template<Operator OP, class T, size_t S, InstructionSets IS , bool TRIVIAL>
		struct Operator2RT_array;

		template<Operator OP, class T, size_t S, InstructionSets IS>
		struct Operator2RT_array<OP, T, S, IS, false> {
		private:
			typedef Operator2RT<OP, T, IS> Operator2Fn;
			uint8_t _operators[sizeof(Operator2Fn) * S];
		public:
			Operator2RT_array(uint64_t mask) {
				Operator2Fn* const blends = reinterpret_cast<Operator2Fn*>(_operators);
				for (size_t i = 0u; i < S; ++i) {
					new(blends + i) Operator2Fn(mask);
					mask >>= VectorTypeProperties<T>::length;
				}
			}

			~Operator2RT_array() {
				if constexpr(! std::is_trivially_destructible<Operator2Fn>::value) {
					Operator2Fn* const blends = reinterpret_cast<Operator2Fn*>(_operators);
					for (size_t i = 0u; i < S; ++i) {
						blends[i].~Operator2RT();
					}
				}
			}

			std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs) const throw() {
				std::array<T, S> tmp;
				const Operator2Fn* const blends = reinterpret_cast<const Operator2Fn*>(_operators);
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = blends[i](src[i], lhs[i], rhs[i]);
				}
				return tmp;
			}
		};

		template<Operator OP, class T, size_t S, InstructionSets IS>
		struct Operator2RT_array<OP, T, S, IS, true> {
		private:
			typedef Operator2RT<OP, T, IS> Operator2Fn;
			const uint64_t _mask;
		public:
			Operator2RT_array(uint64_t mask) :
				_mask(mask)
			{}

			std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs) const throw() {
				std::array<T, S> tmp;
				uint64_t mask = _mask;
				for (size_t i = 0u; i < S; ++i) {
					tmp[i] = Operator2Fn(mask)(src[i], lhs[i], rhs[i]);
					mask >>= VectorTypeProperties<T>::length;
				}
				return tmp;
			}
		};
	}

	template<Operator OP, class T, size_t S, InstructionSets IS>
	struct Operator2RT<OP, std::array<T, S>, IS> {
	private:
		detail::Operator2RT_array<OP, T, S, IS, std::is_integral<T>::value || std::is_floating_point<T>::value> _op;
	public:
		Operator2RT(uint64_t mask) :
			_op(mask)
		{}

		inline std::array<T, S> operator()(const std::array<T, S>& src, const std::array<T, S>& lhs, const std::array<T, S>& rhs) const throw() {
			return _op(src, lhs, rhs);
		}
	};

	// VectorWrapper

	template<Operator OP, size_t S, class WRAPPER, const uint64_t MASK, InstructionSets IS>
	struct Operator2<OP, VectorWrapper<S, WRAPPER>, MASK, IS> {
	private:
		const Operator2<OP, WRAPPER, MASK, IS> _op;
	public:
		inline VectorWrapper<S, WRAPPER> operator()(const VectorWrapper<S, WRAPPER>& src, const VectorWrapper<S, WRAPPER>& lhs, const VectorWrapper<S, WRAPPER>& rhs) const throw() {
			return { _op(src.wrapper, lhs.wrapper, rhs.wrapper) };
		}
	};

	template<Operator OP, size_t S, class WRAPPER, InstructionSets IS>
	struct Operator2RT<OP, VectorWrapper<S, WRAPPER>, IS> {
	private:
		const Operator2RT<OP, WRAPPER, IS> _op;
	public:
		Operator2RT(const uint64_t mask) :
			_op(mask)
		{}

		inline VectorWrapper<S, WRAPPER> operator()(const VectorWrapper<S, WRAPPER>& src, const VectorWrapper<S, WRAPPER>& lhs, const VectorWrapper<S, WRAPPER>& rhs) const throw() {
			return { _op(src.wrapper, lhs.wrapper, rhs.wrapper) };
		}
	};

}}}

#endif