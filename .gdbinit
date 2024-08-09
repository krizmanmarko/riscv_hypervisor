target extended-remote localhost:1111

symbol-file src/guest/xv6-riscv-guest/kernel/kernel
add-symbol-file build/hypervisor
add-symbol-file src/guest/xv6-riscv-guest/user/_usertests

set pagination off
set confirm off
set print pretty on
set print array-indexes on

# if you want complete control on multicore systems
#set scheduler-locking on
#maintenance packet qqemu.sstep=0x1


source src/helper/pgtable.py
source src/helper/scripts.py

define pproc
set $i = $arg0
while ($i < $arg1)
p proc[$i]->$arg2
set $i = $i + 1
end
end
