// Generador de matriz booleana aleatoria + Matriz Básica + Densidad + Guardado
// Compilar: g++ -O2 -std=c++17 rbm.cpp -o rbm
// Ejecutar: ./rbm

#include <iostream>
#include <random>
#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

// --- Densidad: proporción de 1s ---
double densidad(const vector<vector<int>>& M) {
    if (M.empty() || M[0].empty()) return 0.0;
    long long unos = 0;
    for (const auto& fila : M)
        for (int v : fila) unos += v;
    return double(unos) / double(M.size() * M[0].size());
}

// --- Subfila: r <= s elemento a elemento y r != s ---
bool esSubfila(const vector<int>& r, const vector<int>& s) {
    bool menorEstricto = false;
    for (size_t j = 0; j < r.size(); ++j) {
        if (r[j] > s[j]) return false;
        if (r[j] < s[j]) menorEstricto = true;
    }
    return menorEstricto;
}

// --- Eliminar filas duplicadas ---
vector<vector<int>> filasUnicas(const vector<vector<int>>& M) {
    set<string> visto;
    vector<vector<int>> out;
    out.reserve(M.size());
    for (const auto& fila : M) {
        string clave;
        clave.reserve(fila.size());
        for (int v : fila) clave.push_back(char('0' + v));
        if (!visto.count(clave)) {
            visto.insert(clave);
            out.push_back(fila);
        }
    }
    return out;
}

// --- Matriz Básica ---
vector<vector<int>> aMatrizBasica(const vector<vector<int>>& M) {
    if (M.empty()) return M;
    vector<vector<int>> U = filasUnicas(M);
    int r = (int)U.size();
    vector<char> keep(r, 1);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < r; ++j) {
            if (i == j) continue;
            if (esSubfila(U[i], U[j])) {
                keep[i] = 0;
                break;
            }
        }
    }

    vector<vector<int>> BM;
    for (int i = 0; i < r; ++i)
        if (keep[i]) BM.push_back(U[i]);
    return BM;
}

// --- Guardar ambas matrices en un solo archivo ---
void guardarResultadoEnArchivo(
    const vector<vector<int>>& M,
    const vector<vector<int>>& BM,
    double densidadBM,
    const string& nombreArchivo = "resultado_matrices.txt"
) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el archivo '" << nombreArchivo << "' para escritura." << endl;
        return;
    }

    archivo << "=====================================\n";
    archivo << " MATRIZ BOOLEANA ORIGINAL (" << M.size() << "x" << (M.empty() ? 0 : M[0].size()) << ")\n";
    archivo << "=====================================\n\n";
    for (const auto& fila : M) {
        for (size_t j = 0; j < fila.size(); ++j)
            archivo << fila[j] << (j + 1 == fila.size() ? '\n' : ' ');
    }

    archivo << "\n-------------------------------------\n";
    archivo << " MATRIZ BASICA (" << BM.size() << "x" << (BM.empty() ? 0 : BM[0].size()) << ")\n";
    archivo << " densidad = " << fixed << setprecision(4) << densidadBM << "\n";
    archivo << "-------------------------------------\n\n";
    for (const auto& fila : BM) {
        for (size_t j = 0; j < fila.size(); ++j)
            archivo << fila[j] << (j + 1 == fila.size() ? '\n' : ' ');
    }

    archivo.close();
    cout << "✅ Matrices guardadas en: " << nombreArchivo << endl;
}

int main() {
    long long filas, columnas;

    cout << "=====================================" << endl;
    cout << " GENERADOR DE MATRICES BOOLEANAS" << endl;
    cout << "=====================================" << endl << endl;

    cout << "Ingrese el numero de filas: ";
    cin >> filas;
    cout << "Ingrese el numero de columnas: ";
    cin >> columnas;

    if (filas <= 0 || columnas <= 0) {
        cerr << endl << "Error: las filas y columnas deben ser mayores que 0." << endl;
        return 1;
    }

    // Generador aleatorio (50% de probabilidad de 1s)
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution dist(0.5);

    // Construcción de matriz
    vector<vector<int>> M(filas, vector<int>(columnas, 0));
    for (long long i = 0; i < filas; ++i)
        for (long long j = 0; j < columnas; ++j)
            M[i][j] = dist(gen);

    // Mostrar matriz booleana original
    cout << endl << "=====================================" << endl;
    cout << " MATRIZ BOOLEANA ALEATORIA (" << filas << "x" << columnas << ")" << endl;
    cout << "=====================================" << endl << endl;
    for (const auto& fila : M) {
        for (size_t j = 0; j < fila.size(); ++j)
            cout << fila[j] << (j + 1 == fila.size() ? '\n' : ' ');
    }

    // Matriz Básica y densidad
    vector<vector<int>> BM = aMatrizBasica(M);
    double dBM = densidad(BM);

    cout << endl << "-------------------------------------" << endl;
    cout << fixed << setprecision(4);
    cout << "Matriz Basica: " << BM.size() << "x" << (BM.empty() ? 0 : BM[0].size())
         << "  densidad = " << dBM << endl;
    cout << "-------------------------------------" << endl << endl;

    // Mostrar matriz básica
    cout << "MATRIZ BASICA (" << BM.size() << "x" << (BM.empty() ? 0 : BM[0].size()) << ")" << endl;
    for (const auto& fila : BM) {
        for (size_t j = 0; j < fila.size(); ++j)
            cout << fila[j] << (j + 1 == fila.size() ? '\n' : ' ');
    }

    // Preguntar si se guarda
    char opcionGuardar;
    cout << endl << "¿Desea guardar ambas matrices en un archivo de texto? (s/n): ";
    cin >> opcionGuardar;

    if (opcionGuardar == 's' || opcionGuardar == 'S') {
        guardarResultadoEnArchivo(M, BM, dBM);
    }

    cout << endl << "Proceso completado." << endl;
    return 0;
}
