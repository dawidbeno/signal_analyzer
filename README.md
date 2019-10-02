# Signal analyzer

Vyvinuli sme aplikáciu, ktorá zabezpečí pomoc lekárovi pri vyhodnocovaní výsledkov neuromonitoringu. Porgram analyzuje dáta a upozorní lekára na možné komplikácie pred alebo počas operácie. 

Ako prvý sme vytvorili generátor, ktorý generuje prúd dát s podobnými vzormi a tým simuluje reálne meranie pri vyšetrení pacienta. Vytvorené dáta sa následne analyzujú na dvoch úrovniach. Prvá staticky vyhľadáva hodnoty, ktoré prekročia stanovenú hodnotu a tým určuje miesta, kde sa pravdepodobne nachádzajú simulované vlny neurosignálov. Druhá úroveň pomocou natrénovaného modelu neurónovej siete vyhodnotí o aký typ impulzu sa jedná.


## Generátor dát
Tento modul aplikácie simuluje činnosť externých meracích zariadení používaných pri neurofyziologických vyšetreniach. Tieto zariadenia merajú činnosť nervov a svalov pacientov – zachytávajú nervové impulzy, ktoré sú reprezentované vlnami v grafe. Postupnosť jednotlivých vĺn a artefaktov môže používateľ ručne ovládať príkazmi. Na základe týchto príkazov sa v reálnom čase vygenerujú rôzne typy vĺn. 
Typy generovaných vĺn:
![alt text 1](docs/img/wave1.png "wave1") ![alt text](docs/img/wave2.png "wave2")