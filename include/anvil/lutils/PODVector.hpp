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
		class PODVectorHeap_ {
		private:
			void* _data;
			uint32_t _size;
			uint32_t _capacity;
		public:
			enum { pod_bytes = BYTES };

			PODVectorHeap_() throw() :
				_data(nullptr),
				_size(0u),
				_capacity(0u)
			{}

			PODVectorHeap_(PODVectorHeap_<BYTES>&& other) throw() :
				_data(other._data),
				_size(other._size),
				_capacity(other._capacity)
			{
				memset(&other, 0, sizeof(PODVectorHeap_<BYTES>));
			}

			PODVectorHeap_(const PODVectorHeap_<BYTES>& other) throw() :
				_data(operator new(other._size * BYTES)),
				_size(other._size),
				_capacity(other._capacity)
			{
				memcpy(_data, other._data, _size * BYTES);
			}

			~PODVectorHeap_() throw() {
				if (_data) operator delete(_data);
			}

			PODVectorHeap_<BYTES>& operator=(PODVectorHeap_<BYTES>&& other) throw() {
				uint8_t buffer[sizeof(PODVectorHeap_<BYTES>)];
				memcpy(buffer, this, sizeof(PODVectorHeap_<BYTES>));
				memcpy(this, &other, sizeof(PODVectorHeap_<BYTES>));
				memcpy(&other, buffer, sizeof(PODVectorHeap_<BYTES>));
				return *this;
			}

			PODVectorHeap_<BYTES>& operator=(const PODVectorHeap_<BYTES>& other) throw()  {
				clear();
				reserve(other._size);
				_size = other._size;
				memcpy(_data, other._data, _size * BYTES);
				return *this;
			}

			inline void clear() throw() {
				_size = 0u;
			}

			inline bool empty() const throw() {
				return _size == 0u;
			}

			inline uint32_t size() const throw() {
				return _size;
			}

			inline uint32_t capacity() const throw() {
				return _capacity;
			}

			inline void* data() throw() {
				return _data;
			}

			inline const void* data() const throw() {
				return _data;
			}

			bool reserve(const uint32_t size) throw() {
				if (size > _capacity) {
					_capacity = size;
					void* const new_data = operator new(size * BYTES);
					if (new_data == nullptr) return false;
					if (_data > 0u) {
						memcpy(new_data, _data, _size * BYTES);
						operator delete(_data);
					}
					_data = new_data;
				}
				return true;
			}

			inline bool pop_back() throw() {
				if (_size > 0u) {
					--_size;
					return true;
				}
				return false;
			}

			inline void push_back_noreserve_nobounds(const void* src) throw() {
				void* const dst = static_cast<int8_t*>(_data) + _size * BYTES;
				memcpy(dst, src, BYTES);
				++_size;
			}

			bool push_back_noreserve(const void* src) throw() {
				if (_size + 1u > _capacity) return false;
				push_back_noreserve_nobounds(src);
				return true;
			}

			bool push_back(const void* src) {
				if (_size + 1u > _capacity) {
					uint32_t sizeToReserve;
					if (_size == 0u) {
						sizeToReserve = 8u;
					} else {
						sizeToReserve = _size * 2u;
					}
					if (!reserve(sizeToReserve)) return false;
				}

				push_back_noreserve_nobounds(src);
				return true;
			}
		};

		template<uint32_t BYTES, const uint32_t CAPACITY>
		class PODVectorStack_ {
		private:
			uint8_t _data[BYTES * CAPACITY];
			uint32_t _size;
		public:
			enum { pod_bytes = BYTES };

			PODVectorStack_() throw() :
				_size(0u)
			{}

			PODVectorStack_(PODVectorStack_<BYTES, CAPACITY>&& other) throw() :
				_size(other._size)
			{
				other._size = 0u;
			}

			PODVectorStack_(const PODVectorStack_<BYTES, CAPACITY>& other) throw() :
				_size(other._size)
			{
				memcpy(_data, other._data, _size * BYTES);
			}

			PODVectorStack_<BYTES, CAPACITY>& operator=(PODVectorStack_<BYTES, CAPACITY>&& other) throw() {
				uint8_t buffer[sizeof(PODVectorStack_<BYTES, CAPACITY>)];
				memcpy(buffer, this, sizeof(PODVectorStack_<BYTES, CAPACITY>));
				memcpy(this, &other, sizeof(PODVectorStack_<BYTES, CAPACITY>));
				memcpy(&other, buffer, sizeof(PODVectorStack_<BYTES, CAPACITY>));
				return *this;
			}

			PODVectorStack_<BYTES, CAPACITY>& operator=(const PODVectorStack_<BYTES, CAPACITY>& other) throw()  {
				_size = other._size;
				memcpy(_data, other._data, _size * BYTES);
				return *this;
			}

			inline void clear() throw() {
				_size = 0u;
			}

			inline bool empty() const throw() {
				return _size == 0u;
			}

			inline uint32_t size() const throw() {
				return _size;
			}

			inline uint32_t capacity() const throw() {
				return CAPACITY;
			}

			inline void* data() throw() {
				return _data;
			}

			inline const void* data() const throw() {
				return _data;
			}

			bool reserve(const uint32_t size) throw() {
				return size <= CAPACITY;
			}

			inline bool pop_back() throw() {
				if (_size > 0u) {
					--_size;
					return true;
				}
				return false;
			}

			inline void push_back_noreserve_nobounds(const void* src) throw() {
				void* const dst = _data + _size * BYTES;
				memcpy(dst, src, BYTES);
				++_size;
			}

			bool push_back_noreserve(const void* src) throw() {
				if (_size + 1u > CAPACITY) return false;
				push_back_noreserve_nobounds(src);
				return true;
			}

			bool push_back(const void* src) {
				if (_size + 1u > CAPACITY) return false;
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
	using PODVectorHeap = PODVector<T, detail::PODVectorHeap_<sizeof(T)>>;

	template<class T, uint32_t SIZE>
	using PODVectorStack = PODVector<T, detail::PODVectorStack_<sizeof(T), SIZE>>;
}}

#endif
