cmd_/home/noureddine/linux-lab/linux-kernel-labs/deffered_work/3-4-5-deffered/kernel/modules.order := {   echo /home/noureddine/linux-lab/linux-kernel-labs/deffered_work/3-4-5-deffered/kernel/timer.ko; :; } | awk '!x[$$0]++' - > /home/noureddine/linux-lab/linux-kernel-labs/deffered_work/3-4-5-deffered/kernel/modules.order