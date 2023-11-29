# Risc-V Hypervisor

The following document describes some of the techniques used in this project.
These are very common in standard operating systems.


## Boot process

### 0. Begining
qemu-system-riscv64 places RAM at 0x80000000 (check out dtb) which is our
entry point. Harts are currently in Machine mode, with registers a0
(hartid) and a1 (dtb load address) set to specific values.

### 1. boot.S / OpenSBI

Firmware (operating in M-mode) starts setting up the machine. Here are
some things we try to accomplish at this stage:

- install exception and interrupt handlers
- setup physical memory protection
- delegate appropriate exceptions and interrupts
- initialize supervisor registers
- transfer control to S-mode

#### Transfering control to S-mode

```c
mstatus.MPP = S-mode;	// machine previous privilege
mstatus.MPV = 0;	// machine previous virtualization
mepc = address		// where to jump
mret			// perform the transition
```

### 2. sboot.S

This is the first code operating in S-mode. It accomplishes following tasks:
- entering virtual address space (VAS)
- setup stack for C

#### Entering virtual address space (setup)

The whole idea is to map kernel image to some arbitrary virtual address. In order
to achieve this, we have to create our first page table.

Here comes a big issue. Kernel image can quickly occupy multiple pages,
demanding some kind of dynamic memory management. But we do not want to implement
it in assembly...

- Linux first setups everything necessary for C code. Then it uses a seperate
Boot Memory Allocator (just a normal physical allocator). The only issue with
this allocator is that it does work both in virtual address space and actual
physical memory (if VA == PA, then it works). After the boot process is
finished this allocator is retired.

- Luckily for us, risc-v (sv39) supports giga-pages. We can completely
avoid dynamic memory management by only allocating 1 page for root page
table and mapping absurdly huge pages in there.

I decided to create
- identity mapping (meaning VA == PA) for the whole RAM
- linear mapping of kernel image to arbitrary address (VAS_BASE)
- map serial for early printing

#### Actually entering virtual address space

- Valid page table is created (with identity mapping and arbitrary address mapping)
- find address of entry point in virtual address space
- load entry point to some register
- write to satp
- jump to entry point

#### initialization for C

Now that we are in virtual address space we can set up the stack pointer to
our stack (not pointing directly to physical memory). Stack must be 16-bit
aligned, grows towards smaller addresses and the stack pointer points to
last used address. Optionally we can also set up global pointer.


## Start virtualization (enter virtual machine)

### hgatp (GPA -> PA)

1. Page tables are in Sv39x4 format. It is actually just Sv39 with 2 extra bits.
In order to account for that change we enlarge the root page table (only) by
a factor of four (to PAGE_SIZE * 4). Also the alignment is more strict (aligned
to PAGE_SIZE * 4).

2. PTEs must have User bit set

hstatus
- set correct VSXLEN
- set SPV bit

sstatus
- set SPP bit

sepc
- entry point of virtual machine

Now just execute sret


# Glossary

DTB - device tree binary
MMIO - memory mapped input/output
VAS - virtual address space
VA - virtual address
PA - physical address
GPA - guest physical address
identity mapping - va == pa
linear mapping - va == pa + constant

# Extras

## Device Tree Binary (dtb)

This is a binary describing the physical layout of the machine. It
provides information on the number of CPUs, size of RAM, MMIO
addresses... I parsed by hand into the file
[src/include/dtb.h](src/include/dtb.h).
