/*
ZADATAK:

U trećoj laboratorijskoj vježbi potrebno je ostvariti simulator determinističkog potisnog automata (DPA). DPA prihvaća ulazne nizove koristeći prihvatljivo stanje. Ulaz u simulator automata je tekstualni zapis njegove definicije i ulazni niz, dok je izlaz tekstualni zapis skupova stanja u kojima se automat nalazio za svaki učitani znak ulaznog niza, nizove znakova stoga, te informacija o prihvatljivosti ulaznog niza.

**Ulazni format:**
1. redak: Ulazni nizovi odvojeni znakom "|". Simboli svakog niza odvojeni su zarezom.
2. redak: Skup stanja odvojenih zarezom.
3. redak: Skup ulaznih znakova odvojenih zarezom.
4. redak: Skup znakova stoga odvojenih zarezom.
5. redak: Skup prihvatljivih stanja odvojenih zarezom.
6. redak: Početno stanje.
7. redak: Početni znak stoga.
8. redak i dalje: Funkcija prijelaza u formatu trenutnoStanje,ulazniZnak,znakStoga->novoStanje,nizZnakovaStoga.

**Izlaz:**
Program ispisuje stanja automata kroz sve korake izvršavanja, stanja stoga i informacije o prihvatljivosti ulaznog niza.
*/


#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Definiranje funkcije za heširanje std::pair<std::string, std::string>
struct pair_hash {
    template <class T1, class T2>
    size_t operator () (const pair<T1, T2>& pair) const {
        auto hash1 = hash<T1>{}(pair.first);
        auto hash2 = hash<T2>{}(pair.second);
        return hash1 ^ (hash2 << 1);  // Kombiniranje heš vrijednosti
    }
};

// Rekurzivna funkcija koja traži sva dohvatljiva stanja
void findUnreachable(const string& startState, unordered_map<string, bool>& visited, unordered_map<string, unordered_map<char, string>>& stateMap) {
    visited[startState] = true;
    for (auto& transition : stateMap[startState]) {
        if (!visited[transition.second]) {
            findUnreachable(transition.second, visited, stateMap);
        }
    }
}

// Funkcija za pronalaženje istovjetnih stanja
unordered_map<string, unordered_set<string>> findDistinct(unordered_map<string, unordered_map<char, string>>& stateMap, const unordered_set<string>& acceptingStates, unordered_set<string> states, const unordered_set<char>& alphabet) {
    unordered_set<string> nonAcceptable;
    for (const string& state : states) {
        if (acceptingStates.find(state) == acceptingStates.end()) {
            nonAcceptable.insert(state);
        }
    }

    unordered_map<string, unordered_map<string, bool>> distinguishable;
    for (const string& state : states) {
        for (const string& anotherState : states) {
            distinguishable[state][anotherState] = true;
        }
    }

    for (const string& i : acceptingStates) {
        for (const string& n : nonAcceptable) {
            distinguishable[i][n] = distinguishable[n][i] = false;
        }
    }

    unordered_map<pair<string, string>, vector<pair<string, string>>, pair_hash> memoizedPairs;
    for (const string& s1 : states) {
        for (const string& s2 : states) {
            for (char a : alphabet) {
                pair<string, string> left = {stateMap[s1][a], stateMap[s2][a]};
                pair<string, string> right = {s1, s2};
                memoizedPairs[left].push_back(right);
                if (!distinguishable[left.first][left.second]) {
                    distinguishable[s1][s2] = distinguishable[s2][s1] = false;
                    for (auto& pair : memoizedPairs[left]) {
                        distinguishable[pair.first][pair.second] = distinguishable[pair.second][pair.first] = false;
                    }
                }
            }
        }
    }

    unordered_map<string, unordered_set<string>> equivalent;
    for (const string& s1 : states) {
        for (const string& s2 : states) {
            if (distinguishable[s1][s2]) {
                equivalent[s1].insert(s2);
            }
        }
    }

    for (auto& pair : equivalent) {
        pair.second.erase(pair.first);
        for (const string& s : pair.second) {
            if (s < pair.first) {
                equivalent[pair.first].erase(s);
                break;
            }
        }
    }

    return equivalent;
}

int main() {
    string line;

    // Učitavanje skupa stanja
    getline(cin, line);
    unordered_set<string> states;
    stringstream ss(line);
    string state;
    while (getline(ss, state, ',')) {
        states.insert(state);
    }

    // Učitavanje abecede
    getline(cin, line);
    unordered_set<char> alphabet;
    for (char c : line) {
        if (c != ',') {
            alphabet.insert(c);
        }
    }

    // Učitavanje prihvatljivih stanja
    getline(cin, line);
    unordered_set<string> acceptingStates;
    ss.clear();
    ss.str(line);
    while (getline(ss, state, ',')) {
        acceptingStates.insert(state);
    }

    // Učitavanje početnog stanja
    string startState;
    getline(cin, startState);

    // Učitavanje funkcije prijelaza
    unordered_map<string, unordered_map<char, string>> stateMap;
    while (getline(cin, line)) {
        stringstream transitionStream(line);
        getline(transitionStream, state, ',');
        char symbol;
        transitionStream >> symbol;
        string nextState;
        transitionStream.ignore(2); // Ignoriraj "->"
        getline(transitionStream, nextState);
        stateMap[state][symbol] = nextState;
    }

    // Traženje dohvatljivih stanja
    unordered_map<string, bool> visited;
    for (const string& s : states) {
        visited[s] = false;
    }
    findUnreachable(startState, visited, stateMap);

    // Uklanjanje nedohvatljivih stanja
    unordered_set<string> checkStates(states);
    for (const string& state : checkStates) {
        if (!visited[state]) {
            states.erase(state);
            acceptingStates.erase(state);
            stateMap.erase(state);
        }
    }

    // Traženje ekvivalentnih stanja
    unordered_map<string, unordered_set<string>> equivalent = findDistinct(stateMap, acceptingStates, states, alphabet);

    // Ispis ekvivalentnih klasa
    for (auto& e : equivalent) {
        cout << e.first << ": ";
        for (auto& s : e.second) {
            cout << s << " 
