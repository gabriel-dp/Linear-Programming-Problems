#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

/*
 *  Elementos do problema -----------------------------
 */

struct item {
    int id, valor, peso;
};

int quantidade_itens, capacidade;
vector<item> itens;

void cplex() {
    IloEnv env;

    /*  --------------------------------------------------
     *  Variaveis de decisao -----------------------------
     */
    int numberVar = 0;

    // Uma variável binaria para cada mochila
    IloNumVarArray x(env);
    for (int i = 0; i < quantidade_itens; i++) {
        x.add(IloIntVar(env, 0, 1));
        numberVar++;
    }

    /*  ------------------------------------
     *  Modelo -----------------------------
     */
    IloModel model(env);
    IloExpr sum(env);
    IloExpr sum2(env);

    // Funcao objetivo -----------------------------
    // Maximizar o valor dos itens na mochila
    sum.clear();
    for (int i = 0; i < quantidade_itens; i++) {
        sum += (itens[i].valor * x[i]);
    }
    model.add(IloMaximize(env, sum));

    // Restricoes -----------------------------
    int numberRes = 0;

    // Limite de peso
    sum.clear();
    for (int i = 0; i < quantidade_itens; i++) {
        sum += (itens[i].peso * x[i]);
    }
    model.add(sum <= capacidade);
    numberRes++;

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
        for (int i = 0; i < quantidade_itens; i++) {
            value = IloRound(cplex.getValue(x[i]));
            printf("x[%d]: %.0lf\n", i, value);
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
    cin >> quantidade_itens >> capacidade;
    itens.resize(quantidade_itens);

    // N linhas dos itens
    for (int i = 0; i < quantidade_itens; i++) {
        cin >> itens[i].peso >> itens[i].valor;
        itens[i].id = i + 1;
    }

    // Solucao
    cplex();

    return 0;
}