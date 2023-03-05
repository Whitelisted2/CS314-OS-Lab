
In this homework, you will use a simple program, which is known as
paging-linear-translate.py, to see if you understand how simple
virtual-to-physical address translation works with linear page tables. To run
the program, remember to either type just the name of the program
(./paging-linear-translate.py) or possibly this (python
paging-linear-translate.py). When you run it with the -h (help) flag, you 
see:

Usage: paging-linear-translate.py [options]
```
Options:
-h, --help              show this help message and exit
-s SEED, --seed=SEED    the random seed
-a ASIZE, --asize=ASIZE 
                        address space size (e.g., 16, 64k, ...)
-p PSIZE, --physmem=PSIZE
                        physical memory size (e.g., 16, 64k, ...)
-P PAGESIZE, --pagesize=PAGESIZE
                        page size (e.g., 4k, 8k, ...)
-n NUM, --addresses=NUM number of virtual addresses to generate
-u USED, --used=USED    percent of address space that is used
-v                      verbose mode
```
First, run the program without any arguments:
```
ARG seed 0
ARG address space size 16k
ARG phys mem size 64k
ARG page size 4k
ARG verbose False
```
The format of the page table is simple:
The high-order (left-most) bit is the VALID bit.
  If the bit is 1, the rest of the entry is the PFN.
  If the bit is 0, the page is not valid.
Use verbose mode (-v) if you want to print the VPN # by
each entry of the page table.
```
Page Table (from entry 0 down to the max size)
   0x8000000c
   0x00000000
   0x00000000
   0x80000006
```

As you can see, what the program provides for you is a page table for a
particular process (remember, in a real system with linear page tables, there
is one page table per process; here we just focus on one process, its address
space, and thus a single page table). The page table tells you, for each
virtual page number (VPN) of the address space, that the virtual page is
mapped to a particular physical frame number (PFN) and thus valid, or not
valid.

The format of the page-table entry is simple: the left-most (high-order) bit
is the valid bit; the remaining bits, if valid is 1, is the PFN. 

In the example above, the page table maps VPN 0 to PFN 0xc (decimal 12), VPN 3
to PFN 0x6 (decimal 6), and leaves the other two virtual pages, 1 and 2, as
not valid. 

Because the page table is a linear array, what is printed above is a replica
of what you would see in memory if you looked at the bits yourself. However,
it is sometimes easier to use this simulator if you run with the verbose flag
(-v); this flag also prints out the VPN (index) into the page table. From the
example above, run with the -v flag:
```
Page Table (from entry 0 down to the max size)
  [       0]   0x8000000c
  [       1]   0x00000000
  [       2]   0x00000000
  [       3]   0x80000006
```
Your job, then, is to use this page table to translate the virtual addresses
given to you in the trace to physical addresses. 


Of course, you can change many of these parameters to make more interesting
problems. Run the program with the -h flag to see what options there are:

- The -s flag changes the random seed and thus generates different
  page table values as well as different virtual addresses to translate.
- The -a flag changes the size of the address space.
- The -p flag changes the size of physical memory.
- The -P flag changes the size of a page.
- The -n flag can be used to generate more addresses to translate
  (instead of the default 5).
- The -u flag changes the fraction of mappings that are valid, from
  0% (-u 0) up to 100% (-u 100). The default is 50, which means
  that roughly 1/2 of the pages in the virtual address space will be valid.
- The -v flag prints out the VPN numbers to make your life easier.



