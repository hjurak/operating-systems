#include <ctype.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int ShmId;
int M;
int *A;

void proces() { // Funkcija uvećavanja A za svaki novi proces koji je stvoren
   int j;
   for (j = 0; j < M; j++) {
      *A += 1;
   }
}

int main(int argc, char *argv[]) {

   int i, j, N;
   N = atoi(argv[1]); // prvi arg je N, drugi M
   M = atoi(argv[2]);

   ShmId = shmget(IPC_PRIVATE, sizeof(int), 0600);

   if (ShmId == -1)
      exit(1); // greska - nema zajednicke memorije

   A = (int *)shmat(ShmId, NULL, 0); // zajednicka varijabla

   for (i = 0; i < N; i++) {
      switch (fork()) {
      case 0:
         proces(); // sve ok, proces stvoren, pozivamo funk proces()
         exit(0);
      case -1:
         printf("Proces se nije uspio stvoriti :("); // nije ok, nema stvorenog procesa
         exit(0);
         /* default: printf("Stvoren proces");
                 break;*/
      }
   }
   for (j = 0; j < N; j++) { // cekanje kraja procesa

      wait(NULL);
   }
   printf("A=%d\n", *A);
   shmdt((char *)A);
   shmctl(ShmId, IPC_RMID, NULL); // brisanje i oslobadanje memorije
   return 0;
}