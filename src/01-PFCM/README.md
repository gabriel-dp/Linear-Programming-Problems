# 🔄 Problema do Fluxo de Custo Mínimo

## Entrada

```txt
9 14
F origem 10
G origem 10
H origem 10
M transporte
N transporte
P destino 8 
Q destino 7
R destino 6
S destino 9
F M 2 0 8
F P 7 0 -1
G M 5 0 7
G H 2 0 -1
H M 6 0 -1
H N 5 0 9
H S 4 0 6
M P 1 0 17
M Q 3 0 4
M R 4 0 -1
N S 3 0 -1
P Q 1 0 10
Q R 1 0 -1
R N 2 0 -1
```

## Saída

```txt
Variaveis de decisao: 
xRN: -0
xQR: 6
xNS: 3
xHS: 6
xHN: 3
xHM: 4
xGH: 3
xGM: 7
xPQ: 10
xMR: -0
xMQ: 3
xMP: 16
xFP: 2
xFM: 8

Valor da solucao = 184
```
