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
		class PODVectorCoreDynamic {
		private:
			void* _data;
		public:
			void* head;
		private:
			uint32_t _capacity;
		public:

			constexpr PODVectorCoreDynamic() throw() :
				_data(nullptr),
				head(nullptr),
				_capacity(0u)
			{}

			PODVectorCoreDynamic(PODVectorCoreDynamic<BYTES>&& other) throw() :
				_data(other._data),
				_capacity(other._capacity),
				head(other.head)
			{
				other._data = nullptr;
				other.head = nullptr;
				other._capacity = 0u;
			}

			PODVectorCoreDynamic(const PODVectorCoreDynamic<BYTES>& other) :
				_capacity(other.size)
			{
				const uint32_t bytes = other.size_bytes();
				_data = bytes == 0u ? nullptr : operator new(bytes);
				std::memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(_data) + bytes;
			}

			void swap(PODVectorCoreDynamic<BYTES>& other) throw() {
				enum { bytes = sizeof(PODVectorCoreDynamic<BYTES>) };
				uint8_t buffer[bytes];
				std::memcpy(buffer, this, bytes);
				std::memcpy(this, &other, bytes);
				std::memcpy(&other, buffer, bytes);
			}

			inline void operator=(PODVectorCoreDynamic<BYTES>&& other) throw() {
				swap(other);
			}

			void operator=(const PODVectorCoreDynamic<BYTES>& other) throw() {
				head = _data;
				const uint32_t bytes = other.size_bytes();
				const uint32_t size = bytes / BYTES;
				if(size > _capacity) reserve_nobounds(size);
				std::memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(_data) + bytes;
			}

			~PODVectorCoreDynamic() throw() {
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

			bool reserve_nobounds(const uint32_t newSize) throw() {
				_capacity = newSize;
				void* const new_data = operator new(newSize * BYTES);
				if (new_data == nullptr) return false;
				const uint32_t bytes = size_bytes();
				std::memcpy(new_data, _data, bytes);
				if (_data) operator delete(_data);
				_data = new_data;
				return true;
			}
		};

		template<uint32_t BYTES, const uint32_t CAPACITY>
		class PODVectorCoreStatic {
		private:
			uint8_t _data[BYTES * CAPACITY];
		public:
			void* head;

			constexpr PODVectorCoreStatic() throw() :
				head(_data)
			{}

			PODVectorCoreStatic(PODVectorCoreStatic<BYTES, CAPACITY>&& other) throw() {
				const uint32_t bytes = other.size_bytes();
				std::memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			PODVectorCoreStatic(const PODVectorCoreStatic<BYTES, CAPACITY>& other) {
				const uint32_t bytes = other.size_bytes();
				std::memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			void operator=(PODVectorCoreStatic<BYTES, CAPACITY>&& other) throw() {
				const uint32_t bytes = other.size_bytes();
				std::memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			void operator=(const PODVectorCoreStatic<BYTES, CAPACITY>& other) throw() {
				const uint32_t bytes = other.size_bytes();
				std::memcpy(_data, other._data, bytes);
				head = static_cast<int8_t*>(head) + bytes;
			}

			void swap(PODVectorCoreStatic<BYTES, CAPACITY>& other) throw() {
				if (size_bytes() == 0u) {
					*this = std::move(other);
					other.head = other._data;
				} else if (other.size_bytes() == 0u) {
					other = std::move(*this);
					head = _data;
				} else {
					PODVectorCoreStatic<BYTES, CAPACITY> tmp = std::move(other);
					other = std::move(*this);
					*this = std::move(tmp);
				}
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

			constexpr inline bool reserve_nobounds(const uint32_t newSize) const throw() {
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

			inline void swap(PODVector_<BYTES, CORE>& other) throw() {
				_core.swap(other._core);
			}

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

			inline bool reserve(uint32_t newSize) throw() {
				uint32_t cap = _core.capacity();
				if (newSize > cap) {
					if (cap == 0u) {
						cap = 8u;
					} else {
						cap *= 2u;
					}
					return _core.reserve_nobounds(newSize < cap ? cap : newSize);
				}
				return true;
			}

			inline void pop_back_nobounds() throw() {
				_core.head = static_cast<int8_t*>(_core.head) - BYTES;
			}

			inline bool pop_back() throw() {
				if (! empty()) {
					pop_back_nobounds();
					return true;
				}
				return false;
			}

			inline void pop_front_nobounds() {
				const void data = _core.data();
				std::memcpy(data, static_cast<int8_t*>(data) + BYTES, size_bytes() - BYTES);
			}

			inline bool pop_front() {
				if (empty()) return false;
				pop_front_nobounds();
				return true;
			}

			inline void push_back_noreserve_nobounds(const void* src) throw() {
				std::memcpy(_core.head, src, BYTES);
				_core.head = static_cast<int8_t*>(_core.head) + BYTES;
			}

			bool push_back_noreserve(const void* src) throw() {
				if (_core.size() + 1u > _core.capacity()) return false;
				push_back_noreserve_nobounds(src);
				return true;
			}

			bool push_back(const void* src) {
				if (!reserve(_core.size() + 1u)) return false;
				push_back_noreserve_nobounds(src);
				return true;
			}

			inline void push_back_many_noreserve_nobounds(const void* src, const size_t count) throw() {
				const size_t total_bytes = BYTES * count;
				std::memcpy(_core.head, src, total_bytes);
				_core.head = static_cast<int8_t*>(_core.head) + total_bytes;
			}

			bool push_back_many_noreserve(const void* src, const size_t count) throw() {
				if (_core.size() + count > _core.capacity()) return false;
				push_back_many_noreserve_nobounds(src, count);
				return true;
			}

			bool push_back_many(const void* src, const size_t count) {
				if (!reserve(_core.size() + count)) return false;
				push_back_many_noreserve_nobounds(src, count);
				return true;
			}

			void erase_nobounds(const void* begin, const void* end) {
				const uint32_t count = static_cast<uint32_t>(static_cast<const int8_t*>(end) - static_cast<const int8_t*>(begin)) / BYTES;
				void* new_head = static_cast<int8_t*>(_core.head) - (count * BYTES);
				if (new_head != begin) {
					std::memcpy(const_cast<void*>(begin), end, static_cast<const int8_t*>(_core.head) - static_cast<const int8_t*>(end));
				}
				_core.head = new_head;
			}

			inline bool erase(const void* begin, const void* end) {
				if (end < begin || begin < _core.data() || end > _core.head) return false;
				erase_nobounds(begin, end);
				return true;
			}
		private:
			void insert_noreserve_nobounds_(const void* pos, const void* begin, const void* end, const uint32_t otherSize) throw() {
				const uint32_t thisBytes = static_cast<const int8_t*>(_core.head) - static_cast<const int8_t*>(pos);
				const uint32_t otherBytes = otherSize * BYTES;
				void* const newHead = static_cast<int8_t*>(_core.head) + otherBytes;
				std::memcpy(newHead, _core.head, thisBytes);
				std::memcpy(_core.head, begin, otherBytes);
				_core.head = newHead;
			}
		public:
			inline void insert_noreserve_nobounds(const void* pos, const void* begin, const void* end) throw() {
				const uint32_t otherSize = (static_cast<const int8_t*>(end) - static_cast<const int8_t*>(begin)) / BYTES;
				insert_noreserve_nobounds_(pos, begin, end, otherSize);
			}

			inline bool insert_nobounds(const void* pos, const void* begin, const void* end) {
				const uint32_t otherSize = (static_cast<const int8_t*>(end) - static_cast<const int8_t*>(begin)) / BYTES;
				if (!reserve(size() + otherSize)) return false;
				insert_noreserve_nobounds_(pos, begin, end, otherSize);
				return true;
			}

			inline bool insert(const void* pos, const void* begin, const void* end) {
				if (end < begin || pos < _core.data() || pos >= _core.head) return false;
				return insert_nobounds(pos, begin, end);
			}
		};
	}

	enum OptimisationFlags : uint32_t {
		NO_BOUNDARY_CHECKS = 1u,
		NO_MEMORY_RESERVE = 2u,

		OPTIMISE_ALL = NO_BOUNDARY_CHECKS | NO_MEMORY_RESERVE
	};

	template<class T, class IMPLEMENTATION>
	class PODVector {
	private:
		//static_assert(std::is_pod<T>::value, "type must be POD");
		static_assert(std::is_trivially_destructible<T>::value, "T must be trivially destructable");
		static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

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

		PODVector(const PODVector<T, IMPLEMENTATION>& other) :
			_vector(other._vector)
		{}

		inline void swap(PODVector<T, IMPLEMENTATION>& other) throw() {
			_vector.swap(other._vector);
		}

		inline PODVector<T, IMPLEMENTATION>& operator=(PODVector<T, IMPLEMENTATION>&& other) {
			_vector = std::move(other._vector);
			return *this;
		}

		template<class IMPLEMENTATION2>
		inline PODVector<T, IMPLEMENTATION>& operator=(const PODVector<T, IMPLEMENTATION2>& other) {
			clear();
			if (reserve(other.size())) {
				insert<OPTIMISE_ALL>(begin(), other.begin(), other.end());
			} else {
				ANVIL_CONTRACT(false, "Failed to reserve memory");
			}
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

		inline const T* data() const {
			return empty() ? _vector.data() : nullptr;
		}

		inline T* data() {
			return const_cast<T*>(const_cast<PODVector<T, IMPLEMENTATION>*>(this)->data());
		}

		bool reserve(const uint32_t size) throw() {
			return _vector.reserve(size);
		}

		template<uint32_t optimisation_flags>
		inline bool push_back(const T& src) {
			if constexpr ((optimisation_flags & NO_MEMORY_RESERVE) != 0u) {
				if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) != 0u) {
					_vector.push_back_noreserve_nobounds(&src);
					return true;
				} else {
					return _vector.push_back_noreserve(&src);
				}
			} else {
				return _vector.push_back(&src);
			}
		}

		inline bool push_back(const T& src) {
			return push_back<0u>(src);
		}

		template<uint32_t optimisation_flags>
		inline bool push_back_many(const T* src, const size_t count) {
			if constexpr ((optimisation_flags & NO_MEMORY_RESERVE) != 0u) {
				if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) != 0u) {
					_vector.push_back_many_noreserve_nobounds(src, count);
					return true;
				} else {
					return _vector.push_back_many_noreserve(src, count);
				}
			} else {
				return _vector.push_back_many(src, count);
			}
		}

		inline bool push_back_many(const T* src, const size_t count) {
			return push_back_many<0u>(src, count);
		}

		template<uint32_t optimisation_flags>
		inline bool push_front(const T& src) {
			return insert<optimisation_flags>(static_cast<T*>(_vector.data()), &src);
		}

		inline bool push_front(const T& src) {
			return push_front<0u>(src);
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

		template<uint32_t optimisation_flags>
		inline const T& front() const {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) == 0u) {
				ANVIL_CONTRACT(!empty(), "PODVector index out of bounds");
			}
			return *begin();
		}

		template<uint32_t optimisation_flags>
		inline T& front() {
			return const_cast<T&>(const_cast<PODVector<T, IMPLEMENTATION>*>(this)->front<optimisation_flags>());
		}

		template<uint32_t optimisation_flags>
		inline const T& back() const {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) == 0u) {
				ANVIL_CONTRACT(!empty(), "PODVector index out of bounds");
			}
			return *(end() - 1u);
		}

		template<uint32_t optimisation_flags>
		inline T& back() {
			return const_cast<T&>(const_cast<PODVector<T, IMPLEMENTATION>*>(this)->back<optimisation_flags>());
		}

		inline const T& front() const {
			return front<0u>();
		}

		inline T& front() {
			return front<0u>();
		}

		inline const T& back() const {
			return back<0u>();
		}

		inline T& back() {
			return back<0u>();
		}

		template<uint32_t optimisation_flags>
		inline void pop_back() throw() {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) != 0u) {
				_vector.pop_back_nobounds();
				return true;
			} else {
				return _vector.pop_back();
			}
		}

		inline bool pop_back() throw() {
			return pop_back<0u>();
		}

		template<uint32_t optimisation_flags>
		inline bool pop_back(T& value) throw() {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) == 0u) {
				if (!_vector.pop_back()) return false;
			} else {
				_vector.pop_back_nobounds();
			}
			std::memcpy(&value, static_cast<T*>(_vector.end()), sizeof(T));
			return true;
		}

		inline bool pop_back(T& value) throw() {
			return pop_back<0u>(value);
		}

		template<uint32_t optimisation_flags>
		inline bool pop_front() throw() {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) != 0u) {
				_vector.pop_front_nobounds();
				return true;
			} else {
				return _vector.pop_front();
			}
		}

		inline bool pop_front() throw() {
			return pop_front<0u>();
		}
		
		template<uint32_t optimisation_flags>
		inline bool pop_front(T& value) throw() {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) == 0u) {
				if (empty()) return false;
			}
			std::memcpy(&value, static_cast<T*>(_vector.data()), sizeof(T));
			_vector.pop_front_nobounds();
			return true;
		}

		inline bool pop_front(T& value) throw() {
			return pop_front<0u>(value);
		}

		inline T& operator[](const uint32_t index) throw() {
			ANVIL_DEBUG_CONTRACT(index < size(), "PODVector index out of bounds");
			return data()[index];
		}

		inline const T& operator[](const uint32_t index) const throw() {
			ANVIL_DEBUG_CONTRACT(index < size(), "PODVector index out of bounds");
			return data()[index];
		}

		template<uint32_t optimisation_flags>
		inline bool erase(const const_iterator begin, const const_iterator end) throw() {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) != 0u) {
				_vector.erase_nobounds(begin, end);
				return true;
			} else {
				return _vector.erase(begin, end);
			}
		}

		inline bool erase(const const_iterator begin, const const_iterator end) throw() {
			return erase<0u>(begin, end);
		}

		template<uint32_t optimisation_flags>
		inline bool erase(const const_iterator pos) throw() {
			return erase<optimisation_flags>(pos, pos + 1u);
		}

		inline bool erase(const const_iterator pos) throw() {
			return erase<0u>(pos);
		}

		template<uint32_t optimisation_flags>
		inline bool insert(const const_iterator pos, const const_iterator begin, const const_iterator end) throw() {
			if constexpr ((optimisation_flags & NO_BOUNDARY_CHECKS) != 0u) {
				if constexpr ((optimisation_flags & NO_MEMORY_RESERVE) != 0u) {
					_vector.insert_no_reserve_nobounds(pos, begin, end);
					return true;
				} else {
					return _vector.insert_nobounds(pos, begin, end);
				}
			} else {
				return _vector.insert(pos, begin, end);
			}
		}

		inline bool insert(const const_iterator pos, const const_iterator begin, const const_iterator end) throw() {
			return insert<0u>(pos, begin, end);
		}

		template<uint32_t optimisation_flags>
		inline bool insert(const const_iterator pos, const T& value) throw() {
			return insert<optimisation_flags>(pos, &value, &value + 1u);
		}

		inline bool insert(const const_iterator pos, const T& value) throw() {
			return insert<0u>(pos, value);
		}
	};

	template<class T>
	using PODVectorDynamic = PODVector<T, detail::PODVector_<sizeof(T), detail::PODVectorCoreDynamic<sizeof(T)>>>;

	template<class T, uint32_t SIZE>
	using PODVectorStatic = PODVector<T, detail::PODVector_<sizeof(T), detail::PODVectorCoreStatic<sizeof(T), SIZE>>>;
}}

#endif
