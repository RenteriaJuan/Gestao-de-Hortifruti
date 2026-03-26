#ifndef ESTOQUE_CABECALHO_H_INCLUDED
#define ESTOQUE_CABECALHO_H_INCLUDED

/*
 * Módulo de Gestão de Estoque
 * Hortifruti - Sistema de Gerenciamento
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#define MAX_PRODUTOS      100
#define ARQUIVO_ESTOQUE   "estoque.txt"
#define ARQUIVO_DESCARTADOS "descartados.txt"

struct Produto {
    int   id;
    char  nome[50];
    int   quantidade;
    float preco;
};

/* Protótipos */
void salvarDescartados(struct Produto descartados[], int contador);
void carregarDescartados(struct Produto descartados[], int *contador);
void descartadosProduto(struct Produto descartados[], int contador);
void adicionarProduto(struct Produto estoque[], int *contador);
void listarProdutos(struct Produto estoque[], int contador);
void atualizarQuantidade(struct Produto estoque[], int contador);
void removerProduto(struct Produto estoque[], int *contador);
void descartarProduto(struct Produto estoque[], int *contador, struct Produto descartados[], int *contadorDescartados);
void carregarEstoque(struct Produto estoque[], int *contador);
void salvarEstoque(struct Produto estoque[], int contador);

/* ---------------------------------------------------------- */
/*  Menu principal do módulo de estoque (retorna void)        */
/* ---------------------------------------------------------- */
void estoque() {
    setlocale(LC_ALL, "portuguese");

    struct Produto descartados[MAX_PRODUTOS];
    int contadorDescartados = 0;
    struct Produto estoqueLocal[MAX_PRODUTOS];
    int contador = 0;
    char confirm;
    int opcao;

    carregarEstoque(estoqueLocal, &contador);
    carregarDescartados(descartados, &contadorDescartados);

    system("cls");
    do {
        printf("\n=== SISTEMA DE ESTOQUE ===\n");
        printf("1. Adicionar Produto\n");
        printf("2. Lista de Produtos\n");
        printf("3. Atualizar Quantidade\n");
        printf("4. Remover Produto\n");
        printf("5. Descartar Produto\n");
        printf("6. Produtos Descartados\n");
        printf("7. Sair\n");
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarProduto(estoqueLocal, &contador);
                system("pause"); system("cls");
                break;
            case 2:
                listarProdutos(estoqueLocal, contador);
                system("pause"); system("cls");
                break;
            case 3:
                atualizarQuantidade(estoqueLocal, contador);
                system("pause"); system("cls");
                break;
            case 4:
                removerProduto(estoqueLocal, &contador);
                salvarEstoque(estoqueLocal, contador);
                system("pause"); system("cls");
                break;
            case 5:
                /* BUG FIX: era salvarDescartados(descartadosProduto, ...) — ponteiro de função inválido */
                descartarProduto(estoqueLocal, &contador, descartados, &contadorDescartados);
                salvarDescartados(descartados, contadorDescartados);
                salvarEstoque(estoqueLocal, contador);
                system("pause"); system("cls");
                break;
            case 6:
                descartadosProduto(descartados, contadorDescartados);
                system("pause"); system("cls");
                break;
            case 7:
                salvarEstoque(estoqueLocal, contador);
                salvarDescartados(descartados, contadorDescartados);
                printf("Deseja realmente sair (S/N)? ");
                while ((confirm = _getch()) != 'S' && confirm != 's' && confirm != 'N' && confirm != 'n')
                    printf("Opção inválida, digite 'S' ou 'N': ");
                if (confirm == 'S' || confirm == 's')
                    printf("\nSaindo da gestão de estoque...\n");
                else
                    opcao = 0;
                system("pause"); system("cls");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 7);
    /* BUG FIX: versão original tinha "return 0" em função void */
}

/* ---------------------------------------------------------- */
/*  Persistência em arquivo                                   */
/* ---------------------------------------------------------- */

void carregarEstoque(struct Produto estoque[], int *contador) {
    FILE *file = fopen(ARQUIVO_ESTOQUE, "r");
    if (!file) {
        printf("Arquivo de estoque não encontrado. Será criado ao adicionar um produto.\n");
        return;
    }
    *contador = 0;
    while (fscanf(file, "%d;%49[^;];%d;%f\n",
                  &estoque[*contador].id,
                   estoque[*contador].nome,
                  &estoque[*contador].quantidade,
                  &estoque[*contador].preco) == 4) {
        (*contador)++;
        if (*contador >= MAX_PRODUTOS) break;
    }
    fclose(file);
}

void salvarEstoque(struct Produto estoque[], int contador) {
    FILE *file = fopen(ARQUIVO_ESTOQUE, "w");
    if (!file) { printf("Erro ao salvar o estoque.\n"); return; }
    for (int i = 0; i < contador; i++)
        fprintf(file, "%d;%s;%d;%.2f\n",
                estoque[i].id, estoque[i].nome, estoque[i].quantidade, estoque[i].preco);
    fclose(file);
}

void carregarDescartados(struct Produto descartados[], int *contador) {
    FILE *file = fopen(ARQUIVO_DESCARTADOS, "r");
    if (!file) {
        printf("Arquivo de descartados não encontrado. Será criado ao descartar um produto.\n");
        return;
    }
    *contador = 0;
    while (fscanf(file, "%d;%49[^;];%d;%f\n",
                  &descartados[*contador].id,
                   descartados[*contador].nome,
                  &descartados[*contador].quantidade,
                  &descartados[*contador].preco) == 4) {
        (*contador)++;
        if (*contador >= MAX_PRODUTOS) break;
    }
    fclose(file);
}

void salvarDescartados(struct Produto descartados[], int contador) {
    FILE *file = fopen(ARQUIVO_DESCARTADOS, "w");
    if (!file) { printf("Erro ao salvar os descartados.\n"); return; }
    for (int i = 0; i < contador; i++)
        fprintf(file, "%d;%s;%d;%.2f\n",
                descartados[i].id, descartados[i].nome, descartados[i].quantidade, descartados[i].preco);
    fclose(file);
}

/* ---------------------------------------------------------- */
/*  Operações de estoque                                      */
/* ---------------------------------------------------------- */

void adicionarProduto(struct Produto estoque[], int *contador) {
    if (*contador >= MAX_PRODUTOS) { printf("\nEstoque cheio!\n"); return; }

    struct Produto novo;
    novo.id = *contador + 1;

    printf("\nDigite o nome do produto: ");
    getchar();
    scanf("%[^\n]", novo.nome);

    /* Verifica duplicidade (case-insensitive) */
    for (int i = 0; i < *contador; i++) {
        if (_stricmp(estoque[i].nome, novo.nome) == 0) {
            printf("\nEsse produto já está cadastrado!\n");
            return;
        }
    }

    printf("Digite a quantidade: ");
    scanf("%d", &novo.quantidade);
    printf("Digite o preço de custo: ");
    scanf("%f", &novo.preco);

    /* Aplica margem de 20% sobre o preço de custo */
    novo.preco *= 1.20f;

    estoque[*contador] = novo;
    (*contador)++;
    printf("Produto adicionado com sucesso! (preço de venda com 20%% de margem: R$%.2f)\n", novo.preco);
}

void listarProdutos(struct Produto estoque[], int contador) {
    if (contador == 0) { printf("Nenhum produto no estoque.\n"); return; }

    printf("\n%-5s %-25s %-12s %-10s\n", "ID", "Nome", "Quantidade", "Preço");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < contador; i++) {
        printf("%-5d %-25s %-12d R$%.2f\n",
               estoque[i].id, estoque[i].nome, estoque[i].quantidade, estoque[i].preco);
    }
}

void atualizarQuantidade(struct Produto estoque[], int contador) {
    int id, novaQtd;
    printf("\nDigite o ID do produto: ");
    scanf("%d", &id);
    if (id < 1 || id > contador) { printf("Produto não encontrado!\n"); return; }
    printf("Nova quantidade: ");
    scanf("%d", &novaQtd);
    if (novaQtd < 0) { printf("Quantidade não pode ser negativa.\n"); return; }
    estoque[id - 1].quantidade = novaQtd;
    printf("Quantidade atualizada com sucesso!\n");
    salvarEstoque(estoque, contador);
}

void removerProduto(struct Produto estoque[], int *contador) {
    int id;
    printf("\nDigite o ID do produto a remover: ");
    scanf("%d", &id);
    if (id < 1 || id > *contador) { printf("\nProduto não encontrado!\n"); return; }

    /* Desloca os elementos para preencher a lacuna */
    for (int i = id - 1; i < *contador - 1; i++)
        estoque[i] = estoque[i + 1];
    (*contador)--;

    /* Reatribui IDs sequencialmente */
    for (int i = 0; i < *contador; i++)
        estoque[i].id = i + 1;

    printf("\nProduto removido com sucesso!\n");
}

void descartarProduto(struct Produto estoque[], int *contador,
                      struct Produto descartados[], int *contadorDescartados) {
    int id, qtdDescartar;

    printf("\nDigite o ID do produto a descartar: ");
    scanf("%d", &id);
    if (id < 1 || id > *contador) { printf("\nProduto não encontrado!\n"); return; }

    printf("Digite a quantidade a descartar: ");
    scanf("%d", &qtdDescartar);

    if (qtdDescartar <= 0) { printf("Quantidade inválida.\n"); return; }
    if (qtdDescartar > estoque[id - 1].quantidade) {
        printf("\nQuantidade a descartar maior que a disponível (%d unidades)!\n",
               estoque[id - 1].quantidade);
        return;
    }

    if (*contadorDescartados < MAX_PRODUTOS) {
        descartados[*contadorDescartados]            = estoque[id - 1];
        descartados[*contadorDescartados].quantidade = qtdDescartar;
        (*contadorDescartados)++;

        estoque[id - 1].quantidade -= qtdDescartar;

        /* Remove o produto do estoque somente se quantidade chegar a zero */
        if (estoque[id - 1].quantidade == 0)
            removerProduto(estoque, contador);

        /* BUG FIX: mensagem estava dentro do if (qtd==0), nunca aparecia para descartes parciais */
        printf("\nProduto descartado com sucesso! (%d unidades)\n", qtdDescartar);
    }
}

void descartadosProduto(struct Produto descartados[], int contador) {
    if (contador == 0) { printf("Nenhum produto descartado.\n"); return; }

    printf("\n*** Produtos Descartados ***\n\n");
    printf("%-5s %-25s %-12s %-10s\n", "ID", "Nome", "Quantidade", "Preço");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < contador; i++) {
        printf("%-5d %-25s %-12d R$%.2f\n",
               descartados[i].id, descartados[i].nome,
               descartados[i].quantidade, descartados[i].preco);
    }
}

#endif /* ESTOQUE_CABECALHO_H_INCLUDED */
