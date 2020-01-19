#pragma once
#include "node.h"
#include "list.h"

template<typename T>
struct Queue {
	
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
	virtual T peak() = 0;

};

template<typename T>
class NodeQueue: public Queue<T> {

public:
	NodeQueue() {
		head = NULL;
		tail = NULL;
	}
	NodeQueue(T val) {
		head = new Node<T>(val);
		tail = NULL;
	}
	NodeQueue(T val, Node<T> *tail) {
		this->tail = tail;
		head = new Node<T>(val, tail);
	}
	// Maybe a copy constructor.
	~NodeQueue() {
		Node<T> *next;
		// Oddly enough this doesn't seem to output when delete is used. Better ask Joeseph about it.
		std::cout << "Deconstructing NodeQueue" << std::endl;
		while (head != NULL) {
			next = this->head->node;
			delete this->head;
			this->head = next;
		}

	}

	virtual bool enqueue(T val) {
		// Construct Node<T> and make it the tail.
		// Time Complexity O(1).
		Node<T>*node = new Node<T>(val);
		if (node != NULL) {
			if (tail != NULL) {
				tail->node = node;
				tail = node;
			} else {
				this->head = node;
				this->tail = node;
			}
			
			return true;
		} else
			// Maybe throw a std::bad_alloc exception.
			return false;
	}

	virtual T dequeue() {
		// Get value from Node head and update the head.
		// Time Complexity O(1).
		Node<T> *next = head->node;
		T val = head->t;
		delete head;
		head = next;
		return val;
	}

	virtual T peak() {
		// Get value from node, but don't remove.
		// Time Complexity O(1).
		if (head == NULL)
			throw std::length_error("Cannot Peak From an Empty Queue.");
		return head->t;
	}

private:
	Node<T> *head, *tail;
};

template <typename T>
class ArrayQueue: public Queue<T>, private ArrayList<T> {

public:
	ArrayQueue(): ArrayList<T>(1, 1) {}
	ArrayQueue(int size): ArrayList<T>(size, 1){}
	// Maybe a copy constructor.

	virtual bool enqueue(T val) {
		// Append to end of array and increase array size when necessary.
		return this->append(val);
	}

	virtual T dequeue() {
		// Get value from array and shift array.
		return this->remove(0);
	}

	virtual T peak() {
		// Get first value from array but don't remove.
		if (this->filled == 0)
			throw std::length_error("Cannot Peak From an Empty Queue.");
		return this->get(0);
	}

};

