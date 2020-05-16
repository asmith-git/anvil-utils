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

#ifndef ANVIL_LUTILS_FAST_SHARED_PTR_HPP
#define ANVIL_LUTILS_FAST_SHARED_PTR_HPP

#include <cstdint>
#include <atomic>
#include <type_traits>
#include <stdexcept>

namespace anvil { namespace lutils {
	namespace detail {
		template<class T>
		struct FastSharedPtrData {
			T object;
			std::atomic_uint32_t reference_counter;

			FastSharedPtrData(T&& object) :
				object(std::move(object)),
				reference_counter(0)
			{}
		};

		template<class T>
		class _FastSharedPtr {
		private:
			FastSharedPtrData<T>* _data;
		public:
			constexpr _FastSharedPtr() :
				_data(nullptr)
			{}

			_FastSharedPtr(T&& object) :
				_data(new FastSharedPtrData<T>(std::move(object)))
			{
				++_data->reference_counter;
			}

			_FastSharedPtr(const _FastSharedPtr<T>& other) :
				_data(other._data)
			{
				++_data->reference_counter;
			}

			_FastSharedPtr(_FastSharedPtr<T>&& other) :
				_data(other._data)
			{
				other._data = nullptr;
			}

			~_FastSharedPtr() {
				Release();
			}

			inline _FastSharedPtr<T>& operator=(const _FastSharedPtr<T>& other) {
				Release();
				_data = other._data;
				++_data->reference_counter;
				return *this;
			}

			__forceinline _FastSharedPtr& operator=(_FastSharedPtr<T>&& other) {
				Swap(other);
				return *this;
			}

			__forceinline void Release() {
				if (_data && --_data->reference_counter == 0u) delete _data;
				_data = nullptr;
			}

			__forceinline void Release(T&& newObject) {
				Release();
				_data = new FastSharedPtrData<T>(std::move(newObject));
			}

			__forceinline void Release(const _FastSharedPtr<T>& other) {
				operator=(other);
			}

			__forceinline void Release(_FastSharedPtr<T>&& other) {
				Swap(other);
			}

			__forceinline void Swap(_FastSharedPtr<T>& other) {
				std::swap(_data, other._data);
			}

			constexpr __forceinline T* Get() throw() {
				return _data ? &_data->object : nullptr;
			}

			constexpr __forceinline const T* Get() const throw() {
				return _data ? &_data->object : nullptr;
			}

			constexpr __forceinline size_t GetReferenceCount() const throw() {
				return _data ? &_data->reference_counter : 0u;
			}

			constexpr __forceinline T* operator->() throw() {
				return Get();
			}

			constexpr __forceinline const T* operator->() const throw() {
				return Get();
			}

			constexpr __forceinline T& operator*() {
				return *Get();
			}

			constexpr __forceinline const T& operator*() const {
				return *Get();
			}

			constexpr __forceinline operator bool() const throw() {
				return _data != nullptr;
			}
		};
	}

	template<class T>
	using FastSharedPtr = detail::_FastSharedPtr<typename std::remove_const<T>::type>;

}}

#endif
