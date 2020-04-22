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

#ifndef ANVIL_LUTILS_ID_GENERATOR_HPP
#define ANVIL_LUTILS_ID_GENERATOR_HPP

#include <mutex>
#include "anvil/lutils/PODVector.hpp"

namespace anvil { namespace lutils {

	template<class T>
	class IDGenerator {
	public:
		typedef T type;
		virtual ~IDGenerator() {}

		virtual type Generate() = 0;
		virtual void Release(const type id) = 0;
		virtual bool Reserve(const type base, const size_t count) = 0;
	};

	template<class T>
	class IDGeneratorBasic : public IDGenerator<T> {
	private:
		T _base;
	public:
		IDGeneratorBasic() :
			_base(static_cast<T>(0))
		{}

		virtual ~IDGeneratorBasic() {

		}

		T Generate() override {
			if (_base == std::numeric_limits<T>::max()) throw std::runtime_error("All IDs generated");
			return _base++;
		}

		void Release(const T id) override {

		}

		bool Reserve(const T base, const size_t count) override {
			return false;
		}
	};

	template<class T>
	class IDGeneratorBasicReserve : public IDGenerator<T> {
	private:
		PODVectorDynamic<std::pair<T, T>> _reserved_ranges;
		T _base;
	public:
		IDGeneratorBasicReserve() :
			_base(static_cast<T>(0))
		{}

		virtual ~IDGeneratorBasicReserve() {

		}

		T Generate() override {
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

		void Release(const T id) override {

		}

		bool Reserve(const T base, const size_t count) override {
			//! \todo Check if base + count is outside the range of values that T can represent
			_reserved_ranges.push_back({ base, base + static_cast<T>(count) });
			return true;
		}
	};

	template<class GENERATOR>
	class IDGeneratorReuseAdapter : public GENERATOR {
	public:
		typedef typename GENERATOR::type type;
	private:
		PODVectorDynamic<type> _free_ids;
	public:

		IDGeneratorReuseAdapter() :
			GENERATOR()
		{}

		virtual ~IDGeneratorReuseAdapter() {

		}

		type Generate() override {
			if (! _free_ids.empty()) {
				type id;
				_free_ids.pop_back<NO_BOUNDARY_CHECKS>(id);
				return id;
			}
			return GENERATOR::Generate();
		}

		void Release(const type id) override {
			_free_ids.push_back(id);
		}
	};

	template<class GENERATOR, class MUTEX>
	class IDGeneratorMutexAdapter : public GENERATOR {
	private:
		MUTEX _mutex;
	public:
		typedef typename GENERATOR::type type;

		IDGeneratorMutexAdapter() :
			GENERATOR()
		{}

		virtual ~IDGeneratorMutexAdapter() {

		}

		type Generate() override {
			std::lock_guard<MUTEX> lock(_mutex);
			return GENERATOR::Generate();
		}

		void Release(const type id) override {
			std::lock_guard<MUTEX> lock(_mutex);
			GENERATOR::Release();
		}

		bool Reserve(const type base, const size_t count) override {
			std::lock_guard<MUTEX> lock(_mutex);
			return GENERATOR::Reserve(base, count);
		}
	};

	namespace detail {
		template<class T, bool RESUSE, bool RESERVE, bool USE_MUTEX>
		struct _IDGeneratorSelector;

		template<class T>
		struct _IDGeneratorSelector < T, false, false, false> {
			typedef IDGeneratorBasic<T> type;
		};

		template<class T>
		struct _IDGeneratorSelector<T, false, true, false> {
			typedef IDGeneratorBasicReserve<T> type;
		};

		template<class T, bool RESERVE>
		struct _IDGeneratorSelector<T, true, RESERVE, false> {
			typedef IDGeneratorReuseAdapter<typename _IDGeneratorSelector<T, false, RESERVE, false>::type> type;
		};

		template<class T, bool REUSE, bool RESERVE>
		struct _IDGeneratorSelector<T, REUSE, RESERVE, true> {
			typedef IDGeneratorMutexAdapter<typename _IDGeneratorSelector<T, REUSE, RESERVE, false>::type, std::recursive_mutex> type;
		};
	}

	template<class T, bool RESUSE, bool RESERVE, bool USE_MUTEX>
	using IDGeneratorSelector = typename detail::_IDGeneratorSelector<T, RESUSE, RESERVE, USE_MUTEX>::type;

}}

#endif
