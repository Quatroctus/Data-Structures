#pragma once
#include <memory>

template <typename T>
struct List {
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
int defaultCompare(T inList, T value) {
	return value < inList ? -1 : value == inList ? 0 : 1;
}
