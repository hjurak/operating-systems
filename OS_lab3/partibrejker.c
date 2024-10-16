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
#include <unistd.h>

int id = 0;
int stud_uk, stud_u_sobi;
int partibr_u_sobi;

pthread_mutex_t m; // globalne var za monitor
pthread_cond_t red;

void *student(void *p) {
   int n = ++id;
   stud_uk++;

   usleep((rand() % 401 + 100) * 1000); // spavaj izmedu 100 i 500 ms

   for (int i = 0; i < 3; i++) { // ponavljaj 3 puta
      pthread_mutex_lock(&m);
      while (partibr_u_sobi) {
         pthread_cond_wait(&red, &m); // udi u sobu ako nema partibrejkera
      }
      stud_u_sobi++;

      printf("Student %d je ušao u sobu\n", n);
      // ispis da je student usao u  sobu
      pthread_mutex_unlock(&m);
      usleep((rand() % 1001 + 1000) * 1000); // zabavi se

      pthread_mutex_lock(&m);
      stud_u_sobi--; // izadi iz sobe
      printf("Student %d je izašao iz sobe\n", n);

      pthread_mutex_unlock(&m);
      usleep((rand() % 1001 + 1000) * 1000); // odspavaj
   }
   pthread_mutex_lock(&m);
   stud_uk--;
   pthread_mutex_unlock(&m);

   return NULL;
}

void *partibrejker(void *p) {
   while (stud_uk > 0) {                   // dok ima studenata u sustavu
      usleep((rand() % 901 + 100) * 1000); // spavaj izmedu 100 i 1000 ms
      if (stud_u_sobi >= 3) {
         pthread_mutex_lock(&m);
         partibr_u_sobi = 1; // udi u sobu ako je u sobi 3 ili vise studenata
         printf("Partibrejker je ušao u sobu\n");
         pthread_mutex_unlock(&m);

         while (stud_u_sobi > 0) {
         } // cekanje da se soba isprazni
         ;
         pthread_mutex_lock(&m);
         partibr_u_sobi = 0; // partibrejker je izasao iz sobe
         printf("Partibrejker je izašao iz sobe\n");
         pthread_cond_broadcast(&red);
         pthread_mutex_unlock(&m);
      }
   }
   return NULL;
}
/* zadatak:

Dretva student(K)
{
   spavaj X milisekundi; // X je slučajan broj između 100 i 500
   ponavljaj 3 puta {
      uđi u sobu za zabavu ako u sobi nema partibrejkera;
      ispiši "Student K je ušao u sobu";
      zabavi se; // spavaj X milisekundi gdje je X slučajan broj između 1000 i
2000
      izađi iz sobe za zabavu;
      ispiši "Student K je izašao iz sobe";
      odspavaj X milisekundi; // X je slučajan broj između 1000 i 2000
   }
}

Dretva partibrejker()
{
   dok ima studenata u sustavu {
      spavaj X milisekundi; // X je slučajan broj između 100 i 1000
      uđi u sobu za zabavu ako su u sobi 3 ili više studenata;
      ispiši "Partibrejker je ušao u sobu";
      izađi iz sobe ako u sobi nema više studenata;
      ispiši "Partibrejker je izašao iz sobe";
   }
} */

int main(int argc, char *argv[]) {
   int broj_studenata;
   broj_studenata = atoi(argv[1]);
   // inicijalizacija globalnih varijabli
   stud_u_sobi = 0;
   partibr_u_sobi = 0;
   stud_uk = 0;
   srand((unsigned int)time(NULL));

   // inicijalizacija monitora
   pthread_t t_id;
   pthread_attr_t attr;

   pthread_mutex_init(&m, NULL);
   pthread_cond_init(&red, NULL);

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   if (pthread_create(&t_id, &attr, student, NULL)) {
      printf("Dretva nije mogla biti stvorena :(\n");
      exit(1);
   } // stvori dretvu student
   sleep(2);

   if (pthread_create(&t_id, &attr, partibrejker, NULL)) {
      printf("Dretva nije mogla biti stvorena :(\n");
      exit(1);
   } // stvori dretvu patribrejker
   sleep(2);

   // stvaranje onoliko dretvi studenata koliko je
   // uneseno na pocetku
   while (id < broj_studenata) {
      if (pthread_create(&t_id, &attr, student, NULL)) {
         printf("Dretva nije mogla biti stvorena :(\n");
         exit(1);
      }
      sleep(2);
   }

   while (stud_uk > 0) {
      sleep(1);
   }

   pthread_mutex_destroy(&m); // unistavanje monitora
   pthread_cond_destroy(&red);

   return 0;
}
