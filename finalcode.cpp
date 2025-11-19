#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>
#include <numeric>

using namespace std;

// ======================================================================
// ========================== TIPOS Y UTILIDADES ========================
// ======================================================================

typedef vector<vector<int>> Matriz;
typedef set<int> Conjunto;

// Convierte un conjunto en una fila binaria
vector<int> conjunto_a_vector_binario(const Conjunto& C, int columnas) {
    vector<int> v(columnas, 0);
    for (int x : C) v[x] = 1;
    return v;
}

// Imprimir matriz
void imprimir_matriz(const Matriz& M) {
    if (M.empty()) { cout << "Matriz vacia.\n"; return; }

    int filas = M.size();
    int columnas = M[0].size();

    cout << "\n   ";
    for (int j = 0; j < columnas; ++j)
        cout << setw(2) << char('A' + j);
    cout << "\n";

    for (int i = 0; i < filas; ++i) {
        cout << setw(2) << i + 1 << ": ";
        for (int j = 0; j < columnas; ++j)
            cout << setw(2) << M[i][j];
        cout << "\n";
    }
}

// Densidad
double densidad(const Matriz& M) {
    if (M.empty() || M[0].empty()) return 0;
    int total = M.size() * M[0].size();
    int unos = 0;

    for (auto& fila : M)
        for (int v : fila)
            unos += v;

    return double(unos) / total;
}

// Subfila
bool esSubfila(const vector<int>& r, const vector<int>& s) {
    bool menorEstricto = false;
    for (size_t i = 0; i < r.size(); ++i) {
        if (r[i] > s[i]) return false;
        if (r[i] < s[i]) menorEstricto = true;
    }
    return menorEstricto;
}

// Eliminar filas duplicadas
Matriz filasUnicas(const Matriz& M) {
    set<string> visto;
    Matriz out;

    for (const auto& fila : M) {
        string clave;
        for (int v : fila) clave += char('0' + v);

        if (!visto.count(clave)) {
            visto.insert(clave);
            out.push_back(fila);
        }
    }
    return out;
}

// Matriz Basica
Matriz aMatrizBasica(const Matriz& M) {
    Matriz U = filasUnicas(M);
    int r = U.size();

    vector<char> keep(r, 1);

    for (int i = 0; i < r; i++)
        for (int j = 0; j < r; j++)
            if (i != j && esSubfila(U[i], U[j]))
                keep[i] = 0;

    Matriz BM;
    for (int i = 0; i < r; i++)
        if (keep[i]) BM.push_back(U[i]);

    return BM;
}

// Contar unos y ordenar filas (usado para probar YYC con reorden)
int contar_unos(const vector<int>& fila) {
    return accumulate(fila.begin(), fila.end(), 0);
}

Matriz ordenar_filas_por_unos(const Matriz& M) {
    Matriz ordenada = M;
    sort(ordenada.begin(), ordenada.end(), [](const vector<int>& a, const vector<int>& b) {
        return contar_unos(a) < contar_unos(b); // ascendente; empates arbitrarios
    });
    return ordenada;
}

// ======================================================================
// ============================== YYC ==================================
// ======================================================================

bool tiene_uno_en_fila(const vector<int>& fila, const Conjunto& conjunto) {
    for (int c : conjunto)
        if (fila[c] == 1)
            return true;
    return false;
}

bool cumple_criterios(const Matriz& MB, const Conjunto& C, int hastaFila) {
    int filas = hastaFila + 1;
    int columnas = C.size();

    int cuenta_filas = 0;
    vector<int> suma_col(columnas, 0);

    for (int i = 0; i < filas; ++i) {
        int suma = 0;
        int j = 0;
        for (int col : C) {
            suma += MB[i][col];
            suma_col[j++] += MB[i][col];
        }
        if (suma == 1) cuenta_filas++;
    }

    if (cuenta_filas < columnas) return false;
    for (int v : suma_col)
        if (v < 1) return false;

    return true;
}

vector<Conjunto> YYC(const Matriz& MB) {
    int filas = MB.size();
    int columnas = MB[0].size();

    vector<Conjunto> activos;

    auto inicio = chrono::high_resolution_clock::now();

    for (int j = 0; j < columnas; j++)
        if (MB[0][j] == 1)
            activos.push_back({j});

    for (int f = 1; f < filas; f++) {
        vector<Conjunto> nuevos;
        const vector<int>& fila = MB[f];

        set<int> columnasActivas;
        for (int j = 0; j < columnas; j++)
            if (fila[j] == 1)
                columnasActivas.insert(j);

        for (auto& C : activos) {

            if (tiene_uno_en_fila(fila, C)) {
                nuevos.push_back(C);
            } else {
                for (int col : columnasActivas) {
                    if (C.count(col)) continue;

                    Conjunto ext = C;
                    ext.insert(col);

                    if (cumple_criterios(MB, ext, f))
                        nuevos.push_back(ext);
                }
            }
        }

        vector<Conjunto> unicos;
        set<Conjunto> vistos;

        for (auto &c : nuevos) {
            if (!vistos.count(c)) {
                vistos.insert(c);
                unicos.push_back(c);
            }
        }

        activos = unicos;

        auto ahora = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(ahora - inicio).count();

        cout << "\n Tiempo acumulado hasta fila " << f + 1 << ": "
             << fixed << setprecision(3) << ms << " ms\n";
    }

    return activos;
}

// ======================================================================
// ============================== BT ===================================
// ======================================================================
// ==> MISMA LOGICA EXACTA QUE TU BT (con fixes anti-ciclo) <==

vector<vector<int>> BT(const Matriz& MB) {

    int ROWS = MB.size();
    int COLS = MB[0].size();

    vector<vector<int>> testores;
    long long total = (1LL << COLS);

    auto inicio = chrono::high_resolution_clock::now();

    auto es_testor = [&](const vector<int>& nuplo) {
        for (int i = 0; i < ROWS; i++) {
            bool tiene_uno = false;
            for (int j = 0; j < COLS; j++) {
                if (nuplo[j] == 1 && MB[i][j] == 1) {
                    tiene_uno = true;
                    break;
                }
            }
            if (!tiene_uno) return false;
        }
        return true;
    };

    auto es_subtestor = [&](const vector<int>& cand) {
        for (auto& t : testores) {
            bool es_sub = true;
            for (int j = 0; j < COLS; j++) {
                if (t[j] == 1 && cand[j] == 0) {
                    es_sub = false;
                    break;
                }
            }
            if (es_sub) return true;
        }
        return false;
    };

    auto encontrar_ultimo_1 = [&](const vector<int>& b) {
        for (int i = COLS - 1; i >= 0; i--)
            if (b[i] == 1) return i + 1;
        return 0;
    };

    auto encontrar_K = [&](const vector<int>& nuplo) {
        vector<int> filas_ceros;
        for (int i = 0; i < ROWS; i++) {
            bool fila_c = true;
            for (int j = 0; j < COLS; j++) {
                if (nuplo[j] == 1 && MB[i][j] == 1) {
                    fila_c = false;
                    break;
                }
            }
            if (fila_c) filas_ceros.push_back(i);
        }

        int K = COLS + 1; // empezar con el valor maximo
        for (int f : filas_ceros) {
            int ultima_pos = -1;
            for (int j = COLS - 1; j >= 0; j--) {
                if (MB[f][j] == 1) {
                    ultima_pos = j + 1;
                    break;
                }
            }
            if (ultima_pos != -1)
                K = min(K, ultima_pos);
        }

        return K;
    };

    auto avanzar_desde_K = [&](const vector<int>& b, int K) {
        vector<int> nuevo(COLS, 0);
        for (int i = 0; i < COLS; i++) {
            if (i < K - 1) nuevo[i] = b[i];
            else if (i == K - 1) nuevo[i] = 1;
            else nuevo[i] = 0;
        }
        return nuevo;
    };

    vector<int> b(COLS, 0);
    b[COLS - 1] = 1;

    while (true) {

        long long val = 0;
        for (int i = 0; i < COLS; i++)
            val = (val << 1) | b[i];

        if (val >= total) break;

        vector<int> prev = b;

        if (es_testor(b)) {

            if (!es_subtestor(b))
                testores.push_back(b);

            int k = encontrar_ultimo_1(b);
            long long salto = (1LL << (COLS - k)) - 1;

            if (salto <= 0) salto = 1;

            long long nuevo_val = val + salto;
            if (nuevo_val >= total) break;

            vector<int> next(COLS);
            for (int i = COLS - 1; i >= 0; i--) {
                next[i] = nuevo_val & 1;
                nuevo_val >>= 1;
            }
            b = next;
        }
        else {
            int k = encontrar_K(b);
            b = avanzar_desde_K(b, k);
        }

        if (b == prev) {
            long long nuevo_val = val + 1;
            if (nuevo_val >= total) break;

            for (int i = COLS - 1; i >= 0; i--) {
                b[i] = nuevo_val & 1;
                nuevo_val >>= 1;
            }
        }
    }

    auto fin = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(fin - inicio).count();

    cout << "\n Tiempo total algoritmo BT: "
         << fixed << setprecision(3) << ms << " ms\n";

    return testores;
}

// ======================================================================
// ================================ MAIN ================================
// ======================================================================

int main() {
    Matriz M;
    Matriz MB;

    while (true) {
        cout << "\n================ MENU PRINCIPAL ================\n";
        cout << "1) Generar matriz booleana aleatoria\n";
        cout << "2) Generar matriz A\n";
        cout << "3) Utilizar matriz B\n";
        cout << "0) Salir\n";
        cout << "Ingrese opcion: ";

        int op;
        cin >> op;

        if (op == 0) break;

        if (op == 1) {
            int filas, columnas;
            cout << "\nIngrese filas: ";
            cin >> filas;
            cout << "Ingrese columnas: ";
            cin >> columnas;

            random_device rd;
            mt19937 gen(rd());
            bernoulli_distribution dist(0.5);

            M = Matriz(filas, vector<int>(columnas));

            for (int i = 0; i < filas; i++)
                for (int j = 0; j < columnas; j++)
                    M[i][j] = dist(gen);

            cout << "\n Matriz generada:\n";
            imprimir_matriz(M);
        }

        else if (op == 2) {
            M = {
                {0,0,0,0,0,0},
                {0,0,1,1,0,1},
                {0,1,0,1,1,1},
                {0,0,0,1,1,0},
                {0,1,1,0,0,0},
                {1,0,0,1,0,0},
                {1,1,1,1,0,0},
                {0,1,0,0,0,1},
                {1,0,0,1,0,1},
                {0,0,1,1,0,1}
            };

            cout << "\n Matriz cargada:\n";
            imprimir_matriz(M);
        }
        else if (op == 3) {
            M = {
                {1,1,1,0,0,0},
                {1,0,1,0,1,0},
                {0,0,0,1,0,0},
                {1,0,0,0,0,1},
                {0,1,0,0,0,1},
                {0,0,1,0,0,1}
            };

            cout << "\n Matriz B cargada:\n";
            imprimir_matriz(M);
        }
        else {
            cout << "\n Opcion invalida. Intente nuevamente.\n";
            continue;
        }

        MB = aMatrizBasica(M);

        cout << "\n MATRIZ BASICA:\n";
        imprimir_matriz(MB);

        cout << "Densidad: " << densidad(MB) << "\n";

        bool usarPredeterminadoBT = (op == 3);
        cout << "\n===== Algoritmo a ejecutar =====\n";
        cout << "1) YYC\n";
        if (usarPredeterminadoBT) {
            cout << "2) BT (predeterminado)\n";
            cout << "Opcion [2]: ";
        } else {
            cout << "2) BT\n";
            cout << "Opcion: ";
        }

        int alg = 0;
        if (usarPredeterminadoBT) {
            alg = 2;
            string entrada_alg;
            getline(cin >> ws, entrada_alg);
            if (!entrada_alg.empty()) {
                try { alg = stoi(entrada_alg); }
                catch (...) { alg = 2; }
            }
        } else {
            cin >> alg;
        }

        if (alg == 1) {

            cout << "\n Ejecutando algoritmo YYC...\n";
            cout << "Elija modo de ejecucion:\n";
            cout << "1) Orden original\n";
            cout << "2) Filas ordenadas por cantidad de unos\n";
            cout << "3) Ambos\n";
            cout << "Opcion [3]: ";
            int modo = 3;
            string entrada_modo;
            getline(cin >> ws, entrada_modo);
            if (!entrada_modo.empty()) {
                try { modo = stoi(entrada_modo); }
                catch (...) { modo = 3; }
            }
            if (modo < 1 || modo > 3) modo = 3;

            auto ejecutaYYC = [&](const Matriz& mat, const string& titulo) {
                cout << "\n--- " << titulo << " ---\n";
                auto inicio = chrono::high_resolution_clock::now();
                auto testores = YYC(mat);
                auto fin = chrono::high_resolution_clock::now();
                double ms = chrono::duration<double, milli>(fin - inicio).count();
                cout << "\nTiempo YYC (" << titulo << "): "
                     << fixed << setprecision(3) << ms << " ms\n";
                cout << "Testores YYC (" << titulo << "):\n";
                for (auto& C : testores) {
                    vector<int> bin = conjunto_a_vector_binario(C, mat[0].size());
                    for (int b : bin) cout << b << " ";
                    cout << "\n";
                }
                cout << "\n";
            };

            if (modo == 1 || modo == 3) {
                ejecutaYYC(MB, "orden original");
            }
            if (modo == 2 || modo == 3) {
                Matriz MB_ordenada = ordenar_filas_por_unos(MB);
                ejecutaYYC(MB_ordenada, "filas ordenadas por cantidad de unos");
            }
        }
        else {

            cout << "\n Ejecutando algoritmo BT...\n";

            auto testoresBT = BT(MB);

            cout << "\n Testores BT (matriz binaria):\n";
            for (auto& fila : testoresBT) {
                for (int x : fila) cout << x << " ";
                cout << "\n";
            }
            cout << "\n";
        }
    }

    return 0;
}
