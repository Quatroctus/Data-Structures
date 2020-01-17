#pragma once
#include <memory>

template <typename T>
class List {
public:
	virtual bool append(T) = 0;
	virtual bool insert(T, int index = 0) = 0;
	virtual T remove(int) = 0;
	virtual T& get(int) = 0;
	virtual int size() = 0;

	inline int allocatedSize() { return this->length; }

protected:
	int length = 1;
};

template <typename T>
class ArrayList: public List<T> {
public:
	ArrayList() {
		this->resizeAmount = -1;
		this->items = (T *) calloc(this->length, sizeof(T));
	}
	ArrayList(int size) {
		this->length = size;
		this->items = (T*) calloc(size, sizeof(T));
	}
	ArrayList(int size, int resizeAmount) {
		this->length = size;
		this->resizeAmount = resizeAmount;
		this->items = (T *) calloc(size, sizeof(T));
	}
	~ArrayList() {
		delete []this->items;
	}

	virtual bool append(T val) {
		T *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = (T *) calloc(this->length, sizeof(T));
			if (items != NULL) {
				std::memcpy(this->items, backup, (filled) * sizeof(T));
				delete []backup;
			} else {
				// Maybe throw a Out of Memory Exception.
				return false;
			}
		}
		this->items[filled] = val;
		filled++;
		return true;
	}

	virtual bool insert(T val, int index) {
		if (index < 0 || index > filled)
			return false;
		if (index == filled)
			return this->append(val);
		T *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = (T *) calloc(this->length, sizeof(T));
		}
		if (this->items != NULL) {
			// Copy the memory from the backup in sections. Before the index and after the index.
			std::memcpy(this->items, backup, index * sizeof(T));
			std::memcpy(this->items + index + 1, backup + index, (filled - index) * sizeof(T));
		}
		else {
			// Maybe throw an Out of Memory Exception.
			return false;
		}
		this->items[index] = val;
		filled++;
		return true;
	}

	virtual T remove(int index) {
		if (index < 0 || index >= filled)
			throw "Index Out of Bounds Exception";
		T t = items[index];
		std::memcpy(this->items + index, this->items + index+1, (this->filled - index) * sizeof(T));
		filled--;
		return t;
	}

	virtual T& get(int index) {
		if (index < 0 || index >= filled)
			throw "Index Out of Bounds Exception";
		return items[index];
	}

	inline virtual int size() { return filled; }

protected:
	int filled = 0, resizeAmount;
	T *items;
};
