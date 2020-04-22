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

	namespace detail {
		template<bool ENABLED, class MUTEX>
		class IDGeneratorMutex {
		private:
			MUTEX _lock;
		public:
			inline void lock() {
				_lock.lock();
			}

			inline void unlock() {
				_lock.unlock();
			}

			inline bool try_lock() {
				return _lock.try_lock();
			}
		};

		template<class MUTEX>
		class IDGeneratorMutex<false, MUTEX> {
		public:
			inline void lock() {

			}

			inline void unlock() {

			}

			inline bool try_lock() {
				return true;
			}
		};
	}

	template<class T>
	class IDGenerator {
	public:
		virtual ~IDGenerator() {}

		virtual T Generate() = 0;
		virtual void Release(const T id) = 0;
		virtual bool Reserve(const T base, const size_t count) = 0;
	};

	template<class T, bool USE_MUTEX>
	class IDGeneratorBasic : public IDGenerator<T> {
	private:
		detail::IDGeneratorMutex<USE_MUTEX, std::recursive_mutex> _lock;
		T _base;
	public:
		IDGeneratorBasic() :
			_base(static_cast<T>(0))
		{}

		virtual ~IDGeneratorBasic() {

		}

		T Generate() override {
			std::lock_guard<decltype(_lock)> lock(_lock);
			if (_base == std::numeric_limits<T>::max()) throw std::runtime_error("All IDs generated");
			return _base++;
		}

		void Release(const T id) override {

		}

		bool Reserve(const T base, const size_t count) override {
			return false;
		}

		inline void lock() {
			_lock.lock();
		}

		inline void unlock() {
			_lock.unlock();
		}

		inline bool try_lock() {
			return _lock.try_lock();
		}
	};

	template<class T, bool USE_MUTEX>
	class IDGeneratorBasicReserve : public IDGenerator<T> {
	private:
		detail::IDGeneratorMutex<USE_MUTEX, std::recursive_mutex> _lock;
		PODVectorDynamic<std::pair<T, T>> _reserved_ranges;
		T _base;
	public:
		IDGeneratorBasicReserve() :
			_base(static_cast<T>(0))
		{}

		virtual ~IDGeneratorBasicReserve() {

		}

		T Generate() override {
			std::lock_guard<decltype(_lock)> lock(_lock);
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
			std::lock_guard<decltype(_lock)> lock(_lock);
			//! \todo Check if base + count is outside the range of values that T can represent
			_reserved_ranges.push_back({ base, base + static_cast<T>(count) });
			return true;
		}

		inline void lock() {
			_lock.lock();
		}

		inline void unlock() {
			_lock.unlock();
		}

		inline bool try_lock() {
			return _lock.try_lock();
		}
	};

	template<class T, class GENERATOR>
	class IDGeneratorReuseAdapter : public GENERATOR {
	private:
		PODVectorDynamic<T> _free_ids;
	public:
		IDGeneratorReuseAdapter() :
			GENERATOR()
		{}

		virtual ~IDGeneratorReuseAdapter() {

		}

		T Generate() override {
			std::lock_guard<GENERATOR> lock(*this);
			if (! _free_ids.empty()) {
				T id;
				_free_ids.pop_back<NO_BOUNDARY_CHECKS>(id);
				return id;
			}
			return GENERATOR::Generate();
		}

		inline void Release(const T id) override {
			std::lock_guard<GENERATOR> lock(*this);
			_free_ids.push_back(id);
		}
	};

	namespace detail {
		template<class T, bool RESUSE, bool RESERVE, bool USE_MUTEX>
		struct _IDGeneratorSelector;

		template<class T, bool USE_MUTEX>
		struct _IDGeneratorSelector<T, false, false, USE_MUTEX> {
			typedef IDGeneratorBasic<T, USE_MUTEX> type;
		};

		template<class T, bool USE_MUTEX>
		struct _IDGeneratorSelector<T, false, true, USE_MUTEX> {
			typedef IDGeneratorBasicReserve<T, USE_MUTEX> type;
		};

		template<class T, bool RESERVE, bool USE_MUTEX>
		struct _IDGeneratorSelector<T, true, RESERVE, USE_MUTEX> {
			typedef IDGeneratorReuseAdapter<T, typename _IDGeneratorSelector<T, false, RESERVE, USE_MUTEX>::type> type;
		};
	}

	template<class T, bool RESUSE, bool RESERVE, bool USE_MUTEX>
	using IDGeneratorSelector = typename detail::_IDGeneratorSelector<T, RESUSE, RESERVE, USE_MUTEX>::type;

}}

#endif
