/*
ZADATAK:

Zadatak ove laboratorijske vježbe je programski ostvariti simulator nedeterminističkog konačnog automata s epsilon prijelazima (ε-NKA). Ulaz u simulator je tekstualni zapis definicije automata i ulaznog niza, a izlaz je tekstualni zapis skupova stanja u kojima se automat nalazi za svaki učitani znak ulaznog niza.

**Ulazni format:**
1. redak: Ulazni nizovi odvojeni znakom |. Simboli svakog niza odvojeni su zarezom.
2. redak: Leksikografski poredan skup stanja odvojenih zarezom.
3. redak: Leksikografski poredan skup simbola abecede odvojenih zarezom.
4. redak: Leksikografski poredan skup prihvatljivih stanja odvojenih zarezom.
5. redak: Početno stanje.
6. i dalje: Funkcija prijelaza u formatu trenutnoStanje,simbolAbecede->skupIdućihStanja.

U definiciji epsilon-prijelaza, umjesto simbola abecede koristi se znak $. Prijelazi su leksikografski poredani. Ako za par (stanje, ulazni_znak) nije definiran prijelaz, smatra se da postoji prijelaz: stanje,ulazni_znak-># (prazan skup stanja).

**Izlaz:**
Program ispisuje skupove stanja u kojima se automat nalazi za svaki učitani znak iz ulaznog niza.
*/


#include <iostream>
#include <string>
#include <vector>
#include <sstream>  
#include <algorithm>
#include <set>
#include <queue>
#include <stack>
#include <unordered_map>

using namespace std;

// Prototipovi funkcija
void ispis_pq(priority_queue<string, vector<string>, greater<string>> &pq_za_ispis);
void prava_E_okolina_vector(vector<string> &E_okolina_unutarnji, vector<vector<vector<string>>> &funkcija_prijelaza, const vector<string> &skup_stanja);
void prava_E_okolina_set(set<string> &set_E_okruzenje_vanjski, vector<vector<vector<string>>> &funkcija_prijelaza, const vector<string> &skup_stanja);

int main() {
    // Definicija bitnih varijabli
    string procitan_redak;
    vector<vector<string>> Ulazni_nizovi;  // Ulazni nizovi za obradu
    vector<string> skup_stanja, skup_simbola, skup_Prihvatljivih_stanja;
    string pocetno_stanje;

    // Učitavanje 1. reda (ulazni nizovi)
    getline(cin, procitan_redak);
    stringstream X(procitan_redak);
    string niz, simbol, stanje;

    while (getline(X, niz, '|')) {
        stringstream Y(niz);
        vector<string> vektor_simbola;
        while (getline(Y, simbol, ',')) {
            vektor_simbola.push_back(simbol);
        }
        Ulazni_nizovi.push_back(vektor_simbola);
    }
    X.clear();
    
    // Učitavanje 2. reda (skup stanja)
    getline(cin, procitan_redak);
    stringstream Z(procitan_redak);
    while (getline(Z, stanje, ',')) {
        skup_stanja.push_back(stanje);
    }
    Z.clear();

    // Učitavanje 3. reda (skup simbola abecede)
    getline(cin, procitan_redak);
    stringstream Z_3(procitan_redak);
    while (getline(Z_3, simbol, ',')) {
        skup_simbola.push_back(simbol);
    }
    Z_3.clear();  

    // Učitavanje 4. reda (prihvatljiva stanja)
    getline(cin, procitan_redak);
    stringstream Z_4(procitan_redak);
    while (getline(Z_4, stanje, ',')) {
        skup_Prihvatljivih_stanja.push_back(stanje);
    }
    Z_4.clear();  

    // Učitavanje 5. reda (početno stanje)
    getline(cin, pocetno_stanje);

    // Učitavanje funkcije prijelaza
    vector<vector<vector<string>>> funkcija_prijelaza(skup_stanja.size(), vector<vector<string>>(skup_simbola.size() + 1, vector<string>(1, "#")));

    while (getline(cin, procitan_redak)) {
        vector<string> buduca_stanja;
        string buduce_stanje;
        stringstream Z_6(procitan_redak);
        getline(Z_6, stanje, ',');
        getline(Z_6, simbol, '-');
        Z_6.ignore(1);  // Ignore the ">"
        while (getline(Z_6, buduce_stanje, ',')) buduca_stanja.push_back(buduce_stanje);

        int index_stanje = distance(skup_stanja.begin(), find(skup_stanja.begin(), skup_stanja.end(), stanje));
        int index_simbol = (simbol == "$") ? 0 : distance(skup_simbola.begin(), find(skup_simbola.begin(), skup_simbola.end(), simbol)) + 1;

        funkcija_prijelaza[index_stanje][index_simbol] = buduca_stanja;
    }

    // Simulacija ε-NKA
    for (auto i = Ulazni_nizovi.begin(); i != Ulazni_nizovi.end(); i++) {
        vector<string> E_okolina_unutarnji;
        priority_queue<string, vector<string>, greater<string>> pq_za_ispis;

        E_okolina_unutarnji.push_back(pocetno_stanje);
        int index_stanje = distance(skup_stanja.begin(), find(skup_stanja.begin(), skup_stanja.end(), pocetno_stanje));
        int index_simbol = 0;

        for (auto k = funkcija_prijelaza[index_stanje][index_simbol].begin(); 
            k != funkcija_prijelaza[index_stanje][index_simbol].end(); k++) {
            E_okolina_unutarnji.push_back(*k);
        }

        prava_E_okolina_vector(E_okolina_unutarnji, funkcija_prijelaza, skup_stanja);
        for (auto k = E_okolina_unutarnji.begin(); k != E_okolina_unutarnji.end(); k++) {
            pq_za_ispis.emplace(*k);
        }
        ispis_pq(pq_za_ispis);

        // Obrada ulaznog niza
        for (auto j = i->begin(); j != i->end(); j++) {
            cout << "|";
            simbol = *j;
            set<string> set_srednja_razina, set_E_okruzenje_vanjski;

            if (E_okolina_unutarnji.size() == 1 && E_okolina_unutarnji.at(0) == "#") {
                set_srednja_razina.emplace("#");
            } else {
                for (auto k = E_okolina_unutarnji.begin(); k != E_okolina_unutarnji.end(); k++) {
                    if (*k != "#") {
                        index_stanje = distance(skup_stanja.begin(), find(skup_stanja.begin(), skup_stanja.end(), *k));
                        index_simbol = distance(skup_simbola.begin(), find(skup_simbola.begin(), skup_simbola.end(), simbol)) + 1;
                        vector<string> temp = funkcija_prijelaza[index_stanje][index_simbol];

                        for (auto z = temp.begin(); z != temp.end(); z++) {
                            set_srednja_razina.emplace(*z);
                        }
                    }
                }
            }

            if (set_srednja_razina.size() == 1 && *set_srednja_razina.begin() == "#") {
                set_E_okruzenje_vanjski.emplace("#");
            } else {
                for (auto k = set_srednja_razina.begin(); k != set_srednja_razina.end(); k++) {
                    if (*k != "#") {
                        set_E_okruzenje_vanjski.emplace(*k);
                        index_stanje = distance(skup_stanja.begin(), find(skup_stanja.begin(), skup_stanja.end(), *k));
                        index_simbol = 0;
                        vector<string> temp = funkcija_prijelaza[index_stanje][index_simbol];

                        for (auto z = temp.begin(); z != temp.end(); z++) {
                            set_E_okruzenje_vanjski.emplace(*z);
                        }
                    }
                }
            }

            prava_E_okolina_set(set_E_okruzenje_vanjski, funkcija_prijelaza, skup_stanja);
            E_okolina_unutarnji.clear();
            for (auto z = set_E_okruzenje_vanjski.begin(); z != set_E_okruzenje_vanjski.end(); z++) {
                E_okolina_unutarnji.push_back(*z);
                pq_za_ispis.emplace(*z);
            }
            ispis_pq(pq_za_ispis);
        }
        cout << endl;
    }

    return 0;
}

// Funkcije za ispis i obradu epsilon-prijelaza
void ispis_pq(priority_queue<string, vector<string>, greater<string>> &pq_za_ispis) {
    if (pq_za_ispis.size() == 1 && pq_za_ispis.top() == "#") {
        cout << "#";
        pq_za_ispis.pop();
        return;
    } else {
        while (!pq_za_ispis.empty()) {
            if (pq_za_ispis.top() != "#") {
                cout << pq_za_ispis.top();
                pq_za_ispis.pop();
                if (!pq_za_ispis.empty()) cout << ",";
            } else pq_za_ispis.pop();
        }
    }
}

void prava_E_okolina_vector(vector<string> &E_okolina_unutarnji, vector<vector<vector<string>>> &funkcija_prijelaza, const vector<string> &skup_stanja) {
    stack<string> stog;
    unordered_map<string, bool> posjecen;
    set<string> svi_E_prijelazi;

    for (const auto &stanje : E_okolina_unutarnji) {
        stog.push(stanje);
    }

    while (!stog.empty()) {
        string stanje = stog.top();
        stog.pop();

        if (stanje == "#") {
            svi_E_prijelazi.emplace("#");
        } else {
            int index_stanje = distance(skup_stanja.begin(), find(skup_stanja.begin(), skup_stanja.end(), stanje));
            if (!posjecen[stanje]) {
                posjecen[stanje] = true;
                svi_E_prijelazi.emplace(stanje);

                int index_simbola = 0;
                for (const auto &s : funkcija_prijelaza[index_stanje][index_simbola]) {
                    stog.push(s);
                }
            }
        }
    }

    E_okolina_unutarnji.clear();
    for (const auto &stanje : svi_E_prijelazi) {
        E_okolina_unutarnji.push_back(stanje);
    }
}

void prava_E_okolina_set(set<string> &set_E_okruzenje_vanjski, vector<vector<vector<string>>> &funkcija_prijelaza, const vector<string> &skup_stanja) {
    stack<string> stog;
    unordered_map<string, bool> posjecen;
    set<string> svi_E_prijelazi;

    for (const auto &stanje : set_E_okruzenje_vanjski) {
        stog.push(stanje);
    }

    while (!stog.empty()) {
        string stanje = stog.top();
        stog.pop();

        if (stanje == "#") {
            svi_E_prijelazi.emplace("#");
        } else {
            int index_stanje = distance(skup_stanja.begin(), find(skup_stanja.begin(), skup_stanja.end(), stanje));
            if (!posjecen[stanje]) {
                posjecen[stanje] = true;
                svi_E_prijelazi.emplace(stanje);

                int index_simbola = 0;
                for (const auto &s : funkcija_prijelaza[index_stanje][index_simbola]) {
                    stog.push(s);
                }
            }
        }
    }

    set_E_okruzenje_vanjski.clear();
    for (const auto &stanje : svi_E_prijelazi) {
        set_E_okruzenje_vanjski.emplace(stanje);
    }
}
