#include <stdio.h>
#include <stdlib.h>
#include <locale.h> 

typedef struct {
    float valorBruto;
    float valorLiquido;
    float valorIR;
    float valorPIS;
    float valorCOFINS;
    float valorCSLL;
    int dispensaRetencaoTotal; 
    int dispensaRetencaoIR;    
    int tipoOrgao; 
} ResultadoCalculo;


ResultadoCalculo calcularImpostos(float valorParaCalcular, int orgaoPublico) {
    const float ALIQUOTA_IR = 0.015;
    const float ALIQUOTA_IR_OP = 0.012; 
    const float ALIQUOTA_PIS = 0.0065;
    const float ALIQUOTA_COFINS = 0.03;
    const float ALIQUOTA_CSLL = 0.01;

    ResultadoCalculo resultado = {0}; 
    resultado.valorBruto = valorParaCalcular;
    resultado.valorLiquido = valorParaCalcular;
    resultado.tipoOrgao = orgaoPublico; 

    if (orgaoPublico == 1) { 
        resultado.valorIR = valorParaCalcular * ALIQUOTA_IR_OP;
        resultado.valorLiquido = valorParaCalcular - resultado.valorIR;
    } 
    else if (orgaoPublico == 2) { 
        float totalImpostos = (ALIQUOTA_IR + ALIQUOTA_PIS + ALIQUOTA_COFINS + ALIQUOTA_CSLL) * valorParaCalcular;

        if (totalImpostos < 10.0) {
            resultado.dispensaRetencaoTotal = 1; 
        } 
        else {
            float valorIRCalculado = valorParaCalcular * ALIQUOTA_IR;
            resultado.valorPIS = valorParaCalcular * ALIQUOTA_PIS;
            resultado.valorCOFINS = valorParaCalcular * ALIQUOTA_COFINS;
            resultado.valorCSLL = valorParaCalcular * ALIQUOTA_CSLL;

            if (valorIRCalculado < 10.0) {
                resultado.dispensaRetencaoIR = 1; 
                resultado.valorIR = 0; 
            } else {
                resultado.valorIR = valorIRCalculado;
            }
            float totalDescontos = resultado.valorIR + resultado.valorPIS + resultado.valorCOFINS + resultado.valorCSLL;
            resultado.valorLiquido = valorParaCalcular - totalDescontos;
        }
    }
    return resultado;
}


void imprimirDetalhamento(ResultadoCalculo resultado) {
    printf("\n--- Detalhamento do Calculo ---\n");
    printf("Valor Bruto: R$ %.2f\n", resultado.valorBruto);
    
    if (resultado.tipoOrgao == 1) {
        printf("Tributacao para Orgao Publico.\n");
        printf("Valor IR Retido (1.2%%): R$ %.2f\n", resultado.valorIR);
        printf("PIS/COFINS/CSLL: Nao se aplica retencao.\n");
    } 
    else if (resultado.tipoOrgao == 2) {
        if (resultado.dispensaRetencaoTotal) {
            printf("RETENCAO DISPENSADA: O valor total dos impostos e inferior a R$ 10.00.\n");
        } else {
            if (resultado.dispensaRetencaoIR) {
                printf("IR: Retencao dispensada (valor do imposto inferior a R$ 10.00).\n");
            } else {
                printf("Valor IR Retido (1.5%%): R$ %.2f\n", resultado.valorIR);
            }
            printf("Valor PIS Retido (0.65%%): R$ %.2f\n", resultado.valorPIS);
            printf("Valor COFINS Retido (3%%): R$ %.2f\n", resultado.valorCOFINS);
            printf("Valor CSLL Retido (1%%): R$ %.2f\n", resultado.valorCSLL);
        }
    }
    
    printf("-----------------------------------\n");
    printf("Valor Liquido a Receber: R$ %.2f\n", resultado.valorLiquido);
    printf("-----------------------------------\n");
}


void exibirHistorico(const ResultadoCalculo *hist, int tamanho) {
    system("cls"); 
    printf("--- HISTORICO DE CALCULOS ---\n\n");
    if (tamanho == 0) {
        printf("Nenhum calculo foi realizado ainda.\n");
    } else {
        for (int i = 0; i < tamanho; i++) {
            printf("--- Registro #%d ---\n", i + 1);
            printf("Tipo: %s\n", hist[i].tipoOrgao == 1 ? "Orgao Publico" : "Privado");
            printf("Valor Bruto:   R$ %.2f\n", hist[i].valorBruto);
            printf("Valor Liquido: R$ %.2f\n", hist[i].valorLiquido);
            printf("-----------------------\n\n");
        }
    }
}



int main() {
    // --- 2. CONFIGURAR A LOCALIDADE PARA PORTUGUÊS DO BRASIL ---
    // Esta chamada deve ser uma das primeiras coisas no seu programa.
    // "Portuguese" é o padrão para sistemas Windows.
    setlocale(LC_ALL, "Portuguese");

    // Variáveis para o controle do menu e do histórico
    int opcao = -1;
    ResultadoCalculo *historico = NULL; 
    int historico_tamanho = 0;   
    int historico_capacidade = 0; 

    do {
        system("cls");
        printf("========= CALCULADORA DE IMPOSTOS =========\n");
        printf("     (Valores decimais usam VIRGULA)\n"); // Aviso ao usuário
        printf("===========================================\n");
        printf("1. Fazer novo calculo\n");
        printf("2. Ver historico de calculos\n");
        printf("0. Sair\n");
        printf("===========================================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: { 
                float valor = 0;
                int esc;

                // Mensagem atualizada para guiar o usuário
                printf("\nDigite o valor a ser calculado: ");
                scanf("%f", &valor); // scanf agora espera uma vírgula!

                printf("E um orgao Publico? (1) para sim, (2) para nao: ");
                scanf("%d", &esc);

                if (esc != 1 && esc != 2) {
                    printf("\nOpcao invalida. Pressione qualquer tecla para continuar...\n");
                } else {
                    ResultadoCalculo resultadoAtual = calcularImpostos(valor, esc);
                    
                
                    imprimirDetalhamento(resultadoAtual); 

                    
                    if (historico_tamanho == historico_capacidade) {
                        historico_capacidade = (historico_capacidade == 0) ? 10 : historico_capacidade * 2;
                        ResultadoCalculo *temp = realloc(historico, historico_capacidade * sizeof(ResultadoCalculo));
                        
                        if (temp == NULL) {
                            printf("Erro critico de alocacao de memoria!\n");
                            free(historico);
                            return 1;
                        }
                        historico = temp;
                    }
                    historico[historico_tamanho] = resultadoAtual;
                    historico_tamanho++;
                    printf("Calculo salvo no historico.\n");
                }
                system("pause");
                break;
            }
           
             case 2: {
                exibirHistorico(historico, historico_tamanho);
                system("pause");
                break;
            }
            case 0:
                printf("\nSaindo do programa...\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                system("pause");
                break;
        }

    } while (opcao != 0);
    
    free(historico);
    historico = NULL;

    return 0;
}