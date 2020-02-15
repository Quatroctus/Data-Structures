#pragma once
#include <bitset>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef MEMORY_ALLOCATION
constexpr size_t MEMORY_ALLOCATION_SIZE = MEMORY_ALLOCATION;
#else
constexpr size_t MEMORY_ALLOCATION_SIZE = 10240000;
#endif

constexpr size_t SIZE_T_SIZE = sizeof(size_t);
#ifdef USE_BITSET
const size_t RANGE_START = size_t(std::ceil(MEMORY_ALLOCATION_SIZE / 8.0));
#else
constexpr size_t RANGE_START = 0;
#endif

using byte = unsigned char;

class MemoryManager;

template <typename T>
struct ManagedAllocator : public std::allocator<T> {
	
	inline T *allocate(size_t size) {
		return MemoryManager::allocateRaw(size / sizeof(T));
	}
	inline void deallocate(T *ptr, size_t size) {
		MemoryManager::free(ptr);
	}
};

template <typename T>
class managed_ptr {
	size_t *refCount, *length;
	T *start;

public:
	managed_ptr(byte *start) : refCount(MemoryManager::allocateRaw<size_t>(1)), length((size_t *) start), start(reinterpret_cast<T *>(start + SIZE_T_SIZE)) {
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
	inline size_t size() { return (*length); } // I like it. Proves useful since size is already stored no need to have user store it twice or calculate it everytime.
	T &operator[](int index) {
		if (index < 0 || index >= *length) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(index) + " Size: " + std::to_string(*length) + ".");
			exit(-1);
		}
		return this->start[index];
	}
	inline T &operator*() {
		return *this->start;
	}
	inline T *operator->() {
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
		byte *memory;
		std::bitset<MEMORY_ALLOCATION_SIZE> *bits;
		MemoryInfo(byte *memory) : bits(NULL), memory(memory) {}
		void freeRange(size_t start, size_t end) {
			for (; start < end; start++) {
				this->bits->set(start, false);
			}
		}
	};
	#else
	struct MemoryInfo {
		byte *memory;
		std::vector<std::pair<size_t, size_t>> ranges;
		MemoryInfo(byte *memory) : ranges({ { RANGE_START, MEMORY_ALLOCATION_SIZE } }), memory(memory) {}
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
	
	static std::vector<MemoryInfo> memoryChunks;

	static bool initOrExtendMemory() {
		byte *raw = static_cast<byte *>(calloc(MEMORY_ALLOCATION_SIZE + RANGE_START, 1));
		if (raw == NULL)
			return false;
		memoryChunks.push_back(MemoryInfo(raw));
		#ifdef USE_BITSET
		memoryChunks.back().bits = MemoryManager::allocateRaw<std::bitset<MEMORY_ALLOCATION_SIZE>>(1);
		#endif
		return true;
	}
	static byte *allocate(size_t count, size_t size) {
		byte *allocation = NULL;
		size_t byteSize = count * size + SIZE_T_SIZE;
		MemoryInfo *info = NULL;
		for (size_t i = 0; i < memoryChunks.size() && allocation == NULL; i++) {
			info = &memoryChunks.at(i);
#ifdef USE_BITSET
			if (info->bits == NULL) {
				return memoryChunks.at(i).memory;
			} else {
				size_t start = RANGE_START - 1;
				for (size_t j = 0; j < MEMORY_ALLOCATION_SIZE; j++) {
					bool free = !info->bits->test(j);
					if (free && start == RANGE_START - 1)
						start = j + RANGE_START;
					else if (free && ((j + RANGE_START) - start) == byteSize - 1) {
						allocation = memoryChunks.at(i).memory + start;
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
					allocation = memoryChunks.at(i).memory + range.first;
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
			allocation = memoryChunks.at(index).memory + RANGE_START;
#ifdef USE_BITSET
			for (size_t i = 0; i < byteSize; i++)
				memoryChunks.at(index).bits->set(i, true);
#else
			memoryChunks.at(index).ranges.at(0).first += byteSize;
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
			byte *memory = memoryChunks.at(i).memory;
			if (memory <= start && memory + MEMORY_ALLOCATION_SIZE + RANGE_START >= start + size) {
				memOffset = start - memory - RANGE_START;
				size += memOffset;
				(&memoryChunks.at(i))->freeRange(memOffset, size);
				foundMemInfo = true;
				break;
			}
		}
		if (!foundMemInfo) {
			delete[] (start - SIZE_T_SIZE);
		}
	}

public:
	template <typename T>
	inline static managed_ptr<T> allocate(size_t count) {
		return managed_ptr<T>(MemoryManager::allocate(count, sizeof(T)) - SIZE_T_SIZE);
	}

	template <typename T>
	inline static T *allocateRaw(size_t count) {
		return reinterpret_cast<T *> (MemoryManager::allocate(count, sizeof(T)));
	}

	template <typename T>
	inline static void free(T *ptr) {
		byte *data = reinterpret_cast<byte *>(ptr);
		byte *offsetData = data - SIZE_T_SIZE;
		MemoryManager::deallocateRaw(offsetData);
	}

	static void freeMemoryChunks() {
		while (memoryChunks.size() > 0) {
			delete memoryChunks.at(0).memory;
			memoryChunks.erase(memoryChunks.begin());
		}
	}

};

std::vector<MemoryManager::MemoryInfo> MemoryManager::memoryChunks = std::vector<MemoryManager::MemoryInfo>();
