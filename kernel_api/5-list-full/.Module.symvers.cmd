cmd_/home/noureddine/linux-lab/linux-kernel-labs/5-list-full/Module.symvers := sed 's/ko$$/o/' /home/noureddine/linux-lab/linux-kernel-labs/5-list-full/modules.order | scripts/mod/modpost  -a   -o /home/noureddine/linux-lab/linux-kernel-labs/5-list-full/Module.symvers -e -i Module.symvers  -N -T -