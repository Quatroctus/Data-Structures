#pragma once
#include "nodelist.h"

template <typename T>
class SortedNodeList : public NodeList<T> {

public:
	SortedNodeList() : NodeList<T>() {}
	SortedNodeList(bool (*compare)(T inList, T value)) : NodeList<T>() {
		this->compare = compare;
	}
	SortedNodeList(T val, bool (*compare)(T inList, T value) = defaultCompare) : NodeList<T>(val) {
		this->compare = compare;
	}

	bool append(T val) override {
		switch (this->count) {
		case 0:
			return NodeList<T>::append(val);
		case 1:
			if (compare(NodeList<T>::head->t, val) < 0)
				return NodeList<T>::insert(val, 0);
			else
				return NodeList<T>::append(val);
		case 2:
			if (compare(NodeList<T>::head->t, val) < 0)
				return NodeList<T>::insert(val, 0);
			else if (compare(NodeList<T>::tail->t, val) < 0)
				return NodeList<T>::insert(val, 1);
			else
				return NodeList<T>::append(val);
		default:
			Node<T> *left = NULL;
			Node<T> *right = this->head;
			for (int i = 0; i < this->count; i++) {
				if (compare(right->t, val) < 0) {
					Node<T> *node = new Node<T>(val);
					if (node != NULL) {
						node->node = right;
						if (left == NULL)
							this->head = node;
						else
							left->node = node;
						this->count++;
						return true;
					} else
						// Maybe throw std::bad_alloc("SortedNodeList Could not Allocate New Array.");
						return false;
				} else if (right->node == NULL) {
					return NodeList<T>::append(val);
				}
				left = right;
				right = right->node;
			}
			return false;
		}
	}

	bool insert(T val, int index) override {
		return this->append(val);
	}

protected:
	// Returns < 0 if value should be to the left, 0 if value is the same, > 0 if value should go to the right.
	int (*compare)(T inList, T value) = defaultCompare;
};
