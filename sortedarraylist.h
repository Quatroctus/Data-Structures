#pragma once
#include "arraylist.h"

template <typename V>
class SortedArrayList : public ArrayList<V> {

public:
	SortedArrayList() : ArrayList<V>() {}
	SortedArrayList(int size) : ArrayList<V>(size) {}
	SortedArrayList(int size, int resizeAmount) : ArrayList<V>(size, resizeAmount) {}
	SortedArrayList(bool (*compare)(V inList, V value)) : ArrayList<V>() {
		this->compare = compare;
	}
	SortedArrayList(int size, bool (*compare)(V inlist, V value)) : ArrayList<V>(size) {
		this->compare = compare;
	}
	SortedArrayList(int size, int resizeAmount, bool (*compare)(V inList, V value)) : ArrayList<V>(size, resizeAmount) {
		this->compare = compare;
	}

	bool append(V val) override {
		// Cover the base cases of 0, 1, 2 items already in the list.
		switch (this->filled) {
		case 0:
			return ArrayList<V>::append(val);
		case 1:
			if (compare(this->items[0], val) < 0)
				return ArrayList<V>::insert(val, 0);
			else
				return ArrayList<V>::append(val);
		case 2:
			if (compare(this->items[0], val) < 0)
				return ArrayList<V>::insert(val, 0);
			else if (compare(this->items[1], val) < 0)
				return ArrayList<V>::insert(val, 1);
			else
				return ArrayList<V>::append(val);
		default:
			// Binary search then insert insertion.
			int left = 0, right = this->filled;
			int current = right / 2;
			while (left != current && right != current) {
				int comparison = compare(this->get(current), val);
				if (comparison == 0)
					break;
				else if (comparison < 0) {
					right = current;
					current = left + ((current - left) / 2);
				} else {
					left = current;
					current += (right - current) / 2;
				}
			}
			int comparison = compare(this->get(current), val);
			return ArrayList<V>::insert(val, comparison <= 0 ? current : current + 1);
		}
	}

	bool insert(V val, int index) override {
		return this->append(val);
	}

protected:
	// Returns < 0 if value should be to the left, 0 if value is the same, > 0 if value should go to the right.
	int (*compare)(V inList, V value) = defaultCompare;
};
