#pragma once
#include "nodelist.h"
#include "arraylist.h"

template<typename T>
struct Queue {
	virtual ~Queue() {}

	/**
	Adds a value to the back of the Queue.
	 
	@param val The value to add to the Queue.
	@return Whether enqueuing was a success.
	*/
	virtual bool enqueue(T val) = 0;
	/**
	Remove the front value of the Queue and return it.
	
	@return The value in the front of the Queue.
	*/
	virtual T dequeue() = 0;
	/**
	Return the front value of the Queue but don't remove it.
	
	@return The front value of the Queue.
	*/
	virtual T peek() = 0;

};

template<typename T>
struct NodeQueue : public Queue<T>, protected NodeList<T> {
	~NodeQueue() override {
		NodeList<T>::~NodeList();
	}

	bool enqueue(T val) override {
		return NodeList<T>::append(val);
	}

	T dequeue() override {
		return NodeList<T>::remove(0);
	}

	T peek() override {
		return NodeList<T>::get(0);
	}
};

template <typename T>
struct ArrayQueue : public Queue<T>, protected ArrayList<T> {
	ArrayQueue(): ArrayList<T>(1, 1) {}
	ArrayQueue(int size): ArrayList<T>(size, 1){}
	~ArrayQueue() override {
		ArrayList<T>::~ArrayList();
	}

	bool enqueue(T val) override {
		return this->append(val);
	}

	T dequeue() override {
		return this->remove(0);
	}

	T peek() override {
		if (this->filled == 0)
			throw std::length_error("Cannot Peak From an Empty Queue.");
		return this->get(0);
	}

};

