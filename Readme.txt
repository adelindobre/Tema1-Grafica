Tema 1 Elemente de grafica pe calculator

Trimit tema cu doua ore intarziere iar acest lucru sper sa nu se depuncteze. Am mai avut astefl de probleme
si la alte materii, iar inatrzierea nu s-a depunctat.
In cadrul temei , am reusit sa definesc corect scena, sa descriu raza de lumina se iese din sursa.
Raza de lumina se scaleaza cu o valoare constanta.
Am reusit sa implementez coliziunea atat pentru patraturi, dreptunghiuri, cat si pentru cercuri.
La fiecare frame, obtin coordonatele pixelului din capatul razei folosindu-ma de proprietatile dintr-un triunghi dreptunghic
si testez pentru fiecare obiect in parte sa vad daca punctul respectiv a atins obiectul sau nu.

Referitor la reflexie, am reusit sa vad reflexia pentru prima raza ce iese din sursa de lumina. Raza reflectata va avea 
triectoria corecta pentru orice directie a suprafetei. Problema este ca mai multe raze consecutive nu sunt trasate corect,
iar aceasta problema presupun ca este de la determinarea unghiului.
Reflectie implementare:

Am ales sa creez un segment din viitoare raza care sa fie normala la suprafata curenta, dupa care o rotesc cu unghiul
dintre raza incidenta si normala, iar noul segment obtinut incep sa il scalez.

Se poate regenera mersul razelor prin apasarea tastei "1" care va schimba directia primei raze din sursa de lumina.

Dobre Grigore Adelin 333CC