#pragma once

template <class T>
struct Node {
	Node<T> *next;
	T value;
	Node() : next(NULL), value(value) {}
	Node(Node &next) : next(next.next), value(next.value) {}
	Node(T value) : value(value), next(NULL) {}
	Node(T value, Node *next) : value(value), next(next) {}
};

template <typename T>
struct BiNode {
	T value;
	BiNode<T> *left, *right;
	BiNode() : left(NULL), right(NULL) { }
	BiNode(BiNode &next) : left(next.left), right(next.right), value(next.value) { }
	BiNode(T value) : value(value), left(NULL), right(NULL) { }
	BiNode(T value, BiNode<T> *left) : value(value), left(left) {}
	BiNode(T value, BiNode<T> *left, BiNode<T> *right) : value(value), left(left), right(right) {}
};
