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
			void* head;

			constexpr PODVectorCoreHeap() throw() :
				_data(nullptr),
				_capacity(0u),
				head(nullptr)
			{}

			PODVectorCoreHeap(PODVectorCoreHeap<BYTES>&& other) throw() :
				_data(other._data),
				_capacity(other._capacity),
				head(other.head)
			{
				other._data = nullptr;
				other._capacity = 0u;
				other.head = nullptr;
			}

			PODVectorCoreHeap(const PODVectorCoreHeap<BYTES>& other) :
				_capacity(other.size)
			{
				const uint32_t bytes = other.size_bytes();
				_data = bytes == 0u ? nullptr : operator new(bytes);
				memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(_data) + bytes;
			}

			void operator=(PODVectorCoreHeap<BYTES>&& other) throw() {
				enum { bytes = sizeof(PODVectorCoreHeap<BYTES>) };
				uint8_t buffer[bytes];
				memcpy(buffer, this, bytes);
				memcpy(this, &other, bytes);
				memcpy(&other, buffer, bytes);
			}

			void operator=(const PODVectorCoreHeap<BYTES>& other) throw() {
				head = _data;
				reserve(other.size);
				const uint32_t bytes = other.size_bytes();
				memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(_data) + bytes;
			}

			~PODVectorCoreHeap() throw() {
				if (_data) operator delete(_data);
			}

			constexpr inline uint32_t size_bytes() const throw() {
				return static_cast<const int8_t*>(head) - static_cast<const int8_t*>(_data);
			}

			constexpr inline uint32_t size() const throw() {
				return size_bytes() / BYTES;
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
					const uint32_t bytes = size_bytes();
					void* const new_data = operator new(newSize * BYTES);
					if (new_data == nullptr) return false;
					if (_data > 0u) {
						memcpy(new_data, _data, bytes);
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
			void* head;

			constexpr PODVectorCoreStack() throw() :
				head(_data)
			{}

			PODVectorCoreStack(PODVectorCoreStack<BYTES, CAPACITY>&& other) throw() {
				const uint32_t bytes = other.size_bytes();
				memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			PODVectorCoreStack(const PODVectorCoreStack<BYTES, CAPACITY>& other) {
				const uint32_t bytes = other.size_bytes();
				memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			void operator=(PODVectorCoreStack<BYTES, CAPACITY>&& other) throw() {
				const uint32_t bytes = other.size_bytes();
				memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			void operator=(const PODVectorCoreStack<BYTES, CAPACITY>& other) throw() {
				const uint32_t bytes = other.size_bytes();
				memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			constexpr inline uint32_t size_bytes() const throw() {
				return static_cast<const uint8_t*>(head) - _data;
			}

			constexpr inline uint32_t size() const throw() {
				return size_bytes() / BYTES;
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

			inline void* begin() throw() {
				return _core.data();
			}

			inline void* end() throw() {
				return _core.head;
			}

			inline const void* begin() const throw() {
				return _core.data();
			}

			inline const void* end() const throw() {
				return _core.head;
			}

			inline uint32_t size_bytes() const throw() {
				return _core.size_bytes();
			}

			inline uint32_t size() const throw() {
				return _core.size();
			}

			inline void clear() throw() {
				_core.head = _core.data();
			}

			inline bool empty() const throw() {
				return _core.head != _core.data();
			}

			inline bool reserve(const uint32_t newSize) throw() {
				return _core.reserve(newSize);
			}

			inline bool pop_back() throw() {
				if (! empty()) {
					_core.head = static_cast<int8_t*>(_core.head) - BYTES;
					return true;
				}
				return false;
			}

			inline void push_back_noreserve_nobounds(const void* src) throw() {
				memcpy(_core.head, src, BYTES);
				_core.head = static_cast<int8_t*>(_core.head) + BYTES;
			}

			bool push_back_noreserve(const void* src) throw() {
				if (_core.size() + 1u > _core.capacity()) return false;
				push_back_noreserve_nobounds(src);
				return true;
			}

			bool push_back(const void* src) {
				const uint32_t size = _core.size();
				if (size + 1u > _core.capacity()) {
					uint32_t sizeToReserve;
					if (size == 0u) {
						sizeToReserve = 8u;
					} else {
						sizeToReserve = size * 2u;
					}
					if (!_core.reserve(sizeToReserve)) return false;
				}

				push_back_noreserve_nobounds(src);
				return true;
			}

			void erase_nobounds(const void* begin, const void* end) {
				const uint32_t count = (static_cast<const int8_t*>(end) - static_cast<const int8_t*>(begin)) / BYTES;
				void* new_head = static_cast<int8_t*>(_core.head) - (count * BYTES);
				if (new_head != begin) {
					memcpy(const_cast<void*>(begin), end, static_cast<const int8_t*>(_core.head) - static_cast<const int8_t*>(end));
				}
				_core.head = new_head;
			}

			inline bool erase(const void* begin, const void* end) {
				if (end < begin || begin < _core.data() || end > _core.head) return false;
				erase_nobounds(begin, end);
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

		inline uint32_t size_bytes() const throw() {
			return _vector.size_bytes();
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
			return static_cast<iterator>(_vector.begin());
		}

		inline iterator end() throw() {
			return static_cast<iterator>(_vector.end());
		}

		inline const_iterator begin() const throw() {
			return static_cast<const_iterator>(_vector.begin());
		}

		inline const_iterator end() const throw() {
			return static_cast<const_iterator>(_vector.end());
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

		inline bool pop_back() throw() {
			return _vector.pop_back();
		}

		inline T pop_back2() throw() {
			const T tmp = back();
			pop_back();
			return tmp;
		}

		inline bool pop_front() throw() {
			return erase(begin());
		}

		inline T pop_front2() throw() {
			const T tmp = front();
			pop_front();
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

		inline void erase_nobounds(const const_iterator iterator) throw() {
			_vector.erase_nobounds(iterator, iterator + 1u);
		}

		inline bool erase(const const_iterator iterator) throw() {
			return _vector.erase(iterator, iterator + 1u);
		}

		inline bool erase_nobound(const const_iterator begin, const const_iterator end) throw() {
			return _vector.erase_nobound(begin, end);
		}

		inline bool erase(const const_iterator begin, const const_iterator end) throw() {
			return _vector.erase(begin, end);
		}
	};

	template<class T>
	using PODVectorHeap = PODVector<T, detail::PODVector_<sizeof(T), detail::PODVectorCoreHeap<sizeof(T)>>>;

	template<class T, uint32_t SIZE>
	using PODVectorStack = PODVector<T, detail::PODVector_<sizeof(T), detail::PODVectorCoreStack<sizeof(T), SIZE>>>;
}}

#endif
