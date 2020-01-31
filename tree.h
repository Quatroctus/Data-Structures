#pragma once
#include "list.h"
#include "node.h"

template <typename T>
struct Tree : public List<T> {};

template <typename T>
class BinarySearchTree : public Tree<T> {

	T value;
	BinarySearchTree<T> *left, *right;
	int count;
	int (*compare)(T inTree, T value) = defaultCompare;

public:
	BinarySearchTree(T val) : value(val) {}

	bool append(T val) override {
		int comparison = compare(value, val);
		bool added = false;
		if (comparison < 0) {
			if (left == NULL) {
				BinarySearchTree<T> *tree = new BinarySearchTree(val);
				this->left = tree;
				added = true;
			} else
				added = left->append(val);
		} else if (comparison == 0) {
			BinarySearchTree<T> *tree = new BinarySearchTree(val);
			tree->left = this->left;
			this->left = tree;
			added = true;
		} else {
			if (right == NULL) {
				BinarySearchTree<T> *tree = new BinarySearchTree(val);
				this->right = tree;
				added = true;
			} else
				added = right->append(val);
		}
		if (added)
			count++;
		return added;
	}

	bool insert(T val, int index) override {
		return append(val);
	}

	T remove(int index) override {
		// TODO
		/* This is the steps to achieve the 
		First find the parent of the Node that needs to be removed.
		Then find the Node that needs to be removed. 
		Then find the parent of the most left Node of the Node that needs to be removed.
		Grab the most left Node from it's parent.
		Give the parent of the left most Node's left the most left Node's right.
		Give the left most Node the Node that must be removed left and right Node.
		Give the parent of the Node that needs to be removed the left most Node.
		*/
	}

	T &get(int index) override {
		if (left == NULL && index == 0 || left != NULL && left->count == index)
			return this->value;
		else if (left != NULL && left->count > index)
			return left->get(index);
		else
			return this->right->get(left != NULL ? (index - left->count) - 1 : index - 1);
	}

	inline int size() override { return count; }
};
