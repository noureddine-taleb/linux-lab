cmd_/home/noureddine/linux-lab/linux-kernel-labs/Module.symvers := sed 's/ko$$/o/' /home/noureddine/linux-lab/linux-kernel-labs/modules.order | scripts/mod/modpost  -a   -o /home/noureddine/linux-lab/linux-kernel-labs/Module.symvers -e -i Module.symvers  -N -T -
