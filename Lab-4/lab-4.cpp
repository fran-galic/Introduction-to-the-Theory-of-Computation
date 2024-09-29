/*
ZADATAK:

U četvrtoj laboratorijskoj vježbi zadatak je programski ostvariti parser tehnikom rekurzivnog spusta za zadanu LL(1) gramatiku:

S → aAB | bBA  
A → bC | a  
B → ccSbc | ϵ  
C → AA

Program treba parsirati ulazni niz znakova (a, b, c) i ispisati redom nezavršne simbole (S, A, B, C) kada parser uđe u odgovarajuće funkcije. Ukoliko ulazni niz pripada jeziku definiranom ovom gramatikom, ispisuje se riječ "DA". U suprotnom, ispisuje se riječ "NE". Parser koristi tehniku rekurzivnog spusta i obrađuje niz ulaznih znakova po pravilima gramatike.

**Ulazni format:**
- Ulaz je jedan redak s nizom znakova (a, b, c), duljine do 200 znakova, koji se parsira pomoću zadane gramatike.

**Izlaz:**
- U prvom retku izlaza ispisuju se nezavršni znakovi koje parser obrađuje redom (S, A, B, C).
- U drugom retku izlaza ispisuje se "DA" ako ulazni niz pripada jeziku, odnosno "NE" ako ne pripada.

Primjeri:

1. Ulaz: aa  
   Ispis: SAB  
          DA

2. Ulaz: ab  
   Ispis: SACA  
          NE
*/


#include <iostream>
#include <vector>

using namespace std;

// Globalne varijable za ulazni niz i kazaljku
vector<char> ulazniNiz;
int kazaljka = 0;

// Deklaracije funkcija za nezavršne simbole gramatike
void C();
void B();
void A();
void S();

// Funkcija za nezavršni simbol C → AA
void C() {
    cout << "C";
    if (kazaljka < (int)ulazniNiz.size()) {
        A();
        if (kazaljka < (int)ulazniNiz.size()) {
            A();
        }
    }
}

// Funkcija za nezavršni simbol B → ccSbc | ϵ
void B() {
    cout << "B";
    if (kazaljka < (int)ulazniNiz.size() && ulazniNiz[kazaljka] == 'c') {
        kazaljka++;
        if (kazaljka < (int)ulazniNiz.size() && ulazniNiz[kazaljka] == 'c') {
            kazaljka++;
            S();
            if (kazaljka < (int)ulazniNiz.size() && ulazniNiz[kazaljka] == 'b') {
                kazaljka++;
                if (kazaljka < (int)ulazniNiz.size() && ulazniNiz[kazaljka] == 'c') {
                    kazaljka++;
                }
            }
        }
    }
}

// Funkcija za nezavršni simbol A → bC | a
void A() {
    cout << "A";
    if (kazaljka < (int)ulazniNiz.size()) {
        if (ulazniNiz[kazaljka] == 'b') {
            kazaljka++;
            C();
        } else if (ulazniNiz[kazaljka] == 'a') {
            kazaljka++;  // Pomiče kazaljku ako se pročita 'a'
        } else {
            cout << endl << "NE" << endl;
            exit(0);  // Izlaz ako niz nije u jeziku
        }
    } else {
        cout << endl << "NE" << endl;
        exit(0);  // Izlaz ako niz nije u jeziku
    }
}

// Funkcija za nezavršni simbol S → aAB | bBA
void S() {
    cout << "S";
    if (kazaljka < (int)ulazniNiz.size()) {
        if (ulazniNiz[kazaljka] == 'a') {
            kazaljka++;
            A();
            B();
        } else if (ulazniNiz[kazaljka] == 'b') {
            kazaljka++;
            B();
            A();
        } else {
            cout << endl << "NE" << endl;
            exit(0);  // Izlaz ako niz nije u jeziku
        }
    }
}

int main() {
    string ulaz;
    getline(cin, ulaz);  // Učitavanje ulaznog niza
    
    // Punjenje ulaznog niza u vektor karaktera
    for (char c : ulaz) {
        ulazniNiz.push_back(c);
    }
    ulazniNiz.push_back('\0');  // Dodaje terminalni znak za sigurnost

    S();  // Poziv početnog nezavršnog znaka

    // Provjera je li cijeli niz obrađen
    if (kazaljka < (int)ulazniNiz.size() && ulazniNiz[kazaljka] != '\0') {
        cout << endl << "NE";
    } else {
        cout << endl << "DA";
    }
    
    return 0;
}
