Copyright 2023 Teodora Voicu

Prezentare generala:

1) structura de load_balancer contine:
-un vector server_tags ce contine etichetele serverelor in ordine crescatoare
-servers ce tine serverele propriu-zise si este de tip server_memory(*)
-numarul de servere propriu-zise

(*) 2)structura de server_memory contine:
-hashtabelul htserver asociat fiecarui server
-id-ul serverului
-vectorul de chei keys care tine toate cheile hashtabelului serverului
-numarul de chei
-dimensiunea cheii


Functiile din load_balancer:

1) functia init_load_balancer initializeaza load balancerul pentru a putea fi utilizat mai departe

2)functia loader_add_server  adauga in load balancer un nou server. Acest lucru presupune adaugarea celor 3 etichete corespunzătoare serverelor în server_tags alocând un nou vector de dimensiuni corespunzătoare în care sa punem elementele din vectorul vechi plus cele 3 noi etichete și eliberând memoria celui vechi. Analog procedam și cu vectorul de servere propriu-zise, doar ca mărim dimensiunea cu 1 singur element și, în plus, trebuie alocată memorie și pentru câmpurile din cadrul noului element și asociat id-ul corespunzător. În final, cu ajutorul funcției servers_sort sortam în ordine crescătoare vectorul de etichete și redistribuim elementele de pe serverele vecine cu ajutorul funcției rebalance_objects.

3)funcția rebalance_objects primește ca argumente load_balancerul și un server_id și și redistribuie obiectele de pe serverul corespunzător următoarei etichete din server_tags pe serverul corespunzător id-ului primit ca argument.
Parcurgem vectorul de etichete și de fiecare data când dam de o eticheta care aparține de serverul cu id-ul dat ca argument, și, dacă ne aflam pe orice poziție mai putin pe ultima, salvam intr-o variabila noua (next_server) serverul corespunzător etichetei imediat următoare. Parcurg vectorul de chei din acel server și dacă găsesc chei ce trebuie mutate, le salvez în serverul nou și le  păstrez în removed_keys, pentru a le șterge la final din vectorul de chei și din server. În cazul în care ne aflam pe ultima poziție, next_server va fi serverul corespunzător primei etichete din vector care nu apartine tot de serverul nostru, apoi procedam analog.

4)funcția loader_remove_server șterge un server. Acest lucru presupune atât redimensionarea vectorului de etichete cât și al vectorului de servere. Salvam cheile și valorile de pe serverul ce urmează a fi șters, copiem etichetele și serverele pe care le păstrăm la o locație noua, eliberam memoria locatiei vechi, apoi asociem vechii locații noua locație. În final, punem perechile cheie-valoarea pe celelalte servere și eliberam memoria vectorilor în care le tineam.

5)funcția loader_store stochează o pereche cheie-valoare pe unul din serverele din load balancer și intorarce prin parametrul server_id, id-ul serverului în care am stocat perechea. Obținem id-ul serverului pe care ar trebui sa stocam perechea, cu ajutorul funcției retrieve_server_store_id ce compara hashul cheii noastre cu hashul etichetelor din server_tags și returnează id-ul serverului etichetei ce are hashul mai mare decât hashul cheii. În final, stocam perechea cheie-valoare pe serverul corespunzător, cu ajutorul funcției server_store, obținând serverul prin funcția get_server_by_id ce returnează serverul în funcție de id-ul dat ca argument.

6)funcția loader_retrieve returnează valoarea asociata unei chei aflate pe unul din serverele din load balancer. În aceeași maniera ca la funcția precedenta, obținem id-ul serverului pe care ar trebui sa găsim valoarea și returnam valoarea cu ajutorul funcției server_retrieve.

7)funcția free_load_balancer eliberează memoria din load balancer, eliberând memoria fiecărui server cu ajutorul funcției free_server_memory, apoi memoria pointerului ce poiata la vectorul de servere, vectorul de etichete și load balancerului.



Funcțiile din server:

1)funcția init_server_memory aloca memorie pentru un server de tip server_memory, acest lucru presupunând alocare de memorie pentru hashtable, cu ajutorul funcției ht_create și plaseara câmpurilor ce stochează numărul de chei, dimensiunea cheilor pe 0 și vectorul de chei pe NULL.

2)funcția server_store stochează pe un server dat, mai precis în hashtableul aferent serverului dat, o pereche cheie-valoare. Acest lucru se poate realiza direct cu funcția ht_put, rămânând doar de adăugat cheia la vectorul de chei, ce trebuie redimensionat, incrementand la final și numărul de chei.

3)funcția server_retrieve returnează valoarea asociata unei chei, acest lucru realizându-se rapid direct prin apelarea funcției ht_get, pe hashtabelul aferent serverului dat ca parametru.

4)funcția server_remove șterge atât cheia data ca parametru cât și valoarea asociata ei, de pe serverul dat ca parametru. Ștergem perechea cu ajutorul funcției ht_remove_entry pe hashtabelul corespunzător serverului, redimensionand vectorul de chei cu un element mai putin și decremetand numărul de chei.

5)funcția free_server_memory eliberează memoria serverului dat ca parametru, eliberând cu ajutorul funcției ht_free memoria alocată pentru hashtable, apoi eliberează memoria vectorului de chei, a pointerului la vectorul de chei și apoi adresa serverului.