#pragma once
#include "list.h"
#include "node.h"

template <typename V>
class NodeList : public List<V> {
public:
	NodeList() {}
	NodeList(V val) {
		this->head = new Node<V>(val);
		this->tail = this->head;
		this->count = 1;
	}
	~NodeList() override {
		Node<V> *next;
		while (head != NULL) {
			next = this->head->next;
			delete this->head;
			this->head = next;
		}
	}

	bool append(V val) override {
		Node<V> *next = new Node<V>(val);
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

	bool insert(V val, int index) override {
		if (index == count)
			return NodeList<V>::append(val);
		if (index > count || index < 0)
			// Maybe throw a std::out_of_range("Index: " << index << " Size: " << filled);
			return false;
		Node<V> *inserted = new Node<V>(val);
		if (inserted != NULL) {
			if (index == 0) {
				inserted->next = this->head;
				this->head = inserted;
			} else {
				Node<V> *prev = head;
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

	V remove(int index) override {
		if (index > count || index < 0)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(count));
		if (index == 0) {
			Node<V> *remove = head;
			head = head->next;
			V value = remove->value;
			delete remove;
			this->count--;
			return value;
		} else {
			Node<V> *prev = head;
			for (int i = 0; i < index - 1; i++)
				prev = prev->next;
			Node<V> *removal = prev->next;
			prev->next = removal->next;
			V t = removal->value;
			delete removal;
			this->count--;
			return t;
		}
	}

	V &get(int index) override {
		if (index < 0 || index >= count)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(count));
		Node<V> *node = head;
		for (int i = 0; i < index; i++)
			node = node->next;
		return node->value;
	}

	inline int size() override { return count; }

protected:
	int count = 0;
	Node<V> *head = NULL, *tail = NULL;
};
