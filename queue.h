#pragma once
#include "nodelist.h"
#include "arraylist.h"

template<typename V>
struct Queue {
	virtual ~Queue() {}

	/**
	Adds a value to the back of the Queue.
	 
	@param val The value to add to the Queue.
	@return Whether enqueuing was a success.
	*/
	virtual bool enqueue(V val) = 0;
	/**
	Remove the front value of the Queue and return it.
	
	@return The value in the front of the Queue.
	*/
	virtual V dequeue() = 0;
	/**
	Return the front value of the Queue but don't remove it.
	
	@return The front value of the Queue.
	*/
	virtual V peek() = 0;

};

template<typename V>
struct NodeQueue : public Queue<V>, protected NodeList<V> {
	~NodeQueue() override {
		NodeList<V>::~NodeList();
	}

	bool enqueue(V val) override {
		return NodeList<V>::append(val);
	}

	V dequeue() override {
		return NodeList<V>::remove(0);
	}

	V peek() override {
		return NodeList<V>::get(0);
	}
};

template <typename V>
struct ArrayQueue : public Queue<V>, protected ArrayList<V> {
	ArrayQueue(): ArrayList<V>(1, 1) {}
	ArrayQueue(int size): ArrayList<V>(size, 1){}
	~ArrayQueue() override {
		ArrayList<V>::~ArrayList();
	}

	bool enqueue(V val) override {
		return this->append(val);
	}

	V dequeue() override {
		return this->remove(0);
	}

	V peek() override {
		if (this->filled == 0)
			throw std::length_error("Cannot Peak From an Empty Queue.");
		return this->get(0);
	}

};

