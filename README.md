Comparison between 3 maximum flux algorithms: Ford-Fulkerson, Push-Relabel(Relabel-to-front), Malhotra-Kumar-Maheshwari

Calugaritoiu Ion-Victor

Analiza Algoritmilor - Flux Maxim

Continutul arhivei:

	- ff.cpp: algoritmul Ford_fulkerson;
		Credits: https://github.com/BedirT/Algorithms_and_DS/
			blob/master/Algorithms/Graph/Ford%20Fulkerson.cpp

	- pr.cpp: algoritmul de tip push-relabel;
		Credits: https://github.com/Anton-Cherepkov/
			MaximumFlow-MKM-RelabelToFront/blob/master/main.cpp

	- mkm.cpp: algoritmul Malhotra-Kumar-Maheshwari;
		Credits: https://github.com/Anton-Cherepkov/
			MaximumFlow-MKM-RelabelToFront/blob/master/main.cpp

	- maxflow.h: header care are definitia functiei "maxflow";

	- test.cpp: apeleaza algoritmii cu fisierul de intrare test.in si printeaza rezultatul in fisierul test.out;

	- Makefile realizat conform indicatiilor din enunt;

	- directoarele "in" si "out" ce contin fisierele de intrare si cele pentru verificarea testelor, generate utilizand script-ul "test_gen.sh", prezent in directorul test-generator;

	- test-generator: contine script-ul utilizat pentru a genera testele:
		- test_gen.sh: apeleaza la randul sau urmatoarele script-uri:
			* nlogn_test_gen.sh - folosit pentru testele 1-20, in care numarul
		de muchii 'M' este egal cu N * log(N);
			* nsqrtn_test_gen.sh - folosit pentru testele 21-30, in care
		numarul de muchii 'M' este egal cu N * sqrt(N);
			* m>>n_test_gen.sh - folosit pentru testele 31-40, in care numarul
		'M' este generat aleator, fiind mult mai mare decat numarul 'N';
			* nsquaredn_test_gen.sh - folosit pentru testele 41-50, in care
		numarul de muchii 'M' este egal cu N * N;

			- random-graph-generator: contine un script realizat in python care
		in functie de input ('M' si 'N' obtinute de la script-urile de mai sus)
		genereaza un graf aleator;
			Credits: https://github.com/deyuan/random-graph-generator;

			- mkm.out: dupa ce fiecare fisier .in este realizat, se ruleaza 
		toate testele pe algoritmul mkm, iar output-urile sunt redirectate in
		directorul "out";
