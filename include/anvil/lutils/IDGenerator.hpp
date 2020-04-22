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

#ifndef ANVIL_LUTILS_ID_GENERATOR_HPP
#define ANVIL_LUTILS_ID_GENERATOR_HPP

#include "anvil/lutils/PODVector.hpp"

namespace anvil { namespace lutils {
	template<class T, bool REUSE, bool RESERVE>
	class IDGenerator;

	template<class T>
	class IDGenerator<T, false, false> {
	private:
		T _base;
	public:
		IDGenerator() :
			_base(static_cast<T>(0))
		{}

		inline T Generate() {
			if (_base == std::numeric_limits<T>::max()) throw std::runtime_error("All IDs generated");
			return _base++;
		}

		inline void Release(const T id) {

		}

		inline bool Reserve(const T base, const size_t count) {
			return false;
		}
	};

	template<class T>
	class IDGenerator<T, false, true> {
	private:
		PODVectorDynamic<std::pair<T, T>> _reserved_ranges;
		T _base;
	public:
		IDGenerator() :
			_base(static_cast<T>(0))
		{}

		T Generate() {
			if (_base == std::numeric_limits<T>::max()) {
	ALL_GENERATED:
				throw std::runtime_error("All IDs generated");
			}

			// Check if base is a reserved ID
			const std::pair<T, size_t>* const data = _reserved_ranges.data();
			const std::pair<T, size_t>* const end = data + _reserved_ranges.size();
	RECHECK:
			for (const std::pair<T, size_t>* i = data; i < end; ++i) {
				if (_base == i->first) {
					_base = i->second;
					if (_base == std::numeric_limits<T>::max()) goto ALL_GENERATED;
					++_base;
					goto RECHECK;
				}
			}

			// ID is okay to use
			return _base++;
		}

		inline void Release(const T id) {

		}

		inline bool Reserve(const T base, const size_t count) {
			//! \todo Check if base + count is outside the range of values that T can represent
			_reserved_ranges.push_back({ base, base + static_cast<T>(count) });
			return true;
		}
	};

	template<class T, bool RESERVE>
	class IDGenerator<T, true, RESERVE>{
	private:
		PODVectorDynamic<T> _free_ids;
		IDGenerator<T, false, RESERVE> _generator;
	public:
		IDGenerator() :
			_generator()
		{}

		inline T Generate() {
			if (_free_ids.size() > 0u) {
				T id;
				_free_ids.pop_back<NO_BOUNDARY_CHECKS>(id);
				return id;
			}
			return _generator.Generate();
		}

		inline void Release(const T id) {
			_free_ids.push_back(id);
		}

		inline bool Reserve(const T base, const size_t count) {
			return _generator.Reserve(base, count);
		}
	};
}}

#endif
