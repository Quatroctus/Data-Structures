#pragma once
#include "node.h"
#include "list.h"

template<typename T>
struct Queue {
	
	virtual bool enqueue(T) = 0;
	virtual T dequeue() = 0;
	virtual T& peak() = 0;

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
	~NodeQueue() {
		Node<T>* next;
		while (head != NULL) {
			next = this->head->node;
			delete this->head;
			this->head = next;
		}

	}

	virtual bool enqueue(T val) {
		// Construct Node<T> and make it the tail.
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
			return false;
	}

	virtual T dequeue() {
		// Get value from Node head and update the head.
		Node<T> *next = head->node;
		T val = head->t;
		delete head;
		head = next;
		return val;
	}

	virtual T& peak() {
		// Get value from node, but don't remove.
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

	virtual T& peak() {
		// Get first value from array but don't remove.
		return this->get(0);
	}

};

