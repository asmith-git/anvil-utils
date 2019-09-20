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
		class PODVector_ {
		private:
			void* _data;
			uint32_t _size;
			uint32_t _capacity;
		public:
			enum { pod_bytes = BYTES };

			PODVector_() throw() :
				_data(nullptr),
				_size(0u),
				_capacity(0u)
			{}

			PODVector_(PODVector_<BYTES>&& other) throw() :
				_data(other._data),
				_size(other._size),
				_capacity(other._capacity)
			{
				memset(&other, 0, sizeof(PODVector_<BYTES>));
			}

			PODVector_(const PODVector_<BYTES>& other) throw() :
				_data(operator new(other._size * BYTES)),
				_size(other._size),
				_capacity(other._capacity)
			{
				memcpy(_data, other._data, _size * BYTES);
			}

			~PODVector_() throw() {
				if (_data) operator delete(_data);
			}

			PODVector_<BYTES>& operator=(PODVector_<BYTES>&& other) throw() {
				uint8_t buffer[sizeof(PODVector_<BYTES>)];
				memcpy(buffer, this, sizeof(PODVector_<BYTES>));
				memcpy(this, &other, sizeof(PODVector_<BYTES>));
				memcpy(&other, buffer, sizeof(PODVector_<BYTES>));
				return *this;
			}

			PODVector_<BYTES>& operator=(const PODVector_<BYTES>& other) throw()  {
				Clear();
				Reserve(other._size);
				_size = other._size;
				memcpy(_data, other._data, _size * BYTES);
				return *this;
			}

			inline void Clear() throw() {
				_size = 0u;
			}

			inline bool Empty() const throw() {
				return _size == 0u;
			}

			inline uint32_t Size() const throw() {
				return _size;
			}

			inline uint32_t Capacity() const throw() {
				return _capacity;
			}

			inline void* Data() throw() {
				return _data;
			}

			inline const void* Data() const throw() {
				return _data;
			}

			bool Reserve(const uint32_t size) throw() {
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

			inline bool PopBack() throw() {
				if (_size > 0u) {
					--_size;
					return true;
				}
				return false;
			}

			bool PushBack(const void* src) throw() {
				if (_size + 1u > _capacity) {
					uint32_t sizeToReserve;
					if (_size == 0u) {
						sizeToReserve = 8u;
					} else {
						sizeToReserve = _size * 2u;
					}
					if (!Reserve(sizeToReserve)) return false;
				}

				void* const dst = static_cast<int8_t*>(_data) + _size * BYTES;
				memcpy(dst, src, BYTES);
				++_size;
				return true;
			}
		};

	}

	template<class T>
	class PODVector {
	private:
		static_assert(std::is_pod<T>::value, "type must be POD");

		detail::PODVector_<sizeof(T)> _vector;
	public:
		typedef T type;
		typedef T* iterator;
		typedef const T* const_iterator;

		PODVector() throw() :
			_vector()
		{}

		PODVector(PODVector<T>&& other) throw() :
			_vector(std::move(other._vector))
		{}

		PODVector(const PODVector<T>& other) throw() :
			_vector(other._vector)
		{}

		inline PODVector<T>& operator=(PODVector<T>&& other) throw() {
			_vector = std::move(other._vector);
			return *this;
		}

		inline PODVector<T>& operator=(const PODVector<T>& other) throw() {
			_vector = other._vector;
			return *this;
		}

		inline void Clear() throw() {
			_vector.Clear();
		}

		inline bool Empty() const throw() {
			return _vector.Empty();
		}

		inline uint32_t Size() const throw() {
			return _vector.Clear();
		}

		inline uint32_t Capacity() const throw() {
			return _vector.Capacity();
		}

		inline T* Data() throw() {
			return static_cast<T*>(_vector.Data());
		}

		inline const T* Data() const throw() {
			return static_cast<const T*>(_vector.Data());
		}

		bool Reserve(const uint32_t size) throw() {
			return _vector.Reserve(size);
		}

		inline bool PopBack() throw() {
			return _vector.PopBack();
		}

		inline bool PushBack(const T& src) throw() {
			return _vector.PushBack(&src);
		}

		inline iterator begin() throw() {
			return Data();
		}

		inline iterator end() throw() {
			return begin() + _vector.Size();
		}

		inline const_iterator begin() const throw() {
			return Data();
		}

		inline const_iterator end() const throw() {
			return begin() + _vector.Size();
		}

		inline T& Front() throw() {
			return *begin();
		}

		inline T& Back() throw() {
			return *(end() - 1u);
		}

		inline const T& Front() const throw() {
			return *begin();
		}

		inline const T& Back() const throw() {
			return *(end() - 1u);
		}

		inline T PopBack2() throw() {
			const T tmp = Data()[_vector.Size() - 1u];
			ANVIL_CONTRACT(_vector.PopBack());
			return tmp;
		}
	};
}}

#endif
