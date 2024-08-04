// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#pragma once

#include "RTW_CORE.h"
#include "RTW_Logger.h"
#include "RTW_Memory.h"

namespace RTW
{
	namespace Containers {

		template <typename ValueT>
		struct DynamicArray
		{
		public:
			using SizeType = uint64;
			using IndexT = SizeType;
			using InternalAllocator = Memory::MallocAllocator;

			DynamicArray();
			DynamicArray(DynamicArray& Other);
			DynamicArray(DynamicArray&& Other);
			DynamicArray(SizeType capacity);
			DynamicArray(std::initializer_list<ValueT> list);
			~DynamicArray();

			IndexT PushBack(ValueT value);
			IndexT EmplaceBack(ValueT&& value);

			RTW_INLINE void Clear() {
				rtw_zero_memory((void*)MemoryBlock, capacity * sizeof(ValueT));
				elementsInside = 0;
			}

			// Look at the first back element, don't use a pointer after pushes/emplacements elements inside the ring
			ValueT* PeekBack();

			// Look at the first back element, don't use a pointer after pushes/emplacements elements inside the ring
			const ValueT* PeekBack()	const;

			// Pop element from back
			ValueT&& PopBack();

			// Look at this index in the container, don't use a pointer after pushes/emplacements elements inside the ring
			// NOTE: if index will be out of bounds(more that head index and less that tail index) or incorrect, return will be nullptr
			ValueT* LookAtIndex(IndexT index);

			// Look at this index in the container, don't use a pointer after pushes/emplacements elements inside the ring
			// NOTE: if index will be out of bounds(more that head index and less that tail index) or incorrect, return will be nullptr
			const ValueT* LookAtIndex(IndexT index) const;

			// Resize container
			bool Resize(SizeType capacity);

			// Get capacity;
			RTW_INLINE SizeType GetCapacity() const { return capacity; };

			// Get number of elements inside
			RTW_INLINE SizeType GetSize() const { return elementsInside; };

			// Get tail index
			RTW_INLINE IndexT Empty() const {
				return elementsInside == 0;
			};

			RTW_INLINE constexpr IndexT InvalidIndex() const { return IndexT(-1); };

			// Ranges and useful operators
			RTW_INLINE constexpr SizeType size() const { return elementsInside; };

			RTW_INLINE ValueT& operator[](SizeType index) { RTW_ASSERT(index < capacity); return *PointToValueAtIndex(index); }
			RTW_INLINE const ValueT& operator[](SizeType index) const { RTW_ASSERT(index < capacity); return *PointToValueAtIndex(index); }

			RTW_INLINE ValueT& at(SizeType index) { RTW_ASSERT(index < capacity); return *PointToValueAtIndex(index); }
			RTW_INLINE const ValueT& at(SizeType index) const { RTW_ASSERT(index < capacity); return *PointToValueAtIndex(index); }

			RTW_INLINE constexpr ValueT* data() noexcept { return (ValueT*)MemoryBlock; };
			RTW_INLINE constexpr const ValueT* data() const noexcept { return (ValueT*)MemoryBlock; };

			RTW_INLINE constexpr ValueT* begin()	const { return (ValueT*)MemoryBlock; };
			RTW_INLINE constexpr ValueT* end()	const { return (ValueT*)(MemoryBlock)+size(); };

		private:
			RTW_INLINE ValueT* PointToValueAtIndex(size_t index);
			RTW_INLINE const ValueT* PointToValueAtIndex(size_t index) const;
			RTW_INLINE ValueT** GetData() { return MemoryBlock; }
			RTW_INLINE ValueT** GetData() const { return MemoryBlock; };
			InternalAllocator m_InternalAllocator;
			ValueT** MemoryBlock = nullptr;;
			SizeType capacity = 0;
			SizeType elementsInside = 0;
		};

		template<typename ValueT>
		DynamicArray<ValueT>::DynamicArray()
		{
		};

		template<typename ValueT>
		DynamicArray<ValueT>::DynamicArray(DynamicArray& Other)
		{
			Resize(Other.capacity);
			if (Other.elementsInside > 0)
			{
				Memory::copyMemory(Other.MemoryBlock, MemoryBlock, capacity * sizeof(ValueT));
				elementsInside = Other.elementsInside;
			};
		};

		template<typename ValueT>
		DynamicArray<ValueT>::DynamicArray(DynamicArray&& Other)
		{
			MemoryBlock = Other.MemoryBlock;
			elementsInside = Other.elementsInside;
			capacity = Other.capacity;
			m_InternalAllocator = Other.m_InternalAllocator;

			Other.MemoryBlock = nullptr;
			Other.elementsInside = 0;
			Other.capacity = 0;
			Other.m_InternalAllocator = {};
		};

		template<typename ValueT>
		DynamicArray<ValueT>::DynamicArray(SizeType capacity)
		{
			if (capacity > 0 && capacity != InvalidIndex())
			{
				MemoryBlock = (ValueT**)m_InternalAllocator.Allocate(capacity * (sizeof(ValueT)));
				if (MemoryBlock)
				{
					this->capacity = capacity;
				}
			};
		};

		template<typename ValueT>
		DynamicArray<ValueT>::DynamicArray(std::initializer_list<ValueT> list)
		{
			Resize(list.size() * sizeof(ValueT) * 8);
			for (auto& Value : list)
			{
				PushBack(Value);
			}
		};

		template<typename ValueT>
		DynamicArray<ValueT>::~DynamicArray()
		{
			if (MemoryBlock)
			{
				m_InternalAllocator.Deallocate(MemoryBlock);
			}
		};

		template<typename ValueT>
		uint64 DynamicArray<ValueT>::PushBack(ValueT value)
		{
			if (MemoryBlock)
			{
				if (elementsInside + 1 >= capacity)
				{
					bool result = Resize(capacity * 1.25f);
					RTW_ASSERT(result);
				}

				IndexT IndexForPushedElement = elementsInside;

				ValueT* ItemAtIndex = PointToValueAtIndex(IndexForPushedElement);

				*ItemAtIndex = value;
				elementsInside++;
				return IndexForPushedElement;
			}
			return InvalidIndex();
		};

		template<typename ValueT>
		uint64 DynamicArray<ValueT>::EmplaceBack(ValueT&& value)
		{
			if (MemoryBlock)
			{
				if (elementsInside + 1 >= capacity)
				{
					bool result = Resize(capacity * 1.25f);
					RTW_ASSERT(result);
				}

				IndexT IndexForEmplacedElement = elementsInside;

				std::swap(*PointToValueAtIndex(IndexForEmplacedElement), value);
				elementsInside++;

				return IndexForEmplacedElement;
			};
			return InvalidIndex();
		};

		template<typename ValueT>
		ValueT* DynamicArray<ValueT>::PeekBack()
		{
			ValueT* result = nullptr;

			RTW_ASSERT(elementsInside > 0);

			if (elementsInside)
				result = PointToValueAtIndex(elementsInside - 1);

			return result;
		};

		template<typename ValueT>
		const ValueT* DynamicArray<ValueT>::PeekBack() const
		{
			ValueT* result = nullptr;

			RTW_ASSERT(elementsInside > 0);

			if (elementsInside)
				result = PointToValueAtIndex(elementsInside - 1);

			return result;
		};

		template<typename ValueT>
		ValueT&& DynamicArray<ValueT>::PopBack()
		{
			ValueT* Result = nullptr;

			RTW_ASSERT(elementsInside > 0);

			if (elementsInside)
			{
				Result = PointToValueAtIndex(elementsInside - 1);

				elementsInside--;
			};

			if (Result)
			{
				return std::move(*Result);
			};

			return ValueT{};
		};

		template<typename ValueT>
		ValueT* DynamicArray<ValueT>::LookAtIndex(IndexT index)
		{
			if (index >= capacity ||
				elementsInside == 0 ||
				index == InvalidIndex() ||
				index > elementsInside - 1)
				return nullptr;
			return (ValueT*)GetData() + index;
		};

		template<typename ValueT>
		const ValueT* DynamicArray<ValueT>::LookAtIndex(IndexT index) const
		{
			if (index >= capacity ||
				elementsInside == 0 ||
				index == InvalidIndex() ||
				index > elementsInside - 1)
				return nullptr;
			return (ValueT*)GetData() + index;
		};

		template<typename ValueT>
		bool DynamicArray<ValueT>::Resize(SizeType NewCapacity)
		{
			if (NewCapacity > 0 && NewCapacity != InvalidIndex() && NewCapacity >= elementsInside)
			{
				ValueT** NewAllocatedMemory = (ValueT**)m_InternalAllocator.Allocate(NewCapacity * sizeof(ValueT));

				if (NewAllocatedMemory)
				{
					for (uint64 i = 0; i < NewCapacity; i++)
					{
						ValueT* ConstructPtr = (ValueT*)NewAllocatedMemory + i;
						ConstructPtr = new (ConstructPtr) ValueT();
					}

					if (MemoryBlock)
					{
						if (elementsInside > 0)
						{
							// copy all elements into new container
							Memory::copyMemory((void*)MemoryBlock, (void*)NewAllocatedMemory, elementsInside * sizeof(ValueT));
						};
						m_InternalAllocator.Deallocate((void*)MemoryBlock);
					};
					capacity = NewCapacity;
					MemoryBlock = NewAllocatedMemory;
					return true;
				};
			};
			return false;
		};

		template<typename ValueT>
		RTW_INLINE ValueT* DynamicArray<ValueT>::PointToValueAtIndex(size_t index)
		{
			if (index > capacity - 1)
				return nullptr;

			return (ValueT*)GetData() + index;
		}

		template<typename ValueT>
		RTW_INLINE const ValueT* DynamicArray<ValueT>::PointToValueAtIndex(size_t index) const
		{
			if (index > capacity - 1)
				return nullptr;

			return (ValueT*)GetData() + index;
		}
	}
}