cmd_/home/noureddine/linux-lab/linux-kernel-labs/block_device_drivers/1-2-3-6-ram-disk/kernel/Module.symvers := sed 's/ko$$/o/' /home/noureddine/linux-lab/linux-kernel-labs/block_device_drivers/1-2-3-6-ram-disk/kernel/modules.order | scripts/mod/modpost  -a   -o /home/noureddine/linux-lab/linux-kernel-labs/block_device_drivers/1-2-3-6-ram-disk/kernel/Module.symvers -e -i Module.symvers  -N -T -