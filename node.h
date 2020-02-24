#pragma once

template <class V>
struct Node {
	Node<V> *next;
	V value;
	Node() : next(NULL), value(value) {}
	Node(Node &next) : next(next.next), value(next.value) {}
	Node(V value) : value(value), next(NULL) {}
	Node(V value, Node *next) : value(value), next(next) {}
};

template <typename V>
struct BiNode {
	V value;
	BiNode<V> *left, *right;
	BiNode() : left(NULL), right(NULL) { }
	BiNode(BiNode &next) : left(next.left), right(next.right), value(next.value) { }
	BiNode(V value) : value(value), left(NULL), right(NULL) { }
	BiNode(V value, BiNode<V> *left) : value(value), left(left) {}
	BiNode(V value, BiNode<V> *left, BiNode<V> *right) : value(value), left(left), right(right) {}
};
