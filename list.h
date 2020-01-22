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

private:
	int count = 0;
	Node<T> *head = NULL, *tail = NULL;
};

template <typename T>
class ArrayList : public List<T> {
public:
	ArrayList() {
		this->resizeAmount = -1;
		this->items = (T *) calloc(this->length, sizeof(T));
	}
	ArrayList(int size) {
		this->length = size;
		this->items = (T *) calloc(size, sizeof(T));
	}
	ArrayList(int size, int resizeAmount) {
		this->length = size;
		this->resizeAmount = resizeAmount;
		this->items = (T *) calloc(size, sizeof(T));
	}
	~ArrayList() {
		delete[] this->items;
	}

	virtual bool append(T val) {
		// Time Complexity O(1)->O(n).
		T *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = (T *) calloc(this->length, sizeof(T));
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
			this->items = (T *) calloc(this->length, sizeof(T));
		}
		if (this->items != NULL) {
			// Copy the memory from the backup in sections. Before the index and after the index.
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
