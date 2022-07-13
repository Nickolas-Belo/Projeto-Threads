#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <locale.h>
#include "palavras.h"

int logado;

// Variáveis globais da forca
int jogar = 0, erros = 0, contem = 0;
char letra;

// Variáveis globais do termo
int errado = 0;
char palavra_usuario[5];

// Struct que guarda os dados do jogador atual
typedef struct {
  char usuario[20];
  char senha[20];
  int vitorias_forca;
  int derrotas_forca;
  int vitorias_termo;
  int derrotas_termo;
} Jogador;

Jogador player;

// Função de login
void *login(void *param) {
  // Ponteiro de arquivo
	FILE *arq;
  // Abre o arquivo no modo leitura
	arq = fopen("usuarios.dat", "rb");

  // Se o arquivo existe, procede com o login
	if (arq) {
    do {
      printf("Usuario: ");
      fgets(player.usuario, 20, stdin);
    } while (strcmp(player.usuario, "\n") == 0);
  	do {
    	printf("Senha: ");
    	fgets(player.senha, 20, stdin);
    } while (strcmp(player.senha, "\n") == 0);

    // Jogador temporário para guardar dados lidos do arquivo
    Jogador temp;

    // Laço para verificação do usuário e senha
    while (fread(&temp, sizeof(Jogador), 1, arq)) {
      if (strcmp(player.usuario, temp.usuario) == 0 && strcmp(player.senha, temp.senha) == 0) {
        player = temp;
        logado = 1;
        printf("\nLogin bem sucedido\n\n");
        fclose(arq);
        break;
      }
    }

    // Caso o loop acabe e a variável logado ainda vale 0, imprime a mensagem:
    if (logado == 0) {
      printf("\nUsuario ou senha incorretos!\n");
  	  fclose(arq);
    }
	}
    
  // Caso o arquivo não exista, imprime a mensagem:
  else {
    printf("\nNenhum usuario cadastrado!\n");
		fclose(arq);
  }

  // Sai da thread
  pthread_exit(0);
}

// Função de cadastro
void *cadastro(void *param) {
  // Ponteiro de arquivo
	FILE *arq;
  // Abre o arquivo no modo leitura
	arq = fopen("usuarios.dat", "rb");

  // Se o arquivo não existe, cria um usando o modo append + leitura
	if (!arq) {
		fclose(arq);
		arq = fopen("usuarios.dat", "wb+");
	}

  // Jogador temporário para guardar dados lidos do arquivo e verificar se nome de usuário está disponível
	int disp;
  Jogador temp;

  // Laço que pede para o usuário inserir um nome de usuário enquanto o nome não estiver disponível
	do {
    do {
      printf("Usuario: ");
      fgets(player.usuario, 20, stdin);
    } while (strcmp(player.usuario, "\n") == 0);
		
    // Laço para verificação da disponibilidade de nome de usuário
    disp = 1;
    while (fread(&temp, sizeof(Jogador), 1, arq)) {
      // Se o usuário já existe, a variável disp passa a valer 0 e imprime a mensagem:
      if (strcmp(player.usuario, temp.usuario) == 0) {
        printf("\nEste nome já está sendo usado\n\n");
				disp = 0;
      }
    }

    // Reposiciona o indicador de posição de fluxo no início do arquivo
		fseek(arq, 0, SEEK_SET);
	} while (disp == 0);
	fclose(arq);

  do {
  	printf("Senha: ");
  	fgets(player.senha, 20, stdin);
  } while (strcmp(player.senha, "\n") == 0);

  // Escreve o usuário e senha no arquivo, em modo append
	arq = fopen("usuarios.dat", "ab");
  fwrite(&player, sizeof(Jogador), 1, arq);
	fclose(arq);
	printf("\nCadastro realizado com sucesso!\n");

  // Sai da thread
  pthread_exit(0);
}

// Função para sortear uma posição do vetor de strings
int sorteio() {
	srand(time(NULL));
	int pos = rand() % 20;
	return pos;
}

// Função para imprimir a forca
void imprimeForca(char *vetor, int tam) {
	int i;

  // Desenha a forca e as partes do boneco, conforme o número de erros
	printf("   _______      \n");
  printf("  |       |     \n");
  printf("  |       %c    \n", erros>=1?'O':' ');
  printf("  |      %c%c%c \n", erros>=3?'/':' ', erros>=2?'|':' ', erros>=4?'\\':' ');
  printf("  |       %c    \n", erros>=2?'|':' ');
  printf("  |      %c %c  \n", erros>=5?'/':' ', erros>=6?'\\':' ');
  printf("  |             \n");
  printf("  |  ");

  // Imprime os caracteres do vetor
  for (i = 0; i < tam; i++)
    printf(" %c", vetor[i]);

  printf("\n");
}

// Função do jogo da forca
void *forca(void *param) {
  // Cria um vetor de palavras, usando a função escolheNivel do "palavras.h"
  int i = 0;
  char **palavras = escolheNivel();

  // Laço para repetir o jogo enquanto a variável jogar valer 1
  do {
    // Sorteia a palavra do vetor e copia para a variável
    char palavra[12];
    int pos = sorteio();
    strcpy(palavra, palavras[pos]);

    // Reseta a contagem de erros
    erros = 0;

    // Variável que guarda o tamanho da palavra
    int tam = strlen(palavra);

    // Cria um novo vetor, com o mesmo tamanho da palavra, substituindo os caracteres por '_'
    char vetor[tam];
    for (i = 0; i < tam; i++)
      vetor[i] = '_';

    imprimeForca(vetor, tam);

    while (1) {
      printf("\nInsira uma letra: ");
      scanf(" %c", &letra);

      // Verifica se a palavra contém a letra digitada pelo usuário
      contem = 0;
      for (i = 0; i < tam; i++) {
        // Se a letra estiver correta, substitui ela no vetor
        if (palavra[i] == letra) {
          vetor[i] = letra;
          contem = 1;
        }
      }

      // Se a palavra não contém a letra aumenta a contagem de erros
      if (contem == 0) {
        erros++;
        imprimeForca(vetor, tam);
        // Caso o número de erros chegue em 6, o jogador perde
        if (erros >= 6) {
          player.derrotas_forca++;
          printf("\nVocê foi enforcado!\n");
          printf("A palavra era: %s\n", palavra);
          printf("Total de derrotas: %d\n\n", player.derrotas_forca);
          printf("Deseja jogar novamente? Digite 1 para continuar e 0 para sair\n\n>> ");
          scanf("%d", &jogar);
          break;
        }
      } else
        imprimeForca(vetor, tam);

      // Se o vetor foi substituído com todas as letras da palavra, o jogador vence 
      if (strncmp(palavra, vetor, tam) == 0) {
        player.vitorias_forca++;
        printf("\nVocê ganhou!!!\n");
        printf("Total de vitórias: %d\n\n", player.vitorias_forca);
        printf("Deseja jogar novamente? Digite 1 para continuar ou 0 para sair\n\n>> ");
        scanf("%d", &jogar);
        break;
      }
    }
	} while (jogar == 1);

  // Ponteiro de arquivo
  FILE *arq;
  // Abre o arquivo no modo leitura + escrita
  arq = fopen("usuarios.dat", "rb+");
  
  i = 0;
  Jogador jogadores[50];

  while (fread(&jogadores[i], sizeof(Jogador), 1, arq)) {
    if (strcmp(player.usuario, jogadores[i].usuario) == 0)
      break;
    i++;
  }

  fseek(arq, sizeof(Jogador) * i, SEEK_SET);
  fwrite(&player, sizeof(Jogador), 1, arq);
  fclose(arq);

  // Sai da thread
  pthread_exit(0);
}

// Função do jogo termo
void *termo(void *param) {
  // Palavras do jogo que serão sorteadas
  char termos[20][20] = {"nobre", "fazer", "ideia", "poder", "moral", "honra", "justo", "anexo", "sonho", "amigo", "casal", "tempo", "posse", "causa", "dever", "saber", "digno", "mundo", "pauta", "censo"};

  // Inicialização das variáveis locais
  char termo[6][5], letras_erradas[23];
  int i = 0, j = 0, k = 0, l = 0, repetida = 0, contador = 0, conta_letra = 0, conta_verde = 0, conta_amarelo = 0;

  int indice = 0;
  indice = sorteio();
  char* palavra_aleatoria = termos[indice];

  // Laço de repetição das 6 tentativas que o usuário tem para acertar a palavra, enquanto o usuário não acertar a palavra e ainda tiver tentativas sobrando, esse laço continuará rodando
  for(i=0; i<6; i++){
    // Solicitação da palavra do usuário para depois o programa verificar se a palavra é igual a palavra sorteada
    printf("\t\tDigite uma palavra\n\n");
    printf("\t\t");
    scanf("%s", palavra_usuario);
    printf("\n");
    
    // Laço que compara cada caracter da palavra digitada pelo usuário com o caracter de mesma posição do vetor onde está a palavra gerada pelo programa 
    for(j=0; j<strlen(palavra_usuario); j++){
      // Posição atual da matriz recebe o caracter atual do usuário
      termo[i][j] = palavra_usuario[j];

      // Se o caracter atual da palavra do usuário for igual ao caracter atual da palavra gerada pelo programa, a posição da matriz onde fica esse caracter será pintado de verde
      if(palavra_usuario[j] == palavra_aleatoria[j]){
         printf("\033[42m");
         printf("%c", termo[i][j]);
         printf("\033[0m"); 

        // Conta letra, conta os caracteres que o usuário acertou e conta_verde os caracteres certos que serão pintados de verde
         conta_letra++;
         conta_verde++;
      }

      // Condição para verificar se o caracter atual do usuário é diferente do caracter atual do programa, se for, verifico se tem algum caracter da palavra do programa que seja igual ao caracter atual do usuário, se tiver eu pinto de amarelo o caracter atual do usuário, assim indicando pra ele que essa letra não existe na posição que ele colocou, mas existe em outra posição. Se algum caracter tiver sido pintado de amarelo a variável conta_amarelo é incrementada 
      else{
        for(k=0; k<strlen(palavra_usuario); k++){
          if(palavra_usuario[j] == palavra_aleatoria[k]){
            printf("\033[43m");
            printf("%c", termo[i][j]);
            printf("\033[0m");
            
            conta_amarelo++;
            break;
          }
        }  
      }

      // Se a letra atual que estou verificando não tiver sido pintada de verde ou amarelo, verifico se não tem mesmo nenhum caracter igual ao caracter atual do usuário na palavra gerada pelo programa
      if(conta_verde == 0 && conta_amarelo == 0){
        for(k=0; k<strlen(palavra_usuario); k++){
          if(palavra_usuario[j] != palavra_aleatoria[k]){
            // Se na palavra aleatória o caracter atual for diferente do caracter atual do usuário, a variável errado incrementa 
            errado++;
          }
        }  

        // Se todas as letras da palavra do programa forem diferentes da letra atual do usuário, eu coloco a letra errada em um vetor e a variável contador contabiliza que há uma nova letra errada
        if(errado == 5){
          repetida = 0;
          for(l=0; l<contador; l++) {
            if (palavra_usuario[j] == letras_erradas[l])
              repetida = 1;
          }
          if (repetida == 0) {
            letras_erradas[contador] = palavra_usuario[j];
            contador++;
          }
        }

        // Faço errado valer 0 para depois poder fazer a verificação da próxima letra do usuário
        errado= 0;
        // Imprimo a letra errada do usuário
        printf("%c", termo[i][j]);
      }

      // Conta_verde e conta_amarelo recebem 0 para depois poderem fazer uma nova verificação no laço
      conta_verde = 0;
      conta_amarelo = 0;
    }

    // Se a palavra que o usuário digitou for igual a palavra do programa, eu contabilizo como vitória e mostro para o usuário que ele ganhou, assim encerro o jogo 
    if(conta_letra == 5){
        player.vitorias_termo++;
        printf("\n\nVocê ganhou\n");
        printf("Total de vitórias: %d\n\n", player.vitorias_termo);
        break;
    }

    // Conta_letra recebe 0 para depois conseguir fazer uma nova verificação no laço
    conta_letra = 0;

    // Imprime as letras erradas, que não aparecem nenhuma na vez na palavra
    printf("\tLetras erradas: ");
    for(j=0; j<contador; j++){
      printf("%c ",letras_erradas[j]);
    }
    // Imprime o número de erros
    printf("\tNúmero de erros: %d\n\n",contador);
  }

  // Caso o jogador não acerte a palavra em 6 tentativas, uma derrota é contabilizada, a palavra aleatória é mostrada e o jogo é encerrado
  if (i == 6) {
    player.derrotas_termo++;
    printf("Você perdeu\n");
    printf("A palavra era: %s\n", palavra_aleatoria);
    printf("Total de derrotas: %d\n", player.derrotas_termo);
  }

  // Ponteiro de arquivo
  FILE *arq;
  // Abre o arquivo no modo leitura + escrita
  arq = fopen("usuarios.dat", "rb+");
  
  i = 0;
  Jogador jogadores[50];

  while (fread(&jogadores[i], sizeof(Jogador), 1, arq)) {
    if (strcmp(player.usuario, jogadores[i].usuario) == 0)
      break;
    i++;
  }

  fseek(arq, sizeof(Jogador) * i, SEEK_SET);
  fwrite(&player, sizeof(Jogador), 1, arq);
  fclose(arq);
  
  pthread_exit(0);
}

// Função para comparação da pontuação dos jogadores (Forca)
int comparaForca(Jogador *x, Jogador *y) {
  Jogador *a = (void *) x, *b = (void *) y;
    
  if (a->vitorias_forca == b->vitorias_forca) {
    if (a->derrotas_forca == b->derrotas_forca)
      return 0;
    else if (a->derrotas_forca > b->derrotas_forca)
      return 1;
    else
      return -1;
  }
  else if (a->vitorias_forca > b->vitorias_forca)
    return -1;
  else
    return 1;
}

int comparaTermo(Jogador *x, Jogador *y) {
  Jogador *a = (void *) x, *b = (void *) y;
    
  if (a->vitorias_termo == b->vitorias_termo)
    if (a->derrotas_termo == b->derrotas_termo)
      return 0;
    else if (a->derrotas_termo > b->derrotas_termo)
      return 1;
    else
      return -1;
  else if (a->vitorias_termo > b->vitorias_termo)
    return -1;
  else
    return 1;
}

// Função do placar
void *placar(void *param) {
  // Ponteiro de arquivo
  FILE *arq;
  // Abre o arquivo no modo leitura + escrita
  arq = fopen("usuarios.dat", "rb");

  int i = 0, j = 0, tam = 0;
  Jogador jogadores[50];
  
  while (fread(&jogadores[tam], sizeof(Jogador), 1, arq))
    tam++;
  
  fclose(arq);
  
  int escolha;
    do {
      printf("De qual jogo você quer ver o placar:\n\n1 - Forca\n2 - Termo\n0 - Voltar\n\n>> ");
      scanf("%d", &escolha);
    } while (escolha < 0 || escolha > 2);

// Verifica a escolha do usuário e cria uma thread para o jogo escolhido
  switch (escolha) {
    case 1:
      printf("\nRanking (Forca):\n\n");
      qsort(jogadores, tam, sizeof(Jogador), comparaForca);

      printf("*============================*\n");
      printf("   %d - %s   Vitórias: %d\tDerrotas: %d\n", i + 1, jogadores[0].usuario, jogadores[0].vitorias_forca, jogadores[0].derrotas_forca);
      printf("*============================*\n");
      
      for (i = 1; i < tam; i++) {
        if (jogadores[i].vitorias_forca > 0 || jogadores[i].derrotas_forca > 0) {
          printf("   %d - %s   Vitórias: %d\tDerrotas: %d\n", i + 1, jogadores[i].usuario, jogadores[i].vitorias_forca, jogadores[i].derrotas_forca);
          printf("------------------------------\n");
        }
      }
      break;
    case 2:
      printf("\nRanking (Termo):\n\n");
      qsort(jogadores, tam, sizeof(Jogador), comparaTermo);

      printf("*============================*\n");
      printf("   %d - %s   Vitórias: %d\tDerrotas: %d\n", i + 1, jogadores[0].usuario, jogadores[0].vitorias_termo, jogadores[0].derrotas_termo);
      printf("*============================*\n");
      
      for (i = 1; i < tam; i++) {
        if (jogadores[i].vitorias_termo > 0 || jogadores[i].derrotas_termo > 0) {
          printf("   %d - %s   Vitórias: %d\tDerrotas: %d\n", i + 1, jogadores[i].usuario, jogadores[i].vitorias_termo, jogadores[i].derrotas_termo);
          printf("------------------------------\n");
        }
      }
      break;
    default:
      return 0;
  }
  
  pthread_exit(0);
}

// Função para criação de threads, usando um ponteiro para as funções a serem executadas
void criaThread(void* ptrFuncao) {
  pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int threadStatus;

  // Cria uma thread da função passada como parâmetro
	threadStatus = pthread_create(&tid, &attr, ptrFuncao, NULL);

  // Fecha o programa caso não consiga criar a thread
  if (threadStatus != 0) {
		printf("Erro ao criar a thread\n");
		exit(EXIT_FAILURE);
	}

  // Finaliza a thread
  threadStatus = pthread_join(tid, NULL);

  // Fecha o programa caso a thread não consiga ser finalizada
  if (threadStatus != 0) {
    printf("Erro ao aguardar finalização da thread\n");
    exit(EXIT_FAILURE);
  }
}

// Função para escolha dos jogos
int menu() {
  while (1) {
  	int escolha;
    	do {
    		printf("Escolha um jogo:\n\n1 - Forca\n2 - Termo\n3 - Placar\n0 - Voltar\n\n>> ");
    		scanf("%d", &escolha);
        getchar();
    	} while (escolha < 0 || escolha > 3);

  // Verifica a escolha do usuário e cria uma thread para o jogo escolhido
    switch (escolha) {
  		case 1:
        printf("\n");
        criaThread(forca);
  			break;
  		case 2:
        printf("\n");
        criaThread(termo);
  			break;
      case 3:
        printf("\n");
        criaThread(placar);
  			break;
  		default:
        player.vitorias_forca = 0;
        player.derrotas_forca = 0;
        player.vitorias_termo = 0;
        player.derrotas_termo = 0;
        logado = 0;
  			return 0;
	  }

    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");

  // Tela de login e cadastro
  while (1) {
    printf("Selecione uma opção:\n\n");
		int escolha;
    do {
      printf("1 - Login\n2 - Cadastro\n0 - Sair\n\n>> ");
      scanf("%d", &escolha);
      getchar();
    } while (escolha < 0 || escolha > 2);

    // Verifica a escolha do usuário e cria uma thread para a função escolhida
		switch (escolha) {
			case 1:
        printf("\n");
				criaThread(login);
        // Chama a função menu, se o login for bem sucecido
        if (logado == 1)
					menu();
				break;
			case 2:
        printf("\n");
				criaThread(cadastro);
				break;
			default:
				return 0;
		}
		
		printf("\n");
	}
}

