/*
ZADATAK:

Zadatak ove laboratorijske vježbe je programski ostvariti minimizaciju determinističkog konačnog automata (DKA). Programsko rješenje treba ostvariti uklanjanje nedohvatljivih stanja i istovjetnih stanja u automatu. Ulaz u program je tekstualna definicija automata, a izlaz je minimizirana verzija istog automata.

**Ulazni format:**
1. redak: Skup stanja odvojenih zarezom, leksikografski poredanih.
2. redak: Skup simbola abecede odvojenih zarezom, leksikografski poredanih.
3. redak: Skup prihvatljivih stanja odvojenih zarezom, leksikografski poredanih.
4. redak: Početno stanje.
5. redak i dalje: Funkcija prijelaza u formatu trenutnoStanje,simbolAbecede->idućeStanje.

**Izlaz:**
Program ispisuje minimizirani automat, uklanjajući nedohvatljiva i istovjetna stanja.
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
            cout << s << " ";
        }
        cout << endl;
    }

    return 0;
}
