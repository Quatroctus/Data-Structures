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
	~NodeList() override {
		Node<T> *next;
		while (head != NULL) {
			next = this->head->next;
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
				this->tail->next = next;
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
				inserted->next = this->head;
				this->head = inserted;
			} else {
				Node<T> *prev = head;
				for (int i = 0; i < index - 1; i++)
					prev = prev->next;
				inserted->next = prev->next;
				prev->next = inserted;
			}
			this->count++;
			return true;
		} else {
			// Maybe throw std::bad_alloc("ArrayList Could not Allocate New Array.");
			return false;
		}
	}

	T remove(int index) override {
		if (index > count || index < 0)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(count));
		if (index == 0) {
			Node<T> *remove = head;
			head = head->next;
			T value = remove->value;
			delete remove;
			this->count--;
			return value;
		} else {
			Node<T> *prev = head;
			for (int i = 0; i < index - 1; i++)
				prev = prev->next;
			Node<T> *removal = prev->next;
			prev->next = removal->next;
			T t = removal->value;
			delete removal;
			this->count--;
			return t;
		}
	}

	T &get(int index) override {
		if (index < 0 || index >= count)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(count));
		Node<T> *node = head;
		for (int i = 0; i < index; i++)
			node = node->next;
		return node->value;
	}

	inline int size() override { return count; }

protected:
	int count = 0;
	Node<T> *head = NULL, *tail = NULL;
};
