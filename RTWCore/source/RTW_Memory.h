#pragma once

#include "RTW_CORE.h"

#include <memory>
#include <cstdint>

RTW_INLINE RTW_STATIC void rtw_zero_memory(void* block, uint64 size);

template <typename T, typename ...Args>
[[nodiscard]] RTW_STATIC T* rtw_new(Args&&... args);

template <typename T>
RTW_STATIC void rtw_delete(T* address);

namespace RTW
{
	namespace Memory {

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
RTW_STATIC void rtw_delete(T* address)
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
		if (!DataToHandle)
		{
			Release();
			return;
		}
		AcquireDataToHandle(DataToHandle);
	}

	void Reset(UniqueMemoryHandle<T> OtherHandle)
	{
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
