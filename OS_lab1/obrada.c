#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define N 6

int cekanje[N] = {0};
int prioritet[N] = {0};
int tekuci_prioritet = 0;
// uzlazno po prioritetu
int signali[] = {SIGURG, SIGABRT, SIGCONT, SIGCHLD, SIGINT};

void zabrani_prekide() {
   int i;
   for (i = 0; i < N - 1; i++)
      sighold(signali[i]);
}

void dozvoli_prekide() {
   int i;
   for (i = 0; i < N - 1; i++) {
      sigrelse(signali[i]);
   }
}
// trosenje vremena i ispis tijekom obrade od nizeg prior prema visem
void obrada_signala(int sig) {
   int i;
   switch (sig) {
   case SIGURG:
      printf(" -  P  -  -  -  -\n");

      for (i = 1; i < N; i++) { // za svaki 5 sekundi
         sleep(1);
         printf(" -  %d  -  -  -  -\n", i);
      }
      printf(" -  K  -  -  -  -\n");
      break;

   case SIGABRT:
      printf(" -  -  P  -  -  -\n");

      for (i = 1; i < N; i++) {
         sleep(1);
         printf(" -  -  %d  -  -  -\n", i);
      }
      printf(" -  -  K  -  -  -\n");
      break;

   case SIGCONT:
      printf(" -  -  -  P  -  -\n");

      for (i = 1; i < N; i++) {
         sleep(1);
         printf(" -  -  -  %d  -  -\n", i);
      }
      printf(" -  -  -  K  -  -\n");
      break;

   case SIGCHLD:
      printf(" -  -  -  -  P  -\n");

      for (i = 1; i < N; i++) {
         sleep(1);
         printf(" -  -  -  -  %d  -\n", i);
      }
      printf(" -  -  -  -  K  -\n");
      break;

   case SIGINT:
      printf(" -  -  -  -  -  P\n");

      for (i = 1; i < N; i++) {
         sleep(1);
         printf(" -  -  -  -  -  %d\n", i);
      }
      printf(" -  -  -  -  -  K\n");
      break;
   }
}

void prekidna_rutina(int sig) {

   int i, broj, x = 0;
   zabrani_prekide();
   // ispisivanje pojave prekida
   switch (sig) {
   case SIGURG:
      broj = 1;
      printf(" -  X  -  -  -  -\n");
      break;
   case SIGABRT:
      broj = 2;
      printf(" -  -  X  -  -  -\n");
      break;
   case SIGCONT:
      broj = 3;
      printf(" -  -  -  X  -  -\n");
      break;
   case SIGCHLD:
      broj = 4;
      printf(" -  -  -  -  X  -\n");
      break;
   case SIGINT:
      broj = 5;
      printf(" -  -  -  -  -  X\n");
      break;
   }
   cekanje[broj]++;
   // trazi ima li nesto viseg prioriteta
   do {
      x = 0;
      for (i = tekuci_prioritet + 1; i < N; i++) {
         if (cekanje[i] == 1)
            x = i;
      }

      if (x > 0) {
         // obradivanje signala prekida
         cekanje[x] = 0;
         prioritet[x] = tekuci_prioritet;
         tekuci_prioritet = x;

         dozvoli_prekide();
         obrada_signala(signali[x - 1]);
         zabrani_prekide();

         tekuci_prioritet = prioritet[x];
      }
   } while (x > 0);
   dozvoli_prekide();
}

int main(void) {
   int i = 0;

   sigset(SIGURG, prekidna_rutina);
   sigset(SIGABRT, prekidna_rutina);
   sigset(SIGCONT, prekidna_rutina);
   sigset(SIGCHLD, prekidna_rutina);
   sigset(SIGINT, prekidna_rutina);

   printf("Proces obrade prekida, PID=%ld\n", getpid());
   printf("GP S1 S2 S3 S4 S5\n");
   printf("-----------------\n");

   while (1) {
      ++i;
      printf("%d  -  -  -  -  -\n", i);
      sleep(1);                                    
   }
   return 0;
}
