#!/bin/bash

#	TESTELE 41-50

#	vertices_array[i] reprezinta numarul de noduri pentru testul i (vertices_array realizat cu linspace intre 2 si 2500 in MATLAB)
	#	am extras doar primele 10 elemente din vectorul produs de linspace deoarece pentru celelalte 10, elementele vectorului
	#	NsquaredN ar fi depasit valoarea maxima de muchii din restrictiile temei (50 000)
vertices_array=(2 19 35	52 68 85 101 118 134 150)
NsquaredN=(4 361 1225 2704 4624 7225 10201 13924 17956 22500)	#	NsquaredN[i] = vertices_array[i] * vertices_array[i]

for i in {1..10}
do
    vertices=${vertices_array[i - 1]}	#	vertices = N

    edges=${NsquaredN[i - 1]}

    python3 ./random-graph-generator/gen_graph.py -grnm -n $vertices -m $edges --dir > ../in/test$(( i + 40 )).in

    linect=1
done