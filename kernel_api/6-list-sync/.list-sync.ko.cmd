cmd_/home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/list-sync.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000 --build-id  -T ./scripts/module-common.lds -o /home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/list-sync.ko /home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/list-sync.o /home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/list-sync.mod.o;  true