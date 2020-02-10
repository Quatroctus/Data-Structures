#pragma once
#include <bitset>
#include <stdexcept>
#include <string>
#include <vector>

using byte = unsigned char;

constexpr size_t SIZE_T_SIZE = sizeof(size_t);
constexpr size_t MEMORY_ALLOCATION_SIZE = 1024 * 1000 * 10; // bytes * KB * MB 10 MB
constexpr size_t RANGE_START = MEMORY_ALLOCATION_SIZE / 8;

class MemoryManager;

template <typename T>
class managed_ptr {
	size_t *refCount, *size;
	T *start;

public:
	managed_ptr(size_t count, byte *start) : refCount(MemoryManager::allocateRaw<size_t>(1)), size((size_t *)start), start((T *) (start + SIZE_T_SIZE)) {
		*refCount = 1;
		*size -= SIZE_T_SIZE;
		*size /= sizeof(T);
	}
	managed_ptr(const managed_ptr &info) : refCount(info.refCount), size(info.start), start(info.start) {
		(*info.refCount)++;
	}
	~managed_ptr() {
		(*refCount)--;
		if (*refCount <= 0) {
			*size *= sizeof(T);
			*size += SIZE_T_SIZE;
			MemoryManager::template free<T>(start);
			MemoryManager::template free<size_t>(refCount);
		}
	}
	//inline size_t size() { return *size; } // Should this method exist?

	T &operator[](int index) {
		if (index < 0 || index >= *size) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(index) + " Size: " + std::to_string(*size) + ".");
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
		if (offset < 0 || offset >= *size) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(offset) + " Size: " + std::to_string(*size) + ".");
			exit(-1);
		}
		return this->start + offset;
	}

	T *operator-(size_t offset) {
		if (offset < 0 || offset >= *size) {
			throw std::out_of_range("Tried to offset managed_ptr out of managed range Offset: " + std::to_string(offset) + " Size: " + std::to_string(*size) + ".");
			exit(-1);
		}
		return this->start - offset;
	}
};

class MemoryManager {

	struct MemoryInfo {
		// Think of a way to decrease this memory footprint. Maybe actually use a std::bitset<MEMORY_ALLOCATION_SIZE>
		// Which would require a function to look for a given 'size_t size' within the bitset.
		// Basically be more memory intensive or more processing intensive.
		std::vector<std::pair<size_t, size_t>> ranges = { {RANGE_START, MEMORY_ALLOCATION_SIZE} };

		void freeRange(size_t start, size_t end);
	};

	static std::vector<byte *> memory;
	// TODO: Contemplate using one custom resizable bitset which would decrease memory usage at least slightly.
	// However the memory would be allocated on the the heap likely within. WITH THE CURRENT IMPL THIS IS ALREADY REALITY.
	static std::vector<MemoryInfo> allocated;

	static bool initOrExtendMemory();
	static byte *allocate(size_t count, size_t size);
	static void deallocateRaw(byte *start);

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
		byte *data = (byte *) pointer;
		byte *corrected = data - SIZE_T_SIZE;
		MemoryManager::deallocateRaw(corrected);
	}
};
