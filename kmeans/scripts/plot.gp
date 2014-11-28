set term png
set output '../output/result.png'
set datafile separator ','
plot '../input/cluster.csv','../output/result.csv'
