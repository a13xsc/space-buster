Utilizare aplicatie

WASD - deplasare nava
mouse - rotatie nava
click stanga - shoot

Navele albastre au o viata, cele galbene au 2 vieti. Playerul nu poate iesi din ecran.

Am creat doua clase (pe langa clasa generala ce controleaza jocul): Bar si Object.

Clasa Bar reprezinta proiectilele si barele de viata.

Clasa object reprezinta navele.



Variabilele clasei Bar:

angle - unghiul cu care este rotit proiectilul

axisSpeed - vector bidimensional cu vitezele proiectilului pe cele 2 axe

tail - coordonatele punctului ce reprezinta "coada" proiectilului (proiectilele sunt formate din 2 puncte unite printr-o linie ingrosata)



Variabilele clasei Object:

center - punctul din centrul navei

color - culoarea navei

angle - unghiul cu care este rotita nava

lives - cate vieti mai are nava (folosita doar pentru Player)

speed - viteza de deplasare a navei pe cele 4 axe

scale - factorul de scalare al navei (folosit atunci cand o nava inamica pierde o viata)



Logica jocului (clasa GeometryWars metoda Update)

la fiecare 2 secunde se creeaza un nou inamic si se adauga in vectorul enemies ce tine toti inamicii activi

se pozitioneaza inamicul pe un cerc de raza 3 in jurul jucatorului setandu-i variabilele de pozitie

Apoi se actualizeaza informatiile legate de inamicii existenti in vectorul enemies:

se calculeaza noua pozitie a centrului navei folosind viteza de deplasare a navei si unghiul cu care este rotita

se calculeaza noul unghi cu care trebuie rotit inamicul fata de jucator: se calculeaza folosind diferentele de pozitie pe axele OX si OY si tangenta

se verifica coliziunile dintre proiectile si inamici (daca avem coliziune se modifica proprietatile inamicului: viteza, dimensiune, etc.)

daca numarul de vieti ale unui inamic a ajuns la zero acesta este sters

pentru deplasarea shot-urilor se foloseste metoda moveShots ce actualizeaza coordonatele unui shot tinand cont de vitezele pe cele 2 axe

daca un shot a iesit din ecran acesta este sters din memorie

cand numarul de vieti al player-ului a ajuns la zero, toate actualizarile de sus nu se mai executa (practic jocul ingheata) iar ecranul se coloreaza rosu




Meshele sunt create de functiile din fisierul Meshes.h. Cand se deseneaza inamicii se tine cont de numarul de vieti pentru a stabili culoarea acestora.