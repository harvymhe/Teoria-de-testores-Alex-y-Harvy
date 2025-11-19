#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <iomanip>

using namespace std;

typedef vector<vector<int>> Matriz;
typedef set<int> Conjunto;

// Imprimir matriz con cabecera A, B, C...
void imprimir_matriz(const Matriz& MB) {
    int filas = MB.size();
    int columnas = MB[0].size();
    cout << "🟦 Matriz Booleana (MB):\n   ";
    for (int j = 0; j < columnas; ++j)
        cout << setw(2) << char('A' + j);
    cout << "\n";

    for (int i = 0; i < filas; ++i) {
        cout << i + 1 << ": ";
        for (int j = 0; j < columnas; ++j)
            cout << setw(2) << MB[i][j];
        cout << "\n";
    }
    cout << endl;
}

// Verifica si al menos una columna del conjunto tiene un 1 en la fila
bool tiene_uno_en_fila(const vector<int>& fila, const Conjunto& conjunto) {
    for (int col : conjunto)
        if (fila[col] == 1)
            return true;
    return false;
}

// Verifica los dos criterios para extensión
bool cumple_criterios(const Matriz& MB, const Conjunto& conjunto, int hastaFila) {
    int filas = hastaFila + 1;
    int columnas = conjunto.size();

    int cuenta_filas = 0;
    vector<int> suma_col(columnas, 0);

    for (int i = 0; i < filas; ++i) {
        int suma = 0;
        int j = 0;
        for (int col : conjunto) {
            suma += MB[i][col];
            suma_col[j++] += MB[i][col];
        }
        if (suma == 1) cuenta_filas++;
    }

    if (cuenta_filas < columnas) return false;
    for (int suma : suma_col)
        if (suma < 1)
            return false;

    return true;
}

// Convertir conjunto a vector binario
vector<int> conjunto_a_vector(const Conjunto& c, int tamano) {
    vector<int> v(tamano, 0);
    for (int i : c)
        v[i] = 1;
    return v;
}

int main() {
    // MATRIZ BOOLEAN EJEMPLO
    Matriz MB = {
        {0, 0, 1, 1, 0, 1},
        {0, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0, 1}
    };

    imprimir_matriz(MB);

    int filas = MB.size();
    int columnas = MB[0].size();
    vector<Conjunto> activos;

    // Inicializar con columnas activas de la primera fila
    for (int j = 0; j < columnas; ++j) {
        if (MB[0][j] == 1) {
            Conjunto c = {j};
            activos.push_back(c);
        }
    }

    // Recorremos fila por fila
    for (int f = 1; f < filas; ++f) {
        vector<Conjunto> nuevos;
        vector<int> fila = MB[f];
        set<int> columnas_activas;
        for (int j = 0; j < columnas; ++j)
            if (fila[j] == 1)
                columnas_activas.insert(j);

        for (auto& conjunto : activos) {
            if (tiene_uno_en_fila(fila, conjunto)) {
                nuevos.push_back(conjunto);
            } else {
                for (int col : columnas_activas) {
                    if (conjunto.count(col)) continue;
                    Conjunto extendido = conjunto;
                    extendido.insert(col);
                    if (cumple_criterios(MB, extendido, f)) {
                        nuevos.push_back(extendido);
                    }
                }
            }
        }

        // Eliminar duplicados
        vector<Conjunto> sin_repe;
        for (auto& c : nuevos) {
            if (find(sin_repe.begin(), sin_repe.end(), c) == sin_repe.end()) {
                sin_repe.push_back(c);
            }
        }

        activos = sin_repe;
    }

    // Mostrar todos los conjuntos en una sola línea
    cout << "\n🟩 Conjuntos candidatos finales:\n";
    for (size_t i = 0; i < activos.size(); ++i) {
        cout << "{";
        for (auto it = activos[i].begin(); it != activos[i].end(); ++it) {
            cout << char('A' + *it);
            if (next(it) != activos[i].end()) cout << " ";
        }
        cout << "}";
        if (i != activos.size() - 1) cout << ", ";
    }
    cout << "\n\n";

    // Matriz binaria
    cout << "🟩 Matriz binaria de los conjuntos:\n";
    cout << "  ";
    for (int j = 0; j < columnas; ++j)
        cout << char('A' + j) << " ";
    cout << "\n";

    for (const auto& c : activos) {
        auto bin = conjunto_a_vector(c, columnas);
        cout << "  ";
        for (int b : bin)
            cout << b << " ";
        cout << "\n";
    }

    return 0;
}
