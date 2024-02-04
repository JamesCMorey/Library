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
	int arraysize;
	int typesize;
};

struct hm_info {
	int passbyvalue;
	int arraysize;
	int typesize;
	int capacity;
};

unsigned long hash(unsigned char *str);
void init_hm(struct hashmap **hm, struct hm_info hints);
void add_entry(struct hashmap *hm, void *key, void *value);
struct hm_entry *get_entry(struct hashmap *hm, void *key);
void print_hm(struct hashmap *hm);
void del_entry(struct hashmap *hm, void *key);

int main()
{
	struct hashmap *test;
	char key[SIZE] = "iojweif";
	char value[SIZE] = "ajfoewijf";

	struct hm_info hints;
	hints.passbyvalue = 1;
	hints.arraysize = 4096;
	hints.capacity = 12;
	hints.typesize = sizeof(char);

	init_hm(&test, hints);
	/*
	int a = 4, b = 5;
	add_entry(test, &a, &b);
	a = 6, b = 1;
	add_entry(test, &a, &b);
	*/

	add_entry(test, key, value);
	add_entry(test, "test", "sendit");
	add_entry(test, "test", "sendit");
	add_entry(test, "asdf", "sendit");
	add_entry(test, "asdf", "sendit");
	add_entry(test, "iwoef", "sendit");
	add_entry(test, "asdfasfe", "sendit");

	print_hm(test);
	return 0;
}

struct hm_entry *get_entry(struct hashmap *hm, void *key)
{
	int index = hash(key) % hm->capacity;
	struct hm_entry entry;

	while ((entry = hm->entries[index]).key != key)
		index++;

	return &hm->entries[index];
}

void add_entry(struct hashmap *hm, void *key, void *value)
{
	int index = hash(key) % hm->capacity;
	int size = hm->typesize * hm->arraysize;
	printf("Index: %d; Key: %d\n", index, *(int *)key);

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

		printf("Entry->key: %s\n", entry->key);
		if (!memcmp(entry->key, key, size -1)) {
			printf("Key (%s, %s) already in use\n", key, entry->key);
			return;
		}

		if(++index >= hm->capacity) {
			index = 0;
		}

		entry = &hm->entries[index];
	}

	// copy the data in so that they all point to different memory locations
	entry->defined = 1;
	memcpy(entry->key, key, size);
	memcpy(entry->value, value, size);

	hm->entry_count++;
}

void init_hm(struct hashmap **hm, struct hm_info hints)
{
	int arraysize = hints.arraysize;
	int typesize = hints.typesize;
	int capacity = hints.capacity;
	int passbyvalue = hints.passbyvalue;

	// need everything to be set to 0 for adding entries
	*hm = malloc(sizeof(struct hashmap));
	(**hm).entries = calloc(hints.capacity, sizeof(struct hm_entry));

	for (int i = 0; i < capacity; i++) {
		(**hm).entries[i].key = calloc(arraysize, typesize);
		(**hm).entries[i].value = calloc(arraysize, typesize);
	}

	(**hm).arraysize = arraysize;
	(**hm).typesize = typesize;
	(**hm).passbyvalue = passbyvalue;

	(**hm).capacity = capacity;
	(**hm).entry_count = 0;
}

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
