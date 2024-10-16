#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

sem_t *KO;
sem_t *duhan_sibice;
sem_t *papir_sibice;
sem_t *papir_duhan;
sem_t *stol_prazan;

int *s1;
int *s2;

int ID;

void brisi(int sig) { // oslobadanje memorije za ctrl+c
   (void)shmdt((sem_t *)KO);
   (void)shmdt((sem_t *)duhan_sibice);
   (void)shmdt((sem_t *)papir_sibice);
   (void)shmdt((sem_t *)papir_duhan);
   (void)shmdt((sem_t *)stol_prazan);
   (void)shmdt((int *)s1);
   (void)shmdt((int *)s2);

   (void)shmctl(ID, IPC_RMID, NULL);
   exit(0);
}

void trgovac() {
   while (1) {
      sleep(1);
      int sas1, sas2;
      // izaberi 2 sastojka
      sas1 = rand() % 3 + 1; // 1 - papir, 2 - duhan, 3 - sibice
      sas2 = rand() % 3 + 1;
      while (sas1 == sas2) {
         sas2 = rand() % 3 + 1;
      }
      *s1 = sas1;
      *s2 = sas2;

      sem_wait(KO);

      if (sas1 + sas2 == 3) {
         printf("Trgovac stavlja: papir i duhan\n");
      }
      if (sas1 + sas2 == 4) {
         printf("Trgovac stavlja: papir i sibice\n");
      }
      if (sas1 + sas2 == 5) {
         printf("Trgovac stavlja: duhan i sibice\n");
      }
      sem_post(KO);
      sem_post(duhan_sibice);
      sem_post(papir_sibice);
      sem_post(papir_duhan);

      sem_wait(stol_prazan);
   }
}

void pusac1() {
   int r1, r2; // trebaju mu duhan i sibice
   r1 = 2;
   r2 = 3;
   while (1) {
      sem_wait(duhan_sibice);
      sem_wait(KO);
      if ((r1 + r2) == (*s1 + *s2)) {
         // uzmi sastojke
         sem_post(KO);
         sem_post(stol_prazan);
         printf("Pusac 1: uzima sastojke, mota i pusi\n");
         printf("\n");
      } else {
         sem_post(KO);
      }
   }
}

void pusac2() {
   int r1, r2; // trebaju mu papir i sibice
   r1 = 1;
   r2 = 3;
   while (1) {
      sem_wait(papir_sibice);
      sem_wait(KO);
      if ((r1 + r2) == (*s1 + *s2)) {
         // uzmi sastojke
         sem_post(KO);
         sem_post(stol_prazan);
         printf("Pusac 2: uzima sastojke, mota i pusi\n");
         printf("\n");
      } else {
         sem_post(KO);
      }
   }
}

void pusac3() {
   int r1, r2; // trebaju mu papir i duhan
   r1 = 1;
   r2 = 2;
   while (1) {
      sem_wait(papir_duhan);
      sem_wait(KO);
      if ((r1 + r2) == (*s1 + *s2)) {
         // uzmi sastojke
         sem_post(KO);
         sem_post(stol_prazan);
         printf("Pusac 3: uzima sastojke, mota i pusi\n");
         printf("\n");
      } else {
         sem_post(KO);
      }
   }
}

int main() {

   printf("Pusac 1: ima papir\n");
   printf("Pusac 2: ima duhan\n");
   printf("Pusac 3: ima sibice\n");
   printf("\n");
   srand((unsigned int)time(NULL));
   ID = shmget(IPC_PRIVATE, sizeof(sem_t) * 5 + sizeof(int) * 2, 0600);
   // zauzimanje i raspodjela zajednicke memorije za 5 semafora i 2 integera
   KO = (sem_t *)shmat(ID, NULL, 0);
   duhan_sibice = (sem_t *)(KO + 1);
   papir_sibice = (sem_t *)(duhan_sibice + 1);
   papir_duhan = (sem_t *)(papir_sibice + 1);
   stol_prazan = (sem_t *)(papir_duhan + 1);
   s1 = (int *)(stol_prazan + 1);
   s2 = (int *)(s1 + 1);

   // postavljanje semafora, svi 0 osim KO

   sem_init(KO, 1, 1);
   sem_init(duhan_sibice, 1, 0);
   sem_init(papir_sibice, 1, 0);
   sem_init(papir_duhan, 1, 0);
   sem_init(stol_prazan, 1, 0);

   sigset(SIGINT, brisi); // ctrl + c oslobada memoriju

   // kreiranje procesa
   switch (fork()) {
   case -1:
      printf("Proces se nije mogao stvoriti :(\n");
   case 0:
      trgovac();
      exit(0);
   default:
      break;
   }

   switch (fork()) {
   case -1:
      printf("Proces se nije mogao stvoriti :(\n");
   case 0:
      pusac1();
      exit(0);
   default:
      break;
   }

   switch (fork()) {
   case -1:
      printf("Proces se nije mogao stvoriti :(\n");
   case 0:
      pusac2();
      exit(0);
   default:
      break;
   }

   switch (fork()) {
   case -1:
      printf("Proces se nije mogao stvoriti :(\n");
   case 0:
      pusac3();
      exit(0);
   default:
      break;
   }
   // cekanje kraja procesa
   for (int i = 0; i <= 4; i++) {
      wait(NULL);
   }

   // oslobadanje memorije
   shmdt((sem_t *)KO);
   shmdt((sem_t *)duhan_sibice);
   shmdt((sem_t *)papir_sibice);
   shmdt((sem_t *)papir_duhan);
   shmdt((sem_t *)stol_prazan);
   shmdt((int *)s1);
   shmdt((int *)s2);
   shmctl(ID, IPC_RMID, NULL);

   return 0;
}
