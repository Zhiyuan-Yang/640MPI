all: gendata build run plot

gendata:
	cd scripts; ./gendata.sh

build: FORCE
	gcc kmeans-seq.c -o build/kmeans-seq

run:
	build/kmeans-seq input/cluster.csv output/result.csv 2 10 2

plot:
	cd scripts; gnuplot plot.gp

clean:
	rm -f input/*
	rm -f output/*
	rm -f build/*

FORCE:
