#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4096

struct hm_entry {
	int defined;
	void *key;
	void *value;
};

struct hashmap {
	struct hm_entry *entries;
	size_t capacity;
	size_t entry_count;
	int passbyvalue;
	int key_arraysize;
	int key_typesize;
	int value_arraysize;
	int value_typesize;
};

struct hm_info {
	int passbyvalue;
	int key_arraysize;
	int key_typesize;
	int value_arraysize;
	int value_typesize;
	int capacity;
};

unsigned long hash(unsigned char *str);
void init_hm(struct hashmap **hm, struct hm_info hints);
void add_entry(struct hashmap *hm, void *key, void *value);
struct hm_entry *get_entry(struct hashmap *hm, void *key);
void print_hm(struct hashmap *hm);
void del_entry(struct hashmap *hm, void *key);

struct test {
	int balance;
	char *name;
};

int main()
{
	/*
	struct hashmap *test;
	char key[SIZE] = "wiefojwioe";
	int *value = malloc(sizeof(int));
	*value = 12;


	init_hm(&test, hints);
	int a = 4, b = 5;
	add_entry(test, &a, &b);
	a = 6, b = 1;
	add_entry(test, &a, &b);

	add_entry(test, key, value);
	add_entry(test, "test", "iwjoe");
	add_entry(test, "test", "woiejfw");
	add_entry(test, "asdf", "oijweifj");
	add_entry(test, "asdf", "padofij");
	add_entry(test, "iwoef", "ijfewoiejf");
	add_entry(test, "asdfasfe", "fds");
	print_hm(test);
	*/

	struct hashmap *hm;
	struct hm_info hints;
	hints.passbyvalue = 1;
	hints.key_arraysize = 10;
	hints.key_typesize = sizeof(char);
	hints.value_arraysize = 1;
	hints.value_typesize = sizeof(struct test);
	hints.capacity = 12;

	init_hm(&hm, hints);
	struct test tmp;
	tmp.name = "hello";
	tmp.balance = 4;

	add_entry(hm, "em1", &tmp);
	struct hm_entry *gotten = get_entry(hm, "em1");
	struct test *g = gotten->value;
	printf("bal: %d; name: %s\n", g->balance, g->name);

	return 0;
}

struct hm_entry *get_entry(struct hashmap *hm, void *key)
{
	int keysize;
	void *slicedkey;
	int index, checks;
	struct hm_entry *entry;

	// get the size a key is allowed to be and then copy that much from
	// the key parameter into slicedkey
	keysize = hm->key_arraysize * hm->key_typesize;
	slicedkey = calloc(hm->key_arraysize, hm->key_typesize);
	memcpy(slicedkey, key, keysize);

	index = hash(slicedkey) % hm->capacity;
	checks = 0;
	entry = &hm->entries[index];

	while (memcmp(entry->key, key, keysize) && checks <= hm->capacity) {
		checks++;
		index++;

		// wrap around to start of hashmap after reaching the end
		if (index >= hm->capacity) {
			index = 0;
		}

		entry = &hm->entries[index];
	}

	if (checks >= hm->capacity) {
		puts("failed to get");
		return NULL;
	}

	return &hm->entries[index];
}

void add_entry(struct hashmap *hm, void *key, void *value)
{
	int keysize = hm->key_typesize * hm->key_arraysize;
	int valuesize = hm->value_typesize * hm->value_arraysize;

	void *slicedkey = calloc(hm->key_arraysize, hm->key_typesize);
	memcpy(slicedkey, key, keysize);
	int index = hash(slicedkey) % hm->capacity;

	if (hm->entry_count == hm->capacity) {
		puts("Hashmable is full");
		return;
	}

	// look for index not already in use and check to make sure key is not
	// already in use
	// convert void * to char * so that it can be indexed with [] operator
	struct hm_entry *entry = &hm->entries[index];
	// need to add wrap around if goes past last index
	while (entry->defined) {

		if (!memcmp(entry->key, slicedkey, keysize)) {
			return;
		}

		if(++index >= hm->capacity) {
			index = 0;
		}

		entry = &hm->entries[index];
	}

	// copy the data in so that they all point to different memory locations
	entry->defined = 1;
	memcpy(entry->key, key, keysize);
	memcpy(entry->value, value, valuesize);

	hm->entry_count++;
	free(slicedkey);
}

void init_hm(struct hashmap **hm, struct hm_info hints)
{
	int value_arraysize = hints.value_arraysize;
	int value_typesize = hints.value_typesize;

	int key_arraysize = hints.key_arraysize;
	int key_typesize = hints.key_typesize;

	int capacity = hints.capacity;
	int passbyvalue = hints.passbyvalue;

	// need everything to be set to 0 for adding entries
	*hm = malloc(sizeof(struct hashmap));
	(**hm).entries = calloc(hints.capacity, sizeof(struct hm_entry));

	for (int i = 0; i < capacity; i++) {
		(**hm).entries[i].key = calloc(key_arraysize, key_typesize);
		(**hm).entries[i].value = calloc(value_arraysize, value_typesize);
	}

	(**hm).value_arraysize = value_arraysize;
	(**hm).value_typesize = value_typesize;
	(**hm).key_arraysize = key_arraysize;
	(**hm).key_typesize = key_typesize;
	(**hm).passbyvalue = passbyvalue;

	(**hm).capacity = capacity;
	(**hm).entry_count = 0;
}

// only works if both key and value are strings
void print_hm(struct hashmap *hm)
{
	struct hm_entry entry;
	for (int i = 0; i < hm->capacity; i++) {
		entry = hm->entries[i];
		printf("Index: %d; Defined: %d; Key: %s; Value: %s\n",
				i, entry.defined, entry.key, entry.value);

	}
}

// djb2: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}
