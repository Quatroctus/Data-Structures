#pragma once
#include "list.h"
#include "node.h"

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

	bool append(T val) override {
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

	bool insert(T val, int index) override {
		if (index == count)
			return NodeList<T>::append(val);
		if (index > count || index < 0)
			// Maybe throw a std::out_of_range("Index: " << index << " Size: " << filled);
			return false;
		Node<T> *inserted = new Node<T>(val);
		if (inserted != NULL) {
			if (index == 0) {
				inserted->node = this->head;
				this->head = inserted;
			} else {
				Node<T> *prev = head;
				for (int i = 0; i < index - 1; i++)
					prev = prev->node;
				inserted->node = prev->node;
				prev->node = inserted;
			}
			this->count++;
			return true;
		} else {
			// Maybe throw std::bad_alloc("ArrayList Could not Allocate New Array.");
			return false;
		}
	}

	T remove(int index) override{
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

	T &get(int index) override {
		Node<T> *node = head;
		for (int i = 0; i < index; i++)
			node = node->node;
		return node->t;
	}

	inline int size() override { return count; }

protected:
	int count = 0;
	Node<T> *head = NULL, *tail = NULL;
};
