#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4096

struct hm_entry {
	void *key;
	void *value;
};

struct hashmap {
	struct hm_entry *entries;
	size_t capacity;
	size_t size;
};

unsigned long hash(unsigned char *str);
void init_hm(struct hashmap **hm, size_t capacity, int typesize, int arraysize);
void add_entry(struct hashmap *hm, void *key, void *value);
struct hm_entry *get_entry(struct hashmap *hm, void *key);
void print_hm(struct hashmap *hm);
void del_entry(struct hashmap *hm, void *key);

int main()
{
	struct hashmap *test;
	char key[SIZE] = "9ijes";
	char value[SIZE] = "2ji3o";

	// hm mani
	init_hm(&test, 12, sizeof(char), 4096);
	add_entry(test, "test", "sendit");
	add_entry(test, "asdf", "sendit");
	add_entry(test, "iwoef", "sendit");
	add_entry(test, "asdfasfe", "sendit");
	add_entry(test, key, value);

	print_hm(test);
	return 0;
}

void print_hm(struct hashmap *hm)
{
	struct hm_entry entry;
	for (int i = 0; i < hm->capacity; i++) {
		entry = hm->entries[i];
		printf("Index: %d; Key: %s; Value: %s\n", i, entry.key,
								entry.value);
	}
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
	printf("Index: %d\n", index);

	if (hm->size == hm->capacity) {
		puts("Hashmable is full");
		return;
	}

	// look for index not already in use and check to make sure key is not
	// already in use
	// convert void * to char * so that it can be indexed with [] operator
	struct hm_entry entry = hm->entries[index];
	while (((char *)entry.key)[0] != '\0') {

		if (!strcmp(entry.key, key)) {
			printf("Key (%s, %s) already in use\n", key, entry.key);
			return;
		}

		entry = hm->entries[index++];
	}

	// copy the data in so that they all point to different memory locations
	strncpy(entry.key, key, SIZE - 1);
	strncpy(entry.value, value, SIZE - 1);
}

void init_hm(struct hashmap **hm, size_t capacity, int typesize, int arraysize)
{
	*hm = malloc(sizeof(struct hashmap));

	// need everything to be set to 0 for adding entries
	(**hm).entries = calloc(capacity, sizeof(struct hm_entry));
	// give char * vars in entry some memory to memcpy to later
	for (int i = 0; i < capacity; i++) {
		(**hm).entries[i].key = calloc(arraysize, typesize);
		(**hm).entries[i].value = calloc(arraysize, typesize);
	}

	(**hm).capacity = capacity;
	(**hm).size = 0;
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
