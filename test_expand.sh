#!/usr/bin/zsh
for i in {15..30}
do
    ./RunSim -rn 30 -e $i
    mv rule_num_30.xpm rule_num_30_e_$i.xpm
done
