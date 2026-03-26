#ifndef FORNECEDORES_H_INCLUDED
#define FORNECEDORES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_FORNECEDORES 100
#define NOME_LEN 50
#define CONTATO_LEN 100
#define ARQUIVO "fornecedores.txt"

typedef struct {
    char nome[NOME_LEN];
    char contato[CONTATO_LEN];
} Fornecedor;

Fornecedor fornecedores[MAX_FORNECEDORES];
int numFornecedores = 0;

void cadastrarFornecedor();
void listarFornecedores();
int fornecedorExistente(const char *nome);
void salvarFornecedor(Fornecedor fornecedor);
void carregarFornecedores();
void atualizarFornecedor();

void Listforn() {
	setlocale (LC_ALL,"portuguese");
    int opcao;
    system("cls");
    carregarFornecedores();
    char confirm;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Cadastrar Fornecedor\n");
        printf("2. Listar Fornecedores\n");
        printf("3. Atualizar Fornecedor\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // Limpa o buffer do teclado

        switch (opcao) {
            case 1:
                cadastrarFornecedor();
                system("pause");
                system("cls");
                break;
            case 2:
                listarFornecedores();
                system("pause");
                system("cls");
                break;
            case 3:
                atualizarFornecedor();
                system("pause");
                system("cls");
                break;
            case 4:
               printf("\nDeseja realmente sair (S/N)\n");
                while((confirm=_getch()) != 'S' && confirm != 's' && confirm != 'N' && confirm != 'n'){
                    printf("opção invalida, digite 'S' ou 'N' para sair");
                }
                if (confirm == 'S'||confirm == 's'){
                printf("\nSaindo da gestão de fornecedores...\n");
                } else {
                    opcao=0;
                    }
                    system("pause");
                    system("cls");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 4);

    return 0;
}

void cadastrarFornecedor() {
    Fornecedor novoFornecedor;

    printf("Digite o nome do fornecedor: ");
    fgets(novoFornecedor.nome, NOME_LEN, stdin);
    novoFornecedor.nome[strcspn(novoFornecedor.nome, "\n")] = 0;

    if (fornecedorExistente(novoFornecedor.nome)) {
        printf("Fornecedor com este nome já existe!\n");
        system("pause");
         system("cls");
        return;
    }

    printf("Digite o método de contato do fornecedor: ");
    fgets(novoFornecedor.contato, CONTATO_LEN, stdin);
    novoFornecedor.contato[strcspn(novoFornecedor.contato, "\n")] = 0;

    fornecedores[numFornecedores++] = novoFornecedor;
    salvarFornecedor(novoFornecedor);
    printf("Fornecedor cadastrado com sucesso!\n");
    system("pause");
     system("cls");
}


void listarFornecedores() {
	int i;
    printf("\n--- Lista de Fornecedores ---\n");
    for ( i = 0; i < numFornecedores; i++) {
        printf("Nome: %s, Contato: %s\n", fornecedores[i].nome, fornecedores[i].contato);
    }
}

int fornecedorExistente(const char *nome) {
	int i;
    for (i = 0; i < numFornecedores; i++) {
        if (strcmp(fornecedores[i].nome, nome) == 0) {
            return 1; // Fornecedor existe
        }
    }
    return 0; // Fornecedor não existe
}

void salvarFornecedor(Fornecedor fornecedor) {
    FILE *file = fopen(ARQUIVO, "a");
    if (file != NULL) {
        fprintf(file, "%s;%s\n", fornecedor.nome, fornecedor.contato);
        fclose(file);
    } else {
        printf("Erro ao abrir o arquivo para salvar o fornecedor.\n");
    }
}

void carregarFornecedores() {
    FILE *file = fopen(ARQUIVO, "r");
    if (file != NULL) {
        while (fscanf(file, "%49[^;];%99[^\n]\n", fornecedores[numFornecedores].nome, fornecedores[numFornecedores].contato) == 2) {
            numFornecedores++;
        }
        fclose(file);
    }
}

void atualizarFornecedor() {
    char nome[NOME_LEN];
    int i;

    printf("Digite o nome do fornecedor a ser atualizado: ");
    fgets(nome, NOME_LEN, stdin);
    nome[strcspn(nome, "\n")] = 0; // Remove nova linha

    for (i = 0; i < numFornecedores; i++) {
        if (strcmp(fornecedores[i].nome, nome) == 0) {
            printf("Fornecedor encontrado!\n");
            printf("Novo contato: ");
            fgets(fornecedores[i].contato, CONTATO_LEN, stdin);
            fornecedores[i].contato[strcspn(fornecedores[i].contato, "\n")] = 0; // Remove nova linha
            printf("Fornecedor atualizado com sucesso!\n");
            system("pause");
            system("cls");
            return;
        }
    }

    printf("Fornecedor não encontrado!\n");
    system("pause"); // Pausa para exibir a mensagem de erro
}

#endif // FORNECEDORES_H_INCLUDED
