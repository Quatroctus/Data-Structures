#pragma
#include "nodelist.h"
#include <iostream>

// TODO: Restructure this whole file. Move the BinarySearchTree into it's own file.
// TODO: Restructure the class dynamics/methods to include different traversal orders.
// TODO: Potentially use a BiNode and container structure to make the BinarySearchTree.
template <typename T>
struct Tree : public List<T> {
	virtual ~Tree() {}

	virtual bool removeValue(T val) = 0;

	virtual void destroyTree() = 0;

	virtual void print(std::ostream &output, unsigned int indent) = 0;

	friend std::ostream &operator<<(std::ostream &output, Tree<T> &tree) {
		tree.print(output, 0);
		return output;
	}
};

template <typename T>
class BinarySearchTree : public Tree<T> {

	T value;
	BinarySearchTree<T> *left = NULL, *right = NULL;
	int count = 0;
	int (*compare)(T inTree, T value) = defaultCompare;

	void getTree(int index, BinarySearchTree<T> **prev, BinarySearchTree<T> **current, bool remove = false) {
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
	bool searchForTree(T val, BinarySearchTree<T> **prev, BinarySearchTree<T> **current, int *index = NULL, bool remove = false) {
		int comparison = compare(this->value, val);
		if (comparison == 0) {
			*current = this;
			return true;
		} else if (comparison < 0) {
			if (this->left != NULL) {
				*prev = this;
				if (index != NULL)
					*index -= this->left->right != NULL ? this->left->right->count : 0;
				bool found = this->left->searchForTree(val, prev, current);
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
				bool found = this->right->searchForTree(val, prev, current);
				if (found && remove)
					this->count--;
				return found;
			} else
				return false;
		}
	}

public:
	BinarySearchTree(T val) : value(val) {}
	BinarySearchTree(int (*compare)(T inTree, T value)) : compare(compare) {}
	BinarySearchTree(T val, int (*compare)(T inTree, T value)) : value(val), compare(compare) {}

	bool append(T val) override {
		int comparison = compare(value, val);
		bool added = false;
		if (comparison < 0) {
			if (left == NULL) {
				BinarySearchTree<T> *tree = new BinarySearchTree(val, compare);
				this->left = tree;
				added = true;
			} else
				added = left->append(val);
		} else if (comparison == 0) {
			BinarySearchTree<T> *tree = new BinarySearchTree(val, compare);
			tree->left = this->left;
			this->left = tree;
			added = true;
		} else {
			if (right == NULL) {
				BinarySearchTree<T> *tree = new BinarySearchTree(val, compare);
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

	bool removeValue(T val) override {
		int index;
		BinarySearchTree<T> *prev = NULL, *toBeRemoved;
		if (searchForTree(val, &prev, &toBeRemoved, &index, true)) {
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
					BinarySearchTree<T> *prevLeftMost = toBeRemoved, *leftMost;
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
						BinarySearchTree<T> *left = this->left;
						this->right = this->left->right;
						this->left = this->left->left;
						this->value = left->value;
						toBeRemoved = left;
					} else if (toBeRemoved->right != NULL) {
						BinarySearchTree<T> *prevLeftMost = toBeRemoved, *leftMost;
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
	T remove(int index) override {
		BinarySearchTree<T> *prev = NULL, *toBeRemoved;
		getTree(index, &prev, &toBeRemoved, true);
		T value = toBeRemoved->value;
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
				BinarySearchTree<T> *prevLeftMost = toBeRemoved, *leftMost;
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
					BinarySearchTree<T> *left = this->left;
					this->right = this->left->right;
					this->left = this->left->left;
					this->value = left->value;
					toBeRemoved = left;
				} else if (toBeRemoved->right != NULL) {
					BinarySearchTree<T> *prevLeftMost = toBeRemoved, *leftMost;
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
	T &get(int index) override {
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
		NodeList<BinarySearchTree<T> *> list = NodeList<BinarySearchTree<T> *>(this);
		while (list.size() > 0) {
			BinarySearchTree<T> *first = list.get(0);
			if (first->left != NULL)
				list.append(first->left);
			if (first->right != NULL)
				list.append(first->right);
			delete first;
			list.remove(0);
		}
	}

	void print(std::ostream &output, unsigned int indent) override {
		if (this->left != NULL)
			this->left->print(output, indent + 1);
		output << std::string(size_t(indent) * 2, ' ') << ": " << value << std::endl;
		if (this->right != NULL)
			this->right->print(output, indent + 1);
	}

	inline int size() override { return count; }
};
