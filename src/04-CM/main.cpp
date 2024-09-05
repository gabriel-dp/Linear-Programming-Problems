#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

typedef struct {
    int custo;
} Aresta;

int quantidade_vertices, quantidade_arestas;
int inicio, fim;
unordered_map<int, string> vertice_label;
unordered_map<string, int> vertice_id;
unordered_map<int, unordered_map<int, Aresta>> grafo;
unordered_map<int, int> S, D;

void cplex() {
    IloEnv env;

    /*  --------------------------------------------------
     *  Variaveis de decisao -----------------------------
     */

    // Uma variável para cada quantidade de carga transportada pela aresta
    IloArray<IloNumVarArray> x(env);
    for (int i = 0; i < quantidade_vertices; i++) {
        x.add(IloNumVarArray(env));
        for (int j = 0; j < quantidade_vertices; j++) {
            x[i].add(IloIntVar(env, 0, 1));
        }
    }

    /*  ------------------------------------
     *  Modelo -----------------------------
     */
    IloModel model(env);
    IloExpr sum(env), sum2(env);

    // Funcao objetivo -----------------------------
    // Minimizar custo total do caminho
    sum.clear();
    for (auto origem : grafo) {
        for (auto destino : origem.second) {
            sum += destino.second.custo * x[origem.first][destino.first];
        }
    }
    model.add(IloMinimize(env, sum));

    // Restricoes -----------------------------

    // Conservacao do caminho
    for (int i = 0; i < quantidade_vertices; i++) {
        sum.clear();
        sum2.clear();

        for (auto destinos : grafo[i]) {
            sum += x[i][destinos.first];
        }

        for (auto origem : grafo) {
            if (origem.second.find(i) != origem.second.end()) {
                sum2 += x[origem.first][i];
            }
        }

        if (i == inicio) {
            model.add(sum - sum2 == 1);
        } else if (i == fim) {
            model.add(sum - sum2 == -1);
        } else {
            model.add(sum - sum2 == 0);
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

    // Uma vertice por linha
    for (int i = 0; i < quantidade_vertices; i++) {
        cin >> vertice_label[i];
        vertice_id[vertice_label[i]] = i;
    }

    // Origem e destino
    string origem, destino;
    cin >> origem >> destino;
    inicio = vertice_id[origem];
    fim = vertice_id[destino];

    // Um destino por linha
    string v1,
        v2;
    for (int i = 0; i < quantidade_arestas; i++) {
        cin >> v1 >> v2;

        Aresta aresta;
        cin >> aresta.custo;
        grafo[vertice_id[v1]][vertice_id[v2]] = aresta;
        grafo[vertice_id[v2]][vertice_id[v1]] = aresta;
    }

    cplex();
}