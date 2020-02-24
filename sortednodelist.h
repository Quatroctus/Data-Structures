#pragma once
#include "nodelist.h"

template <typename V>
class SortedNodeList : public NodeList<V> {

public:
	SortedNodeList() : NodeList<V>() {}
	SortedNodeList(bool (*compare)(V inList, V value)) : NodeList<V>() {
		this->compare = compare;
	}
	SortedNodeList(V val, bool (*compare)(V inList, V value) = defaultCompare) : NodeList<V>(val) {
		this->compare = compare;
	}

	bool append(V val) override {
		switch (this->count) {
		case 0:
			return NodeList<V>::append(val);
		case 1:
			if (compare(NodeList<V>::head->value, val) < 0)
				return NodeList<V>::insert(val, 0);
			else
				return NodeList<V>::append(val);
		case 2:
			if (compare(NodeList<V>::head->value, val) < 0)
				return NodeList<V>::insert(val, 0);
			else if (compare(NodeList<V>::tail->value, val) < 0)
				return NodeList<V>::insert(val, 1);
			else
				return NodeList<V>::append(val);
		default:
			Node<V> *left = NULL;
			Node<V> *right = this->head;
			for (int i = 0; i < this->count; i++) {
				if (compare(right->value, val) < 0) {
					Node<V> *node = new Node<V>(val);
					if (node != NULL) {
						node->next = right;
						if (left == NULL)
							this->head = node;
						else
							left->next = node;
						this->count++;
						return true;
					} else
						// Maybe throw std::bad_alloc("SortedNodeList Could not Allocate New Array.");
						return false;
				} else if (right->next == NULL) {
					return NodeList<V>::append(val);
				}
				left = right;
				right = right->next;
			}
			return false;
		}
	}

	bool insert(V val, int index) override {
		return this->append(val);
	}

protected:
	// Returns < 0 if value should be to the left, 0 if value is the same, > 0 if value should go to the right.
	int (*compare)(V inList, V value) = defaultCompare;
};
