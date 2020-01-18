#pragma once

template <class T>
struct Node {
	Node<T> *node;
	T t;
	Node() {
		node = NULL;
		t = NULL;
	}
	Node(Node &node) {
		this->node = node->node;
		this->t = node->t;
	}
	Node(T t) {
		this->t = t;
		this->node = NULL;
	}
	Node(T t, Node *node) {
		this->t = t;
		this->node = node;
	}
};
