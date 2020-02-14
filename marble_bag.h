#pragma once
#include <iostream>

/// Practically an ArrayList...
template<typename Marble>
class MarbleBag {

public:
	MarbleBag() { }
	MarbleBag(unsigned int size) {
		this->size = size;
		this->marbles = new Marble[size];
	}
	~MarbleBag() {
		delete[] marbles;
	}

	void drop(Marble marble) {
		Marble *marbleBackups = this->marbles;
		bool resize = filled == size;
		if (resize) {
			size == 0 ? size = 1 : size *= 2;
			this->marbles = new Marble[size];
			for (unsigned int i = 0; i < filled; i++) {
				this->marbles[i] = marbleBackups[i];
			}
		}
		this->marbles[filled] = marble;
		if (resize)
			delete[] marbleBackups;
		filled++;
	}

	Marble grab() {
		if (size == 0 || filled == 0)
			throw "No Marbles in Bag.";
		unsigned int i = i = rand() % filled;
		Marble marble = this->marbles[i];
		for (unsigned int k = i+1; k < filled; k++) {
			this->marbles[k - 1] = this->marbles[k];
		}
		filled--;
		return marble;
	}

private:
	unsigned int filled = 0U, size = 0U;
	Marble *marbles = new Marble[0U];

};
