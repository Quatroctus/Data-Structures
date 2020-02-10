#pragma once
#include "arraylist.h"
#include "node.h"

template <typename T>
class LinkedArrayList : List<T> {
	
	size_t arrayLength;
	Node<ArrayList<T>> *head;

public:
	LinkedArrayList(size_t arrayLength) : arrayLength(arrayLength), head = NULL {}


	bool append(T val) override {
		
	}

	bool insert(T val) override {
		
	}

	T remove(int index) override {
		T &val = get(index);
		val = T();
		return val;
	}

	T &get(int index) override {
		int arrayIndex = index / arrayLength, itemIndex = index % arrayLength;
		Node<T *> *location = head;
		for (int i = 0; i < arrayIndex; i++)
			head = head->next;
		return location->value[itemIndex];
	}

};
