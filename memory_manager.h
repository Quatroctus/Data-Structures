#pragma once
#include <algorithm>
#include <bitset>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

using byte = unsigned char;

#ifndef ALLOCATION_SIZE
#define ALLOCATION_SIZE 10240000
#endif
#ifdef USE_BITSET
constexpr size_t START = sizeof(std::bitset<ALLOCATION_SIZE>);
#else
constexpr size_t START = 0;
#endif
constexpr size_t TRUE_ALLOC_SIZE = ALLOCATION_SIZE + START;
constexpr size_t SIZE_OF_SIZET = sizeof(size_t);

class MemoryManager;

template <typename V>
struct managed_allocator {

	V *allocate(size_t count) {
		MemoryManager::allocateRaw(count);
	}

	void deallocate(V *ptr, size_t s) {
		MemoryManager::deallocateRaw(ptr);
	}
};

template <typename V>
class managed_ptr {
	size_t *refCount, *length;
	V *start;

public:
	managed_ptr(byte *start) : length(reinterpret_cast<size_t *>(start - SIZE_OF_SIZET)), // Get address of the length.
							   refCount(reinterpret_cast<size_t *>(start)), // The refCount is the beginning of our requested memory.
							   start(reinterpret_cast<V *>(start + SIZE_OF_SIZET)) { // After the refCount is where our start begins.
		*refCount = 1; // Our refCount is 1 at the beginning.
		*length = (*length - SIZE_OF_SIZET) / sizeof(V); // Convert the length from bytes to T's.
	}
	managed_ptr(const managed_ptr &info) : length(info.length), refCount(info.refCount), start(info.start) {
		(*refCount)++; // Every time we copy increase the refCount by 1.
	}
	~managed_ptr() {
		(*refCount)--; // Every destruct decrease the refCount by 1.
		if (*refCount == 0) {
			*length = *length * sizeof(V) + SIZE_OF_SIZET; // Convert the length from T's to bytes.
			MemoryManager::template free(refCount); // Tell our MemoryManager to free our requested memory.
		}
	}
	inline V &operator[](size_t index) {
		if (index < 0 || index >= *length)
			throw std::out_of_range("Tried to offset managed_ptr out of it's range.");
		return this->start[index];
	}
	inline V &operator*() {
		return *this->start;
	}
	inline V *operator+(size_t offset) {
		if (offset < 0 || offset >= *length)
			throw std::out_of_range("Tried to offset managed_ptr out of it's range.");
		return this->start + offset;
	}
	inline V *operator-(size_t offset) {
		if (-offset < 0 || -offset >= *length)
			throw std::out_of_range("Tried to offset managed_ptr out of it's range.");
	return this->start - offset;
	}
	managed_ptr<V> &operator=(managed_ptr<V> ptr2) { // ptr2 will be a copy constructed value.
		std::swap(this->length, ptr2.length); // We need to swap the values from ptr2 to this.
		std::swap(this->refCount, ptr2.refCount);
		std::swap(this->start, ptr2.start);
		(*this->refCount)++; // Increase the refCount because this is a new reference.
		return *this;
	}
	template <typename V>
	inline friend std::ostream &operator<<(std::ostream &output, const managed_ptr<V> &ptr) {
		return output << ptr.start;
	}
	inline size_t size() { return *length; }
};

class MemoryManager {

#ifdef USE_BITSET
	struct Allocation {
		bool nextContiguous; // Is the next Allocation contiguous to this one.
		byte *memory;
		std::bitset<ALLOCATION_SIZE> *bits; // Contains if the byte at the specified index is allocated.
		Allocation(byte *memory, bool nextContiguous) : bits(NULL), memory(memory), nextContiguous(nextContiguous) {}
		void freeRange(size_t start, size_t end, size_t thisIndex) {
			for (; start < end; start++) {
				if (start > ALLOCATION_SIZE) {
					MemoryManager::allocations[thisIndex].freeRange(0, end - start, thisIndex + 1); // We are about to step out of this Allocation tell the next one to free the remaining range.
					break; // We finished on the last iteration.
				} else
					this->bits->set(start, false); // This byte is no longer allocated.
			}
		}
		byte *request(size_t s, size_t thisIndex, bool calledFromPrev = false) {
			constexpr size_t DEFAULT = START - 1; // This is the DEFAULT start value.
			size_t start = DEFAULT;
			for (size_t i = 0; i < ALLOCATION_SIZE; i++) {
				bool free = !bits->test(i); // Check this byte in the bitset.
				if (free && start == DEFAULT) { // If we are free and we are the default value this i is our relative start.
					start = i + START;
				} else if (free && ((i + START) - start) == s - 1) { // If we are free and our range is one less than our size we have found the first range.
					for (i = 0; i < s; i++) {
						bits->set(i + start - START, true); // This byte is now allocated.
					}
					return memory + start; // Offset by the start.
				} else if (!free && start != DEFAULT) { // If we have found a byte that is not free reset to our default.
					if (calledFromPrev)
						break;
					start = DEFAULT;
				} else if (free && i + 1 == ALLOCATION_SIZE && nextContiguous) { // If we are contiguous and at the end of our bitset.
					const size_t newSize = s - (i - (start - START)); // Get the remaining size.
					if (MemoryManager::allocations[thisIndex + 1].request(newSize, thisIndex + 1, true) != NULL) { // Ask our next if their beginning is free.
						for (i = 0; i < s - newSize; i++) {
							bits->set(i + start - START, true); // This byte is now allocated.
						}
						return memory + start; // Offset by the start.
					}
				}
			}
			return NULL; // We were unable to find any free ranges of the size.
		}
	};
#else
	struct Allocation {
		bool nextContiguous; // Is the next Allocation contiguous to this one.
		byte *memory;
		std::vector<std::pair<size_t, size_t>> ranges; // Each pair represents the start and end of a free range.
		Allocation(byte *memory, bool nextContiguous) : ranges({ { START, ALLOCATION_SIZE } }), memory(memory), nextContiguous(nextContiguous) {}
		void freeRange(size_t start, size_t end, size_t thisIndex) {
			if (end > ALLOCATION_SIZE) {
				MemoryManager::allocations[thisIndex + 1].freeRange(0, end - ALLOCATION_SIZE, thisIndex + 1);
				end = ALLOCATION_SIZE;
			}
			byte state = 0;
			size_t matchedRanges[2] = { 0, 0 };
			for (size_t i = 0; i < ranges.size(); i++) {
				if (ranges[i].second == start) {
					matchedRanges[0] = i;
					state++;
				} else if (ranges[i].first == end) {
					matchedRanges[1] = i;
					state += 2;
				}
			}
			switch (state) {
			case 0:
				ranges.push_back(std::make_pair(start, end));
				std::sort(ranges.begin(), ranges.end());
				break;
			case 1:
				ranges[matchedRanges[0]].second = end;
				break;
			case 2:
				ranges[matchedRanges[1]].first = start;
				break;
			case 3:
				ranges[matchedRanges[0]].second = ranges[matchedRanges[1]].second;
				ranges.erase(ranges.begin() + matchedRanges[1]);
				break;
			}
		}
		byte *request(size_t s, size_t thisIndex, bool calledFromPrev = false) {
			for (size_t i = 0; i < ranges.size(); i++) {
				auto &range = ranges[i];
				if (range.first != 0 && calledFromPrev)
					break;
				bool askNext = nextContiguous && range.second == ALLOCATION_SIZE;
				if (askNext && MemoryManager::allocations[thisIndex + 1].request(s - (range.second - range.first), thisIndex + 1, true) == NULL)
					break;
				if ((range.second - range.first >= s) || askNext) {
					byte *start = memory + range.first;
					range.first += s;
					if (range.first >= range.second) {
						ranges.erase(ranges.begin() + i);
					}
					return start;
				}
			}
			return NULL;
		}
	};
#endif

	static std::vector<Allocation> allocations;

	static bool initOrExtend(size_t times) {
		byte *raw = static_cast<byte *>(calloc(TRUE_ALLOC_SIZE, times)); // Request the allocation size amount of memory the requested number of times.
		if (raw == NULL) // If we were unable to get that amount of memory from the OS we were unable to extend.
			return false;
		for (size_t i = 0; i < times; i++) {
			allocations.push_back(Allocation(raw + i * TRUE_ALLOC_SIZE, i + 1 < times)); // Put the beginning of each time into the allocations vector.
#ifdef USE_BITSET
			allocations[allocations.size() - 1].bits = reinterpret_cast<std::bitset<ALLOCATION_SIZE> *>(allocations[allocations.size() - 1].memory); // Give our bits the start of the requested memory.
#endif
		}
		return true;
	}

	static byte *allocate(size_t s) {
		size_t allocSize = s + SIZE_OF_SIZET; // Our allocSize also contains the size of the allocation.
		byte *start = NULL;
		for (size_t i = 0; i < allocations.size() && start == NULL; i++) { // Loop through our allocations and request memory until a request is successful or we run out of allocations.
			start = allocations[i].request(allocSize, i);
		}
		if (start == NULL) {
			size_t times = (allocSize / ALLOCATION_SIZE) + ((allocSize % ALLOCATION_SIZE) != 0); // The number of contiguous allocations to allocate.
			if (initOrExtend(times)) // Does this extend call succeed?
				start = allocations[allocations.size() - times].request(allocSize, allocations.size() - times); // Request from the first new allocation.
		}
		if (start == NULL)
			return NULL;
		*(reinterpret_cast<size_t *>(start)) = allocSize; // Put the allocated size at the beginning of start.
		start += SIZE_OF_SIZET; // Offset start by the size of a size_t.
		return start;
	}

	static void deallocateRaw(byte *start) {
		size_t s = *(reinterpret_cast<size_t *>(start)), offset; // Get the size from the beginning.
		for (size_t i = 0; i < allocations.size(); i++) {
			Allocation &allocation = allocations[i];
			byte *memory = allocation.memory;
			if (memory <= start && memory + TRUE_ALLOC_SIZE >= start + s) { // Is our start within this allocation.
				offset = start - memory - START; // Our offset is the relative start to this memory.
				allocation.freeRange(offset, offset + s, i); // Tell the allocation to free the offset to the size of the allocation.
				return;
			}
		}
	}

public:
	template <typename V>
	inline static managed_ptr<V> allocate(size_t count) {
		return managed_ptr<V>(MemoryManager::allocate(count * sizeof(V) + SIZE_OF_SIZET));
	}

	template <typename V>
	inline static V *allocateRaw(size_t count) {
		return reinterpret_cast<V *>(MemoryManager::allocate(count * sizeof(V)));
	}

	template <typename V>
	inline static void free(V *ptr) {
		MemoryManager::deallocateRaw(reinterpret_cast<byte *>(ptr) - SIZE_OF_SIZET);
	}

	static void freeAllocations() {
		while (allocations.size() > 0) {
			delete allocations[0].memory; // Give the memory back to the OS.
			Allocation allocation = allocations[0];
			do {
				allocation = allocations[0];
				allocations.erase(allocations.begin());
			} while (allocation.nextContiguous); // If the allocation was contigous we already told the OS to delete that memory so just erase the next allocation.
		}
	}
};

std::vector<MemoryManager::Allocation> MemoryManager::allocations = std::vector<MemoryManager::Allocation>();
