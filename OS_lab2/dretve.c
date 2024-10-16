#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int A = 0;
int N, M;

void *dret_funkcija(void *r) { // dretvena funckija koju se poziva stvaranjem svake dretve

   int *n = r;

   for (int i = 1; i <= M; i++) { // broj iteracija = M
      ++A;
   }
   return NULL;
}
int main(int argc, char *argv[]) {

   N = atoi(argv[1]); // prvi broj je N, drugi M
   M = atoi(argv[2]);
   int i, j;
   int *br;
   pthread_t *t; // polje za threads

   br = malloc(N * sizeof(int));
   t = malloc(N * sizeof(pthread_t)); // alociranje memorije

   for (i = 0; i < N; i++) {
      br[i] = i; // svakoj dretvi dajemo vrijednost
      // stvaranje nove dretve
      if (pthread_create(&t[i], NULL, dret_funkcija,
                         &br[i])) { // nula ako je dretva stvorena uspjesno
         printf("Stvaranje %d. dretve nije uspjelo!\n", i);
         exit(1); // izlaz s greskom (nije 0)
      }
   }
   for (j = 0; j < N; j++) {
      pthread_join(t[j], NULL); // cekanje kraja dretve
   }
   printf("A=%d\n", A); // output A

   return 0;
}