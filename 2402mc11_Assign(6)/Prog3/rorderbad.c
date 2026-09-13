#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LOCK1 0
#define LOCK2 1

// Synchronization signals used only to force the deadlock ordering
#define A_HAS_LOCK1 2
#define B_HAS_LOCK2 3

void
process_a(void)
{
  printf("Process A: starting\n");
  printf("Process A: trying to acquire Lock1\n");

  sem_wait(LOCK1);

  printf("Process A: acquired Lock1\n");

  // Tell Process B that A has Lock1
  sem_signal(A_HAS_LOCK1);

  // Wait until B has acquired Lock2
  sem_wait(B_HAS_LOCK2);

  printf("Process A: waiting for Lock2\n");

  // This will block because B holds Lock2
  sem_wait(LOCK2);

  // This point should never be reached
  printf("Process A: acquired Lock2\n");
  printf("Process A: critical section\n");

  sem_signal(LOCK2);
  sem_signal(LOCK1);

  printf("Process A: released both locks\n");
  exit(0);
}

void
process_b(void)
{
  printf("Process B: starting\n");
  printf("Process B: trying to acquire Lock2\n");

  sem_wait(LOCK2);

  printf("Process B: acquired Lock2\n");

  // Tell Process A that B has Lock2
  sem_signal(B_HAS_LOCK2);

  // Wait until A has acquired Lock1
  sem_wait(A_HAS_LOCK1);

  printf("Process B: waiting for Lock1\n");

  // This will block because A holds Lock1
  sem_wait(LOCK1);

  // This point should never be reached
  printf("Process B: acquired Lock1\n");
  printf("Process B: critical section\n");

  sem_signal(LOCK1);
  sem_signal(LOCK2);

  printf("Process B: released both locks\n");
  exit(0);
}

int
main(int argc, char *argv[])
{
  int pid;

  // Initialize the two actual resources
  sem_init(LOCK1, 1);
  sem_init(LOCK2, 1);

  // Initialize synchronization signals
  sem_init(A_HAS_LOCK1, 0);
  sem_init(B_HAS_LOCK2, 0);

  printf("Resource Ordering - BAD version\n");
  printf("This version intentionally creates a deadlock.\n\n");

  // Create Process A
  pid = fork();

  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    process_a();
  }

  // Create Process B
  pid = fork();

  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    process_b();
  }

  printf("Parent: waiting for processes...\n");
  printf("Expected deadlock:\n");
  printf("Process A holds Lock1 and waits for Lock2.\n");
  printf("Process B holds Lock2 and waits for Lock1.\n");
  printf("Interrupt this run manually after observing the deadlock.\n");

  // These waits will never finish because both children are deadlocked
  wait(0);
  wait(0);

  printf("BAD version completed unexpectedly.\n");
  exit(0);
}