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

    // cplex();
}