#ifndef VENDAS_H_INCLUDED
#define VENDAS_H_INCLUDED

#include <stdio.h>
#include <locale.h>
#include "Estoque_cabeçalho.h"

#define DIAS_DO_MES 30
#define MAX_PRODUTOS 100
#define COMPRAS_DO_MES "compras.txt"

typedef struct {
    int id;
    char nome[50];
    int quantidade;
    float preco;
} Produto;

typedef struct {
    float total;
} Venda;

void listarVendas(Venda vendas[], int tamanhovendas);
void salvarEstado(Venda vendas[], int tamanhovendas, int diaAtual);
void guardarCompraEmArquivo(Venda venda, Produto produtos[], int quantidades[], int tamanho);
void carregarEstoque(struct Produto produtos[], int *contador);
void calcularFaturamentoMensal(Venda vendas[], int tamanhovendas);
void calcularVendasSemana(Venda vendas[], int tamanhovendas);
void gerarCupom(Venda venda);
void exibirProdutos(Produto produtos[], int tamanho);
float registrarCompra(Produto produtos[], int tamanho, Venda vendas[], int *tamanhovendas);

// Função para salvar as vendas e o estado no arquivo compartilhado
void salvarEstado(Venda vendas[], int tamanhovendas, int diaAtual) {
    FILE *file = fopen("vendas_do_mes.txt", "w");
    if (!file) {
        printf("Erro ao abrir arquivo para salvar o estado.\n");
        return;
    }

    // Salva o dia atual
    fprintf(file, "%d\n", diaAtual);

    // Salva todas as vendas
    for (int i = 0; i < tamanhovendas; i++) {
        fprintf(file, "%.2f\n", vendas[i].total);
    }

    fclose(file);
}

// Função para carregar o estado (vendas e dia atual) do arquivo
void carregarEstado(Venda vendas[], int *tamanhovendas, int *diaAtual) {
    FILE *file = fopen("vendas_do_mes.txt", "r");
    if (!file) {
        printf("Nenhum estado anterior encontrado. Começando do dia 1.\n");
        *diaAtual = 0;
        *tamanhovendas = 0;
        return;
    }

    // Carrega o dia atual
    fscanf(file, "%d\n", diaAtual);

    // Carrega as vendas registradas até o momento
    *tamanhovendas = 0;
    while (fscanf(file, "%f\n", &vendas[*tamanhovendas].total) == 1) {
        (*tamanhovendas)++;
    }

    fclose(file);
}


int vendas() {
    Produto estoque[MAX_PRODUTOS];
    Venda vendas[DIAS_DO_MES];
    float faturamentoDiario[DIAS_DO_MES] = {0};
    int tamanhovendas = 0;
    int diaAtual = 0;
    int contadorEstoque = 0;
    int opcao;
    char confirm, continuarDia = 'n';
    char confirmacao = 'n';

    // Carregar dados iniciais
    carregarEstoque(estoque, &contadorEstoque);
    carregarEstado(vendas, &tamanhovendas, &diaAtual); // Carga de estado ao iniciar

    do {
        printf("\nMenu:\n");
        printf("1. Registrar compra\n");
        printf("2. Exibir faturamento semanal\n");
        printf("3. Exibir faturamento mensal\n");
        printf("4. Listar vendas\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        switch (opcao) {
            case 1:
                printf("\nTem certeza de que deseja iniciar um novo dia? (S/N): \n");
                while ((confirmacao = _getch()) != 'S' && confirmacao != 's' && confirmacao != 'N' && confirmacao != 'n') {
                    printf("\nOpção inválida! Digite 'S' ou 'N': ");
                }

                if (confirmacao == 'S' || confirmacao == 's') {
                    continuarDia = 's';
                    printf("Iniciando um novo dia...\n");
                    system("pause");
                    system("cls");

                    // Atualizando o dia
                    printf("\n== Dia %d ==\n", diaAtual + 1);
                    faturamentoDiario[diaAtual] = registrarCompra(estoque, contadorEstoque, vendas, &tamanhovendas);
                    diaAtual++; // Incrementando o dia

                    // Salvando o estado ao final do dia
                    salvarEstado(vendas, tamanhovendas, diaAtual);

                } else {
                    printf("Novo dia cancelado. Voltando ao menu de vendas...\n");
                    continuarDia = 'n';
                    system("pause");
                    system("cls");
                }
                break;
            case 2:
                calcularVendasSemana(vendas, tamanhovendas);
                break;
            case 3:
                calcularFaturamentoMensal(vendas, tamanhovendas);
                break;
            case 4:
                listarVendas(vendas, tamanhovendas);
                break;
            case 5:
                printf("Saindo do sistema de vendas...\n");
                return 0;
            default:
                printf("\nOpção inválida! Tente novamente.\n");
        }

        system("pause");
        system("cls");

    } while (opcao != 5);

    return 0;
}

// Definicao da funcao exibirProdutos
void exibirProdutos(Produto produtos[], int tamanho) {
    printf("\n--- Produtos Disponíveis ---\n");
    for (int i = 0; i < tamanho; i++) {
        printf("%d. %s - %d unidades - R$%.2f\n", i + 1, produtos[i].nome, produtos[i].quantidade, produtos[i].preco);
    }
}

// Definicao da funcao RegistrarCompra
float registrarCompra(Produto produtos[], int tamanho, Venda vendas[], int *tamanhovendas) {
    int quantidades[MAX_PRODUTOS] = {0};
    int escolha;
    char continuar;
    char tipoVenda;
    float quantidade; // Pode ser tanto para quilos como para unidades
    float totalCompra = 0;

    do {
        exibirProdutos(produtos, tamanho);

        printf("Escolha o número do produto que deseja comprar: ");
        scanf("%d", &escolha);
        if (escolha < 1 || escolha > tamanho) {
            printf("Escolha inválida!\n");
            continue;
        }

        Produto *produtoSelecionado = &produtos[escolha - 1];

        // Perguntar se será vendido por quilos ou unidades
        printf("Deseja comprar %s por quilos (K) ou por unidades (U)? ", produtoSelecionado->nome);
        scanf(" %c", &tipoVenda);
        tipoVenda = toupper(tipoVenda); // Converte para maiúsculo para facilitar a comparação

        if (tipoVenda == 'K') {
            printf("Informe a quantidade em quilos: ");
            scanf("%f", &quantidade);
            // Verifica se há estoque suficiente em quilos
            if (produtoSelecionado->quantidade < quantidade) {
                printf("Quantidade indisponível. Temos apenas %.2f quilos de %s no estoque.\n",
                    produtoSelecionado->quantidade, produtoSelecionado->nome);
                continue;
            }
            totalCompra += produtoSelecionado->preco * quantidade;
        } else if (tipoVenda == 'U') {
            printf("Informe a quantidade em unidades: ");
            scanf("%f", &quantidade);
            if (quantidade != (int)quantidade) {  // Verifica se a quantidade é inteira
                printf("Erro: Para unidades, informe um número inteiro.\n");
                continue;
            }
            // Verifica se há estoque suficiente em unidades
            if (produtoSelecionado->quantidade < quantidade) {
                printf("Quantidade indisponível. Temos apenas %d unidades de %s no estoque.\n",
                    produtoSelecionado->quantidade, produtoSelecionado->nome);
                continue;
            }
            totalCompra += produtoSelecionado->preco * quantidade;
        } else {
            printf("Tipo de venda inválido. Escolha 'K' para quilos ou 'U' para unidades.\n");
            continue;
        }

        // Atualiza a quantidade no estoque
        produtoSelecionado->quantidade -= quantidade;

        // Pergunta se deseja comprar mais produtos
        printf("Deseja comprar mais algum produto? (s/n): ");
        scanf(" %c", &continuar);

    } while (continuar == 's' || continuar == 'S');

    // Guarda a venda no array de vendas
    if (*tamanhovendas < DIAS_DO_MES) {
        vendas[*tamanhovendas].total = totalCompra;
        (*tamanhovendas)++;
    }

    // Salva a compra em arquivo
    guardarCompraEmArquivo(vendas[*tamanhovendas - 1], produtos, quantidades, tamanho);

    // Guarda o estoque atualizado em arquivo
    salvarEstoque(produtos, tamanho);

    // Gera o cupom após registrar uma compra
    gerarCupom(vendas[*tamanhovendas - 1]);

    return totalCompra;
}

void listarVendas(Venda vendas[], int tamanhovendas) {
    if (tamanhovendas == 0) {
        printf("\nNenhuma venda registrada até agora.\n");
        return;
    }

    printf("\n--- Lista de Vendas ---\n");
    for (int i = 0; i < tamanhovendas; i++) {
        printf("Dia %d: Venda total - R$%.2f\n", i + 1, vendas[i].total);
    }
    printf("------------------------\n");
}


void guardarCompraEmArquivo(Venda venda, Produto produtos[], int quantidades[], int tamanho) {
    FILE *arquivo = fopen("vendas_do_mes.txt", "a"); // Abrir para adicionar ao final
    if (!arquivo) {
        printf("Não foi possível abrir o arquivo para guardar a compra.\n");
        return;
    }

    // Salva os detalhes da venda
    fprintf(arquivo, "Compra - Total: R$%.2f\n", venda.total);
    for (int i = 0; i < tamanho; i++) {
        if (quantidades[i] > 0) {
            fprintf(arquivo, "%s - Quantidade: %d - Total: R$%.2f\n",
                    produtos[i].nome, quantidades[i], produtos[i].preco * quantidades[i]);
        }
    }
    fprintf(arquivo, "----------------------\n");
    fclose(arquivo);
}


// Definicao da funcao calcularFaturamentoMensal
void calcularFaturamentoMensal(Venda vendas[], int tamanhovendas) {
    float faturamentoMensal = 0;
    for (int i = 0; i < tamanhovendas && i < DIAS_DO_MES; i++) {
        faturamentoMensal += vendas[i].total;
    }

    printf("Faturamento do mês: R$%.2f\n", faturamentoMensal);
}

// Definicao da funcao calcularVendasSemana
void calcularVendasSemana(Venda vendas[], int tamanhovendas) {
    float faturamentoSemanal = 0;
    int diasSemanais = 7;

    for (int i = tamanhovendas - 1; i >= 0 && diasSemanais > 0; i--, diasSemanais--) {
        faturamentoSemanal += vendas[i].total;
    }

    printf("Faturamento da última semana: R$%.2f\n", faturamentoSemanal);
}

    // Definicao da funcao gerarCupom
void gerarCupom(Venda venda) {
    printf("\n--- Cupom de Venda ---\n");
    printf("Total da Venda: R$%.2f\n", venda.total);
    printf("------------------------\n");
}

#endif // VENDAS_H_INCLUDED
