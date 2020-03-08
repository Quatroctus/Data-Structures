#pragma once
#include "hash_primitives.h"
#include "optional.h"
#include <string>
#include <vector>

template <typename V>
struct Set {
	virtual ~Set() {}

	virtual Set<V> &add(V val) = 0;

	virtual bool remove(V val) = 0;

	virtual bool contains(V key) = 0;

	virtual Optional<V> query(V key) = 0;

	virtual void getEntries(std::vector<V> *entries) = 0;

protected:
	struct Entry {
		bool isValue;
		union Something {
			V value;
			Set<V> *set;
			Something() : value(V()) {}
			~Something() {
				value.~V();
			}
		} entry;
		Entry() : isValue(true) {}
	};
};

template <typename V>
class HashSet : public Set<V> {
	size_t length, count, depth;
	float loadFactor;
	const float rehashFactor = 0.65F;
	Optional<Set<V>::template Entry> *values;

	size_t (*hash)(V val);

	size_t hashEntry(V value) {
		size_t depthMultiplier = 13 * depth;
		for (size_t i = 0; i < depth; i++)
			depthMultiplier *= 27;
		if (hash != NULL)
			return (hash(value) * depthMultiplier) % length;
		else if (primitive<V>::exists())
			return (primitive<V>::hash(value) * depthMultiplier) % length;
		else if (std::string *val = dynamic_cast<std::string *>(&value)) {
			size_t hash = 7;
			for (size_t i = 0; i < val->size(); i++)
				hash = (hash * 17) + ((*val)[i] * i + 1);
			return (hash * depthMultiplier) % length;
		}
		throw "Unable to hash value.";
	}

	void rehash() {
		std::vector<V> entries;
		getEntries(&entries);
		length *= 2;
		count = 0;
		loadFactor = 0.0F;
		values = new Optional<Set<V>::template Entry>[length];
		for (V value : entries) {
			this->add(value);
		}
	}

public:
	HashSet() : length(2), count(0), depth(0), loadFactor(0.0F), hash(nullptr) {
		values = new Optional<Set<V>::template Entry>[length];
	}
	HashSet(size_t length, size_t depth, size_t (*hash)(V key)) : length(length), count(0), depth(depth), loadFactor(0.0F), hash(hash) {
		values = new Optional<Set<V>::template Entry>[length];
	}
	HashSet(size_t (*hash)(V key)) : length(2), count(0), depth(0), loadFactor(0.0F), hash(hash) {
		values = new Optional<Set<V>::template Entry>[length];
	}
	HashSet(size_t length, size_t (*hash)(V key) = nullptr) : length(length), count(0), depth(0), loadFactor(0.0F), hash(hash) {
		values = new Optional<Set<V>::template Entry>[length];
	}
	~HashSet() override {
		if (values != NULL)
			getEntries(NULL);
	}

	Set<V> &add(V val) {
		size_t index = hashEntry(val);
		Optional<Set<V>::template Entry> &entry = values[index];
		Set<V>::template Entry &value = entry.value;
		if (!entry.exists) {
			entry.exists = true;
			value.entry.value = val;
		} else {
			if (value.isValue) {
				V v = value.entry.value;
				value.isValue = false;
				value.entry.set = new HashSet<V>(2, depth + 1, hash);
				value.entry.set->add(v);
				value.entry.set->add(val);
			} else {
				value.entry.set->add(val);
			}
		}
		return *this;
	}

	bool remove(V val) {
		size_t index = hashEntry(val);
		Optional<Set<V>::template Entry> &entry = values[index];
		if (entry.exists) {
			Set<V>::template Entry &value = entry.value;
			if (value.isValue) {
				entry.exists = false;
				return true;
			} else
				return value.entry.set->remove(val);
		}
		return false;
	}

	bool contains(V val) {
		size_t index = hashEntry(val);
		Optional<Set<V>::template Entry> &entry = values[index];
		Set<V>::template Entry &value = entry.value;
		return entry.exists && (value.isValue && value.entry.value == val || !value.isValue && value.entry.set->contains(val));
	}

	Optional<V> query(V val) {
		size_t index = hashEntry(val);
		Optional<Set<V>::template Entry> &entry = values[index];
		if (!entry.exists)
			return Optional<V>();
		Set<V>::template Entry &value = entry.value;
		if (value.isValue && value.entry.value == val)
			return Optional<V>(value.entry.value);
		else if (!value.isValue)
			return value.entry.set->query(val);
		return Optional<V>();
	}

	void getEntries(std::vector<V> *entries) {
		for (size_t i = 0; i < length; i++) {
			Optional<Set<V>::template Entry> &entry = values[i];
			if (entry.exists) {
				Set<V>::template Entry &value = entry.value;
				if (value.isValue && entries != NULL)
					entries->push_back(value.entry.value);
				else {
					value.entry.set->getEntries(entries);
					delete value.entry.set;
				}
			}
		}
		delete[] values;
		values = NULL;
	}
};
