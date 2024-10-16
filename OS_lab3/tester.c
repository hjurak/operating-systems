#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int *s1;
int *s2;

void nasumicno_izaberi() {
   int sas1, sas2;
   int sastojci[] = {1, 2, 3}; // 1 - papir, 2 - duhan, 3 - sibice
   int sastojci2[2];
   sas1 = sastojci[rand() % 3];
   int j = 0;
   for (int i = 0; i < 3; i++) {
      if (sastojci[i] != sas1) {
         sastojci2[j] = sastojci[i]; // dodavanje onih koji nisu prvi put
                                     // izabrani u drugu skupinu sastojaka
         j++;
      }
   }
   sas2 = sastojci2[rand() % 2];
   *s1 = sas1;
   *s2 = sas2;
}

int main() {
   *s1 = 0;
   *s2 = 0;
   printf("Pusac 1: ima papir\n");
   printf("Pusac 2: ima duhan\n");
   printf("Pusac 3: ima sibice\n");
   printf("\n");
   nasumicno_izaberi();
   printf("s1: %d\n", *s1);
   printf("s2: %d\n", *s2);
   printf("\n");

   return 0;
}