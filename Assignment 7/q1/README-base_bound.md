
This program allows you to see how address translations are performed in a
system with base and bounds registers. 

In this homework, we will assume a slightly different address space than our
canonical one with a heap and stack at opposite ends of the space. Rather, we
will assume that the address space has a code section, then a fixed-sized
(small) stack, and a heap that grows downward right after, looking something
like you see in the Figure below. In this configuration, there is only one
direction of growth, towards higher regions of the address space.

```
  -------------- 0KB 
  |    Code    | 
  -------------- 2KB
  |   Stack    |
  -------------- 4KB
  |    Heap    |
  |     |      |
  |     v      |
  -------------- 7KB
  |   (free)   |
  |     ...    |
```
In the figure, the bounds register would be set to 7~KB, as that represents
the end of the address space. References to any address within the bounds
would be considered legal; references above this value are out of bounds and
thus the hardware would raise an exception.

To run with the default flags, type relocation.py at the command line. 

As you can see, the homework simply generates randomized virtual
addresses. For each, you should determine whether it is in bounds, and if so,
determine to which physical address it translates
There are a few flags you can use to control what's going on better:

prompt> ./relocation.py -h \
Usage: relocation.py [options]
```
Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -a ASIZE, --asize=ASIZE address space size (e.g., 16, 64k, 32m)
  -p PSIZE, --physmem=PSIZE physical memory size (e.g., 16, 64k)
  -n NUM, --addresses=NUM # of virtual addresses to generate
  -b BASE, --b=BASE     value of base register
  -l LIMIT, --l=LIMIT   value of limit register
```
<!-- ] -->

In particular, you can control the virtual address-space size (-a), the size
of physical memory (-p), the number of virtual addresses to generate (-n), and
the values of the base and bounds registers for this process (-b and -l,
respectively).

