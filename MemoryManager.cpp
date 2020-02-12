#include "MemoryManager.h"

std::vector<byte *> MemoryManager::memory = std::vector<byte *>();
#ifndef USE_BITSET
std::vector<MemoryManager::MemoryInfo> MemoryManager::allocated = std::vector<MemoryManager::MemoryInfo>();
#else
std::vector<MemoryManager::MemoryInfo> MemoryManager::allocated = std::vector<MemoryManager::MemoryInfo>();
#endif

//bool MemoryManager::initOrExtendMemory() {
//	byte *rawMemory = (byte *) calloc(MEMORY_ALLOCATION_SIZE + RANGE_START, 1); // Request memory from the OS.
//	if (rawMemory == NULL) // Did we receive memory.
//		return false;
//	memory.push_back(rawMemory); // Give the raw-memory to the memory vector.
//	allocated.push_back(MemoryManager::MemoryInfo()); // Create a MemoryInfo for this new allocation.
//	#ifdef USE_BITSET
//	allocated.ranges = MemoryManager::allocateRaw<std::bitset<MEMORY_ALLOCATION_SIZE>>(1);
//	#endif
//	return true;
//}

//byte *MemoryManager::allocate(size_t count, size_t size) {
//	byte *allocation = NULL;
//	// The size of the requested allocation in bytes in addition to the amount of bytes it takes to store the size of the allocation.
//	size_t byteSize = count * size + SIZE_T_SIZE;
//	MemoryInfo *info = NULL;
//	for (size_t i = 0; i < memory.size() && allocation == NULL; i++) {
//		info = &allocated.at(i);
//		#ifndef USE_BITSET
//		for (size_t j = 0; j < info->ranges.size(); j++) {
//			std::pair<size_t, size_t> &range = info->ranges.at(j);
//			if (range.second - range.first >= byteSize) { // Is this range big enough for this allocation.
//				allocation = memory.at(i) + range.first;  // Set our allocation variable.
//				range.first += byteSize;				  // Update the range.
//				if (range.second - range.first == 0)	  // If the range is now 0 remove it.
//					info->ranges.erase(info->ranges.begin() + j);
//				break; // We've found our allocation ptr we are done.
//			}
//		}
//		#endif
//		#ifdef USE_BITSET
//		if (info->ranges == NULL) {
//			allocation = memory.at(i);
//		} else {
//			size_t start = RANGE_START - 1;
//			for (size_t j = 0; j < MEMORY_ALLOCATION_SIZE; j++) {
//				bool free = !info->ranges->test(j);
//				if (free && start != RANGE_START - 1) {
//					start = j + RANGE_START;
//				} else if (free && (j - start == byteSize)) {
//					allocation = memory.at(i) + start;
//					for (j = 0; j < byteSize; j++) {
//						this->ranges->set(j+start, true);
//					}
//					break;
//				} else {
//					start = RANGE_START - 1;
//				}
//			}
//		}
//		#endif
//	}
//	
//	if (allocation == NULL && byteSize <= MEMORY_ALLOCATION_SIZE) {
//		// We need to allocate a new block of memory then grab the specific pointer.
//		initOrExtendMemory();
//		size_t index = memory.size() - 1; // The last index in memory and allocated.
//		allocation = memory.at(index) + RANGE_START; // The beginning of our extension is the beginning of our allocation offset by the bitsets memory size.
//		allocated.at(index).ranges.at(0).first += byteSize; // The range for our memory is offset by our byteSize.
//	} else if (allocation == NULL) {
//		// Allocate directly from the heap. Hopefully this will only happen when they request a managed_ptr.
//		// Otherwise they will have to make sure to free/delete this memory when they are done with it.
//		allocation = new byte[byteSize]; 
//	}
//
//	*((size_t *) allocation) = byteSize; // Put the size at the beginning of this allocation.
//	allocation += SIZE_T_SIZE; // The allocation we will return is offset by the SIZE_T_SIZE.
//
//	return allocation;
//}

void MemoryManager::deallocateRaw(byte *start) {
	size_t size = *((size_t *) start);
	size_t memOffset = 0;
	bool foundMemInfo = false;
	for (size_t arrayIndex = 0; arrayIndex < memory.size(); arrayIndex++) {
		byte *memry = memory.at(arrayIndex);
		if (memry <= start && memry + MEMORY_ALLOCATION_SIZE >= start + size) {	 // Does our start lay within this memry.
			memOffset = start - memry;								 // Get the offset from the beginning of this memry to our start.
			size += memOffset;										 // Size will now represent the relative end of our start.
			(&allocated.at(arrayIndex))->freeRange(memOffset, size); // Free the range of the MemoryInfo.
			foundMemInfo = true;									 // Flip the flag.
			break;
		}
	}
	if (!foundMemInfo) {
		// If the start array is not within any of the allocated memory blocks.
		//We allocated them dynamically outside of the MemoryManagers.
		delete[](start - SIZE_T_SIZE);
	}
}

void MemoryManager::freeMemory() {
	while (memory.size() > 0) {
		delete memory.at(0);
		memory.erase(memory.begin());
		allocated.erase(allocated.begin());
	}
}

//void MemoryManager::MemoryInfo::freeRange(size_t start, size_t end) {
//	#ifndef USE_BITSET
//	size_t matchedRanges[2] = { 0, 0 };
//	for (size_t i = 0; i < ranges.size(); i++) {
//		if (ranges[i].first == start) // If the range has the first value equal to the start it's our 0-index matched range.
//			matchedRanges[0] = i;
//		if (ranges[i].first == end) // If the range has the first value equal to the start it's our 1-index matched range.
//			matchedRanges[1] = i;
//	}
//	if (matchedRanges[0] == matchedRanges[1]) {
//		ranges[matchedRanges[0]].first = 0;
//	} else {
//		ranges.at(matchedRanges[0]).second = end; // Merge the two matchedRanges.
//		ranges.erase(ranges.begin() + matchedRanges[1]); // Delete the range we didn't change.
//	}
//	#else
//	start += RANGE_START;
//	end += RANGE_START;
//	for (; start < end; start++) {
//		this->ranges->set(start, false);
//	}
//	#endif
//}
