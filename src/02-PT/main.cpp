#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

typedef struct {
    int custo;
} Aresta;

int quantidade_origens, quantidade_destinos;
unordered_map<int, string> origem_label, destino_label;
unordered_map<int, unordered_map<int, Aresta>> grafo;
unordered_map<int, int> S, D;

void cplex() {
    IloEnv env;

    /*  --------------------------------------------------
     *  Variaveis de decisao -----------------------------
     */

    // Uma variável para cada quantidade de carga transportada pela aresta
    IloArray<IloNumVarArray> x(env);
    for (int i = 0; i < quantidade_origens; i++) {
        x.add(IloNumVarArray(env));
        for (int j = 0; j < quantidade_destinos; j++) {
            x[i].add(IloIntVar(env, 0, INT_MAX));
        }
    }

    /*  ------------------------------------
     *  Modelo -----------------------------
     */
    IloModel model(env);
    IloExpr sum(env);

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

        for (auto destino : grafo.find(s.first)->second) {
            sum += x[s.first][destino.first];
        }

        model.add(sum <= s.second);
    }

    // Demanda
    for (auto d : D) {
        sum.clear();

        for (auto origem : grafo) {
            if (origem.second.find(d.first) != origem.second.end()) {
                sum += x[origem.first][d.first];
            }
        }

        model.add(sum == d.second);
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
                    << origem_label[origem.first]
                    << destino_label[destino.first]
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
    model.end();

    env.end();
}

int main() {
    // Primeira linha
    cin >> quantidade_origens >> quantidade_destinos;

    // Uma origem por linha
    for (int i = 0; i < quantidade_origens; i++) {
        cin >> origem_label[i];
        cin >> S[i];
    }

    // Um destino por linha
    for (int i = 0; i < quantidade_destinos; i++) {
        cin >> destino_label[i];
        cin >> D[i];
    }

    // Todas as arestas que saem de uma origem por linha
    for (int i = 0; i < quantidade_origens; i++) {
        for (int j = 0; j < quantidade_destinos; j++) {
            Aresta caminho;
            cin >> caminho.custo;
            grafo[i][j] = caminho;
        }
    }

    cplex();
}