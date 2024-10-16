#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int t; // simulacija vremena (trenutno vrijeme)

struct dretva {
   int id;   // 1, 2, 3, ...
   int p;    // preostalo vrijeme rada
   int prio; // prioritet
   int rasp; // način raspoređivanja
};

#define MAX_DRETVI 5
struct dretva *P[MAX_DRETVI]; // red pripravnih dretvi, P[0] = aktivna dretva

/* podaci o događajima pojave novih poslova - dretvi */
#define DRETVI 6
int nove[DRETVI][5] = {
    /* trenutak dolaska, id, p, prio, rasp (prio i rasp se koriste ovisno o
       rasporedivacu) */
    {1, 3, 5, 3, 1}, /* rasp = 0 => PRIO+FIFO; 1 => PRIO+RR */
    {3, 5, 6, 5, 1},  {7, 2, 3, 5, 0},  {12, 1, 5, 3, 0},
    {20, 6, 3, 6, 1}, {20, 7, 4, 7, 1},
};

void ispis_stanja(int ispisi_zaglavlje) {
   int i;

   if (ispisi_zaglavlje) {
      printf("  t    AKT");
      for (i = 1; i < MAX_DRETVI; i++)
         printf("     PR%d", i);
      printf("\n");
   } else {

      printf("%3d ", t);
      for (i = 0; i < MAX_DRETVI; i++)
         if (P[i] != NULL && P[i]->p != 0) // ako dretva nema preostalog vremena
                                           // ona je gotova i ispisuju se -/-/-
            printf("  %d/%d/%d ", P[i]->id, P[i]->prio, P[i]->p);
         else
            printf("  -/-/- ");
      printf("\n");
   }
}

int main() {
   int threadNum = 0; // broj dretvi
   int header = 1;
   t = 0;
   int kvant = 1;
   int ima = DRETVI; // za while

   struct dretva *temp;

   for (int i = 0; i < MAX_DRETVI; i++) { // zauzimanje mjesta za dretve u P
      P[i] = malloc(sizeof(struct dretva));
      if (P[i] == NULL) {
         printf("Malloc fail :(\n");

         exit(1);
      }
   }

   temp = malloc(sizeof(struct dretva));

   while (ima > 0) {

      if (t == 0) {
         ispis_stanja(header);
         header = 0;
      } // pocetni ispis

      for (int i = 0; i < DRETVI; i++) { // stavljanje dretve u red pripravnih
         if (nove[i][0] == t) {          // ako u tom trenutku dode nova dretva
            P[threadNum]->id = nove[i][1];
            P[threadNum]->p = nove[i][2];
            P[threadNum]->prio = nove[i][3];
            P[threadNum]->rasp = nove[i][4];
            printf("%3d -- nova dretva id=%d, p=%d, prio=%d\n", t,
                   P[threadNum]->id, P[threadNum]->p,
                   P[threadNum]->prio); // ispis
            threadNum++;                // dodaj dretvu u br dretvi
         }
      }

      if (P[0] != NULL && threadNum != 0) {
         // ako ima dretvi u redu i ako br dretvi nije nula

        
         if (threadNum > 1) { // ako nije jedina dretva
            for (int i = 0; i < MAX_DRETVI; i++) {
               // poredaj pripravne dretve po prioritetu
               if (P[i] != NULL && P[i]->prio < P[i + 1]->prio) {
                  temp = P[i];
                  P[i] = P[i + 1];
                  P[i + 1] = temp;
               }
            }
         }
         P[0]->p -= 1;

         if (P[threadNum - 1]->p == 0) {
            // ako je dretva zavrsila tj ako joj je preostalo vrijeme 0,
            // promijeni podatke
            printf("Dretva %d zavrsila\n", P[threadNum - 1]->id);
            P[threadNum - 1]->id = -1;
            // gotove dretve imaju id -1 i sve ostalo 0
            P[threadNum - 1]->p = 0;
            P[threadNum - 1]->prio = 0;

            threadNum--;
            ima--;
         }
      }

      sleep(1); // ili ne
      ispis_stanja(header);
      ++t;
   }

   return 0;
}