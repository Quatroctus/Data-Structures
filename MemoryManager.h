#pragma once
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#ifndef MEMORY_ALLOCATION
constexpr size_t MEMORY_ALLOCATION_SIZE = 10240000;
#else
constexpr size_t MEMORY_ALLOCATION_SIZE = MEMORY_ALLOCATION;
#endif

using byte = unsigned char;

constexpr size_t SIZE_T_SIZE = sizeof(size_t);
#ifndef USE_BITSET
constexpr int RANGE_START = 0;
#else
constexpr int RANGE_START = MEMORY_ALLOCATION_SIZE / 8;
#endif

class MemoryManager;

template <typename T>
class managed_ptr {
	size_t *refCount, *length;
	T *start;

public:
	managed_ptr(size_t count, byte *start) : refCount(MemoryManager::allocateRaw<size_t>(1)), length((size_t *)start), start((T *) (start + SIZE_T_SIZE)) {
		*refCount = 1;
		*length -= SIZE_T_SIZE;
		*length /= sizeof(T);
	}
	managed_ptr(const managed_ptr &info) : refCount(info.refCount), length(info.start), start(info.start) {
		(*info.refCount)++;
	}
	~managed_ptr() {
		(*refCount)--;
		if (*refCount <= 0) {
			*length *= sizeof(T);
			*length += SIZE_T_SIZE;
			MemoryManager::template free<T>(start);
			MemoryManager::template free<size_t>(refCount);
			*length = 0;
		}
	}
	inline size_t size() { return (*length); } // Should this method exist?
	T &operator[](int index) {
		if (index < 0 || index >= *length) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(index) + " Size: " + std::to_string(*length) + ".");
			exit(-1);
		}
		return this->start[index];
	}
	T &operator*() {
		return *this->start;
	}
	T *operator->() {
		return this->start;
	}
	T *operator+(size_t offset) {
		if (offset < 0 || offset >= *length) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(offset) + " Size: " + std::to_string(*length) + ".");
			exit(-1);
		}
		return this->start + offset;
	}
	T *operator-(size_t offset) {
		if (offset < 0 || offset >= *length) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(offset) + " Size: " + std::to_string(*length) + ".");
			exit(-1);
		}
		return this->start - offset;
	}
	template<typename T>
	friend std::ostream &operator<<(std::ostream &output, const managed_ptr<T> &ptr) {
		return output << ptr.start;
	}
};

class MemoryManager {

	struct MemoryInfo {
		// Think of a way to decrease this memory footprint. Maybe actually use a std::bitset<MEMORY_ALLOCATION_SIZE>
		// Which would require a function to look for a given 'size_t size' within the bitset.
		// Basically be more memory intensive or more processing intensive.
		#ifdef USE_BITSET
		std::bitset<MEMORY_ALLOCATION_SIZE> *ranges = NULL;
		void freeRange(size_t start, size_t end) {
			start += RANGE_START;
			end += RANGE_START;
			for (; start < end; start++) {
				this->ranges->set(start, false);
			}
		}
		#else
		std::vector<std::pair<size_t, size_t>> ranges = { { RANGE_START, MEMORY_ALLOCATION_SIZE } };
		void freeRange(size_t start, size_t end) {
			size_t matchedRanges[2] = { 0, 0 };
			for (size_t i = 0; i < ranges.size(); i++) {
				if (ranges[i].first == start) // If the range has the first value equal to the start it's our 0-index matched range.
					matchedRanges[0] = i;
				if (ranges[i].first == end) // If the range has the first value equal to the start it's our 1-index matched range.
					matchedRanges[1] = i;
			}
			if (matchedRanges[0] == matchedRanges[1]) {
				ranges[matchedRanges[0]].first = 0;
			} else {
				ranges.at(matchedRanges[0]).second = end;		 // Merge the two matchedRanges.
				ranges.erase(ranges.begin() + matchedRanges[1]); // Delete the range we didn't change.
			}
		}
		#endif
	};

	static std::vector<byte *> memory;
	// TODO: Contemplate using one custom resizable bitset which would decrease memory usage at least slightly.
	// However the memory would be allocated on the the heap likely within. WITH THE CURRENT IMPL THIS IS ALREADY REALITY.
	static std::vector<MemoryInfo> allocated;

	static bool initOrExtendMemory() {
		byte *rawMemory = (byte *) calloc(MEMORY_ALLOCATION_SIZE + RANGE_START, 1); // Request memory from the OS.
		if (rawMemory == NULL)														// Did we receive memory.
			return false;
		memory.push_back(rawMemory);					  // Give the raw-memory to the memory vector.
		allocated.push_back(MemoryManager::MemoryInfo()); // Create a MemoryInfo for this new allocation.
		#ifdef USE_BITSET
		allocated.at(allocated.size() - 1).ranges = MemoryManager::allocateRaw<std::bitset<MEMORY_ALLOCATION_SIZE>>(1);
		#endif
		return true;
	}

	static byte *allocate(size_t count, size_t size) {
		byte *allocation = NULL;
		// The size of the requested allocation in bytes in addition to the amount of bytes it takes to store the size of the allocation.
		size_t byteSize = count * size + SIZE_T_SIZE;
		MemoryInfo *info = NULL;
		for (size_t i = 0; i < memory.size() && allocation == NULL; i++) {
			info = &allocated.at(i);
			#ifndef USE_BITSET
			for (size_t j = 0; j < info->ranges.size(); j++) {
				std::pair<size_t, size_t> &range = info->ranges.at(j);
				if (range.second - range.first >= byteSize) { // Is this range big enough for this allocation.
					allocation = memory.at(i) + range.first;  // Set our allocation variable.
					range.first += byteSize;				  // Update the range.
					if (range.second - range.first == 0)	  // If the range is now 0 remove it.
						info->ranges.erase(info->ranges.begin() + j);
					break; // We've found our allocation ptr we are done.
				}
			}
			#else
			if (info->ranges == NULL) {
				allocation = memory.at(i);
			} else {
				size_t start = RANGE_START - 1;
				for (size_t j = 0; j < MEMORY_ALLOCATION_SIZE; j++) {
					bool free = !info->ranges->test(j);
					if (free && start == RANGE_START - 1) {
						start = j + RANGE_START;
					} else if (free && ((j + RANGE_START) - start == byteSize)) {
						allocation = memory.at(i) + start;
						for (j = 0; j < byteSize; j++) {
							info->ranges->set(j + (start - RANGE_START), true);
						}
						break;
					} else if (!free) {
						start = RANGE_START - 1;
					}
				}
			}
			#endif
		}

		if (allocation == NULL && byteSize <= MEMORY_ALLOCATION_SIZE) {
			// We need to allocate a new block of memory then grab the specific pointer.
			initOrExtendMemory();
			size_t index = memory.size() - 1;					// The last index in memory and allocated.
			allocation = memory.at(index) + RANGE_START;		// The beginning of our extension is the beginning of our allocation offset by the bitsets memory size.
			#ifndef USE_BITSET
			allocated.at(index).ranges.at(0).first += byteSize; // The range for our memory is offset by our byteSize.
			#else
			MemoryInfo &info = allocated.at(index);
			for (size_t i = 0; i < byteSize; i++)
				info.ranges->set(i, true);
			#endif
		} else if (allocation == NULL) {
			// Allocate directly from the heap. Hopefully this will only happen when they request a managed_ptr.
			// Otherwise they will have to make sure to free/delete this memory when they are done with it.
			allocation = new byte[byteSize];
		}

		*((size_t *) allocation) = byteSize; // Put the size at the beginning of this allocation.
		allocation += SIZE_T_SIZE;			 // The allocation we will return is offset by the SIZE_T_SIZE.

		return allocation;
	}
	static void deallocateRaw(byte *start);
	static void freeMemory();

public:
	template <typename T>
	inline static managed_ptr<T> allocate(size_t count) {
		return managed_ptr<T>(count * sizeof(T), MemoryManager::allocate(count, sizeof(T)) - SIZE_T_SIZE);
	}

	template <typename T>
	inline static T *allocateRaw(size_t count) {
		return (T *) (MemoryManager::allocate(count, sizeof(T)));
	}

	template <typename T>
	inline static void free(T *pointer) {
		MemoryManager::deallocateRaw(((byte *) pointer) - SIZE_T_SIZE);
	}
};
