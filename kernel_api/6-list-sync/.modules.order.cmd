cmd_/home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/modules.order := {   echo /home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/list-sync.ko; :; } | awk '!x[$$0]++' - > /home/noureddine/linux-lab/linux-kernel-labs/6-list-sync/modules.order