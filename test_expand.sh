#!/usr/bin/zsh
for i in {1..5}
do
    ./RunSim -s 10 -b -e $i
    mkdir tau_${i}_one_init
    mv *.xpm tau_${i}_one_init
    mv batch-table.org tau_${i}_one_init
    cd tau_${i}_one_init 
    mogrify -format gif *.xpm
    emacs batch-table.org --batch --eval="(org-export-as-html)"
    cd ../   
done
