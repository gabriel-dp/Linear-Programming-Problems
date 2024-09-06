#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

/*
 *  Elementos do problema -----------------------------
 */

int quantidade_pessoas, quantidade_tarefas;
vector<vector<int>> custos;

void cplex(){
    IloEnv env;

    /*  --------------------------------------------------
     *  Variaveis de decisao -----------------------------
     */
    /*

    *   Criação de um array de arrays (matriz bidimensional)
    *   Cada posição do array terá um outro array do tamanho da quantidade de tarefas
    */
    IloArray<IloIntVarArray> x(env, quantidade_pessoas);
    for (int i = 0; i < quantidade_pessoas; i++) {
        x[i] = IloIntVarArray(env, quantidade_tarefas, 0, 1);
    }

    /*  ------------------------------------
     *  Modelo -----------------------------
     */
    IloModel model(env);
    IloExpr sum(env);

    // Funcao objetivo -----------------------------
    // Minimizar o custo das alocacoes
    for(int i = 0; i < quantidade_pessoas; i++){
        for(int j = 0; j < quantidade_tarefas; j++){
            sum += x[i][j] * custos[i][j];
        }
    }

    model.add(IloMinimize(env, sum));

    // Restricoes -----------------------------

    // Tarefa designada
    for(int j = 0; j < quantidade_tarefas; j++) {
        IloExpr sum(env);
        for(int i = 0; i < quantidade_pessoas; i++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
    }

    // Pessoa designada
    for (int i = 0; i < quantidade_pessoas; i++) {
        IloExpr sum(env);
        for (int j = 0; j < quantidade_tarefas; j++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
    }

    /*  --------------------------------------
     *  Execucao -----------------------------
     */
    IloCplex cplex(model);

    time_t timer, timer2;
    IloNum value, objValue;
    double runTime;
    string status;

    time(&timer);
    cplex.solve();
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
        for (int i = 0; i < quantidade_pessoas; i++) {
            for (int j = 0; j < quantidade_tarefas; j++) {
                value = IloRound(cplex.getValue(x[i][j]));
                if (value > 0) {
                    printf("Pessoa %d atribuído à tarefa %d\n", i + 1, j + 1);
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
    model.end();

    env.end();
}

int main() {
    cin >> quantidade_pessoas >> quantidade_tarefas;
    custos.resize(quantidade_pessoas, vector<int>(quantidade_tarefas));

    for(int i = 0; i < quantidade_pessoas; i++){
        for(int j = 0; j < quantidade_tarefas; j++){
            cin >> custos[i][j];
        }
    }

    // Solucao
    cplex();

    return 0;
}