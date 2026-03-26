<div align="center">

```
╔══════════════════════════════════════╗
║   SISTEMA DE GESTÃO - HORTIFRUTI     ║
╚══════════════════════════════════════╝
```

# 🥦 Sistema de Gestão Hortifruti

**Software completo em C para gerenciamento de pequenos e médios hortifrutis.**  
Estoque · Vendas · Fornecedores · Controle de Usuários

[![Linguagem](https://img.shields.io/badge/Linguagem-C%20%28C99%29-00b894?style=flat-square&logo=c)](https://en.wikipedia.org/wiki/C99)
[![IDE](https://img.shields.io/badge/IDE-Code%3A%3ABlocks-0984e3?style=flat-square)](http://www.codeblocks.org/)
[![Plataforma](https://img.shields.io/badge/Plataforma-Windows-636e72?style=flat-square&logo=windows)](https://www.microsoft.com/windows)
[![Autor](https://img.shields.io/badge/Autor-Juan%20Vargas-6c5ce7?style=flat-square)](https://github.com/RenteriaJuan) [![Autor](https://img.shields.io/badge/Autor-Christian%20Fernandes-6c5ce7?style=flat-square)](https://github.com/ChristianFernandesLemos) 

---

📄 **[Ver página do projeto](https://renteriajuan.github.io/Gestao-de-Hortifruti/CODIGO-MODULADO-HORTIFRUTI/portfolio-hortifruti.html)**

</div>

---

## 📋 Sobre o projeto

Este é o meu primeiro software completo — um sistema de gerenciamento desenvolvido em linguagem C para hortifrutis de pequeno e médio porte. O projeto foi desenvolvido com foco em modularização, persistência de dados em arquivos `.txt` e controle de acesso por nível de usuário.

---

## ✨ Funcionalidades

| Módulo | Descrição |
|---|---|
| 📦 **Estoque** | Adicionar, listar, atualizar e remover produtos. Margem de 20% aplicada automaticamente. |
| 🛒 **Vendas** | Registro por quilos ou unidades. Cupom no terminal. Faturamento diário, semanal e mensal. |
| 🤝 **Fornecedores** | Cadastro, listagem e atualização com verificação de duplicidade. |
| 🔐 **Autenticação** | Login com senha oculta (asteriscos). 3 níveis de acesso com permissões distintas. |
| 🗑️ **Descarte** | Produtos vencidos movidos para lista de descartados com histórico em arquivo. |
| 🔍 **Busca** | Pesquisa de produto por nome (parcial, sem diferenciação de maiúsculas/minúsculas). |
| ⚠️ **Alertas** | Alerta automático de estoque baixo ao entrar no menu Caixa. |
| 📊 **Relatórios** | Geração de `relatorio.txt` com inventário completo e valor total do estoque. |

---

## 🏗️ Arquitetura

O projeto é dividido em módulos independentes:

```
CODIGO_MODULADO_HORTIFRUTI/
├── main.c                  ← Ponto de entrada, menus, autenticação
├── Estoque_cabecalho.h     ← Módulo de estoque e descarte
├── Fornecedores.h          ← Módulo de fornecedores
├── Vendas.h                ← Módulo de vendas e cupons
│
├── estoque.txt             ← Dados do estoque
├── descartados.txt         ← Histórico de descarte
├── fornecedores.txt        ← Cadastro de fornecedores
├── compras.txt             ← Histórico de compras
├── vendas_do_mes.txt       ← Estado de vendas
├── users.txt               ← Usuários e senhas
└── relatorio.txt           ← Gerado pelo sistema
```

---

## 🔐 Níveis de acesso

| Nível | Perfil | Permissões |
|---|---|---|
| `2` | 👑 **Admin** | Acesso total: usuários, estoque, fornecedores, vendas, relatórios |
| `1` | 👤 **Funcionário** | Listar estoque/fornecedores, descartar produtos, registrar vendas |
| `3` | 🧾 **Caixa** | Registrar vendas, emitir cupom, listar vendas do dia |

> O registro de usuário Admin é protegido por um código secreto.

---

## 🐛 Bugs corrigidos

Durante o desenvolvimento e revisão do código, os seguintes bugs foram identificados e corrigidos:

- `estoque()` e `Listforn()` declaradas como `void` mas continham `return 0`
- `salvarDescartados()` recebia ponteiro de função em vez do array de descartados
- Mensagem de sucesso em `descartarProduto()` só aparecia quando quantidade chegava a zero
- `guardarCompraEmArquivo()` escrevia em `vendas_do_mes.txt` em vez de `compras.txt`
- Protótipo de função declarado dentro de bloco `switch` (código sem efeito)
- IDs de produtos não eram reatribuídos após remoção, causando duplicatas

---

## 🚀 Como executar

**Requisitos:** Windows · GCC / MinGW · Code::Blocks (recomendado)

> ⚠️ O projeto usa `conio.h` (`_getch`), portanto é **exclusivo para Windows**.

```bash
# 1. Clone o repositório
git clone https://github.com/RenteriaJuan/hortifruti-system.git

# 2. Abra o arquivo .cbp no Code::Blocks
#    ou compile manualmente:
gcc main.c -o hortifruti.exe

# 3. Execute
./hortifruti.exe
```

Os arquivos `.txt` de dados são criados automaticamente na primeira execução.

---

## 🗂️ Usuários padrão para teste

```
Usuário: Pedrito   | Senha: 1234  | Nível: Funcionário
Usuário: Fermin    | Senha: 1234  | Nível: Funcionário
Usuário: Juan      | Senha: 1234  | Nível: Funcionário
Usuário: Corxea    | Senha: 1234  | Nível: Caixa
```

---

## 👤 Autor

**Juan Vargas**  
[![GitHub](https://img.shields.io/badge/GitHub-RenteriaJuan-181717?style=flat-square&logo=github)](https://github.com/RenteriaJuan)

[![GitHub](https://img.shields.io/badge/GitHub-ChristianFernandesLemos-181717?style=flat-square&logo=github)](https://github.com/ChristianFernandesLemos)
---

<div align="center">
<sub>Primeiro projeto completo em C · Desenvolvido com fins educacionais</sub>
</div>
