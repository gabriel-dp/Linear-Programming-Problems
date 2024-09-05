#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

typedef struct {
    int custo, minino, maximo;
} Aresta;

int quantidade_vertices, quantidade_arestas;
unordered_map<int, string> vertice_label;
unordered_map<string, int> vertice_id;
map<int, map<int, Aresta>> grafo;
map<int, int> S, D;
set<int> T;

void cplex() {
    IloEnv env;

    /*  --------------------------------------------------
     *  Variaveis de decisao -----------------------------
     */
    int numberVar = 0;

    // Uma variável binaria para cada mochila
    IloArray<IloNumVarArray> x(env);
    for (int i = 0; i < quantidade_vertices; i++) {
        x.add(IloNumVarArray(env));

        auto destinos = grafo.find(i)->second;
        for (int j = 0; j < quantidade_vertices; j++) {
            if (destinos.find(j) != destinos.end()) {
                x[i].add(IloIntVar(env, 0, INT_MAX));
            } else {
                x[i].add(IloIntVar(env, INFINITY, INFINITY));
            }
            numberVar++;
        }
    }

    /*  ------------------------------------
     *  Modelo -----------------------------
     */
    IloModel model(env);
    IloExpr sum(env);
    IloExpr sum2(env);

    // Funcao objetivo -----------------------------
    // Minimizar custo total de transporte
    sum.clear();
    for (auto origem : grafo) {
        for (auto destino : origem.second) {
            sum += destino.second.custo * x[origem.first][destino.first];
        }
    }
    model.add(IloMaximize(env, sum));

    // Restricoes -----------------------------
    int numberRes = 0;

    // Oferta
    for (auto s : S) {
        sum.clear();
        sum2.clear();

        auto destinos_s = grafo.find(s.first)->second;
        for (auto destino : destinos_s) {
            sum += x[s.first][destino.first];
        }

        for (auto origem : grafo) {
            auto possivel_origem_s = origem.second.find(s.first);
            if (possivel_origem_s != origem.second.end()) {
                sum2 += x[possivel_origem_s->first][s.first];
            }
        }

        model.add(sum - sum2 <= s.second);
        numberRes++;
    }

    // Demanda
    for (auto d : D) {
        sum.clear();
        sum2.clear();

        auto destinos_d = grafo.find(d.first)->second;
        for (auto destino : destinos_d) {
            sum += x[d.first][destino.first];
        }

        for (auto origem : grafo) {
            auto possivel_origem_d = origem.second.find(d.first);
            if (possivel_origem_d != origem.second.end()) {
                sum2 += x[possivel_origem_d->first][d.first];
            }
        }

        model.add(sum - sum2 <= -d.second);
        numberRes++;
    }

    // Conservacao fluxo
    for (int t : T) {
        sum.clear();
        sum2.clear();

        auto destinos_t = grafo.find(t)->second;
        for (auto destino : destinos_t) {
            sum += x[t][destino.first];
        }

        for (auto origem : grafo) {
            auto possivel_origem_t = origem.second.find(t);
            if (possivel_origem_t != origem.second.end()) {
                sum2 += x[possivel_origem_t->first][t];
            }
        }

        model.add(sum - sum2 == 0);
        numberRes++;
    }

    // Capacidades
    for (auto origem : grafo) {
        for (auto destino : origem.second) {
            model.add(x[origem.first][destino.first] <= destino.second.maximo);
            numberRes++;
        }
    }

    cout << "\n\n"
         << model.getName() << "\n\n";

    /*  --------------------------------------
     *  Execucao -----------------------------
     */
    IloCplex cplex(model);

    time_t timer, timer2;
    IloNum value, objValue;
    double runTime;
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
         << endl
         << "Status da FO: " << status << endl;

    if (sol) {
        objValue = cplex.getObjValue();
        runTime = difftime(timer2, timer);

        cout << "Variaveis de decisao: " << endl;
        for (int i = 1; i <= quantidade_vertices; i++) {
            auto destinos = grafo.find(i)->second;
            for (int j = 1; j <= quantidade_vertices; j++) {
                if (destinos.find(j) != destinos.end()) {
                    value = IloRound(cplex.getValue(x[i - 1][j - 1]));
                    printf("x[%d][%d]: %.0lf\n", i, j, value);
                }
            }
        }

        printf("\n");

        cout << "Funcao Objetivo Valor = " << objValue << endl;
        printf("(%.6lf seconds)\n", runTime);
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

    for (int i = 0; i < quantidade_vertices; i++) {
        cout << vertice_label[i] << endl;
        for (auto destino : grafo[i]) {
            cout
                << "  -> "
                << vertice_label[destino.first]
                << " ("
                << destino.second.custo
                << ", "
                << destino.second.minino
                << ", "
                << destino.second.maximo
                << ")"
                << endl;
        }
    }
    cout << "\n";

    cout << "Origem -> ";
    for (auto s : S) {
        cout << vertice_label[s.first] << " ";
    }
    cout << endl;

    cout << "Transporte -> ";
    for (int t : T) {
        cout << vertice_label[t] << " ";
    }
    cout << endl;

    cout << "Destino -> ";
    for (auto d : D) {
        cout << vertice_label[d.first] << " ";
    }
    cout << endl;

    cplex();
}