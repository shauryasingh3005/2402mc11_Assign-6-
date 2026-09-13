#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LOCK1 0
#define LOCK2 1

void
delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++)
    ;
}

void
process_a(void)
{
  printf("Process A: starting\n");

  printf("Process A: trying to acquire Lock1\n");
  sem_wait(LOCK1);
  printf("Process A: acquired Lock1\n");

  delay();

  printf("Process A: trying to acquire Lock2\n");
  sem_wait(LOCK2);
  printf("Process A: acquired Lock2\n");

  printf("Process A: critical section\n");

  delay();

  sem_signal(LOCK2);
  sem_signal(LOCK1);

  printf("Process A: released Lock2 and Lock1\n");
  printf("Process A: completed successfully\n");

  exit(0);
}

void
process_b(void)
{
  printf("Process B: starting\n");

  printf("Process B: trying to acquire Lock1\n");
  sem_wait(LOCK1);
  printf("Process B: acquired Lock1\n");

  delay();

  printf("Process B: trying to acquire Lock2\n");
  sem_wait(LOCK2);
  printf("Process B: acquired Lock2\n");

  printf("Process B: critical section\n");

  delay();

  sem_signal(LOCK2);
  sem_signal(LOCK1);

  printf("Process B: released Lock2 and Lock1\n");
  printf("Process B: completed successfully\n");

  exit(0);
}

int
main(int argc, char *argv[])
{
  int pid;

  sem_init(LOCK1, 1);
  sem_init(LOCK2, 1);

  printf("Resource Ordering - FIXED version\n");
  printf("Both processes acquire locks in the order:\n");
  printf("Lock1 -> Lock2\n\n");

  pid = fork();

  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    process_a();
  }

  pid = fork();

  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    process_b();
  }

  wait(0);
  wait(0);

  printf("\nBoth processes completed successfully.\n");
  printf("No deadlock occurred.\n");

  exit(0);
}