#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

#define INFINITO -1

using namespace std;
ILOSTLBEGIN

typedef struct {
    int custo, minino, maximo;
} Aresta;

int quantidade_vertices, quantidade_arestas;
unordered_map<int, string> vertice_label;
unordered_map<string, int> vertice_id;
unordered_map<int, unordered_map<int, Aresta>> grafo;
unordered_map<int, int> S, D;
unordered_set<int> T;

void cplex() {
    IloEnv env;

    /*  --------------------------------------------------
     *  Variaveis de decisao -----------------------------
     */

    // Uma variável para cada possivel aresta, mesmo que nao exista
    IloArray<IloNumVarArray> x(env);
    for (int i = 0; i < quantidade_vertices; i++) {
        x.add(IloNumVarArray(env));
        for (int j = 0; j < quantidade_vertices; j++) {
            x[i].add(IloIntVar(env, 0, INT_MAX));
        }
    }

    /*  ------------------------------------
     *  Modelo -----------------------------
     */
    IloModel model(env);
    IloExpr sum(env), sum2(env);

    // Funcao objetivo -----------------------------
    // Minimizar custo total de transporte
    sum.clear();
    for (auto origem : grafo) {
        for (auto destino : origem.second) {
            sum += destino.second.custo * x[origem.first][destino.first];
        }
    }
    model.add(IloMinimize(env, sum));

    // Restricoes -----------------------------

    // Oferta
    for (auto s : S) {
        sum.clear();
        sum2.clear();

        for (auto destino : grafo.find(s.first)->second) {
            sum += x[s.first][destino.first];
        }

        for (auto origem : grafo) {
            if (origem.second.find(s.first) != origem.second.end()) {
                sum2 += x[origem.first][s.first];
            }
        }

        model.add(sum - sum2 <= s.second);
    }

    // Demanda
    for (auto d : D) {
        sum.clear();
        sum2.clear();

        for (auto destino : grafo.find(d.first)->second) {
            sum += x[d.first][destino.first];
        }

        for (auto origem : grafo) {
            if (origem.second.find(d.first) != origem.second.end()) {
                sum2 += x[origem.first][d.first];
            }
        }

        model.add(sum - sum2 <= -d.second);
    }

    // Conservacao fluxo
    for (int t : T) {
        sum.clear();
        sum2.clear();

        for (auto destino : grafo.find(t)->second) {
            sum += x[t][destino.first];
        }

        for (auto origem : grafo) {
            if (origem.second.find(t) != origem.second.end()) {
                sum2 += x[origem.first][t];
            }
        }

        model.add(sum - sum2 == 0);
    }

    // Capacidades
    for (int i = 0; i < quantidade_vertices; i++) {
        for (int j = 0; j < quantidade_vertices; j++) {
            auto destino = grafo[i].find(j);
            if (destino != grafo[i].end() && destino->second.maximo != INFINITO) {
                model.add(x[i][destino->first] <= destino->second.maximo);
            }
        }
    }

    /*  --------------------------------------
     *  Execucao -----------------------------
     */
    IloCplex cplex(model);

    time_t timer, timer2;
    string status;

    time(&timer);
    cplex.solve();  // Run
    time(&timer2);

    /*  ----------------------------------------
     *  Resultados -----------------------------
     */

    bool sol = true;
    switch (cplex.getStatus()) {
        case IloAlgorithm::Optimal:
            status = "Optimal";
            break;
        case IloAlgorithm::Feasible:
            status = "Feasible";
            break;
        default:
            status = "No Solution";
            sol = false;
    }

    cout << endl
         << "Status da solucao: " << status << endl
         << endl;

    if (sol) {
        IloNum objValue = cplex.getObjValue();

        cout << "Variaveis de decisao: " << endl;
        for (auto origem : grafo) {
            for (auto destino : origem.second) {
                IloNum value = cplex.getValue(x[origem.first][destino.first]);
                cout
                    << "x"
                    << vertice_label[origem.first]
                    << vertice_label[destino.first]
                    << ": "
                    << value
                    << endl;
            }
        }

        cout << endl
             << "Valor da solucao = " << objValue << endl;

        std::cout << std::fixed;
        std::cout << std::setprecision(6);
        cout << "(" << difftime(timer2, timer) << " segundos)" << endl;
    }

    /*  -----------------------------------------
     *  Memory free -----------------------------
     */
    sum.end();
    sum2.end();
    model.end();

    env.end();
}

int main() {
    // Primeira linha
    cin >> quantidade_vertices >> quantidade_arestas;

    // Um vertice por linha
    string tipo;
    for (int i = 0; i < quantidade_vertices; i++) {
        cin >> vertice_label[i];
        vertice_id[vertice_label[i]] = i;

        cin >> tipo;
        if (tipo == "origem") {
            cin >> S[i];
        } else if (tipo == "destino") {
            cin >> D[i];
        } else {
            T.insert(i);
        }
    }

    // Uma aresta por linha
    string origem, destino;
    for (int i = 0; i < quantidade_arestas; i++) {
        Aresta aresta;
        cin >> origem >> destino >> aresta.custo >> aresta.minino >> aresta.maximo;

        grafo[vertice_id[origem]][vertice_id[destino]] = aresta;
        grafo[vertice_id[destino]];
    }

    cplex();
}