/*
 * ============================================================
 *  SISTEMA DE GESTÃO HORTIFRUTI
 *  Gerenciamento de Estoque, Vendas e Fornecedores
 * ============================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <conio.h>
#include "Estoque_cabecalho.h"
#include "Fornecedores.h"
#include "Vendas.h"

#define FILENAME     "users.txt"
#define CODIGO_ADM   "28719210"
#define ESTOQUE_MIN  10   /* Alerta quando produto tem menos que esta quantidade */

extern int tamanhovendas;
void carregarEstoque(struct Produto produtos[], int *contador);
void listarProdutos(struct Produto estoque[], int contador);
void descartarProduto(struct Produto estoque[], int *contador, struct Produto descartados[], int *contadorDescartados);
void carregarDescartados(struct Produto descartados[], int *contador);

/* ---------------------------------------------------------- */
/*  Utilitários de autenticação                               */
/* ---------------------------------------------------------- */

/* Captura senha mostrando apenas asteriscos */
void getPassword(char *password) {
    char ch;
    int i = 0;
    while ((ch = getch()) != '\r') {
        if (ch == '\b') {
            if (i > 0) { i--; printf("\b \b"); }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
}

/* Verifica se nome de usuário já existe */
int userExists(const char *username) {
    char existingUsername[30], password[30];
    int accessLevel;
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 0;
    while (fscanf(file, "%s %s %d", existingUsername, password, &accessLevel) != EOF) {
        if (strcmp(existingUsername, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

/* Registra novo usuário */
void registerUser() {
    char username[30], password[30], adminCode[30];
    int accessLevel;
    FILE *file;

    printf("\n=== Registro de Novo Usuario ===\n");
    printf("Novo Usuario: ");
    scanf("%s", username);

    if (userExists(username)) {
        printf("Erro: Nome de usuario ja existe! Tente outro.\n");
        return;
    }

    printf("Nova Senha: ");
    getPassword(password);
    printf("\n");

    printf("Nível de Acesso (1 - Funcionario, 2 - Admin, 3 - Caixa): ");
    scanf("%d", &accessLevel);

    if (accessLevel == 2) {
        printf("Digite o código de administrador: ");
        getPassword(adminCode);
        printf("\n");
        if (strcmp(adminCode, CODIGO_ADM) != 0) {
            printf("Erro: Código de administrador incorreto! Registro negado.\n");
            return;
        }
    }

    file = fopen(FILENAME, "a");
    if (!file) { printf("Erro ao abrir o arquivo!\n"); return; }
    fprintf(file, "%s %s %d\n", username, password, accessLevel);
    fclose(file);

    printf("Usuário registrado com sucesso!\n");
}

/* Realiza login e retorna nível de acesso */
int loginUser(int *accessLevel) {
    char inputUsername[30], inputPassword[30], username[30], password[30];
    int loginSuccess = 0;

    FILE *file = fopen(FILENAME, "r");
    if (!file) { printf("Erro ao abrir o arquivo de usuarios!\n"); return 0; }

    printf("\n=== Tela de Login ===\n");
    printf("Usuario: ");
    scanf("%s", inputUsername);
    printf("Senha: ");
    getPassword(inputPassword);
    printf("\n");

    while (fscanf(file, "%s %s %d", username, password, accessLevel) != EOF) {
        if (strcmp(inputUsername, username) == 0 && strcmp(inputPassword, password) == 0) {
            loginSuccess = 1;
            break;
        }
    }
    fclose(file);

    if (loginSuccess) {
        const char *niveis[] = {"", "Funcionario", "Admin", "Caixa"};
        printf("Login bem-sucedido! Bem-vindo, %s (%s).\n",
               inputUsername, (*accessLevel >= 1 && *accessLevel <= 3) ? niveis[*accessLevel] : "Desconhecido");
        return 1;
    } else {
        printf("Usuario ou senha incorretos.\n");
        return 0;
    }
}

/* Exclui usuário (apenas Admin) */
void deleteUser() {
    char username[30], tempUsername[30], tempPassword[30];
    int tempAccessLevel, found = 0;

    printf("Digite o nome do usuario que deseja excluir: ");
    scanf("%s", username);

    FILE *file     = fopen(FILENAME, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if (!file || !tempFile) { printf("Erro ao abrir o arquivo!\n"); return; }

    while (fscanf(file, "%s %s %d", tempUsername, tempPassword, &tempAccessLevel) != EOF) {
        if (strcmp(tempUsername, username) != 0)
            fprintf(tempFile, "%s %s %d\n", tempUsername, tempPassword, tempAccessLevel);
        else
            found = 1;
    }
    fclose(file);
    fclose(tempFile);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    printf(found ? "Usuário %s excluído com sucesso!\n" : "Usuario não encontrado.\n", username);
}

/* Lista todos os usuários (apenas Admin) */
void listUsers() {
    char username[30], password[30];
    int accessLevel;
    const char *niveis[] = {"", "Funcionário", "Admin", "Caixa"};

    FILE *file = fopen(FILENAME, "r");
    if (!file) { printf("Erro ao abrir o arquivo!\n"); return; }

    printf("\n=== Lista de Usuários Registrados ===\n\n");
    while (fscanf(file, "%s %s %d", username, password, &accessLevel) != EOF) {
        const char *nivel = (accessLevel >= 1 && accessLevel <= 3) ? niveis[accessLevel] : "Desconhecido";
        printf("  Usuario: %-20s | Nível: %s\n", username, nivel);
    }
    fclose(file);
}

/* ---------------------------------------------------------- */
/*  NOVA FUNCIONALIDADE: Busca produto por nome no estoque    */
/* ---------------------------------------------------------- */
void buscarProdutoNome(struct Produto estoque[], int contador) {
    char nome[50];
    int encontrado = 0;

    printf("\nDigite o nome (ou parte do nome) do produto: ");
    getchar();
    scanf("%[^\n]", nome);

    printf("\n--- Resultado da Busca: \"%s\" ---\n", nome);
    for (int i = 0; i < contador; i++) {
        /* Busca case-insensitive usando strstr simples */
        char nomeLower[50], buscaLower[50];
        int j;
        for (j = 0; estoque[i].nome[j]; j++) nomeLower[j] = tolower((unsigned char)estoque[i].nome[j]);
        nomeLower[j] = '\0';
        for (j = 0; nome[j]; j++) buscaLower[j] = tolower((unsigned char)nome[j]);
        buscaLower[j] = '\0';

        if (strstr(nomeLower, buscaLower)) {
            printf("  ID: %d | Nome: %-20s | Quantidade: %4d | Preço: R$%.2f\n",
                   estoque[i].id, estoque[i].nome, estoque[i].quantidade, estoque[i].preco);
            encontrado = 1;
        }
    }
    if (!encontrado) printf("  Nenhum produto encontrado com esse nome.\n");
}

/* ---------------------------------------------------------- */
/*  NOVA FUNCIONALIDADE: Alerta de estoque baixo              */
/* ---------------------------------------------------------- */
void alertaEstoqueBaixo(struct Produto estoque[], int contador) {
    int algum = 0;
    printf("\n⚠  Produtos com estoque baixo (< %d unidades):\n", ESTOQUE_MIN);
    for (int i = 0; i < contador; i++) {
        if (estoque[i].quantidade < ESTOQUE_MIN) {
            printf("  [!] ID: %d | %-20s | Apenas %d unidades restantes!\n",
                   estoque[i].id, estoque[i].nome, estoque[i].quantidade);
            algum = 1;
        }
    }
    if (!algum) printf("  Todos os produtos tem estoque adequado.\n");
}

/* ---------------------------------------------------------- */
/*  NOVA FUNCIONALIDADE: Relatório exportado para arquivo     */
/* ---------------------------------------------------------- */
void gerarRelatorio(struct Produto estoque[], int contador) {
    FILE *f = fopen("relatorio.txt", "w");
    if (!f) { printf("Erro ao criar o arquivo de relatório.\n"); return; }

    fprintf(f, "============================================\n");
    fprintf(f, "   RELATÓRIO DE ESTOQUE - HORTIFRUTI\n");
    fprintf(f, "============================================\n\n");

    fprintf(f, "%-5s %-25s %-12s %-10s\n", "ID", "Nome", "Quantidade", "Preco");
    fprintf(f, "--------------------------------------------\n");

    float totalValor = 0;
    int alertas = 0;
    for (int i = 0; i < contador; i++) {
        float valor = estoque[i].quantidade * estoque[i].preco;
        totalValor += valor;
        const char *aviso = (estoque[i].quantidade < ESTOQUE_MIN) ? " [BAIXO]" : "";
        fprintf(f, "%-5d %-25s %-12d R$%-9.2f%s\n",
                estoque[i].id, estoque[i].nome, estoque[i].quantidade, estoque[i].preco, aviso);
        if (estoque[i].quantidade < ESTOQUE_MIN) alertas++;
    }

    fprintf(f, "\n--------------------------------------------\n");
    fprintf(f, "Total de produtos cadastrados : %d\n", contador);
    fprintf(f, "Valor total do estoque        : R$%.2f\n", totalValor);
    fprintf(f, "Produtos com estoque baixo    : %d\n", alertas);
    fprintf(f, "============================================\n");

    fclose(f);
    printf("Relatório salvo em 'relatorio.txt' com sucesso!\n");
}

/* ---------------------------------------------------------- */
/*  Menus por nível de acesso                                 */
/* ---------------------------------------------------------- */

void menuAdmin() {
    int adminChoice;
    char confirm;
    system("pause");
    system("cls");

    struct Produto estoqueAdmin[MAX_PRODUTOS];
    int contadorAdmin = 0;
    carregarEstoque(estoqueAdmin, &contadorAdmin);

    do {
        printf("\n=== MENU ADMIN ===\n");
        printf("1. Excluir usuario\n");
        printf("2. Listar usuarios\n");
        printf("3. Gerenciar estoque\n");
        printf("4. Gerenciar fornecedores\n");
        printf("5. Gerenciar vendas\n");
        printf("6. Buscar produto por nome\n");
        printf("7. Alerta de estoque baixo\n");
        printf("8. Gerar relatorio de estoque\n");
        printf("9. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &adminChoice);

        switch (adminChoice) {
            case 1:
                deleteUser();
                system("pause"); system("cls");
                break;
            case 2:
                listUsers();
                system("pause"); system("cls");
                break;
            case 3:
                estoque();
                /* Recarrega estoque após modificações */
                contadorAdmin = 0;
                carregarEstoque(estoqueAdmin, &contadorAdmin);
                break;
            case 4:
                Listforn();
                break;
            case 5:
                vendas();
                break;
            case 6:
                buscarProdutoNome(estoqueAdmin, contadorAdmin);
                system("pause"); system("cls");
                break;
            case 7:
                alertaEstoqueBaixo(estoqueAdmin, contadorAdmin);
                system("pause"); system("cls");
                break;
            case 8:
                gerarRelatorio(estoqueAdmin, contadorAdmin);
                system("pause"); system("cls");
                break;
            case 9:
                printf("Deseja realmente sair (S/N)? ");
                while ((confirm = _getch()) != 'S' && confirm != 's' && confirm != 'N' && confirm != 'n')
                    printf("Opcao inválida, digite 'S' ou 'N': ");
                if (confirm == 'S' || confirm == 's')
                    printf("\nVoltando ao menu principal...\n");
                else
                    adminChoice = 0;
                system("pause"); system("cls");
                break;
            default:
                printf("Opcao inválida!\n");
        }
    } while (adminChoice != 9);
}

void menuFuncionario() {
    system("cls");

    struct Produto estoqueFunc[MAX_PRODUTOS];
    struct Produto descartados[MAX_PRODUTOS];
    int contadorEstoque = 0, contadorDescartados = 0;
    int funcChoice;
    char confirm, confirmacao;
    Venda vendas[DIAS_DO_MES];
    int totalVendas = 0;

    carregarEstoque(estoqueFunc, &contadorEstoque);

    do {
        printf("\n=== MENU FUNCIONÁRIO ===\n");
        printf("1. Listar fornecedores\n");
        printf("2. Listar produtos\n");
        printf("3. Descartar produto\n");
        printf("4. Registrar venda do dia\n");
        printf("5. Buscar produto por nome\n");
        printf("6. Alerta de estoque baixo\n");
        printf("7. Sair\n");
        printf("Escolha uma opção: ");

        if (scanf("%d", &funcChoice) != 1) {
            printf("Entrada inválida.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (funcChoice) {
            case 1:
                carregarFornecedores();
                listarFornecedores();
                printf("\nPressione qualquer tecla para continuar...");
                getch();
                system("cls");
                break;

            case 2:
                /* Recarrega para mostrar dados atualizados */
                contadorEstoque = 0;
                carregarEstoque(estoqueFunc, &contadorEstoque);
                if (contadorEstoque > 0)
                    listarProdutos(estoqueFunc, contadorEstoque);
                else
                    printf("Estoque vazio.\n");
                printf("\nPressione qualquer tecla para continuar...");
                getch();
                system("cls");
                break;

            case 3:
                contadorDescartados = 0;
                carregarDescartados(descartados, &contadorDescartados);
                descartarProduto(estoqueFunc, &contadorEstoque, descartados, &contadorDescartados);
                salvarDescartados(descartados, contadorDescartados);
                salvarEstoque(estoqueFunc, contadorEstoque);
                printf("\nPressione qualquer tecla para continuar...");
                getch();
                system("cls");
                break;

            case 4:
                printf("Tem certeza de que deseja registrar uma venda? (S/N): ");
                while ((confirmacao = _getch()) != 'S' && confirmacao != 's'
                       && confirmacao != 'N' && confirmacao != 'n')
                    printf("\nOpção inválida! Digite 'S' ou 'N': ");

                if (confirmacao == 'S' || confirmacao == 's') {
                    printf("\nIniciando registro de venda...\n");
                    system("pause"); system("cls");
                    registrarCompra(estoqueFunc, contadorEstoque, vendas, &totalVendas);
                    salvarEstado(vendas, totalVendas, totalVendas);
                } else {
                    printf("\nOperacao cancelada.\n");
                    system("pause"); system("cls");
                }
                break;

            case 5:
                buscarProdutoNome(estoqueFunc, contadorEstoque);
                system("pause"); system("cls");
                break;

            case 6:
                alertaEstoqueBaixo(estoqueFunc, contadorEstoque);
                system("pause"); system("cls");
                break;

            case 7:
                printf("Deseja realmente sair (S/N)? ");
                while ((confirm = _getch()) != 'S' && confirm != 's' && confirm != 'N' && confirm != 'n')
                    printf("Opção inválida. Digite 'S' ou 'N': ");
                if (confirm == 'S' || confirm == 's')
                    printf("\nVoltando ao menu principal...\n");
                else
                    funcChoice = 0;
                system("pause"); system("cls");
                break;

            default:
                printf("Opção inválida.\n");
        }
    } while (funcChoice != 7);
}

void menuCaixa() {
    system("pause");
    system("cls");

    int diaAtual = 0, tamanhovendas = 0;
    Venda vendas[DIAS_DO_MES];
    int totalVendas = 0;
    Produto estoqueC[MAX_PRODUTOS];
    int contadorEstoque = 0;
    int caixaChoice;
    char confirm;

    carregarEstoque(estoqueC, &contadorEstoque);
    carregarEstado(vendas, &tamanhovendas, &diaAtual);

    /* Alerta automático de estoque baixo ao entrar no menu caixa */
    alertaEstoqueBaixo(estoqueC, contadorEstoque);
    system("pause");

    do {
        printf("\n=== MENU CAIXA ===\n");
        printf("1. Registrar venda\n");
        printf("2. Listar vendas do dia\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");

        if (scanf("%d", &caixaChoice) != 1) {
            printf("Entrada inválida.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (caixaChoice) {
            case 1:
                registrarCompra(estoqueC, contadorEstoque, vendas, &totalVendas);
                salvarEstado(vendas, totalVendas, totalVendas);
                system("pause"); system("cls");
                break;

            case 2:
                carregarEstado(vendas, &tamanhovendas, &diaAtual);
                listarVendas(vendas, tamanhovendas);
                system("pause"); system("cls");
                break;

            case 3:
                printf("Deseja realmente sair (S/N)? ");
                while ((confirm = _getch()) != 'S' && confirm != 's' && confirm != 'N' && confirm != 'n')
                    printf("Opcao inválida. Digite 'S' ou 'N': ");
                if (confirm == 'S' || confirm == 's') {
                    printf("\nSaindo do menu caixa...\n");
                    system("pause"); system("cls");
                } else {
                    caixaChoice = 0;
                }
                break;

            default:
                printf("Opção inválida.\n");
        }
    } while (caixaChoice != 3);
}

/* ---------------------------------------------------------- */
/*  Menu principal                                            */
/* ---------------------------------------------------------- */
int main() {
    int choice, accessLevel = 0;
    char confirm;

    setlocale(LC_ALL, "Portuguese_Brazil");

    printf("\n");
    printf("  ========================================\n");
    printf("  |   SISTEMA DE GESTAO - HORTIFRUTI     |\n");
    printf("  ========================================\n");

    while (1) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Login\n");
        printf("2. Registrar novo usuario\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (loginUser(&accessLevel)) {
                    system("pause"); system("cls");
                    switch (accessLevel) {
                        case 1: menuFuncionario(); break;
                        case 2: menuAdmin();       break;
                        case 3: menuCaixa();       break;
                        default: printf("Nível de acesso desconhecido.\n");
                    }
                }
                break;
            case 2:
                registerUser();
                break;
            case 3:
                printf("\nDeseja realmente sair (S/N)? ");
                while ((confirm = _getch()) != 'S' && confirm != 's' && confirm != 'N' && confirm != 'n')
                    printf("Opção inválida, digite 'S' ou 'N': ");
                if (confirm == 'S' || confirm == 's') {
                    printf("\nSaindo do sistema. Até logo!\n");
                    system("pause");
                    exit(0);
                }
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}
