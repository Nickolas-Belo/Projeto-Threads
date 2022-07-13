#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **escolheNivel() {
  int i, nivel;
  
  char facil[20][12] = {"cachorro", "gato", "leao", "girafa", "elefante", "lobo", "morcego", "coelho", "tartaruga", "pinguim", "cavalo", "golfinho", "urso", "aranha", "baleia", "vaca", "lince", "tigre", "tubarao", "camelo"};
  
  char normal[20][12] = {"programador", "engenheiro", "medico", "professor", "advogado", "vendedor", "veterinario", "motorista", "pedreiro", "floricultor", "mecanico", "eletricista", "cientista", "piloto", "astronauta", "lixeiro", "policial", "bombeiro", "arquiteto", "psicologo"};
  
  char dificil[20][15] = {"brasil", "mexico", "argentina", "estados unidos", "canada", "inglaterra", "italia", "franca", "alemanha", "suecia", "japao", "china", "india", "cingapura", "camaroes", "nigeria", "egito", "gana", "australia", "nova zelandia"};

  do {
    printf("Digite o nível de dificuldade:\n\n 1 - Fácil (Nome de Animais)\n 2 - Normal (Nome de Profissões)\n 3 - Difícil (Nome de Países) \n\n>> ");
    scanf("%d", &nivel);
  } while (nivel < 1 || nivel > 3);

  char **ptr = (char**)malloc(sizeof(char*) * 20);
  for (i = 0; i < 20; i++) {
    ptr[i] = (char*)malloc(sizeof(char) * 15);
    if (nivel == 1)
      strcpy(ptr[i], facil[i]);
    else if (nivel == 2)
      strcpy(ptr[i], normal[i]);
    else
      strcpy(ptr[i], dificil[i]);
  }

  return ptr;
}

