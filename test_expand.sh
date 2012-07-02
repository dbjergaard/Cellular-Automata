#!/usr/bin/zsh
seed=137
isize=4
for i in {2..5}
do
    ./RunSim -s ${seed} -i ${isize} -b -e $i
    mkdir tau_${i}_isize_${isize}_seed_${seed}
    mv *.xpm tau_${i}_isize_${isize}_seed_${seed}
    mv batch-table.org tau_${i}_isize_${isize}_seed_${seed}
    cp one_init/batch-table.html tau_${i}_isize_${isize}_seed_${seed}/ 
    cd tau_${i}_isize_${isize}_seed_${seed}
    mogrify -format gif *.xpm
    cd ../   
done
