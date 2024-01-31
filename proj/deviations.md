# Deviations
In this file, describe all deviations from the full specification. 
For every feature X that you do not deliver, explain why you do not deliver it, 
how the feature could be integrated in the future in your system, and sketch the high-level design.

## No chars in structs with pointers

Calculating the size of a struct is difficult because of structure padding. There can in some cases be empty space between fields in a struct which is predictable for a given system. It's relatively easy and memory efficient to account for padding when each field is of a datatype of size 4 or 8 bytes, but when there's something that's not that size it becomes a problem. Characters, which are of size 1, are therefore not officialy supported as fields in structs. If there was no time limitation a solution might have been reasonable but since character as a field is not common there were things that are more important to spend time on.
Note that it's still possible to allocate the character as an array. 

## False positives from allocation map

Due to differing interpretations of how the modules `allocation_map` and `allocation` would operate, which did not come to light until after a fair amount of code had been written, there exists what is believed to be a critical bug in the program.

### The problem
Each bit in the allocation map corresponds to a word-aligned address, 16 bytes apart. Allocation, on the other hand, allocates structs and data at the next word-aligned address. That is, all word-aligned addresses are valid. There is therefore a mismatch where two different valid addresses can cause a particular bit on the allocation map to be set to True. As a consequence of this, should the module `find_root_set` interpret an 8-byte word on the stack as a pointer and this ”pointer” should by chance point to the second (non-allocated) address that corresponds to an otherwise positively-set bit in the allocation map, the allocation map will return a false positive.

A false positive means that we retain the potential pointer, treating it as ”unsafe” if that flag has been chosen, and protecting both the root object and the stack. But the 8 bytes preceding this address will still be interpreted as a header, and in the (probably extremely unlikely) possibility that those bits would happen to point our `traverse_and_copy` module to another - ostensibly safe - ”pointer” some bytes ahead, that pointer will be interpreted as safe and treated as such. If these 8 bytes in turn point inside our heap, then the ”object” that it points to will be copied over to a new page, and the ”safe pointer’s header” overwritten with a forwarding address. This would consist of an illegal write to memory that has a high chance of trashing the memory of the program being run.

It should be pointed out that the probability of this happening (three consecutive chance interpretations of random bits) on a single scan of the stack is probably so small as to be verging on the non-existent. Nonetheless, at least two factors lead us to the conclusion that we should take this possibility seriously.

1. Certain programs can be run many times over, which with small memory allocations could lead to GC also being triggered many times while running a program. This increases the likelihood of the event occurring.
2. Even with an infinitesimally small chance of the event occuring, *we should not release a program that we know has a non-zero chance of trashing a user’s memory*.

Additionally, if the `unsafe_stack` flag is set to interpret all stack pointers as safe, the likelihood of trashing memory is much higher, requiring only the false positive from the initial stack pointer.

### Mitigations

A number of partial safeguards have been implemented into the `traverse_and_copy` module to attempt to mitigate the negative effects of this bug.

- Safe pointers are checked in the allocation map, as well as unsafe pointers. If one of these returns false, the program is aborted.
- If the two-bit pattern at the beginning of a header is the "unused" pattern, the program is aborted.
- The program is not aborted in the two cases above if we are dealing with the root object, stack pointers are condidered unsafe, and we have not moved past the first "pointer" in the struct. In this case nothing has yet been copiedn and we can safely return to `find_root_set`.
- If a pointer leads us out of our heap, the program is aborted.

The choice was made to abort in these cases as it is then almost certain that an invalid write to memory has occured. Unfortunately, it is still possible that an invalid write is made that doesn't get picked up by a subsequent invalid pointer, and thus the possibility remains that such a write can pass unnoticed.

### Potential solutions
While we have not had time to correct this bug, there are a number of obvious routes available to us. The first is to change `allocation` so that it only allocates at 16-byte intervals, in line with the allocation map. The second would be to extend the allocation map so that each bit represents addresses at 8 byte intervals.

The fact that `allocation` can allocate at any word-aligned address is deemed by us to be a feature of the program, which allows a more efficient use of the memory space allocated to our heap. Extending the allocation map from 16 to 8 bytes would mean doubling its contribution to our GC’s overload, with each page requiring 32 bytes instead of the current 16. Nonetheless, this overhead would be compensated for with only two allocations that utilise the extra addresses available to it, and so it should be assumed that on average extending the `allocation_map` would constitute a more efficient use of memory than altering `allocation`. It should also be noted that doing so would increase the number of potential pointers that can be discarded, reducing the workload of the module `traverse_and_copy`, although this contribution is likely to be minimal.

Bitvectors are represented in our program as an array of `uint64_t` integers, so extending the allocation map would simply be a matter of extending this array. A macro has been used throughout the code to represent the size of a single bitvector, and `BITVECTOR_SIZE * 2` used to represent the size of the allocation map. This would need to be updated to `BITVECTOR_SIZE * 4` in any places in the code that uses it, and would be a good opportunity to replace these with an additional macro, `ALLOCTION_MAP_SIZE`. All tests that rely upon the size of the allocation map would also have to be checked to ensure that they use the updated value. Regression testing should be employed while making the changes, to establish a baseline and locate any other places in the code that may be affected by this change.

## Integration with assignment 2
The integration with assignment 2 is not fully working. Given a big enough heap, the program will work as expected until the heap memory-pressure hits the threshold, resulting in a crash.

### Possible Reasons
The reason for the crash might have something to do with the implementation of the `buckets` in `hash_table.c`, resulting in wrongful traversal of each entry in the hash table.

The other potential reason is that our garbage collector doesn't support `unions`, and unions are declared as `*` (pointers) resulting in traversal of said pointers, which would result in wrongful traversal and interpretation of e.g. an `int` as a pointer that doesn't point into our heap.

Given more time we could've properly found the true reason for crashing and fixing it. 
- In case of the problem being the implementaion of the `hash_table` the solution would be changing the implementation to fit the garbage collector. This would possibly mean changing `hash_table.c` internal functions for traversing its entries.
- In case of the problem being `unions`, the solution would be to implement support of `unions` in our garbage collector.

## Four linked lists
The integration with four linked lists does not work reliably. When testing with M's larger than 10000, we usually crash.

### Possible reasons
This is probably due to find_root_set finding the pointers of structs such as the page struct or heap struct. Due to limited time we have not been able to properly debug this and thus we are not sure that this is actually the issue.

### Potential solutions
Since this error isn't consistent, it is hard to debug this in a controlled manner. A possible solution would be to recreate exactly what creates this bug and then hard code a solution (assuming the error is due to us finding structs which are not correct allocs). We have safety measures in place in case we were to find a pointer which turns out to be incorrect, but this bug proves that our safety protocol is lacking. 
