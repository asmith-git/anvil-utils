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

#ifndef ANVIL_LUTILS_POD_VECTOR_HPP
#define ANVIL_LUTILS_POD_VECTOR_HPP

#include <cstdint>
#include <type_traits>
#include "anvil/lutils/Assert.hpp"

namespace anvil { namespace lutils {
	namespace detail {

		template<uint32_t BYTES>
		class PODVectorCoreHeap {
		private:
			void* _data;
			uint32_t _capacity;
		public:
			uint32_t size;

			constexpr PODVectorCoreHeap() throw() :
				_data(nullptr),
				_capacity(0u),
				size(0u)
			{}

			PODVectorCoreHeap(PODVectorCoreHeap<BYTES>&& other) throw() :
				_data(other._data),
				_capacity(other._capacity),
				size(other.size)
			{
				other._data = nullptr;
				other._capacity = 0u;
				other.size = 0u;
			}

			PODVectorCoreHeap(const PODVectorCoreHeap<BYTES>& other) :
				_data(other.size == 0u ? nullptr : operator new(other.size * BYTES)),
				_capacity(other.size),
				size(other.size)
			{
				memcpy(_data, other._data, other.size * BYTES);
			}

			void operator=(PODVectorCoreHeap<BYTES>&& other) throw() {
				uint8_t buffer[sizeof(PODVectorCoreHeap<BYTES>)];
				memcpy(buffer, this, sizeof(PODVectorCoreHeap<BYTES>));
				memcpy(this, &other, sizeof(PODVectorCoreHeap<BYTES>));
				memcpy(&other, buffer, sizeof(PODVectorCoreHeap<BYTES>));
			}

			void operator=(const PODVectorCoreHeap<BYTES>& other) throw() {
				size = 0u;
				reserve(other.size);
				memcpy(_data, other._data, other.size * BYTES);
				size = other.size;
			}

			~PODVectorCoreHeap() throw() {
				if (_data) operator delete(_data);
			}

			constexpr inline uint32_t capacity() const throw() {
				return _capacity;
			}

			constexpr inline void* data() throw() {
				return _data;
			}

			constexpr inline const void* data() const throw() {
				return _data;
			}

			bool reserve(const uint32_t newSize) throw() {
				if (newSize > _capacity) {
					_capacity = newSize;
					void* const new_data = operator new(newSize * BYTES);
					if (new_data == nullptr) return false;
					if (_data > 0u) {
						memcpy(new_data, _data, size * BYTES);
						operator delete(_data);
					}
					_data = new_data;
				}
				return true;
			}
		};

		template<uint32_t BYTES, const uint32_t CAPACITY>
		class PODVectorCoreStack {
		private:
			uint8_t _data[BYTES * CAPACITY];
		public:
			uint32_t size;

			constexpr PODVectorCoreStack() throw() :
				size(0u)
			{}

			PODVectorCoreStack(PODVectorCoreStack<BYTES, CAPACITY>&& other) throw() :
				_data(other._data),
				size(other.size)
			{
				ANVIL_ASSUME(size <= CAPACITY);
				memcpy(_data, other._data, size * BYTES);
			}

			PODVectorCoreStack(const PODVectorCoreStack<BYTES, CAPACITY>& other) :
				size(other.size)
			{
				ANVIL_ASSUME(size <= CAPACITY);
				memcpy(_data, other._data, size * BYTES);
			}

			void operator=(PODVectorCoreStack<BYTES, CAPACITY>&& other) throw() {
				size = other.size;
				ANVIL_ASSUME(size <= CAPACITY);
				memcpy(_data, other._data, other.size * BYTES);
			}

			void operator=(const PODVectorCoreStack<BYTES, CAPACITY>& other) throw() {
				size = other.size;
				ANVIL_ASSUME(size <= CAPACITY);
				memcpy(_data, other._data, other.size * BYTES);
			}

			constexpr inline uint32_t capacity() const throw() {
				return CAPACITY;
			}

			constexpr inline void* data() throw() {
				return _data;
			}

			constexpr inline const void* data() const throw() {
				return _data;
			}

			constexpr bool reserve(const uint32_t newSize) throw() {
				return newSize <= CAPACITY;
			}
		};

		template<uint32_t BYTES, class CORE>
		class PODVector_ {
		private:
			CORE _core;
		public:
			enum { pod_bytes = BYTES };

			PODVector_() throw() :
				_core()
			{}

			PODVector_(PODVector_<BYTES,CORE>&& other) throw() :
				_core(std::move(other._core))
			{}

			PODVector_(const PODVector_<BYTES, CORE>& other) throw() :
				_core(other._core)
			{}

			inline PODVector_<BYTES, CORE>& operator=(PODVector_<BYTES, CORE>&& other) throw() {
				_core = std::move(other._core);
				return *this;
			}

			inline PODVector_<BYTES, CORE>& operator=(const PODVector_<BYTES, CORE>& other) throw()  {
				_core = other._core;
				return *this;
			}

			inline uint32_t capacity() const throw() {
				return _core.capacity();
			}

			inline void* data() throw() {
				return _core.data();
			}

			inline const void* data() const throw() {
				return _core.data();
			}

			inline uint32_t size() const throw() {
				return _core.size;
			}

			inline void clear() throw() {
				_core.size = 0u;
			}

			inline bool empty() const throw() {
				return _core.size == 0u;
			}

			inline bool reserve(const uint32_t newSize) throw() {
				return _core.reserve(newSize);
			}

			inline bool pop_back() throw() {
				if (_core.size > 0u) {
					--_core.size;
					return true;
				}
				return false;
			}

			inline void push_back_noreserve_nobounds(const void* src) throw() {
				void* const dst = static_cast<int8_t*>(_core.data()) + _core.size * BYTES;
				memcpy(dst, src, BYTES);
				++_core.size;
			}

			bool push_back_noreserve(const void* src) throw() {
				if (_core.size + 1u > _core.capacity()) return false;
				push_back_noreserve_nobounds(src);
				return true;
			}

			bool push_back(const void* src) {
				if (_core.size + 1u > _core.capacity()) {
					uint32_t sizeToReserve;
					if (_core.size == 0u) {
						sizeToReserve = 8u;
					} else {
						sizeToReserve = _core.size * 2u;
					}
					if (!_core.reserve(sizeToReserve)) return false;
				}

				push_back_noreserve_nobounds(src);
				return true;
			}
		};

	}

	template<class T, class IMPLEMENTATION>
	class PODVector {
	private:
		static_assert(std::is_pod<T>::value, "type must be POD");

		IMPLEMENTATION _vector;
	public:
		typedef T type;
		typedef T* iterator;
		typedef const T* const_iterator;

		PODVector() throw() :
			_vector()
		{}

		PODVector(PODVector<T, IMPLEMENTATION>&& other) throw() :
			_vector(std::move(other._vector))
		{}

		PODVector(const PODVector<T, IMPLEMENTATION>& other) throw() :
			_vector(other._vector)
		{}

		inline PODVector<T, IMPLEMENTATION>& operator=(PODVector<T, IMPLEMENTATION>&& other) throw() {
			_vector = std::move(other._vector);
			return *this;
		}

		inline PODVector<T, IMPLEMENTATION>& operator=(const PODVector<T, IMPLEMENTATION>& other) throw() {
			_vector = other._vector;
			return *this;
		}

		inline void clear() throw() {
			_vector.clear();
		}

		inline bool empty() const throw() {
			return _vector.empty();
		}

		inline uint32_t size() const throw() {
			return _vector.size();
		}

		inline uint32_t capacity() const throw() {
			return _vector.capacity();
		}

		inline T* data() throw() {
			ANVIL_DEBUG_CONTRACT(!empty());
			return static_cast<T*>(_vector.data());
		}

		inline const T* data() const throw() {
			ANVIL_DEBUG_CONTRACT(!empty());
			return static_cast<const T*>(_vector.data());
		}

		bool reserve(const uint32_t size) throw() {
			return _vector.reserve(size);
		}

		inline bool pop_back() throw() {
			return _vector.pop_back();
		}

		inline void push_back_noreserve_nobounds(const T& src) throw() {
			_vector.push_back_noreserve_nobounds(&src);
		}

		bool push_back_noreserve(const const T& src) throw() {
			return _vector.push_back_noreserve(&src);
		}

		inline bool push_back(const T& src) {
			return _vector.push_back(&src);
		}

		inline iterator begin() throw() {
			return data();
		}

		inline iterator end() throw() {
			return begin() + _vector.size();
		}

		inline const_iterator begin() const throw() {
			return data();
		}

		inline const_iterator end() const throw() {
			return begin() + _vector.size();
		}

		inline T& front() throw() {
			ANVIL_DEBUG_CONTRACT(!empty());
			return *begin();
		}

		inline T& back() throw() {
			ANVIL_DEBUG_CONTRACT(!empty());
			return *(end() - 1u);
		}

		inline const T& front() const throw() {
			ANVIL_DEBUG_CONTRACT(! empty());
			return *begin();
		}

		inline const T& back() const throw() {
			ANVIL_DEBUG_CONTRACT(!empty());
			return *(end() - 1u);
		}

		inline T pop_back2() throw() {
			const T tmp = data()[_vector.size() - 1u];
			ANVIL_CONTRACT(_vector.pop_back());
			return tmp;
		}

		inline T& operator[](const uint32_t index) throw() {
			ANVIL_DEBUG_CONTRACT(index < size());
			return data()[index];
		}

		inline const T& operator[](const uint32_t index) const throw() {
			ANVIL_DEBUG_CONTRACT(index < size());
			return data()[index];
		}
	};

	template<class T>
	using PODVectorHeap = PODVector<T, detail::PODVector_<sizeof(T), detail::PODVectorCoreHeap<sizeof(T)>>>;

	template<class T, uint32_t SIZE>
	using PODVectorStack = PODVector<T, detail::PODVector_<sizeof(T), detail::PODVectorCoreStack<sizeof(T), SIZE>>>;
}}

#endif
