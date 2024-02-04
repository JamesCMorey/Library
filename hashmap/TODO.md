**Hashmap**

1. Add del_entry function
2. Add cleanup functions and check with valgrind to remove memory leaks
3. Clean up the process of initializing a hashmap

	Either clean up or completely remove hm_info. Try and make it so that
	there aren't variables for both arraysize and typesize. Maybe switch out
	typesize with overall size and get typesize through a size/arraysize
	operation.

4. Clean up the file so that it is easier to include as a library file in other
   projects
