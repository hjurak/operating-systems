#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int pid = 0;

void prekidna_rutina(int sig) {
   kill(pid, SIGKILL);
   exit(0);
}

int main(int argc, char *argv[]) {
   pid = atoi(argv[1]);
   sigset(SIGINT, prekidna_rutina);
   srand((unsigned)time(NULL));
   // randomizirati vrijeme da se svako malo postavi prekid
   while (1) {
      sleep(4 + rand() % 4);
      switch (rand() % 4 + 1) { // generirati jedan od 4 prekida nasumicno
      case 1:
         kill(pid, SIGURG);
         break;
      case 2:
         kill(pid, SIGABRT);
         break;
      case 3:
         kill(pid, SIGCONT);
         break;
      case 4:
         kill(pid, SIGCHLD);
         break;
      }
   }
   return 0;
}
