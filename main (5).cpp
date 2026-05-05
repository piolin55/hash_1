#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>

using namespace std;

class TablaHash {
private:
    int tamaño;
    vector<list<int>> tabla;

public:
    TablaHash(int t) {
        tamaño = t;
        tabla.resize(tamaño);
    }

    int hashFuncion(int valor) {
        return valor % tamaño;
    }

    void insertar(int valor) {
        int index = hashFuncion(valor);
        tabla[index].push_back(valor);
    }

    void buscar(int valor) {
        int index = hashFuncion(valor);
        for (int v : tabla[index]) {
            if (v == valor) {
                cout << "Encontrado en índice " << index << endl;
                return;
            }
        }
        cout << "No encontrado" << endl;
    }

    void eliminar(int valor) {
        int index = hashFuncion(valor);
        tabla[index].remove(valor);
    }

    void mostrar() {
        cout << "\nTabla Hash:\n";
        for (int i = 0; i < tamaño; i++) {
            cout << i << ": ";
            for (int v : tabla[i]) {
                cout << v << " -> ";
            }
            cout << endl;
        }
    }

    void cargarCSV(string nombreArchivo) {
        ifstream archivo(nombreArchivo);
        string linea;
        int contador = 0;

        if (!archivo.is_open()) {
            cout << "Error al abrir el archivo\n";
            return;
        }

        while (getline(archivo, linea)) {
            stringstream ss(linea);
            int valor;

            if (ss >> valor) {
                insertar(valor);
                contador++;
            }
        }

        archivo.close();
        cout << "Datos cargados: " << contador << endl;
    }
};

int main() {
    TablaHash hash(10);
    int opcion, valor;
    string archivo;

    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Insertar\n";
        cout << "2. Buscar\n";
        cout << "3. Eliminar\n";
        cout << "4. Mostrar tabla\n";
        cout << "5. Cargar CSV\n";
        cout << "0. Salir\n";
        cout << "Opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "Ingrese valor: ";
                cin >> valor;
                hash.insertar(valor);
                break;

            case 2:
                cout << "Ingrese valor: ";
                cin >> valor;
                hash.buscar(valor);
                break;

            case 3:
                cout << "Ingrese valor: ";
                cin >> valor;
                hash.eliminar(valor);
                break;

            case 4:
                hash.mostrar();
                break;

            case 5:
                cout << "Ingrese nombre del archivo CSV: ";
                cin >> archivo;
                hash.cargarCSV(archivo);
                break;
        }

    } while (opcion != 0);

    return 0;
}