# Risc-V Hypervisor

The following document describes some of the techniques used in this project.
These are very common in standard operating systems.

# Installation

> NOTE: watch out for hardcoded absolute paths (qemu and gnu toolchain).
> This is very crude documentation on how to get started

1. install qemu 8.0.2 (https://download.qemu.org/qemu-8.0.2.tar.xz)
2. install and compile https://github.com/riscv/riscv-gnu-toolchain (commit 8c969a9efe68a811cf524174d25255632029f3d3)
3. git clone https://github.com/krizmanmarko/riscv_hypervisor
4. git submodule update --init --recursive
5. build src/guest/xv6-riscv-guest
  - `cd src/guest/xv6-riscv-guest
  - `make kernel/kernel fs.img`
6. build other guests (use your imagination)

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

- Linux first sets up everything necessary for C code. Then it uses a seperate
Boot Memory Allocator (just a normal physical allocator). The only issue with
this allocator is that it only works in actual physical memory. After the boot
process is finished this allocator is retired.

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

- hgatp (GPA -> PA)
  - Page tables are in Sv39x4 format. It is actually just Sv39 with 2 extra bits.
  In order to account for that change we enlarge the root page table (only) by
  a factor of four (to PAGE_SIZE * 4). Also the alignment is more strict (aligned
  to PAGE_SIZE * 4).
  - PTEs must have User bit set

- hstatus.VSXLEN = 64-bit (0b10)
- hstatus.SPV = 1
- sstatus.SPP = 1
- sepc = vm entry point

Now just execute `sret`


## Delegating timer interrupts to VS

- mstatus.MIE = 1
- mideleg.VSTI = 1
- hideleg.VSTI = 1
- sie.STIE = 0
- hie.VSTIE = 1

to set interrupt 1 second in advance do
`sbi_set_timer(time + 10**7)`



# Glossary

- DTB - device tree binary
- MMIO - memory mapped input/output
- VAS - virtual address space
- VA - virtual address
- PA - physical address
- GPA - guest physical address
- identity mapping - va == pa
- linear mapping - va == pa + constant

# Extras

## Device Tree Binary (dtb)

This is a binary describing the physical layout of the machine. It
provides information on the number of CPUs, size of RAM, MMIO
addresses... I parsed it by hand into the file
[src/include/dtb.h](src/include/dtb.h).
