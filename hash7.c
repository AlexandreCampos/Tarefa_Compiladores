/*
 * =====================================================================================
 *
 *       Filename:  hash.c
 *
 *    Description:  tabela hash - Compiladores
 *
 *        Version:  7.2
 *        Created:  08/30/16 10:23:00
 *       Revision:  13/09/16
 *       Compiler:  gcc
 *
 *         Author:  ALEXANDRE CAMPOS (), 
 *   Organization:  PUC-SP
 *
 * =====================================================================================
 */

#include<stdio.h>
#include <string.h>

#define n 113 // numero primo para o calculo da funcao hash
#define MAX_SECUNDARIA 226 
#define MAX_TABELA 236
#define MAX_PALAVRA 255

typedef struct {
	int colisao; 
	char simbolo[MAX_PALAVRA];
} Tabela;

int N = 113; // variavel global que percorre a area de colisao a procura do 1 indice livre

int consulta_Insere(Tabela *tabela, char x[], int acao, int debug);
int grava_Binario(Tabela *tabela, FILE *arq, char nome_arquivo[]);
int le_Binario(Tabela *tabela, FILE *arq, char nome_arquivo[]);
void imprime_tst(Tabela *tabela);
int funcao_debuga(char debug_escolha);


//int main (void )
int main (int argc, char *argv[])
{
			
	Tabela tst[MAX_TABELA];
	char nome_arquivo_incluir[MAX_PALAVRA]; //= "dados.txt";			// PASSADO VIA PARAMETRO	
	char nome_arquivo_consultar[MAX_PALAVRA]; //= "dados2.txt";			// PASSADO VIA PARAMETRO	
	char nome_arquivo_binario[MAX_PALAVRA]; //= "dados_binario";		// PASSADO VIA PARAMETRO
	char debug_escolha = 'S';
	
	strcpy (nome_arquivo_incluir,argv[1]);
	strcpy (nome_arquivo_consultar,argv[2]);
	strcpy (nome_arquivo_binario,argv[3]);
		
	int debugar;	
	int i, j;				
	int indice_retornado;
	char info[MAX_PALAVRA];
	FILE *arq;
			
	memset(&tst, '\0', sizeof(tst));
	debugar = funcao_debuga(debug_escolha);

	// inicializa "colisao" como -1
	for (i = 0; i < MAX_TABELA; i++)
	{
		tst[i].colisao = -1;
	}

	// inicializa 10 pontos arbitrarios para simbolos especiais e palavras reservadas 
	char palavras_reservadas[10][MAX_PALAVRA] = {"auto", "register", "goto", "--", "-", "==", "=", "+", "*", "&"};		
	for (i = 0; i < 10; i++)
	{
		strcpy(tst[i].simbolo, palavras_reservadas[i]);
	}

	// inicializa 10 entradas para tokens especiais no fim da TST
	char tokens_especiais[10][MAX_PALAVRA] = {"ID", "NUM", "FLOAT", "ALPHA", "EOF", "TOKEN6", "TOKEN7", "TOKEN8", "TOKEN9", "TOKEN10"};	
	j = 0;	
	for (i = MAX_SECUNDARIA; i < MAX_TABELA; i++)
	{
		strcpy(tst[i].simbolo, tokens_especiais[j]);
		j++;
	}

	// exibindo tabela hash vazia
	//imprime_tst(tst);

	/* le arquivo de entrada linha por vez
	** e chama funcao consulta_insere com acao 'I' e debug */		
	arq = fopen(nome_arquivo_incluir, "r");	
	if(arq == NULL)
		printf("Erro, nao foi possivel abrir o arquivo de insercao\n");
	else
	{
		while( (fgets(info, sizeof(info), arq))!=NULL )
		{
			info[strcspn(info, "\n")] = 0; // retira o /n de cada lexema
			indice_retornado = consulta_Insere( tst, info, 'I', debugar);
			if (debugar) printf("Indice = %3d \n", indice_retornado);
		}
		fclose(arq);
	}
	
	// gravacao de binario	
	grava_Binario(tst, arq, nome_arquivo_binario);
		
	// leitura de binario	
	Tabela nova[MAX_TABELA];
	le_Binario(nova, arq, nome_arquivo_binario);
	
	/* le arquivo de entrada linha por vez
	** e chama funcao consulta_insere com acao 'C' e debug */		
	arq = fopen(nome_arquivo_consultar, "r");	
	if(arq == NULL)
		printf("Erro, nao foi possivel abrir o arquivo de consulta\n");
	else
	{
		while( (fgets(info, sizeof(info), arq))!=NULL )
		{
			info[strcspn(info, "\n")] = 0; // retira o /n de cada lexema
			indice_retornado = consulta_Insere( nova, info, 'C', debugar);
			if (debugar) printf("Indice = %3d \n", indice_retornado);
		}
		fclose(arq);
	}
	
	// exibindo tabela hash ao final
	imprime_tst(nova);
	
	return(0);
}

int consulta_Insere(Tabela *tabela, char x[], int acao, int debug)
{

	int chave;
	switch ( acao )
	{

	// busca e insere	
	case 'I':

		//printf("Entrei no case I \n");
		if (debug) printf("Simbolo: %10s | ", x);
		// funcao hash		
		chave = (x[0] + x[1] + x[2]) % n;
		if (debug)
		{
			printf("F(x): %3d | ", chave);
			printf("Acao: %c | ", acao);
		}

		// se simbolo ja foi inserido
		if (strcmp(tabela[chave].simbolo, x) == 0)
		{	
			return chave;  
		}

		else
		{
			// se chave esta vazia
			if ( strlen(tabela[chave].simbolo) == 0)
			{
				// insere na area primaria
				strcpy(tabela[chave].simbolo, x);
				return chave;

			}
			else
			{
				// enquanto colisao diferente de-1   E    simbolo for diferente do x 
				while ( tabela[chave].colisao != -1 && strcmp(tabela[chave].simbolo, x) != 0)
				{
					chave = tabela[chave].colisao;
					//printf("chave - %d \n", chave);

				}

				// se x ja existe
				if (strcmp(tabela[chave].simbolo, x) == 0)
				{
					return chave;  
				}
				else
				{
					// insere na area secundaria
					tabela[chave].colisao = N;
					chave = N;
					strcpy(tabela[chave].simbolo, x);
					N = N + 1;
					return chave;
				}
			}
		}

		break;

	// apenas busca
	case 'C':
		
		//printf("Entrei no case C \n");
		if (debug )printf("Simbolo: %10s | ", x);
		// funcao hash		
		chave = (x[0] + x[1] + x[2]) % n;
		if (debug)
		{
			printf("F(x): %3d | ", chave);
			printf("Acao: %c | ", acao);
		}

		// se simbolo ja foi inserido
		if (strcmp(tabela[chave].simbolo, x) == 0)
		{
			return chave;  
		}

		else
		{
			// se chave esta vazia
			if ( strlen(tabela[chave].simbolo) == 0)
			{
				return -1;
			}
			else
			{
				// enquanto colisao diferente de-1   E    simbolo for diferente do x  
				while ( tabela[chave].colisao != -1 && strcmp(tabela[chave].simbolo, x) != 0)
				{
					chave = tabela[chave].colisao;
					//printf("chave - %d \n", chave);
				}

				// se x ja existe
				if (strcmp(tabela[chave].simbolo, x) == 0)
				{
					return chave;  
				}
				else
				{
					return -1;
				}
			}
		}

		break;

	default:
		printf("Erro ao executar a funcao de consulta_insercao \n");
	}

}

int grava_Binario(Tabela *tabela, FILE *arq, char nome_arquivo[])
{
	//printf("Entrei na funcao grava Binario \n");
	arq = fopen(nome_arquivo, "wb");
         if (arq == NULL)
         {
                printf("Erro na abertura do arquivo binario - gravacao\n");
                return 1;
         }
	
	if(fwrite(&tabela[0], sizeof(Tabela), MAX_TABELA, arq) != MAX_TABELA)
	{
    	printf("Erro na escrita do arquivo binario\n");
	}	
	else
	{
		printf("Arquivo binario gravado com sucesso \n");
	}	
		
	fclose(arq);
	
	return 0;
}


int le_Binario(Tabela *tabela, FILE *arq, char nome_arquivo[])
{
	arq = fopen(nome_arquivo, "rb");
	if (arq == NULL)
         {
                printf("Erro na abertura do arquivo binario - leitura \n");
                return 1;
         }
	
	int result;
	result = fread (&tabela[0], sizeof(Tabela), MAX_TABELA, arq);
  	printf("Nro de elementos lidos: %d\n", result);
	fclose(arq); 
	
	return 0;
}


void imprime_tst(Tabela *tabela)
{
	int i;
	for (i = 0; i < MAX_TABELA; i++)
	{
		printf(" %3d - %10s - %3d \n", i, tabela[i].simbolo, tabela[i].colisao );
	}	
}


int funcao_debuga(char opcao)
{
	switch ( opcao )
	{
		
	case 'S':

		return 1;
		break;

	case 'N':		
	
		return 0;
		break;
		
	}
}
