#include "trie/trie.h"
#include "assert.h"
#include "stdlib.h"

void trie_init(struct trie *trie) {
	trie->is_end = false;
	trie->children_count = 0;
	trie->children_size = 2;
	trie->children = (struct trie *) malloc(trie->children_size * sizeof(struct trie));
}

struct trie *make_trie() {
	struct trie *trie = (struct trie *) malloc(sizeof(struct trie));
	trie_init(trie);
	return trie;
}

void trie_children_free(struct trie *trie) {
	size_t i;
	for(i = 0; i < trie->children_count; ++i) {
		trie_children_free(trie->children + i);
	}
	free(trie->children);
}

void trie_free(struct trie *trie) {
	trie_children_free(trie);
	free(trie);
}

void trie_set(struct trie *trie, const char *key, trie_value value) {
	assert(trie != NULL);

	//set value if key is empty
	if(key[0] == 0) {
		trie->value = value;
		trie->is_end = true;
		return;
	}

	//check if child with symbol exists
	unsigned char i;
	for(i = 0; i < trie->children_count; ++i) {
		if(trie->children[i].symbol == *key) {
			trie_set(trie->children + i, key + 1, value);
			return;
		}
	}

	//create new child
	trie->children_count += 1;

	//allocate new space if necessary
	if(trie->children_count > trie->children_size) {
		trie->children_size *= 2;
		trie->children = (struct trie *) realloc(trie->children,
				trie->children_size * sizeof(struct trie));
	}
	struct trie *new_trie = trie->children + trie->children_count - 1;
	trie_init(new_trie);
	new_trie->symbol = *key;
	trie_set(new_trie, key + 1, value);
}

trie_value trie_search(const struct trie *trie, const char *key) {
	assert(trie != NULL);

	//check if this node is the destination
	if(*key == 0) {
		return trie->is_end ? trie->value : -1;
	}

	//check children
	unsigned char i;
	for(i = 0; i < trie->children_count; ++i) {
		if(trie->children[i].symbol == *key) {
			return trie_search(trie->children + i, key + 1);
		}
	}

	return -1;
}

void trie_delete(struct trie *trie, const char *key) {
	if(*key == 0) {
		trie->is_end = false;
		trie->value = 0;
		return;
	}

	//find correct child
	unsigned char i;
	for(i = 0; i < trie->children_count; ++i) {
		if(trie->children[i].symbol == *key) {
			break;
		}
	}

	trie_delete(trie->children + i, key + 1);

	//check if child exists
	if(trie->children[i].children_count != 0) {
		return;
	}

	//decrement children count
	trie->children_count -= 1;

	//shift rest of array
	for(; i < trie->children_count; ++i) {
		trie->children[i] = trie->children[i + 1];
	}

	//free unnecessary memory
	if(2 * trie->children_count <= trie->children_size) {
		trie->children_size /= 2;
		trie->children = (struct trie *) realloc(trie->children,
				trie->children_size * sizeof(struct trie));
	}
}
