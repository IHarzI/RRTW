// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_Logger.h"
#include <memory>
#include <cstdint>
#include <map>
#include <mutex>

RTW_INLINE RTW_STATIC void rtw_zero_memory(void* block, uint64 size);

RTW_INLINE RTW_STATIC void rtw_set_memory(void* block, uint64 size, int32 Value);

template <typename T, typename ...Args>
[[nodiscard]] RTW_STATIC T* rtw_new(Args&&... args);

template <typename T>
RTW_STATIC void rtw_delete(T* address) noexcept;

#define REFERENCE_INTERLOCK_GUARD std::lock_guard<std::mutex> InterlockGuard{RTW::Memory::detail::GetInterlockedMutex()};

namespace RTW
{
	namespace Memory {
		namespace detail
		{
			RTW_STATIC RTW_INLINE std::mutex& GetInterlockedMutex() { RTW_STATIC std::mutex InterlockedMutex{}; return InterlockedMutex; }

			RTW_STATIC RTW_INLINE void ReferenceIncrementInterlocked(uint64& ref) {
				REFERENCE_INTERLOCK_GUARD
				ref++;
			}

			RTW_STATIC RTW_INLINE void ReferenceDecrementInterlocked(uint64& ref) {
				REFERENCE_INTERLOCK_GUARD
				ref--;
			}

			RTW_STATIC RTW_INLINE uint64 ReferenceGetValueInterlocked(uint64& ref) {
				REFERENCE_INTERLOCK_GUARD
				return ref;
			}
		}

		class MallocAllocator
		{
		public:
			using pointer = void*;
			RTW_STATIC RTW_INLINE pointer Allocate(size_t allocationsize)
			{
				pointer allocation = malloc(allocationsize);
				rtw_zero_memory(allocation, allocationsize);
				return allocation;
			};

			RTW_STATIC RTW_INLINE bool Deallocate(void* allocation)
			{
				free(allocation);
				return true;
			};
		};

		RTW_INLINE RTW_STATIC void* copyMemory(void* src, void* dst, size_t size) { memcpy_s(dst, size, src, size);	return dst; };
	}
};

// Call deconstructor for object and release occupied memory block
template <typename T>
RTW_STATIC void rtw_delete(T* address) noexcept
{
	RTW_ASSERT_MSG(address, "Invalid poitner to delete!");
	address->~T();
	RTW::Memory::MallocAllocator::Deallocate(address);
}

// Allocate memory block of <T> object size and call <T> constructor with arguments in-place 
template <typename T, typename ...Args>
[[nodiscard]] RTW_STATIC T* rtw_new(Args&&... args)
{
	void* ptr = RTW::Memory::MallocAllocator::Allocate(sizeof(T));
	T* ResultNew = nullptr;
	if (ptr)
		ResultNew = (new(ptr) T(std::forward<Args>(args)...)); // create object in-place
	return ResultNew;
}

RTW_INLINE RTW_STATIC void rtw_zero_memory(void* block, uint64 size) { memset(block, 0, size); };

RTW_INLINE RTW_STATIC void rtw_set_memory(void* block, uint64 size, int32 Value) { memset(block, Value, size); };

template <typename T>
struct UniqueMemoryHandle
{
public:
	template <typename ...Args>
	RTW_STATIC UniqueMemoryHandle Create(Args&&... args)
	{
		return UniqueMemoryHandle((T*)(rtw_new<T>(args...)));
	}

	UniqueMemoryHandle() : Data(nullptr) {};

	UniqueMemoryHandle(const UniqueMemoryHandle&) = delete;
	UniqueMemoryHandle& operator=(const UniqueMemoryHandle&) = delete;

	UniqueMemoryHandle(UniqueMemoryHandle& OtherHandle)
	{
		AcquireDataToHandle(OtherHandle.RetrieveResourse());
	}

	UniqueMemoryHandle(UniqueMemoryHandle&& OtherHandle) noexcept
	{
		AcquireDataToHandle(OtherHandle.RetrieveResourse());
	}

	UniqueMemoryHandle& operator=(UniqueMemoryHandle&& OtherHandle) {
		if (this != &OtherHandle) {
			if (IsValid())
			{
				Release();
			}
			AcquireDataToHandle(OtherHandle.RetrieveResourse());
		}
		return *this;
	}

	UniqueMemoryHandle(T* DataToHandle)
	{
		AcquireDataToHandle(DataToHandle);
	}

	~UniqueMemoryHandle()
	{
		if (IsValid())
		{
			Release();
		};
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	T* Get() const { return Data; };
	T* Get() { return Data; };

	const T& GetReference() const { RTW_ASSERT(IsValid()); return *Data; };
	T& GetReference() { RTW_ASSERT(IsValid()); return *Data; };

	void Release() {
		ReleaseResourseChecked();
	};

	// Deletes contaiend data(if valid) and creates new in place
	template <typename ...Args>
	void ResetNew(Args&&... args)
	{
		AcquireDataToHandle((T*)(rtw_new<T>(args...)));
	}

	void Reset(T* DataToHandle)
	{
		if (Data == DataToHandle)
		{
			return;
		}

		if (!DataToHandle)
		{
			Release();
			return;
		}
		AcquireDataToHandle(DataToHandle);
	}

	void Reset(UniqueMemoryHandle<T> OtherHandle)
	{
		if (Data == OtherHandle.Get())
		{
			// Two same unique handles??
			RTW_ERROR("Resourse ownership violation");
			return;
		}

		if (!OtherHandle.IsValid())
		{
			Release();
			return;
		}
		AcquireDataToHandle(OtherHandle.RetrieveResourse());
	}

	T* RetrieveResourse()
	{
		if (!IsValid())
		{
			return nullptr;
		};

		T* ResourseToReturn = Data;

		Data = nullptr;

		return ResourseToReturn;
	};

	explicit operator bool() const {
		return IsValid();
	}

	bool operator==(T* DataPtr)
	{
		return Data == DataPtr;
	}

	T& operator*() const {
		return GetReference();
	}

	T* operator->() const {
		return Get();
	}

	T& operator*() {
		return GetReference();
	}

	T* operator->() {
		return Get();
	}

private:
	T* Data = nullptr;

	void AcquireDataToHandle(T* DataToHandle)
	{
		if (!DataToHandle)
		{
			return;
		}

		if (Data == DataToHandle)
		{
			return;
		}

		Release();
		Data = DataToHandle;
	}

	void ReleaseResourseChecked()
	{
		if (!Data)
		{
			return;
		}

		rtw_delete(Data);
		Data = nullptr;
	}
};


template <typename T>
struct SharedMemoryHandle
{
public:
	template <typename ...Args>
	RTW_STATIC SharedMemoryHandle Create(Args&&... args)
	{
		return SharedMemoryHandle((T*)(rtw_new<T>(args...)));
	}

	SharedMemoryHandle() : Data(nullptr) {};

	SharedMemoryHandle(const SharedMemoryHandle& OtherHandle)
	{
		AcquireDataToHandle(OtherHandle.Get());
	}

	SharedMemoryHandle& operator=(const SharedMemoryHandle& OtherHandle)
	{
		if (this != &OtherHandle) {
			if (IsValid())
			{
				Release();
			}
			AcquireDataToHandle(OtherHandle.Get());
		}
		return *this;
	}

	SharedMemoryHandle(SharedMemoryHandle& OtherHandle)
	{
		if (IsValid())
		{
			Release();
		}
		AcquireDataToHandle(OtherHandle.Data);
	}

	SharedMemoryHandle(UniqueMemoryHandle<T>&& OtherUniqueHandle) noexcept
	{
		AcquireDataToHandle(OtherUniqueHandle.RetrieveResourse());
	}

	SharedMemoryHandle(SharedMemoryHandle&& OtherHandle) noexcept
	{
		AcquireDataToHandle(OtherHandle.Get());
		OtherHandle.Release();
	}

	SharedMemoryHandle& operator=(SharedMemoryHandle&& OtherHandle) noexcept
	{
		if (this != &OtherHandle) {
			if (IsValid())
			{
				Release();
			}
			AcquireDataToHandle(OtherHandle.Get());
			OtherHandle.Release();
		}
		return *this;
	}

	SharedMemoryHandle(T* DataToHandle)
	{
		AcquireDataToHandle(DataToHandle);
	}

	~SharedMemoryHandle()
	{
		if (IsValid())
		{
			Release();
		};
	}

	bool IsValid() const
	{
		RTW_ASSERT(!Data || Data && RefMap->find(Data) != RefMap->end());

		return Data != nullptr;
	}

	T* Get() const { return Data; };
	T* Get() { return Data; };

	const T& GetReference() const { RTW_ASSERT(IsValid()); return *Data; };
	T& GetReference() { RTW_ASSERT(IsValid()); return *Data; };

	void Release() { ReleaseResourseChecked(); };

	void Reset(T* DataToHandle)
	{
		if (Data == DataToHandle)
		{
			return;
		}

		if (IsValid())
		{
			Release();
		}
		AcquireDataToHandle(DataToHandle);
	}

	void Reset(SharedMemoryHandle<T> OtherHandle)
	{
		if (Data == OtherHandle.Get())
		{
			return;
		}

		if (IsValid())
		{
			Release();
		}
		AcquireDataToHandle(OtherHandle.Get());
	}

	explicit operator bool() const {
		return IsValid();
	}

	bool operator==(T* DataPtr)
	{
		return Data == DataPtr;
	}

	bool operator==(SharedMemoryHandle<T>& OtherHandle)
	{
		return Data == OtherHandle.Data;
	}

	bool operator!=(T* DataPtr)
	{
		return !(Data == DataPtr);
	}

	bool operator!=(SharedMemoryHandle<T>& OtherHandle)
	{
		return !(Data == OtherHandle.Data_);
	}

	T& operator*() const {
		return GetReference();
	}

	T* operator->() const {
		return Get();
	}

	T& operator*() {
		return GetReference();
	}

	T* operator->() {
		return Get();
	}

private:

	void AcquireDataToHandle(T* DataToHandle)
	{
		if (!DataToHandle)
		{
			return;
		}

		if (Data == DataToHandle)
		{
			return;
		}

		InitCheckedRefMap();

		if (auto RefCounterToData = RefMap->find(DataToHandle); RefCounterToData != RefMap->end())
		{
			RTW::Memory::detail::ReferenceIncrementInterlocked(RefCounterToData->second);
		}
		else
		{
			REFERENCE_INTERLOCK_GUARD
			RefMap->insert({ DataToHandle, 1 });
		}

		Data = DataToHandle;
	}

	void InitCheckedRefMap()
	{
		if (!RefMap)
		{
			REFERENCE_INTERLOCK_GUARD
			RefMap = rtw_new<RefMapType>();
			RTW_ASSERT(RefMap);
		}
	}

	void ReleaseRefMapIfEmpty()
	{
		if (RefMap && RefMap->size() < 1)
		{
			rtw_delete(RefMap);
			RefMap = nullptr;
		}
	}

	void ReleaseResourseChecked()
	{
		RTW_ASSERT(!Data || Data && RefMap);
		if (Data && RefMap)
		{
			REFERENCE_INTERLOCK_GUARD
			auto RefCounterToData = RefMap->find(Data);
			RTW_ASSERT(RefCounterToData != RefMap->end());
			if (RefCounterToData != RefMap->end())
			{
				RefCounterToData->second--;
				if (RefCounterToData->second < 1)
				{
					RefMap->erase(Data);
					rtw_delete(Data);
					ReleaseRefMapIfEmpty();
				}
			};
		};
		Data = nullptr;
	}

	T* Data = nullptr;
	using RefCount = uint64;
	using ValuePtr = T*;
	using RefMapType = std::unordered_map<ValuePtr, RefCount>;
	RTW_STATIC RTW_INLINE RefMapType* RefMap = nullptr;
};


template <typename T>
struct WeakMemoryHandle
{
	WeakMemoryHandle() : Data(nullptr) {};

	WeakMemoryHandle(const WeakMemoryHandle& OtherHandle)
	{
		Data = OtherHandle.Data;
	}

	WeakMemoryHandle(const SharedMemoryHandle<T>& SharedHandle)
	{
		Data = SharedHandle.Get();
	}

	WeakMemoryHandle& operator=(const WeakMemoryHandle& OtherHandle)
	{
		Data = OtherHandle.Data;
		return *this;
	}

	WeakMemoryHandle(WeakMemoryHandle& OtherHandle)
	{
		Data = OtherHandle.Data;
	}

	WeakMemoryHandle& operator=(WeakMemoryHandle&& OtherHandle)
	{
		Data = OtherHandle.Data;
		return *this;
	}

	~WeakMemoryHandle()
	{}

	bool IsValid() const { return Data != nullptr && CheckRefCount(Data); }

	T* Get() const { return Data; };

	T& GetReference() { RTW_ASSERT(IsValid()); return *Data; };

	void Reset(SharedMemoryHandle<T>& SharedHandle)
	{
		Data = SharedHandle.Get();
	}

	explicit operator bool() const {
		return IsValid();
	}

	bool operator==(T* DataPtr)
	{
		return Data == DataPtr;
	}

	bool operator==(WeakMemoryHandle<T>& OtherHandle)
	{
		return Data == OtherHandle.Data;
	}

	bool operator==(SharedMemoryHandle<T>& OtherHandle)
	{
		return Data == OtherHandle.Get();
	}

	bool operator!=(T* DataPtr)
	{
		return !(Data == DataPtr);
	}

	bool operator!=(WeakMemoryHandle<T>& OtherHandle)
	{
		return !(Data == OtherHandle.Data);
	}

	bool operator!=(SharedMemoryHandle<T>& OtherHandle)
	{
		return !(Data == OtherHandle.Data);
	}

	T& operator*() const {
		return GetReference();
	}

	T* operator->() const {
		return Get();
	}

	T& operator*() {
		return GetReference();
	}

	T* operator->() {
		return Get();
	}

private:

	bool CheckRefCount(T* Ptr) const
	{
#ifdef RTW_BUILD_DEBUG

		if (SharedMemoryHandle<T>::RefMap &&
			SharedMemoryHandle<T>::RefMap->find(Ptr) != SharedMemoryHandle<T>::RefMap->end()
			&& SharedMemoryHandle<T>::RefMap->find(Ptr)->second > 0)
		{
			return true;
		}

		return false
#else
		return true;
#endif
	}

	T* Data = nullptr;
	using ValuePtr = T*;
};

template<typename T, typename ...Args>
RTW_STATIC SharedMemoryHandle<T> MakeSharedHandle(Args&&... args)
{
	return std::move(SharedMemoryHandle<T>::Create(args...));
}

template<typename T, typename ...Args>
RTW_STATIC UniqueMemoryHandle<T> MakeUniqueHandle(Args&&... args)
{
	return std::move(UniqueMemoryHandle<T>::Create(args...));
}

#undef REFERENCE_INTERLOCK_GUARD