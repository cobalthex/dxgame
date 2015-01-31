#pragma once

#include "Pch.hpp"

//Aligned storage system
template <size_t Alignment>
struct AlignedStorage
{
public:
	void* operator new (size_t Size)
	{
		if (Size < 1)
			Size = 1;

		void* mem = _aligned_malloc(Size, Alignment);
		
		if (mem == nullptr)
			throw std::bad_alloc();

		return mem;
	}
	void operator delete (void* Memory)
	{
		if (Memory != nullptr)
			_aligned_free(Memory);
	}
};