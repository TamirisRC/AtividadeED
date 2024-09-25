#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 50
#define MAX_NOME 60
#define MAX_SIGLA 10

typedef struct {
    char nome[MAX_NOME];
    int idade;
} Pessoa;

typedef struct {
    Pessoa pes;
    int codigo;
    char depto[MAX_SIGLA];
} Professor;

typedef struct {
    Pessoa pes;
    int matricula;
    int ano;
    char depto[MAX_SIGLA];
} Aluno;

typedef struct {
    int codigo;
    int autor;
    int orientador;
    char titulo[MAX_NOME];
    int qtdeVotos;
} TG;

typedef struct {
    char cpf[15];
    bool votou;
    int codigoTG;
} Eleitor;

Professor docentes[MAX];
int qtdeDocentes = 0;

Aluno formandos[MAX];
int qtdeFormandos = 0;

TG listaPIs[MAX];
int qtdeTGs = 0;

Eleitor comissao[MAX];
int qtdeEleitores = 0;

bool votacaoAtiva = true; 

bool verificarCPF(char cpf[15]) {
    if (strlen(cpf) != 14 || cpf[3] != '.' || cpf[7] != '.' || cpf[11] != '-') {
        return false;
    }
    for (int i = 0; i < 14; i++) {
        if ((i != 3 && i != 7 && i != 11) && !isdigit(cpf[i])) {
            return false;
        }
    }
    return true;
}

void lerArquivoProfessores() {
    FILE *arquivo = fopen("professor.txt", "r");
    if (arquivo == NULL) {
        printf("Arquivo professor.txt faltando\n");
        exit(1);
    }
    fscanf(arquivo, "%d", &qtdeDocentes);
    for (int i = 0; i < qtdeDocentes; i++) {
        fscanf(arquivo, "%d %s %d %[^\n]s", &docentes[i].codigo, docentes[i].depto, &docentes[i].pes.idade, docentes[i].pes.nome);
    }
    fclose(arquivo);
}

void lerArquivoAlunos() {
    FILE *arquivo = fopen("aluno.txt", "r");
    if (arquivo == NULL) {
        printf("Arquivo aluno.txt faltando\n");
        exit(1);
    }
    fscanf(arquivo, "%d", &qtdeFormandos);
    for (int i = 0; i < qtdeFormandos; i++) {
        fscanf(arquivo, "%d %d %s %d %[^\n]s", &formandos[i].matricula, &formandos[i].ano, formandos[i].depto, &formandos[i].pes.idade, formandos[i].pes.nome);
    }
    fclose(arquivo);
}

void lerArquivoPI(char *filename) {
    FILE *arquivo = fopen(filename, "r");
    if (arquivo == NULL) {
        printf("Arquivo %s faltando\n", filename);
        exit(1);
    }
    int qtd;
    fscanf(arquivo, "%d", &qtd);
    for (int i = 0; i < qtd; i++) {
        fscanf(arquivo, "%d %d %d %[^\n]s", &listaPIs[qtdeTGs].codigo, &listaPIs[qtdeTGs].autor, &listaPIs[qtdeTGs].orientador, listaPIs[qtdeTGs].titulo);
        listaPIs[qtdeTGs].qtdeVotos = 0;
        qtdeTGs++;
    }
    fclose(arquivo);
}

void lerArquivoComissao() {
    FILE *arquivo = fopen("comissao.txt", "r");
    if (arquivo == NULL) {
        printf("Arquivo comissao.txt faltando\n");
        exit(1);
    }
    fscanf(arquivo, "%d", &qtdeEleitores);
    for (int i = 0; i < qtdeEleitores; i++) {
        fscanf(arquivo, "%s", comissao[i].cpf);
        comissao[i].votou = false;
    }
    fclose(arquivo);
}

TG* encontrarPI(int codigo) {
    for (int i = 0; i < qtdeTGs; i++) {
        if (listaPIs[i].codigo == codigo) {
            return &listaPIs[i];
        }
    }
    return NULL;
}

Eleitor* encontrarEleitor(char cpf[15]) {
    for (int i = 0; i < qtdeEleitores; i++) {
        if (strcmp(comissao[i].cpf, cpf) == 0) {
            return &comissao[i];
        }
    }
    return NULL;
}

void votar() {
    if (!votacaoAtiva) {
        printf("A votacao esta suspensa!\n");
        return;
    }

    char cpf[15];
    printf("Digite o CPF do eleitor: ");
    scanf("%s", cpf);
    if (!verificarCPF(cpf)) {
        printf("CPF %s invalido\n", cpf);
        return;
    }

    Eleitor *eleitor = encontrarEleitor(cpf);
    if (eleitor == NULL) {
        printf("Eleitor nao encontrado na comissao\n");
        return;
    }

    if (eleitor->votou) {
        printf("Este eleitor ja votou\n");
        return;
    }

    int codigoPI;
    printf("Digite o codigo do PI: ");
    scanf("%d", &codigoPI);

    TG *pi = encontrarPI(codigoPI);
    if (pi == NULL) {
        printf("PI invalido\n");
        return;
    }

    pi->qtdeVotos++;
    eleitor->votou = true;
    eleitor->codigoTG = codigoPI;

    printf("Voto computado com sucesso!\n");
}

void suspenderVotacao() {
    votacaoAtiva = false;
    printf("Votacao suspensa!\n");
}

void retomarVotacao() {
    votacaoAtiva = true;
    printf("Votacao retomada!\n");
}

void gerarRelatorioParcial() {
    FILE *arquivo = fopen("parcial.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo parcial.txt\n");
        return;
    }
    int votosComputados = 0;
    for (int i = 0; i < qtdeEleitores; i++) {
        if (comissao[i].votou) {
            votosComputados++;
        }
    }
    fprintf(arquivo, "%d\n", votosComputados);
    for (int i = 0; i < qtdeEleitores; i++) {
        if (comissao[i].votou) {
            fprintf(arquivo, "%s %d\n", comissao[i].cpf, comissao[i].codigoTG);
        }
    }
    fclose(arquivo);
    printf("Relatorio parcial gerado com sucesso\n");
}

void gerarRelatorioFinal() {
    FILE *arquivo = fopen("resultado.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo resultado.txt\n");
        return;
    }

    int maxVotos = 0;
    for (int i = 0; i < qtdeTGs; i++) {
        if (listaPIs[i].qtdeVotos > maxVotos) {
            maxVotos = listaPIs[i].qtdeVotos;
        }
    }

    fprintf(arquivo, "PI vencedor(es):\n");
    for (int i = 0; i < qtdeTGs; i++) {
        if (listaPIs[i].qtdeVotos == maxVotos) {
            fprintf(arquivo, "Codigo: %d\nTitulo: %s\n", listaPIs[i].codigo, listaPIs[i].titulo);
        }
    }

    fprintf(arquivo, "Eleitores presentes:\n");
    for (int i = 0; i < qtdeEleitores; i++) {
        if (comissao[i].votou) {
            fprintf(arquivo, "%s\n", comissao[i].cpf);
        }
    }

    fprintf(arquivo, "Eleitores ausentes:\n");
    for (int i = 0; i < qtdeEleitores; i++) {
        if (!comissao[i].votou) {
            fprintf(arquivo, "%s\n", comissao[i].cpf);
        }
    }
    fclose(arquivo);
    printf("Relatorio final gerado com sucesso\n");
}

int main() {
    lerArquivoProfessores();
    lerArquivoAlunos();

    lerArquivoPI("PI_AMS.txt");
    lerArquivoPI("PI_ADS.txt");
    lerArquivoPI("PI_DSM.txt");
    lerArquivoPI("PI_CEX.txt");
    lerArquivoPI("PI_EMP.txt");
    lerArquivoPI("PI_LOG.txt");
    lerArquivoPI("PI_POL.txt");

    lerArquivoComissao();

    char opcao;
    do {
        printf("\nMENU1:\n");
        printf("a) Iniciar nova votacao\n");
        printf("b) Continuar votacao gravada\n");
        printf("c) Concluir votacao\n");
        printf("d) Suspender votacao\n");
        printf("e) Retomar votacao\n");
        printf("Escolha uma opcao: ");
        scanf(" %c", &opcao);

        switch (opcao) {
            case 'a':
            case 'A':
                votar();
                break;
            case 'b':
            case 'B':
                gerarRelatorioParcial();
                break;
            case 'c':
            case 'C':
                gerarRelatorioFinal();
                break;
            case 'd':
            case 'D':
                suspenderVotacao();
                break;
            case 'e':
            case 'E':
                retomarVotacao();
                break;
            default:
                printf("Opcao invalida\n");
        }
    } while (opcao != 'c' && opcao != 'C');

    return 0;
}