#!/bin/bash

#	TESTELE 1-20

#	vertices_array[i] reprezinta numarul de noduri pentru testul i (vertices_array realizat cu linspace intre 2 si 2500 in MATLAB)
vertices_array=(2 134 265 397 528 660 791 923 1054 1186 1317 1449 1580 1712 1843 1975 2106 2238 2369 2500)
#	NlogN[i] = vertices_array[i] * log(vertices_array[i])
NlogN=(2 657 1479 2376 3311 4285 5279 6302 7337 8395 9461 10547 11637 12747 13858 14987 16117 17263 18408 19561)

for i in {1..20}
do
    vertices=${vertices_array[i - 1]}	#	vertices = N

    edges=${NlogN[i - 1]}

    python3 ./random-graph-generator/gen_graph.py -grnm -n $vertices -m $edges --dir > ../in/test$i.in

    linect=1
done