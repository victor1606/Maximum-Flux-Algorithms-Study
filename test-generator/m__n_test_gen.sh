#!/bin/bash

#	TESTELE 31-40

#	vertices_array[i] reprezinta numarul de noduri pentru testul i (vertices_array realizat cu linspace intre 2 si 2500 in MATLAB)
	#	am extras doar primele 10 elemente din vectorul produs de linspace deoarece pentru celelalte 10, elementele vectorului
	#	Nsqrt ar fi depasit valoarea maxima de muchii din restrictiile temei (50 000)
vertices_array=(2 134 265 397 528 660 791 923 1054 1186)

for i in {1..10}
do
    vertices=${vertices_array[i - 1]}	#	vertices = N

    maxedges=$(( vertices * (vertices - 1) ))
	
    edges=$(shuf -i $vertices-50000 -n 1)     #	pentru testele 31-40 avem M >> N, folosim functia shuf pentru M random
	#   in cazul in care numarul de muchii ales depaseste numarul maxim de muchii, folosesc shuf din nou; 
		if [ $edges -ge $maxedges ]; then
			edges=$(shuf -i 1-$vertices -n 1)	
		fi

    python3 ./random-graph-generator/gen_graph.py -grnm -n $vertices -m $edges --dir > ../in/test$(( i + 30 )).in

    linect=1
done