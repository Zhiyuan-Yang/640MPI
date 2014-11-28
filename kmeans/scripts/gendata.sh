#!/usr/bin/env bash

#Number of Points in each cluster
b=10

#Number of Cluster
k=2


		echo ********GENERATING $b INPUT POINTS EACH IN $k CLUSTERS 
		python generaterawdata.py -c $k  -p $b -o ../input/cluster.csv

