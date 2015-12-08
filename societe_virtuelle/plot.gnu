reset
set title textcolor rgb "red" "Pourcentage du temps passé en tant que leader"
set xlabel "Combat-ability"
set ylabel "Charisme"
set zlabel offset +5,+4 "Pourcentage du temps"
splot 'out1.txt' using 1:2:3

