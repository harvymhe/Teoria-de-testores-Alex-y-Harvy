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
    if (M.empty()) {
        cout << "Matriz vacia.\n";
        return;
    }

    int filas = M.size();
    int columnas = M[0].size();

    cout << "\n     ";  // espacio para el índice de filas
    for (int j = 1; j <= columnas; ++j)
        cout << setw(3) << j;
    cout << "\n";

    for (int i = 0; i < filas; ++i) {
        cout << setw(3) << i + 1 << ": ";
        for (int j = 0; j < columnas; ++j)
            cout << setw(3) << M[i][j];
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

int contar_unos(const vector<int>& fila) {
    return accumulate(fila.begin(), fila.end(), 0);
}

Matriz ordenar_filas_por_unos(const Matriz& M) {
    Matriz ordenada = M;
    sort(ordenada.begin(), ordenada.end(), [](const vector<int>& a, const vector<int>& b) {
        return contar_unos(a) < contar_unos(b);
    });
    return ordenada;
}

// ======================================================================
// ========================== OPERADORES ================================
// ======================================================================

Matriz phi(const Matriz& A, const Matriz& B) {
    if (A.size() != B.size()) {
        cerr << "Error: phi requiere que A y B tengan el mismo numero de filas.\n";
        return {};
    }

    Matriz resultado;
    for (size_t i = 0; i < A.size(); ++i) {
        vector<int> fila = A[i];
        fila.insert(fila.end(), B[i].begin(), B[i].end());
        resultado.push_back(fila);
    }
    return resultado;
}

Matriz theta(const Matriz& A, const Matriz& B) {
    Matriz resultado;
    for (const auto& filaA : A) {
        for (const auto& filaB : B) {
            vector<int> fila = filaA;
            fila.insert(fila.end(), filaB.begin(), filaB.end());
            resultado.push_back(fila);
        }
    }
    return resultado;
}

Matriz gamma(const Matriz& A, const Matriz& B) {
    size_t filasA = A.size(), filasB = B.size();
    size_t colsA = A[0].size(), colsB = B[0].size();
    size_t filas = filasA + filasB;
    size_t cols = colsA + colsB;

    Matriz resultado(filas, vector<int>(cols, 0));

    // Copiar A en la esquina superior izquierda
    for (size_t i = 0; i < filasA; ++i)
        for (size_t j = 0; j < colsA; ++j)
            resultado[i][j] = A[i][j];

    // Copiar B en la esquina inferior derecha
    for (size_t i = 0; i < filasB; ++i)
        for (size_t j = 0; j < colsB; ++j)
            resultado[filasA + i][colsA + j] = B[i][j];

    return resultado;
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
    int columnas = C.size();
    int cuenta_filas = 0;
    vector<int> suma_col(columnas, 0);

    for (int i = 0; i <= hastaFila; ++i) {
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

        int K = COLS + 1;
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

int main() {
    Matriz M, MB, ultimaGeneradaPotenciada;
    int op;

    while (true) {
        cout << "\n================ MENU PRINCIPAL ================\n";
        cout << "1) Generar matriz aleatoria y aplicar algoritmo\n";
        cout << "2) Usar Matriz A y aplicar algoritmo\n";
        cout << "3) Usar Matriz B y aplicar algoritmo\n";
        cout << "4) Operadores entre A y B\n";
        cout << "5) phi^n o gamma^n desde theta(A,B)\n";
        cout << "6) Aplicar YYC o BT sobre ultima generada\n";
        cout << "0) Salir\n";
        cout << "Ingrese opcion: ";
        cin >> op;

        if (op == 0) break;

        if (op >= 1 && op <= 3) {
            // ------------------ CREAR MATRIZ ------------------
            if (op == 1) {
                int filas, columnas;
                cout << "\nIngrese filas: "; cin >> filas;
                cout << "Ingrese columnas: "; cin >> columnas;

                random_device rd;
                mt19937 gen(rd());
                bernoulli_distribution dist(0.5);
                M = Matriz(filas, vector<int>(columnas));

                for (int i = 0; i < filas; ++i)
                    for (int j = 0; j < columnas; ++j)
                        M[i][j] = dist(gen);

                cout << "\nMatriz aleatoria generada:\n";
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
                cout << "\nMatriz A cargada:\n";
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
                cout << "\nMatriz B cargada:\n";
            }

            imprimir_matriz(M);

            // ------------------ CALCULAR MATRIZ BASICA ------------------
            MB = aMatrizBasica(M);
            cout << "\nMatriz Basica (MB):\n";
            imprimir_matriz(MB);
            cout << "Densidad: " << densidad(MB) << "\n";

            // ------------------ ALGORITMO ------------------
            cout << "\nSeleccione algoritmo:\n1) YYC\n2) BT\nOpcion: ";
            int alg;
            cin >> alg;

            cout << "\nOrdenamiento:\n1) Normal\n2) Ascendente por unos\nOpcion: ";
            int orden;
            cin >> orden;

            auto start = chrono::high_resolution_clock::now();

            Matriz MB_usada = (orden == 2) ? ordenar_filas_por_unos(MB) : MB;

            if (alg == 1) {
                auto testores = YYC(MB_usada);
                auto end = chrono::high_resolution_clock::now();
                auto dur = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                cout << "\nTestores YYC:\n";
                for (auto& C : testores) {
                    vector<int> bin = conjunto_a_vector_binario(C, MB_usada[0].size());
                    for (int b : bin) cout << b << " ";
                    cout << "\n";
                }
                cout << "Cantidad: " << testores.size() << "\n";
                cout << "Tiempo: " << dur << " ms\n";
            } else {
                auto testores = BT(MB_usada);
                auto end = chrono::high_resolution_clock::now();
                auto dur = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                cout << "\nTestores BT:\n";
                for (auto& fila : testores) {
                    for (int x : fila) cout << x << " ";
                    cout << "\n";
                }
                cout << "Cantidad: " << testores.size() << "\n";
                cout << "Tiempo: " << dur << " ms\n";
            }
            continue;
        }

		else if (op == 4) {
    		cout << "\n===== APLICAR OPERADORES ENTRE MATRICES A y B =====\n";

    		Matriz MB_A = {
        	{0,0,1,1,0,1},
        	{0,1,0,1,1,1},
        	{1,1,1,1,0,0},
        	{1,0,0,1,0,1}
        	
    	};

    		Matriz MB_B = {
        	{1,1,1,0,0,0},
        	{1,0,1,0,1,0},
        	{0,0,0,1,0,0},
        	{1,0,0,0,0,1},
        	{0,1,0,0,0,1},
        	{0,0,1,0,0,1}
    	};

    	cout << "Seleccione el operador a aplicar entre A y B:\n";
    	cout << "1) theta(A, B)\n";
    	cout << "2) phi(A, B)\n";
    	cout << "3) gamma(A, B)\n";
    	cout << "Opcion: ";

    	int opOperador;
    	cin >> opOperador;

   		Matriz resultado;

    	if (opOperador == 1) resultado = theta(MB_A, MB_B);
    	else if (opOperador == 2) resultado = phi(MB_A, MB_B);
    	else if (opOperador == 3) resultado = gamma(MB_A, MB_B);
    	else {
        	cout << "Opción invalida.\n";
        	continue;
    	}

    	cout << "\nMatriz resultado del operador seleccionado:\n";
    	imprimir_matriz(resultado);
    	cout << "Filas: " << resultado.size()
         	<< ", Columnas: " << (resultado.empty() ? 0 : resultado[0].size()) << "\n";
        	continue;
	}	
		else if (op == 5) {
    		cout << "\n===== GENERAR phi^n o gamma^n A PARTIR DE theta(A, B) =====\n";

    		Matriz MB_A = {
        		{0,0,1,1,0,1},
        		{0,1,0,1,1,1},
        		{1,1,1,1,0,0},
        		{1,0,0,1,0,1}
    		};

    		Matriz MB_B = {
       		 	{1,1,1,0,0,0},
        		{1,0,1,0,1,0},
        		{0,0,0,1,0,0},
        		{1,0,0,0,0,1},
        		{0,1,0,0,0,1},
        		{0,0,1,0,0,1}
    		};

    		Matriz base = theta(MB_A, MB_B);
    		cout << "theta(A, B):\n";
    		imprimir_matriz(base);

    		cout << "\nSeleccione operador:\n1) phi\n2) gamma\nOpcion: ";
    		int opOp;
    		cin >> opOp;

    		cout << "Ingrese el valor de n (potencia del operador): ";
    		int n;
    		cin >> n;

    		Matriz resultado = base;

    		for (int i = 1; i <= n; ++i) {
        		if (opOp == 1)
            		resultado = phi(resultado, resultado);
        		else if (opOp == 2)
            		resultado = gamma(resultado, resultado);
    		}

    		cout << "\nResultado final del operador aplicado " << n << " veces:\n";
    		imprimir_matriz(resultado);
    		cout << "Filas: " << resultado.size()
         		<< ", Columnas: " << (resultado.empty() ? 0 : resultado[0].size()) << "\n";
         	 	ultimaGeneradaPotenciada = resultado;	
         		continue;
         }
         
         // ------------------------------------------------------
        // 6. Aplicar YYC o BT
        // ------------------------------------------------------
        
		
		else if (op == 6) {
    		if (ultimaGeneradaPotenciada.empty()) {
        		cout << "\nNo hay una matriz generada aún. Usa la opción 5 primero.\n";
        		continue;
    		}

    		cout << "\n===== APLICAR YYC o BT SOBRE LA ÚLTIMA MATRIZ GENERADA =====\n";

    		imprimir_matriz(ultimaGeneradaPotenciada);
    		cout << "Filas: " << ultimaGeneradaPotenciada.size()
        		<< ", Columnas: " << (ultimaGeneradaPotenciada.empty() ? 0 : ultimaGeneradaPotenciada[0].size()) << "\n";

    		// Escoger algoritmo
    		cout << "\nSeleccione algoritmo:\n";
    		cout << "1) YYC\n";
    		cout << "2) BT\n";
    		cout << "Opción: ";
   		 	int alg;
    		cin >> alg;

    		// Escoger tipo de ordenamiento
    		cout << "\nTipo de ordenamiento:\n";
    		cout << "1) Orden normal\n";
    		cout << "2) Orden ascendente por número de unos\n";
    		cout << "Opción: ";
    		int orden;
    		cin >> orden;

    		// Preparar matriz básica
    		Matriz MB = aMatrizBasica(ultimaGeneradaPotenciada);

    		if (MB.empty()) {
        		cout << "\nLa matriz básica resultó vacía. No se puede continuar.\n";
        		continue;
    		}

    		// Iniciar cronómetro desde antes del ordenamiento
    		auto start = std::chrono::high_resolution_clock::now();

    		Matriz MB_usada = (orden == 2) ? ordenar_filas_por_unos(MB) : MB;

    		if (alg == 1) {
        		auto testores = YYC(MB_usada);

        		auto end = std::chrono::high_resolution_clock::now();
        		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        		cout << "Testores YYC:\n";
        		for (auto& C : testores) {
            		vector<int> bin = conjunto_a_vector_binario(C, MB_usada[0].size());
            		for (int b : bin) cout << b << " ";
            		cout << "\n";
        		}

        		cout << "Número de testores típicos encontrados: " << testores.size() << "\n";
        		cout << "Tiempo de ejecución total: " << duration << " ms\n";

    	} else if (alg == 2) {
        	auto testores = BT(MB_usada);

        	auto end = std::chrono::high_resolution_clock::now();
        	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        	cout << "Testores BT:\n";
        	for (auto& fila : testores) {
           		for (int x : fila) cout << x << " ";
            	cout << "\n";
        	}

        		cout << "Número de testores típicos encontrados: " << testores.size() << "\n";
        		cout << "Tiempo de ejecución total: " << duration << " ms\n";
        		continue;
    }
}

        // ------------------------------------------------------
        // Opción inválida
        // ------------------------------------------------------
        else {
            cout << "\n Opcion invalida.\n";
            continue;
        }
    }
	
    return 0;
}