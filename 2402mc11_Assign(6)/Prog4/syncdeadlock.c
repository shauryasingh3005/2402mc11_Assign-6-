#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 5
#define DEFAULT_CYCLES 5

// Resource IDs
#define PRINTER 0
#define SCANNER 1
#define DISK    2

// Available instances
#define PRINTER_COUNT 2
#define SCANNER_COUNT 1
#define DISK_COUNT    2

// Small busy-wait delay to allow process interleaving
void
delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++)
    ;
}

// Print resource name
void
print_resource(int resource)
{
  if (resource == PRINTER)
    printf("Printer");
  else if (resource == SCANNER)
    printf("Scanner");
  else
    printf("Disk");
}

// Acquire one resource
void
acquire_resource(int pid, int resource)
{
  printf("Process %d: requesting ", pid);
  print_resource(resource);
  printf("\n");

  sem_wait(resource);

  printf("Process %d: granted ", pid);
  print_resource(resource);
  printf("\n");
}

// Release one resource
void
release_resource(int pid, int resource)
{
  sem_signal(resource);

  printf("Process %d: released ", pid);
  print_resource(resource);
  printf("\n");
}

void
worker(int pid, int cycles)
{
  int r1, r2;
  int first, second;

  /*
   * Each process requires 2 of the 3 resource types.
   *
   * P0 -> Printer + Scanner
   * P1 -> Scanner + Disk
   * P2 -> Printer + Disk
   * P3 -> Printer + Scanner
   * P4 -> Scanner + Disk
   */

  if (pid == 0 || pid == 3) {
    r1 = PRINTER;
    r2 = SCANNER;
  }
  else if (pid == 1 || pid == 4) {
    r1 = SCANNER;
    r2 = DISK;
  }
  else {
    r1 = PRINTER;
    r2 = DISK;
  }

  /*
   * RESOURCE ORDERING
   *
   * Printer = 0
   * Scanner = 1
   * Disk    = 2
   *
   * Always acquire the lower-numbered resource first.
   */
  if (r1 < r2) {
    first = r1;
    second = r2;
  }
  else {
    first = r2;
    second = r1;
  }

  for (int cycle = 1; cycle <= cycles; cycle++) {

    printf("\nProcess %d: starting cycle %d\n", pid, cycle);

    // Request first resource
    acquire_resource(pid, first);

    // Delay while holding first resource
    delay();

    // Request second resource
    acquire_resource(pid, second);

    // Both resources have now been acquired
    printf("Process %d: both resources acquired\n", pid);

    printf("Process %d: starting work\n", pid);

    // Simulate work
    delay();

    printf("Process %d: work completed\n", pid);

    /*
     * Release in reverse order.
     */
    release_resource(pid, second);
    release_resource(pid, first);

    printf("Process %d: completed cycle %d\n", pid, cycle);

    // Give other processes a chance to run
    delay();
  }

  printf("\nProcess %d: ALL CYCLES COMPLETED\n", pid);

  exit(0);
}

int
main(int argc, char *argv[])
{
  int cycles = DEFAULT_CYCLES;

  if (argc > 1) {
    cycles = atoi(argv[1]);

    if (cycles <= 0)
      cycles = DEFAULT_CYCLES;
  }

  printf("Combined Synchronization and Deadlock Avoidance\n");
  printf("================================================\n");
  printf("Processes: 5\n");
  printf("Resources: 2 Printers, 1 Scanner, 2 Disks\n");
  printf("Strategy: Resource Ordering\n");
  printf("Global order: Printer -> Scanner -> Disk\n");
  printf("Each process requires 2 resources\n");
  printf("Each process will perform %d cycles\n\n", cycles);

  /*
   * Initialize counting semaphores.
   *
   * Printer: 2 instances
   * Scanner: 1 instance
   * Disk:    2 instances
   */
  sem_init(PRINTER, PRINTER_COUNT);
  sem_init(SCANNER, SCANNER_COUNT);
  sem_init(DISK, DISK_COUNT);

  // Create 5 philosopher/worker processes
  for (int i = 0; i < NPROC; i++) {

    int pid = fork();

    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      worker(i, cycles);
    }
  }

  // Parent waits for all 5 processes
  for (int i = 0; i < NPROC; i++)
    wait(0);

  printf("All 5 processes completed successfully.\n");
  printf("No deadlock occurred.\n");
  printf("All resources were shared within their limits.\n");

  exit(0);
}