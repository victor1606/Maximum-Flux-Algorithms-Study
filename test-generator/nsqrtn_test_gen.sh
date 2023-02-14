#!/bin/bash

#	TESTELE 21-30

#	vertices_array[i] reprezinta numarul de noduri pentru testul i (vertices_array realizat cu linspace intre 2 si 2500 in MATLAB)
	#	am extras doar primele 10 elemente din vectorul produs de linspace deoarece pentru celelalte 10, elementele vectorului
	#	Nsqrt ar fi depasit valoarea maxima de muchii din restrictiile temei (50 000)
vertices_array=(2 134 265 397 528 660 791 923 1054 1186)
NsqrtN=(3 1552 4314 7911 12133 16956 22247 28042 34219 40844)	#	NsqrtN[i] = vertices_array[i] * sqrt(vertices_array[i])

for i in {1..10}
do
    vertices=${vertices_array[i - 1]}	#	vertices = N

    edges=${NsqrtN[i - 1]}

    python3 ./random-graph-generator/gen_graph.py -grnm -n $vertices -m $edges --dir > ../in/test$(( i + 20 )).in

    linect=1
done