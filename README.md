# Projeto Prático - Estrutura de Dados (FATEC Ribeirão Preto, ADS 3º sem.)

**Tema escolhido:** C - Biblioteca (livros e relacionamentos)  
**Linguagem:** C  **Aluna:**  Débora Santos  

Aplicação de terminal que integra TADs, ponteiros, lista encadeada, pilha, fila,
ordenação (Merge Sort próprio), BST indexada por ID e grafo não direcionado.

## Compilação, execução e testes

```bash
make            # gera bin/biblioteca
./bin/biblioteca
make test       # 120 testes reproduzíveis (entrada, operação, esperado, obtido)
```

Sem `make`: `gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude -o bin/biblioteca src/*.c`
(para o programa principal, use todos os `.c` de `src/`).

## Estrutura

| Arquivo | Responsabilidade |
|---|---|
| `registro.h` | struct `Livro` (id, título, autor, ano; textos em `char[80]`) |
| `entrada.h/.c` | leitura segura (`fgets` + `strtol`), limpeza de buffer, validações |
| `bst.h/.c` | BST por ID: inserir, buscar, remover (0/1/2 filhos), em-ordem, menor/maior, altura |
| `grafo.h/.c` | matriz de adjacência + mapeamento ID→índice compacto, capacidade 100, BFS, caminho |
| `lista.h/.c` | lista encadeada com inserção no fim (ordem da BFS) |
| `fila.h/.c` | fila encadeada (BFS) |
| `pilha.h/.c` | pilha encadeada (histórico), consulta não destrutiva das 5 últimas |
| `ordenacao.h/.c` | Merge Sort próprio por título (`strcmp`), desempate por ID |
| `sistema.h/.c` | integração BST + grafo + histórico com rollback |
| `main.c` | menu |
| `tests/test_main.c` | testes |

## Estratégia de consistência / rollback

- **Cadastro:** valida campos → insere na BST → cria vértice → empilha no histórico.
  Se o vértice falhar (grafo cheio com 100 vértices, falha de alocação), a inserção na BST é
  desfeita (`bst_remover`); se o `push` falhar, desfaz vértice e BST. Nenhum registro parcial fica.
- **Remoção:** verifica existência (ID inexistente não altera nada) → guarda a lista de vizinhos →
  remove vértice (e arestas incidentes) → remove da BST. Se a remoção na BST falhar, o vértice é
  recriado e as arestas restauradas a partir dos vizinhos salvos.
- O grafo usa apenas **IDs** (nunca ponteiros para nós da BST).
- Falhas são simuladas nos testes com os ganchos `simular_falha_vertice` e
  `simular_falha_remocao_bst` de `Sistema`.
- Histórico: só operações concluídas com sucesso (cadastro, remoção, add/rem conexão).

## Complexidade (V = vértices, n = registros, h = altura da BST)

| Operação | Tempo | Observação |
|---|---|---|
| BST inserir/buscar/remover | O(h) | melhor/médio O(log n); pior O(n) se degenerar (IDs ordenados) |
| BST em-ordem / cópia | O(n) | |
| Mapeamento ID→índice | O(V) | busca linear em até 100 slots |
| BFS (matriz) | O(V²) | + O(V²) para ordenar índices por ID (inserção) |
| Ordenação (Merge Sort) | O(n log n) em todos os casos | espaço O(n) auxiliar; estável |
| Pilha/Fila/Lista (fim) | O(1) | lista mantém ponteiro para o fim |
| Espaço do grafo | O(V²) | 100×100 bytes |

## Declaração de uso de IA

Utilizado ChatGPT (OpenAI) e Claude (Anthropic) em alguns pontos onde surgiram dúvidas ou erros de execução.
