#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

short pauza = 0;
unsigned long int broj = 1000000001;
unsigned long int zadnji = 1000000001;

void per_ispis(int sig) { printf("\nzadnji prosti broj = %ld ", zadnji); }

void stavi_pauzu(int sig) {
   pauza = 1 - pauza;
   // return pauza;
}

void prekini(int sig) {
   printf("\nzadnji = %ld ", zadnji);
   exit(0);
}
/*void periodicki_posao(int sig) { printf("Radim periodicki posao\n"); }*/

int prost(unsigned long n) {
   unsigned long k, max;
   if ((n & 1) == 0)
      return 0;
   max = sqrt(n);
   for (k = 3; k <= max; k += 2)
      if ((n % k) == 0)
         return 0;

   return 1; // naden prost broj!
}

int main(void) {
   struct itimerval t;
   // svakih 5 sekundi pozovi funkciju periodicki_ispis();
   sigset(SIGALRM, per_ispis);
   sigset(SIGINT, stavi_pauzu);
   sigset(SIGTERM, prekini); // na signal SIGTERM pozovi funkciju prekini()

   t.it_value.tv_sec = 0;
   t.it_value.tv_usec = 500000;
   t.it_interval.tv_sec = 0;
   t.it_interval.tv_usec = 500000;

   setitimer(ITIMER_REAL, &t, NULL);
   do {
      if (prost(broj) == 1)
         zadnji = broj;
      broj++;
      while (pauza == 1)
         pause();
   } while (1);

   return 0;
}
