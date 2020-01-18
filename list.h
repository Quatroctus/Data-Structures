#pragma once
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
	virtual T& get(int index) = 0;
	/**
	@return The filled size of the List.
	*/
	virtual int size() = 0;

	/**
	@return The allocated size of this List.
	*/
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
		// Time Complexity O(1)->O(n).
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
		// Time Complexity O(1)->O(n).
		if (index < 0 || index > filled)
			// Maybe throw Out of Bounds Exception.
			return false;
		if (index == filled)
			return this->append(val);
		T *backup = this->items;
		if (filled == this->length) {
			this->length = resizeAmount < 0 ? this->length * 2 : this->length + resizeAmount;
			this->items = (T *)calloc(this->length, sizeof(T));
		}
		if (this->items != NULL) {
			// Copy the memory from the backup in sections. Before the index and after the index.
			std::memcpy(this->items, backup, index * sizeof(T));
			std::memcpy(this->items + index + 1, backup + index, (filled - index) * sizeof(T));
		} else {
			// Maybe throw an Out of Memory Exception.
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
			throw "Index Out of Bounds Exception";
		T t = items[index];
		std::memcpy(this->items + index, this->items + index+1, (this->filled - index) * sizeof(T));
		filled--;
		return t;
	}

	virtual T& get(int index) {
		// Time Complexity O(1).
		if (index < 0 || index >= filled)
			throw "Index Out of Bounds Exception";
		return items[index];
	}

	inline virtual int size() { return filled; }

protected:
	int filled = 0, resizeAmount;
	T *items;
};
