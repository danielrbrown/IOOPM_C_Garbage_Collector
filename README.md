Here I have copied, as-is, a group project from the course Imperative and Object Oriented Programming Methodologies at Uppsala University. The aim of the project was primarily to develop collaborative programming practices and version control, secondly to produce a conservative, compacting garbage collector after Bartlett, in and for C. While time did not suffice to iron out all the bugs necessary to meet this secondary goal, it showcases to some extent where my abilities stood in regards to C programming in the winter of 2022/2023.

Areas of the code which I made significant contributions to include the following modules:

[Bitvector](src/bitvector)

[Garbage Collector](src/garbage_collector)

[Traverse and Copy](src/traverse_and_copy)

The project report and documentation (beyond the README below) can be found at [Project Report](proj), and includes 

Below is the orginal README file.

# Undefined Garbage Collector
Do you also hate having to manage memory,\
but desperately need to put C programming knowledge on your resume?\
Well do we have the solution for you!\
Introducing the auto-garbage-collecting memory-managing library, written in C, for C.\
Implemented using Bartletts algorithm with virtual memory blocks and bump pointers\
for conservative compacting of allocated memory space.\
Do you need to understand all these concepts to use this garbage collector?\
Nope, we covered that part for you, so lets get started!

## Installation
Great! So how do I get it up and running?

It's a library so it needs to be included into your project.

first go to your project directory;
```bash
cd your/project/directory
```
then download the garbage collector;
```bash
curl -L https://github.com/IOOPM-UU/undefined/archive/main.zip
```
and unzip it;
```bash
unzip main.zip
```
Then include it into your code with
```c
#include "Undefined-main/src/gc.h"
```
## Interface functions
No library is complete without a set of sweet functions

```c
/// Create a new heap with bytes total size (including both spaces
/// and metadata), meaning strictly less than bytes will be
/// available for allocation.
///
/// \param bytes the total size of the heap in bytes
/// \param unsafe_stack true if pointers on the stack are to be considered unsafe pointers
/// \param gc_threshold the memory pressure at which gc should be triggered (1.0 = full memory)
/// \return the new heap
heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold);
```
```c
/// Delete a heap.
///
/// \param h the heap
void h_delete(heap_t *h);
```
```c
/// Delete a heap and trace, killing off stack pointers.
///
/// \param h the heap 
/// \param dbg_value a value to be written into every pointer into h on the stack
void h_delete_dbg(heap_t *h, void *dbg_value);
```
```c
/// Allocate a new object on a heap with a given format string.
///
/// Valid characters in format strings are:
/// - 'i' -- for sizeof(int) bytes 'raw' data
/// - 'l' -- for sizeof(long) bytes 'raw' data
/// - 'f' -- for sizeof(float) bytes 'raw' data
/// - 'c' -- for sizeof(char) bytes 'raw' data
/// - 'd' -- for sizeof(double) bytes 'raw' data
/// - '*' -- for a sizeof(void *) bytes pointer value
/// - '\0' -- null-character terminates the format string
///
/// \param h the heap
/// \param layout the format string
/// \return the newly allocated object
///
/// Note: the heap does *not* retain an alias to layout.
void *h_alloc_struct(heap_t *h, char *layout);
```
```c
/// Allocate a new object on a heap with a given size.
///
/// Objects allocated with this function will *not* be 
/// further traced for pointers. 
///
/// \param h the heap
/// \param bytes the size in bytes
/// \return the newly allocated object
void *h_alloc_data(heap_t *h, size_t bytes);
```
```c
/// Manually trigger garbage collection.
///
/// Garbage collection is otherwise run when an allocation is
/// impossible in the available consecutive free memory.
///
/// \param h the heap
/// \return the number of bytes collected
size_t h_gc(heap_t *h);
```
```c
/// Manually trigger garbage collection with the ability to 
/// override the setting for how stack pointers are treated. 
/// 
/// Garbage collection is otherwise run when an allocation is
/// impossible in the available consecutive free memory.
///
/// \param h the heap
/// \param unsafe_stack true if pointers on the stack are to be considered unsafe pointers
/// \return the number of bytes collected
size_t h_gc_dbg(heap_t *h, bool unsafe_stack);
```
```c
/// Returns the available free memory. 
///
/// \param h the heap
/// \return the available free memory. 
size_t h_avail(heap_t *h);
```
```c
/// Returns the bytes currently in use by user structures. This
/// should not include the collector's own meta data. Notably,
/// this means that h_avail + h_used will not equal the size of
/// the heap passed to h_init.
/// 
/// \param h the heap
/// \return the bytes currently in use by user structures. 
size_t h_used(heap_t *h);
```


## Authors
- Simon Eriksson, GitHub: [@sier8161](https://www.github.com/sier8161)
Email: simon.eriksson.8161@student.uu.se
- Daniel Brown, GitHub: [@danielrbrown](https://www.github.com/danielrbrown)
Email: daniel.brown.4990@student.uu.se
- Oliver Hansson, GitHub: [@OliverHansson](https://www.github.com/OliverHansson)
Email: Oliver.hansson.0007@student.uu.se
- Erik Braathen, GitHub: [@Erikbraa](https://www.github.com/Erikbraa)
Email: erik.braathen.9122@student.uu.se
- Ville Friberg Elings, GitHub: [@vfe01](https://www.github.com/vfe01)
Email: ville.fribergelings.2166@student.uu.se
- Fredrik Jäderblom, GitHub: [@jaderflow](https://www.github.com/jaderflow)
Email: fredrik.jaderblom.0183@student.uu.se

## Running Tests

To run all tests, run the following command

```bash
make test
```
To run all tests with memory checking through `valgrind`, run the following command

```bash
make memtest
```

## Known Bugs
- Sometimes there can be false positives in how the allocation map keeps track\
of pointers, resulting in garbage data being saved and copied over during\
garbage collection. This is extremly rare though.
- See [deviations.md](proj/deviations.md)

## Project Done

The final commit marking the completion of this project was: e9cb423
## Project Complement

To run the failing test, type:
```
make gc_crash_test
```
