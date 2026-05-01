// ================================================================
// PRÁCTICA: TABLAS HASH EN C++
// Compilar: g++ -O2 -std=c++17 -o hash_tabla hash_tabla.cpp
// ================================================================

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// ─────────────────────────────────────────────
// ESTRUCTURA
// ─────────────────────────────────────────────
struct Registro {
    string clave;
    string valor;
};

// ─────────────────────────────────────────────
// FUNCIÓN HASH
// ─────────────────────────────────────────────
size_t funcionHash(const string& clave, size_t tamanio) {
    size_t hash = 0;
    for (char c : clave)
        hash = (hash * 31 + c) % tamanio;
    return hash;
}

// ─────────────────────────────────────────────
// ENCADENAMIENTO
// ─────────────────────────────────────────────
class TablaHashEncadenamiento {
public:
    size_t tamanio;
    vector<list<Registro>> tabla;
    long long colisiones = 0;
    long long elementos = 0;

    TablaHashEncadenamiento(size_t m) : tamanio(m), tabla(m) {}

    void insertar(const Registro& reg) {
        size_t idx = funcionHash(reg.clave, tamanio);
        if (!tabla[idx].empty())
            colisiones++;
        tabla[idx].push_back(reg);
        elementos++;
    }

    Registro* buscar(const string& clave) {
        size_t idx = funcionHash(clave, tamanio);
        for (auto& reg : tabla[idx]) {
            if (reg.clave == clave)
                return &reg;
        }
        return nullptr;
    }

    double factorDeCarga() const {
        return (double)elementos / tamanio;
    }
};

// ─────────────────────────────────────────────
// SONDEO LINEAL
// ─────────────────────────────────────────────
class TablaHashSondeoLineal {
public:
    enum Estado { VACIO, OCUPADO };

    size_t tamanio;
    vector<Registro> tabla;
    vector<Estado> estados;
    long long colisiones = 0;
    long long elementos = 0;

    TablaHashSondeoLineal(size_t m)
        : tamanio(m), tabla(m), estados(m, VACIO) {}

    bool insertar(const Registro& reg) {
        if ((double)elementos / tamanio >= 0.75)
            return false;

        size_t idx = funcionHash(reg.clave, tamanio);
        size_t pasos = 0;

        while (estados[idx] == OCUPADO) {
            colisiones++;
            idx = (idx + 1) % tamanio;
            if (++pasos >= tamanio)
                return false;
        }

        tabla[idx] = reg;
        estados[idx] = OCUPADO;
        elementos++;
        return true;
    }

    Registro* buscar(const string& clave) {
        size_t idx = funcionHash(clave, tamanio);
        size_t pasos = 0;

        while (estados[idx] != VACIO) {
            if (estados[idx] == OCUPADO && tabla[idx].clave == clave)
                return &tabla[idx];
            idx = (idx + 1) % tamanio;
            if (++pasos >= tamanio)
                break;
        }
        return nullptr;
    }

    double factorDeCarga() const {
        return (double)elementos / tamanio;
    }
};

// ─────────────────────────────────────────────
// BENCHMARK
// ─────────────────────────────────────────────
void benchmark(const vector<Registro>& datos, vector<size_t> tamanios) {

    cout << left
         << setw(8) << "m"
         << setw(8) << "n"
         << setw(12) << "Col_Enc"
         << setw(12) << "Time_Enc(us)"
         << setw(12) << "Col_SL"
         << setw(12) << "Time_SL(us)"
         << endl;

    cout << string(72, '-') << endl;

    for (size_t m : tamanios) {

        size_t n = min(datos.size(), (size_t)(m * 0.65));

        // Encadenamiento
        TablaHashEncadenamiento enc(m);
        auto t0 = high_resolution_clock::now();
        for (size_t i = 0; i < n; i++)
            enc.insertar(datos[i]);
        auto t1 = high_resolution_clock::now();
        auto tiempo_enc = duration_cast<microseconds>(t1 - t0).count();

        // Sondeo lineal
        TablaHashSondeoLineal sl(m);
        t0 = high_resolution_clock::now();
        for (size_t i = 0; i < n; i++)
            sl.insertar(datos[i]);
        t1 = high_resolution_clock::now();
        auto tiempo_sl = duration_cast<microseconds>(t1 - t0).count();

        cout << setw(8) << m
             << setw(8) << n
             << setw(12) << enc.colisiones
             << setw(12) << tiempo_enc
             << setw(12) << sl.colisiones
             << setw(12) << tiempo_sl
             << endl;
    }
}

// ─────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────
int main() {

    cout << "=== BENCHMARK TABLAS HASH (C++) ===\n\n";

    vector<Registro> datos;

    // Datos sintéticos
    for (int i = 0; i < 5000; i++) {
        Registro r;
        r.clave = "user_" + to_string(i + 1);
        r.valor = "valor_" + to_string(i);
        datos.push_back(r);
    }

    cout << "Registros generados: " << datos.size() << "\n\n";

    vector<size_t> tamanios = {1009, 2003, 4001, 8009};

    benchmark(datos, tamanios);

    return 0;
}