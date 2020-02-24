#pragma
#include "nodelist.h"
#include <iostream>

// TODO: Restructure this whole file. Move the BinarySearchTree into it's own file.
// TODO: Restructure the class dynamics/methods to include different traversal orders.
// TODO: Potentially use a BiNode and container structure to make the BinarySearchTree.
template <typename V, typename K>
struct Tree : public List<V> {
	virtual ~Tree() {}

	virtual void getTree(int index, K **prev, K **current, bool remove = false) = 0;
	virtual bool search(V value, K **prev, K**curent, int *index = NULL, bool remove = false) = 0;
	virtual bool removeValue(V val) = 0;
	virtual void print(std::ostream &output, unsigned int order, unsigned int indent = 0) = 0;
	virtual NodeList<V> &breadthFirst(NodeList<V> *list = new NodeList<V>()) = 0;

	virtual void destroyTree() {
		this->~Tree();
	}
};

template <typename V>
class BinarySearchTree : public Tree<V, BinarySearchTree<V>> {

	V value;
	BinarySearchTree<V> *left = NULL, *right = NULL;
	int count = 0;
	int (*compare)(V inTree, V value) = defaultCompare;

public:
	static const unsigned int PRE_ORDER = 0, IN_ORDER = 1, POST_ORDER = 2;

	BinarySearchTree(V val) : value(val) {}
	BinarySearchTree(int (*compare)(V inTree, V value)) : compare(compare) {}
	BinarySearchTree(V val, int (*compare)(V inTree, V value)) : value(val), compare(compare) {}

	bool append(V val) override {
		int comparison = compare(value, val);
		bool added = false;
		if (comparison < 0) {
			if (left == NULL) {
				BinarySearchTree<V> *tree = new BinarySearchTree(val, compare);
				this->left = tree;
				added = true;
			} else
				added = left->append(val);
		} else if (comparison == 0) {
			BinarySearchTree<V> *tree = new BinarySearchTree(val, compare);
			tree->left = this->left;
			this->left = tree;
			added = true;
		} else {
			if (right == NULL) {
				BinarySearchTree<V> *tree = new BinarySearchTree(val, compare);
				this->right = tree;
				added = true;
			} else
				added = right->append(val);
		}
		if (added)
			count++;
		return added;
	}

	bool insert(V val, int index) override {
		return append(val);
	}

	bool removeValue(V val) override {
		int index;
		BinarySearchTree<V> *prev = NULL, *toBeRemoved;
		if (search(val, &prev, &toBeRemoved, &index, true)) {
			bool leftOfPrev = prev != NULL ? prev->left == toBeRemoved : false;
			if (prev != NULL && toBeRemoved->left == NULL && toBeRemoved->right == NULL) {
				if (leftOfPrev)
					prev->left = NULL;
				else
					prev->right = NULL;
			} else {
				if (toBeRemoved->right == NULL && prev != NULL) {
					if (leftOfPrev)
						prev->left = toBeRemoved->left;
					else
						prev->right = toBeRemoved->left;
				} else if (prev != NULL) {
					BinarySearchTree<V> *prevLeftMost = toBeRemoved, *leftMost;
					// Get the left most node of the node to the right.
					// Doing it this way decreases our time complexity from O(2log(n))-O(n) to O(log(n))-O(n). Which is minimal.
					toBeRemoved->right->getTree(0, &prevLeftMost, &leftMost, true);
					if (prevLeftMost == toBeRemoved)
						prevLeftMost->right = NULL;
					else
						prevLeftMost->left = NULL;
					if (leftOfPrev)
						prev->left = leftMost;
					else
						prev->right = leftMost;
					//}
				} else { // prev == NULL which means we are at the root of our tree.
					// Current Implementation will correct the tree so that if we remove the root we change the data of the root and delete what ever BinarySearchTree we took values from.
					if (toBeRemoved->right == NULL && this->left != NULL) {
						BinarySearchTree<V> *left = this->left;
						this->right = this->left->right;
						this->left = this->left->left;
						this->value = left->value;
						toBeRemoved = left;
					} else if (toBeRemoved->right != NULL) {
						BinarySearchTree<V> *prevLeftMost = toBeRemoved, *leftMost;
						toBeRemoved->right->getTree(0, &prevLeftMost, &leftMost, true);
						if (prevLeftMost == toBeRemoved)
							prevLeftMost->right = NULL;
						else
							prevLeftMost->left = NULL;
						this->value = leftMost->value;
						toBeRemoved = leftMost;
					}
				}
			}
			delete toBeRemoved;
			return true;
		}
		return false;
	}

	/**
	 * 
	**/
	V remove(int index) override {
		BinarySearchTree<V> *prev = NULL, *toBeRemoved;
		getTree(index, &prev, &toBeRemoved, true);
		V value = toBeRemoved->value;
		bool leftOfPrev = prev != NULL ? prev->left == toBeRemoved : false;
		if (prev != NULL && toBeRemoved->left == NULL && toBeRemoved->right == NULL) {
			if (leftOfPrev)
				prev->left = NULL;
			else
				prev->right = NULL;
		} else {
			if (toBeRemoved->right == NULL && prev != NULL) {
				if (leftOfPrev)
					prev->left = toBeRemoved->left;
				else
					prev->right = toBeRemoved->left;
			} else if (prev != NULL) {
				BinarySearchTree<V> *prevLeftMost = toBeRemoved, *leftMost;
				// Get the left most node of the node to the right.
				// Doing it this way decreases our time complexity from O(2log(n))-O(n) to O(log(n))-O(n). Which is minimal.
				toBeRemoved->right->getTree(0, &prevLeftMost, &leftMost, true);
				if (prevLeftMost == toBeRemoved)
					prevLeftMost->right = NULL;
				else
					prevLeftMost->left = NULL;	
				if (leftOfPrev)
					prev->left = leftMost;
				else
					prev->right = leftMost;
				//}
			} else { // prev == NULL which means we are at the root of our tree.
				// Current Implementation will correct the tree so that if we remove the root we change the data of the root and delete what ever BinarySearchTree we took values from.
				if (toBeRemoved->right == NULL && this->left != NULL) {
					BinarySearchTree<V> *left = this->left;
					this->right = this->left->right;
					this->left = this->left->left;
					this->value = left->value;
					toBeRemoved = left;
				} else if (toBeRemoved->right != NULL) {
					BinarySearchTree<V> *prevLeftMost = toBeRemoved, *leftMost;
					toBeRemoved->right->getTree(0, &prevLeftMost, &leftMost, true);
					if (prevLeftMost == toBeRemoved)
						prevLeftMost->right = NULL;
					else
						prevLeftMost->left = NULL;
					this->value = leftMost->value;
					toBeRemoved = leftMost;
				}
			}
		}
		delete toBeRemoved;
		return value;
	}

	/**
	 * This function will get values from the Tree in sorted order as if it was a SortedArrayList but with O(log(n))-O(n) time.
	**/
	V &get(int index) override {
		if (index < 0 || index > count)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(count));
		if (left == NULL && index == 0 || left != NULL && left->count + 1 == index)
			return this->value;
		else if (left != NULL && left->count + 1 > index)
			return left->get(index);
		else
			return this->right->get(left != NULL ? (index - left->count) - 2 : index - 1);
	}

	void destroyTree() override {
		NodeList<BinarySearchTree<V> *> list = NodeList<BinarySearchTree<V> *>(this);
		while (list.size() > 0) {
			BinarySearchTree<V> *first = list.get(0);
			if (first->left != NULL)
				list.append(first->left);
			if (first->right != NULL)
				list.append(first->right);
			delete first;
			list.remove(0);
		}
	}

	NodeList<V> &breadthFirst(NodeList<V> *list) override {
		list->append(this->value);
		if (this->left != NULL)
			this->left->breadthFirst(list);
		if (this->right != NULL)
			this->right->breadthFirst(list);
		return *list;
	}

	void getTree(int index, BinarySearchTree<V> **prev, BinarySearchTree<V> **current, bool remove = false) {
		if (index < 0 || index > count)
			throw std::out_of_range("Index: " + std::to_string(index) + " Size: " + std::to_string(count));
		if (remove)
			this->count--;
		if (left == NULL && index == 0 || left != NULL && left->count + 1 == index)
			*current = this;
		else if (left != NULL && left->count + 1 > index) {
			*prev = this;
			return left->getTree(index, prev, current, remove);
		} else {
			*prev = this;
			return this->right->getTree((left != NULL ? (index - left->count) - 2 : index - 1), prev, current, remove);
		}
	}

	// Searches for a specific value within the BinarySearchTree.
	bool search(V val, BinarySearchTree<V> **prev, BinarySearchTree<V> **current, int *index = NULL, bool remove = false) {
		int comparison = compare(this->value, val);
		if (comparison == 0) {
			*current = this;
			return true;
		} else if (comparison < 0) {
			if (this->left != NULL) {
				*prev = this;
				if (index != NULL)
					*index -= this->left->right != NULL ? this->left->right->count : 0;
				bool found = this->left->search(val, prev, current);
				if (found && remove)
					this->count--;
				return found;
			} else
				return false;
		} else {
			if (this->right != NULL) {
				*prev = this;
				if (index != NULL)
					*index += this->right->left != NULL ? this->right->left->count : 0;
				bool found = this->right->search(val, prev, current);
				if (found && remove)
					this->count--;
				return found;
			} else
				return false;
		}
	}

	void print(std::ostream &output, unsigned int order, unsigned int indent) override {
		if (order == BinarySearchTree::PRE_ORDER)
			output << std::string(size_t(indent) * 2, ' ') << ": " << value << std::endl;
		if (this->left != NULL)
			this->left->print(output, order, indent + 1);
		if (order == BinarySearchTree::IN_ORDER)
			output << std::string(size_t(indent) * 2, ' ') << ": " << value << std::endl;
		if (this->right != NULL)
			this->right->print(output, order, indent + 1);
		if (order == BinarySearchTree::POST_ORDER)
			output << std::string(size_t(indent) * 2, ' ') << ": " << value << std::endl;
	}

	inline int size() override { return count; }
};
