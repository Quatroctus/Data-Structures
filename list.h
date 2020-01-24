#pragma once
#include "node.h"
#include <memory>

template <typename T>
class List {
public:
	/**
	Appends a value to the back of this List.
	
	Ex. For a list which looks like [1, 2, 3, 4, 5, 6, 7]
	calling List::append(1000) would change it to [1, 2, 3, 4, 5, 6, 7, 1000].
	
	@param val The value to be appended to the back of the List.
	@return Whether the operation succeeded or not.
	*/
	virtual bool append(T val) = 0;
	/**
	Inserts a value into this ArrayList at the specified index.
	
	Ex. For a list which looks like [1, 2, 3, 4, 5, 6, 7]
	calling List::insert(1000, 4) would change it to [1, 2, 3, 4, 1000, 5, 6, 7].
	
	@param val The value to be inserted into the List.
	@param index The index of the list for which to insert at.
	@return Whether the operation succeeded or not.
	*/
	virtual bool insert(T val, int index = 0) = 0;
	/**
	Removes a value at the specified index.
	
	Ex. For a list which looks like [1, 2, 3, 4, 5, 6, 7]
	calling List::remove(1) would change it to [1, 3, 4, 5, 6, 7] and return 2.
	
	@param index The index of the list for which to remove.
	@return The value which was removed at the index.
	*/
	virtual T remove(int index) = 0;
	/**
	Returns the value at the index as a reference.
	
	Ex. For a list that looks like [1, 2, 3, 4, 5, 6, 7]
	calling List::get(5) will return 6
	
	@param index The index of the value you want.
	@return The value at the specified index.
	*/
	virtual T &get(int index) = 0;
	/**
	@return The filled size of the List.
	*/
	virtual int size() = 0;
};

template <typename T>
class NodeList : public List<T> {
public:
	NodeList() {}
	NodeList(T val) {
		this->head = new Node<T>(val);
		this->tail = this->head;
		this->count = 1;
	}
	~NodeList() {
		Node<T> *next;
		// Oddly enough this doesn't seem to output when delete is used. Better ask Joeseph about it.
		std::cout << "Deconstructing NodeList" << std::endl;
		while (head != NULL) {
			next = this->head->node;
			delete this->head;
			this->head = next;
		}
	}

	virtual bool append(T val) {
		Node<T> *next = new Node<T>(val);
		if (next != NULL) {
			if (head == NULL) {
				this->head = next;
				this->tail = next;
			} else {
				this->tail->node = next;
				this->tail = next;
			}
			this->count++;
			return true;
		} else {
			// Maybe throw std::bad_alloc("ArrayList Could not Allocate New Array.");
			return false;
		}
	}

	virtual bool insert(T val, int index) {
		if (index == count)
			return append(val);
		if (index > count || index < 0)
			// Maybe throw a std::out_of_range("Index: " << index << " Size: " << filled);
			return false;
		Node<T> *inserted = new Node<T>(val);
		if (inserted != NULL) {
			Node<T> *prev = head;
			for (int i = 0; i < index - 1; i++)
				prev = prev->node;
			inserted->node = prev->node;
			prev->node = inserted;
			this->count++;
			return true;
		} else {
			// Maybe throw std::bad_alloc("ArrayList Could not Allocate New Array.");
			return false;
		}
	}

	virtual T remove(int index) {
		if (index > count || index < 0)
			// Maybe throw a std::out_of_range("Index: " << index << " Size: " << filled);
			return false;
		Node<T> *prev = head;
		for (int i = 0; i < index - 1; i++)
			prev = prev->node;
		Node<T> *removal = prev->node;
		prev->node = removal->node;
		T t = removal->t;
		delete removal;
		this->count--;
		return t;
	}

	virtual T &get(int index) {
		Node<T> *node = head;
		for (int i = 0; i < index; i++)
			node = node->node;
		return node->t;
	}

	inline virtual int size() { return count; }

protected:
	int count = 0;
	Node<T> *head = NULL, *tail = NULL;
};

template <typename T>
class ArrayList : public List<T> {
public:
	ArrayList() {
		this->resizeAmount = -1;
		this->items = new T[this->length];
	}
	ArrayList(int size) {
		this->length = size;
		this->items = new T[this->length];
	}
	ArrayList(int size, int resizeAmount) {
		this->length = size;
		this->resizeAmount = resizeAmount;
		this->items = new T[this->length];
	}
	~ArrayList() {
		delete[] this->items;
	}

	virtual bool append(T val) {
		// Time Complexity O(1)->O(n).
		T *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = new T[this->length];
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

	virtual bool insert(T val, int index) {
		// Time Complexity O(1)->O(n).
		if (index < 0 || index > filled)
			// Maybe throw a std::out_of_range("Index: " << index << " Size: " << filled);
			return false;
		if (index == filled)
			return this->append(val);
		T *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = new T[this->length];
		}
		if (this->items != NULL) {
			// Copy the memory from the backup in sections. Before the index and after the index.
			if (backup != items) // We only have to copy before the index if we created a new array.
				std::copy(backup, backup + index - 1, this->items);
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

	virtual T remove(int index) {
		// Time Complexity O(1)->O(n).
		if (index < 0 || index >= filled)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(filled));
		T t = items[index];
		std::copy(this->items + index + 1, this->items + filled, this->items + index);
		filled--;
		return t;
	}

	virtual T &get(int index) {
		// Time Complexity O(1).
		if (index < 0 || index >= filled)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(filled));
		return items[index];
	}

	inline virtual int size() { return filled; }

protected:
	int filled = 0, length = 0, resizeAmount;
	T *items;
};

template <typename T>
bool defaultCompare(T inList, T value) {
	return value < inList;
}

template <typename T>
class SortedNodeList : public NodeList<T> {

public:
	SortedNodeList(): NodeList<T>() {}
	SortedNodeList(bool (*compare)(T inList, T value)): NodeList<T>() {
		this->compare = compare;
	}
	SortedNodeList(T val, bool (*compare)(T inList, T value)=defaultCompare): NodeList<T>(val) {
		this->compare = compare;
	}

	virtual bool append(T val) {
		switch (this->count) {
		case 0:
			return NodeList<T>::append(val);
		case 1:
			if (compare(NodeList<T>::head->t, val))
				return NodeList<T>::insert(val, 0);
			else
				return NodeList<T>::append(val);
		case 2:
			if (compare(NodeList<T>::head->t, val))
				return NodeList<T>::insert(val, 0);
			else if (compare(NodeList<T>::tail->t, val))
				return NodeList<T>::insert(val, 1);
			else
				return NodeList<T>::append(val);
		default:
			// TODO: Binary search then insert insertion.
			break;
		}
	}

	virtual bool insert(T val, int index) {
		return this->append(val);
	}

protected:
	// Returns true if the value should be to the left of inList.
	bool (*compare)(T inList, T value) = defaultCompare;

};

template <typename T>
class SortedArrayList : public ArrayList<T> {

public:
	SortedArrayList(): ArrayList<T>() {}
	SortedArrayList(int size) : ArrayList<T>(size) {}
	SortedArrayList(int size, int resizeAmount): ArrayList<T>(size, resizeAmount) {}
	SortedArrayList(bool (*compare)(T inList, T value)): ArrayList<T>() {
		this->compare = compare;
	}
	SortedArrayList(int size, bool (*compare)(T inlist, T value)): ArrayList<T>(size) {
		this->compare = compare;
	}
	SortedArrayList(int size, int resizeAmount, bool (*compare)(T inList, T value)): ArrayList<T>(size, resizeAmount) {
		this->compare = compare;
	}

	virtual bool append(T val) {
		// Cover the base cases of 0, 1, 2 items already in the list.
		switch (this->filled) {
		case 0:
			return ArrayList<T>::append(val);
		case 1:
			if (compare(this->items[0], val))
				return ArrayList<T>::insert(val, 0);
			else
				return ArrayList<T>::append(val);
		case 2:
			if (compare(this->items[0], val))
				return ArrayList<T>::insert(val, 0);
			else if (compare(this->items[1], val))
				return ArrayList<T>::insert(val, 1);
			else
				return ArrayList<T>::append(val);
		default:
			// TODO: Binary search then insert insertion.
			break;
		}
	}

	virtual bool insert(T val, int index) {
		return this->append(val);
	}

protected:
	// Returns true if the value should be to the left of inList.
	bool (*compare)(T inList, T value) = defaultCompare;
};
