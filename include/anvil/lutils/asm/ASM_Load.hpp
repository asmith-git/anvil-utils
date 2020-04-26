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

	template<class T, const uint64_t MASK = DefaultMask<T>::value, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	struct Load;

	template<class T, InstructionSets IS = VectorTypeProperties<T>::min_instruction_set>
	struct LoadRT;

	// Primatives

#define ANVIL_HELPER(TYPE)\
	template<const uint64_t MASK, InstructionSets IS>\
	struct Load<TYPE, MASK, IS> {\
		enum : bool { has_context = false };\
		inline TYPE operator()(const TYPE* const ptr) const throw() {\
			if constexpr ((MASK & 1ull) == 0ull) {\
				return static_cast<TYPE>(0);\
			} else {\
				return *ptr;\
			}\
		}\
	};\
	template<InstructionSets IS>\
	struct LoadRT<uint8_t, IS> {\
	private:\
		const bool _condition;\
	public:\
		enum : bool { has_context = false };\
		\
		LoadRT(const uint64_t mask) :\
			_mask((MASK & 1ull) == 0ull)\
		{}\
		\
		inline TYPE operator()(const TYPE* const ptr) const throw() {\
			return _condition ? static_cast<TYPE>(0) : *ptr;\
		}\
	};

	ANVIL_HELPER(uint8_t)
	ANVIL_HELPER(uint16_t)
	ANVIL_HELPER(uint32_t)
	ANVIL_HELPER(uint64_t)
	ANVIL_HELPER(int8_t)
	ANVIL_HELPER(int16_t)
	ANVIL_HELPER(int32_t)
	ANVIL_HELPER(int64_t)
	ANVIL_HELPER(float)
	ANVIL_HELPER(double)

#undef ANVIL_HELPER

	// Pair

	template<class T, class T2, const uint64_t MASK, InstructionSets IS>
	struct Load<std::pair<T, T2>, MASK, IS> {
	public:
		typedef std::pair<T, T2> out;
		typedef typename VectorTypeProperties<out>::element_type element_type;
	private:
		enum : uint64_t { MASK2 = MASK >> VectorTypeProperties<T>::length };
		Load<T, MASK, IS> _load_lo;
		Load<T2, MASK2, IS> _load_li;
	private:
		enum : bool { has_context = Load<T, MASK, IS>::has_context || Load<T2, MASK2, IS>::has_context };

		inline out operator()(const element_type* const ptr) const throw() {
			return {
				_load_lo(ptr),
				_load_hi(ptr + VectorTypeProperties<T>::length)
			};
		}
	};

	template<class T, class T2, InstructionSets IS>
	struct LoadRT<std::pair<T, T2>, IS> {
	public:
		typedef std::pair<T, T2> out;
		typedef typename VectorTypeProperties<out>::element_type element_type;
	private:
		LoadRT<T, IS> _load_lo;
		LoadRT<T2, IS> _load_li;
	public:
		enum : bool { has_context = Load<T, MASK, IS>::has_context || Load<T2, MASK2, IS>::has_context };
		
		LoadRT(const uint64_t mask) :
			_load_lo(mask),
			_load_hi(mask >> VectorTypeProperties<T>::length)
		{}
		
		inline out operator()(const element_type* const ptr) const throw() {
			return {
				_load_lo(ptr),
				_load_hi(ptr + VectorTypeProperties<T>::length)
			};
		}
	};

	// Vector
	//! \todo Optimise

	template<class T, size_t S, const uint64_t MASK, InstructionSets IS>
	struct Load<std::array<T, S>, MASK, IS> {
	public:
		typedef std::array<T, S> out;
		typedef typename VectorTypeProperties<out>::element_type element_type;
	private:
		template<size_t I>
		static inline void Execute(const out& dst, const element_type* const src) throw() {
			enum : uint64_t { MASK2 = MASK >> (VectorTypeProperties<T>::length * I) };
			dst[I] = Load<T, MASK2, IS>()(src);
			Execute<I + 1u>(dst, src + VectorTypeProperties<T>::length);
		}

		template<>
		static inline void Execute<S>(const out& dst, const element_type* const src) throw() {
			// Do nothing
		}

		enum : bool { has_context = false };

		inline out operator()(const element_type* const ptr) const throw() {
			out tmp;
			Execute<0u>(out, ptr);
			return tmp;
		}
	};

	template<class T, size_t S, InstructionSets IS>
	struct LoadRT<std::array<T, S>, IS> {
	private:
		const uint64_t _mask;
	public:
		typedef std::array<T, S> out;
		typedef typename VectorTypeProperties<out>::element_type element_type;
		enum : bool { has_context = false };

		LoadRT(const uint64_t mask) :
			_mask(mask)
		{}

		inline out operator()(const element_type* ptr) const throw() {
			out tmp;
			uint64_t mask = _mask;

			const element_type* const end = ptr + VectorTypeProperties<T>::length * S;
			for (ptr; ptr < end; ptr += VectorTypeProperties<T>::length) {
				tmp[i] = LoadRT<T, IS>(mask)(ptr);
				mask >>= VectorTypeProperties<T>::length;
			}
			return tmp;
		}
	};

	// x86 Optimisations

#if ANVIL_EXPERIMENTAL_X86
	//! \todo Implement
#endif

}}}

#endif