#include <math.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <values.h>

int ShmId;
int *ULAZ;
int *BROJ;
int M;
int N;
int *A;

void udi_u_kriticni_odsjecak(int i) {
   int k, j;
   // Lamportov algoritam
   ULAZ[i] = 1;
   // trazenje max
   for (int k = 0; k < N; k++) {
      if (BROJ[k] > BROJ[i]) {
         BROJ[i] = BROJ[k];
      }
   }

   BROJ[i] += 1;
   ULAZ[i] = 0;
   // radna cekanja u petlji
   for (j = 0; j < N; j++) {

      while (ULAZ[j] != 0) {
      }

      while (BROJ[j] != 0 &&
             ((BROJ[j]) < (BROJ[i]) || ((BROJ[j]) == (BROJ[i]) && j < i))) {
      }
   }
}

void izadi_iz_kriticnog_odsjecka(int i) { (BROJ[i]) = 0; }

void brisi(int sig) {
   (void)shmdt((char *)BROJ);
   (void)shmdt((char *)ULAZ);
   (void)shmdt((char *)A);
   (void)shmctl(ShmId, IPC_RMID, NULL);
   exit(0);
}

void proces(int i) {
   int j;

   for (j = 0; j < M; j++) {

      udi_u_kriticni_odsjecak(i);

      *A += 1;

      izadi_iz_kriticnog_odsjecka(i);
   }
}

int main(int argc, char *argv[]) {
   int i, j;
   N = atoi(argv[1]);
   M = atoi(argv[2]);

   ShmId = shmget(IPC_PRIVATE, sizeof(int) * (N + N + 1),
                  0600); // 2 polja velicine int*N i jedan int

   if (ShmId == -1)
      exit(1);

   A = (int *)shmat(ShmId, NULL, 0); // raspodjela memorije
   ULAZ = A + 1;
   BROJ = ULAZ + N;

   // inicijalizacija memorije
   *A = 0;

   for (i = 0; i < N; i++) {
      ULAZ[i] = 0;
      BROJ[i] = 0;
   }

   sigset(SIGINT, brisi); // signal ctrl+c brise sve

   for (i = 0; i < N; i++) {
      switch (fork()) {
      case 0:
         proces(i);
         exit(0);
      case -1:
         printf("Proces nije mogao biti stvoren :(\n");
         break;
      }
   }

   for (j = 0; j < N; j++) { // cekanje kraja procesa
      wait(NULL);
   }

   printf("A=%d\n", *A); // ispis i oslobadanje memorije
   shmdt((char *)BROJ);
   shmdt((char *)ULAZ);
   shmdt((char *)A);

   shmctl(ShmId, IPC_RMID, NULL);

   return 0;
}
