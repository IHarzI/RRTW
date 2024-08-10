// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
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


#define RTW_MEMORY_REF_INTRLK_GRD_DEC_U64(value) RTW::Memory::detail::ReferenceDecrementInterlocked(value)
#define RTW_MEMORY_REF_INTRLK_GRD_INCR_U64(value) RTW::Memory::detail::ReferenceIncrementInterlocked(value)
#define RTW_MEMORY_REF_INTRLK_GRD_GET_U64(value) RTW::Memory::detail::ReferenceGetValueInterlocked(value)
#define RTW_MEMORY_REFMAP_INTRLK_GRD_GETREF_U64(value) RTW::Memory::detail::RefMapGetValueInterlocked(value, *RTW::Memory::detail::RefMap)
#define RTW_MEMORY_REFMAP RTW::Memory::detail::RefMap

namespace RTW
{
	namespace Memory {
		namespace detail
		{
			using RefCount = uint64;
			using ValuePtr = void*;
			using RefMapType = std::unordered_map<void*, uint64>;
			RTW_STATIC RTW_INLINE RefMapType* RefMap = nullptr;

			RTW_STATIC RTW_INLINE std::mutex& GetInterlockedMutex() { RTW_STATIC std::mutex InterlockedMutex{}; return InterlockedMutex; }

			RTW_STATIC RTW_INLINE uint64* RefMapGetValueInterlocked(void* address, std::unordered_map<void*, uint64>& RefMap) {
				REFERENCE_INTERLOCK_GUARD
				if (auto RefCounterToData = RefMap.find(address); RefCounterToData != RefMap.end())
				{
					return &RefCounterToData->second;
				}
				return nullptr;
			}

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
				if (allocation)
				{
					free(allocation);
					return true;
				};
				return false;
			};
		};

		RTW_INLINE RTW_STATIC void* copyMemory(void* src, void* dst, size_t size) { memcpy_s(dst, size, src, size);	return dst; };
	}
};

// Call deconstructor for object and release occupied memory block
template <typename T>
RTW_STATIC void rtw_delete(T* address) noexcept
{
	if (address)
	{
		address->~T();
		RTW::Memory::MallocAllocator::Deallocate(address);
	};
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
		return (T*)(rtw_new<T>(args...));
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
		return (T*)(rtw_new<T>(args...));
	}

	SharedMemoryHandle() : Data(nullptr) {};

	SharedMemoryHandle(const SharedMemoryHandle& OtherHandle)
	{
		AcquireDataToHandle(OtherHandle.Data);
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
		RTW_ASSERT(!Data || Data && RTW_MEMORY_REFMAP && RTW_MEMORY_REFMAP->find(Data) != RTW_MEMORY_REFMAP->end());

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


		if (uint64* RefCount = RTW_MEMORY_REFMAP_INTRLK_GRD_GETREF_U64((void*)DataToHandle))
		{
			RTW_MEMORY_REF_INTRLK_GRD_INCR_U64(*RefCount);
		}
		else
		{
			REFERENCE_INTERLOCK_GUARD
			RTW_MEMORY_REFMAP->insert({ (void*)DataToHandle, 1 });
		}

		Data = DataToHandle;
	}

	void InitCheckedRefMap()
	{
		if (!RTW_MEMORY_REFMAP)
		{
			REFERENCE_INTERLOCK_GUARD
			RTW_MEMORY_REFMAP = rtw_new<RTW::Memory::detail::RefMapType>();
			RTW_ASSERT(RTW_MEMORY_REFMAP);
		}
	}

	void ReleaseRefMapIfEmpty()
	{
		if (RTW_MEMORY_REFMAP && RTW_MEMORY_REFMAP->empty())
		{
			REFERENCE_INTERLOCK_GUARD
			rtw_delete(RTW_MEMORY_REFMAP);
			RTW_MEMORY_REFMAP = nullptr;
		}
	}

	void ReleaseResourseChecked()
	{
		RTW_ASSERT(!Data || Data && RTW_MEMORY_REFMAP);
		if (Data && RTW_MEMORY_REFMAP)
		{
			auto RefCounterToData = RTW_MEMORY_REFMAP->find(Data);
			RTW_ASSERT(RefCounterToData != RTW_MEMORY_REFMAP->end());
			if (RefCounterToData != RTW_MEMORY_REFMAP->end())
			{
				RTW_MEMORY_REF_INTRLK_GRD_DEC_U64(RefCounterToData->second);
				if (RTW_MEMORY_REF_INTRLK_GRD_GET_U64(RefCounterToData->second) < 1)
				{
					RTW_MEMORY_REFMAP->erase(Data);
					rtw_delete(Data);
					ReleaseRefMapIfEmpty();
				}
			};
		};
		Data = nullptr;
	}

	T* Data = nullptr;
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