#pragma once
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef MEMORY_ALLOCATION
constexpr size_t MEMORY_ALLOCATION_SIZE = MEMORY_ALLOCATION;
#else
constexpr size_t MEMORY_ALLOCATION_SIZE = 10240000;
#endif

using byte = unsigned char;

constexpr size_t SIZE_T_SIZE = sizeof(size_t);
#ifdef USE_BITSET
constexpr size_t RANGE_START = MEMORY_ALLOCATION_SIZE / 8;
#else
constexpr size_t RANGE_START = 0;
#endif

class MemoryManager;

template <typename T>
class managed_ptr {
	size_t *refCount, *length;
	T *start;

public:
	managed_ptr(byte *start) : refCount(MemoryManager::allocateRaw<size_t>(1)), length((size_t *) start), start((T *) (start + SIZE_T_SIZE)) {
		*refCount = 1;
		*length -= SIZE_T_SIZE;
		*length /= sizeof(T);
	}
	managed_ptr(const managed_ptr &info) : refCount(info.refCount), length(info.length), start(info.start) {
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
	inline size_t size() { return (*length); } // I like it. Proves useful since size is already stored no need to have user store it twice.
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
	managed_ptr<T> &operator=(const managed_ptr<T> &ptr2) {
		managed_ptr<T> tmp = managed_ptr<T>(ptr2);
		std::swap(this->length, tmp.length);
		std::swap(this->refCount, tmp.refCount);
		std::swap(this->start, tmp.start);
		(*this->refCount)++;
		return *this;
	}
	template <typename T>
	friend std::ostream &operator<<(std::ostream &output, const managed_ptr<T> &ptr) {
		return output << ptr.start;
	}
};

class MemoryManager {

	struct MemoryInfo;
	#ifdef USE_BITSET
	struct MemoryInfo {
		std::bitset<MEMORY_ALLOCATION_SIZE> *bits;
		MemoryInfo() : bits(nullptr) {}
		void freeRange(size_t start, size_t end) {
			for (; start < end; start++) {
				this->bits->set(start, false);
			}
		}
	};
	#else
	struct MemoryInfo {
		std::vector<std::pair<size_t, size_t>> ranges = { { RANGE_START, MEMORY_ALLOCATION_SIZE } };
		void freeRange(size_t start, size_t end) {
			byte matched = 0, lastMatched = 4;
			size_t matchedRanges[2] = { 0, 0 };
			for (size_t i = ranges.size() - 1; (i < ranges.size() &&  i > 0) || i == 0; i--) {
				if (ranges[i].second == start) {
					matchedRanges[0] = i;
					matched++;
					lastMatched = 0;
				}
				if (ranges[i].first == end) {
					matchedRanges[1] = i;
					matched++;
					lastMatched = 1;
				}
			}
			switch (matched) {
			case 0:
				ranges.push_back(std::make_pair(start, end));
				break;
			case 1:
				switch (lastMatched) {
				case 0:
					ranges[matchedRanges[0]].second = end;
					break;
				case 1:
					ranges[matchedRanges[1]].first = start;
					break;
				}
				break;
			case 2:
				ranges.at(matchedRanges[0]).second = ranges.at(matchedRanges[1]).second;
				ranges.erase(ranges.begin() + matchedRanges[1]);
				break;
			}
		}
	};
#endif
	
	static std::vector<byte *> memoryChunks;
	static std::vector<MemoryInfo> memoryChunkInfo;

	static bool initOrExtendMemory() {
		byte *raw = (byte *) calloc(MEMORY_ALLOCATION_SIZE + RANGE_START, 1);
		if (raw == NULL)
			return false;
		memoryChunks.push_back(raw);
		memoryChunkInfo.push_back(MemoryInfo());
		#ifdef USE_BITSET
		memoryChunkInfo.at(memoryChunkInfo.size() - 1).bits = MemoryManager::allocateRaw<std::bitset<MEMORY_ALLOCATION_SIZE>>(1);
		#endif
		return true;
	}
	static byte *allocate(size_t count, size_t size) {
		byte *allocation = NULL;
		size_t byteSize = count * size + SIZE_T_SIZE;
		MemoryInfo *info = NULL;
		for (size_t i = 0; i < memoryChunks.size() && allocation == NULL; i++) {
			info = &memoryChunkInfo.at(i);
#ifdef USE_BITSET
			if (info->bits == NULL) {
				return memoryChunks.at(i);
			} else {
				size_t start = RANGE_START - 1;
				for (size_t j = 0; j < MEMORY_ALLOCATION_SIZE; j++) {
					bool free = !info->bits->test(j);
					if (free && start == RANGE_START - 1)
						start = j + RANGE_START;
					else if (free && ((j + RANGE_START) - start) == byteSize) {
						allocation = memoryChunks.at(i) + start;
						for (j = 0; j < byteSize; j++) {
							info->bits->set(j + start - RANGE_START, true);
						}
						break;
					} else if (!free && start != RANGE_START - 1) {
						start = RANGE_START - 1;
					}
				}
			}
#else
			for (size_t j = info->ranges.size() - 1; (i < info->ranges.size() && i > 0) || i == 0; j--) {
				std::pair<size_t, size_t> &range = info->ranges.at(j);
				if (range.second - range.first >= byteSize) {
					allocation = memoryChunks.at(i) + range.first;
					range.first += byteSize;
					if (range.second - range.first == 0)
						info->ranges.erase(info->ranges.begin() + j);
					break;
				}
			}
#endif
		}

		if (allocation == NULL && byteSize <= MEMORY_ALLOCATION_SIZE) {
			initOrExtendMemory();
			size_t index = memoryChunks.size() - 1;
			allocation = memoryChunks.at(index) + RANGE_START;
#ifdef USE_BITSET
			for (size_t i = 0; i < byteSize; i++)
				memoryChunkInfo.at(index).bits->set(i, true);
#else
			memoryChunkInfo.at(index).ranges.at(0).first += byteSize;
#endif
		} else if (allocation == NULL) {
			allocation = new byte[byteSize];
		}
		if (allocation == NULL)
			return NULL;
		*((size_t *) allocation) = byteSize;
		allocation += SIZE_T_SIZE;

		return allocation;
	}

	static void deallocateRaw(byte *start) {
		size_t size = *((size_t *) start);
		size_t memOffset;
		bool foundMemInfo = false;
		for (size_t i = 0; i < memoryChunks.size(); i++) {
			byte *memory = memoryChunks.at(i);
			if (memory <= start && memory + MEMORY_ALLOCATION_SIZE > start + size) {
				memOffset = start - memory - RANGE_START;
				size += memOffset;
				(&memoryChunkInfo.at(i))->freeRange(memOffset, size);
				foundMemInfo = true;
				break;
			}
		}
		if (!foundMemInfo) {
			delete[](start - SIZE_T_SIZE);
		}
	}

	static void freeMemoryChunks() {
		while (memoryChunks.size() > 0) {
			delete memoryChunks.at(0);
			memoryChunks.erase(memoryChunks.begin());
			memoryChunkInfo.erase(memoryChunkInfo.begin());
		}
	}

public:
	template <typename T>
	inline static managed_ptr<T> allocate(size_t count) {
		return managed_ptr<T>(MemoryManager::allocate(count, sizeof(T)) - SIZE_T_SIZE);
	}

	template <typename T>
	inline static T *allocateRaw(size_t count) {
		return (T *) (MemoryManager::allocate(count, sizeof(T)));
	}

	template <typename T>
	inline static void free(T *ptr) {
		byte *data = (byte *) ptr;
		byte *offsetData = data - SIZE_T_SIZE;
		MemoryManager::deallocateRaw(offsetData);
	}

};

std::vector<byte *> MemoryManager::memoryChunks = std::vector<byte *>();
std::vector<MemoryManager::MemoryInfo> MemoryManager::memoryChunkInfo = std::vector<MemoryManager::MemoryInfo>();
