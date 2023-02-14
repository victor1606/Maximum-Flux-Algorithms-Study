#!/bin/bash
echo "Generating first 20 tests"
./nlogn_test_gen.sh

echo "Generating tests 21-30"
./nsqrtn_test_gen.sh

echo "Generating tests 31-40"
./m\>\>n_test_gen.sh

echo "Generating tests 41-50"
./nsquared_test_gen.sh 

#   Adaugam un numar random la finalul fiecarei linii (mai putin prima) din fisierele de test, numarul fiind fluxul asociat muchiei
for i in {1..50}
do
    awk -i inplace -v seed=$RANDOM 'BEGIN{srand(seed)} {print $0, int(rand() * 10^5+1)}' ../in/test$((i)).in
    sed -i '1s/\w*$//' ../in/test$((i)).in
done

echo "Done generating .in files"

#   Rulez fiecare test generat folosind algoritmul MKM si redirectez output-ul in fisierul out
echo "Generating .out files"

cd ..
make
cd ./test-generator

for i in {1..50}
do 
    ./mkm.out < ../in/test$((i)).in > ../out/test$((i)).out
done

echo Done