#pragma once
#include <string>
#include "list.h"

template <typename V>
class ArrayList : public List<V> {
public:
	ArrayList() {
		this->resizeAmount = -1;
		this->items = new V[this->length];
	}
	ArrayList(int size) {
		this->length = size;
		this->items = new V[this->length];
	}
	ArrayList(int size, int resizeAmount) {
		this->length = size;
		this->resizeAmount = resizeAmount;
		this->items = new V[this->length];
	}
	~ArrayList() override {
		delete[] items;
	}

	bool append(V val) override {
		// Time Complexity O(1)->O(n).
		V *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = new V[this->length];
			if (items != NULL) {
				std::copy(backup, backup + filled, this->items);
				delete[] backup;
			} else {
				// Maybe throw std::bad_alloc("ArrayList Could not Allocate New Array.");
				return false;
			}
		}
		this->items[filled] = val;
		filled++;
		return true;
	}

	bool insert(V val, int index) override {
		// Time Complexity O(1)->O(n).
		if (index < 0 || index > filled)
			// Maybe throw a std::out_of_range("Index: " << index << " Size: " << filled);
			return false;
		if (index == filled)
			return ArrayList<V>::append(val);
		V *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = new V[this->length];
		}
		if (this->items != NULL) {
			// Copy the memory from the backup in sections. Before the index and after the index.
			if (backup != items) // We only have to copy before the index if we created a new array.
				std::copy(backup, backup + index, this->items);
			std::copy(backup + index, backup + filled, this->items + index + 1);
		} else {
			// Maybe throw std::bad_alloc("ArrayList Could not Allocate New Array.");
			this->items = backup;
			return false;
		}
		if (this->items != backup)
			delete[] backup;
		this->items[index] = val;
		filled++;
		return true;
	}

	V remove(int index) override {
		// Time Complexity O(1)->O(n).
		if (index < 0 || index >= filled)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(filled));
		V t = items[index];
		std::copy(this->items + index + 1, this->items + filled, this->items + index);
		filled--;
		return t;
	}

	V &get(int index) override {
		// Time Complexity O(1).
		if (index < 0 || index >= filled)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(filled));
		return items[index];
	}

	inline int size() override { return filled; }

protected:
	int filled = 0, length = 1, resizeAmount;
	V *items;
};
