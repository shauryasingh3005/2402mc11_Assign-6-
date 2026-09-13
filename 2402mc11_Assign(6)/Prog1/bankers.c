#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 5
#define M 3

int Allocation[N][M] = {
  {0, 1, 0},
  {2, 0, 0},
  {3, 0, 2},
  {2, 1, 1},
  {0, 0, 2}
};

int Max[N][M] = {
  {7, 5, 3},
  {3, 2, 2},
  {9, 0, 2},
  {2, 2, 2},
  {4, 3, 3}
};

int Available[M] = {3, 3, 2};

int Need[N][M];

/* Compute Need = Max - Allocation */
void
calculate_need(void)
{
  int i, j;

  for(i = 0; i < N; i++) {
    for(j = 0; j < M; j++) {
      Need[i][j] = Max[i][j] - Allocation[i][j];
    }
  }
}

/* Check whether a process can finish */
int
can_finish(int p, int work[])
{
  int j;

  for(j = 0; j < M; j++) {
    if(Need[p][j] > work[j])
      return 0;
  }

  return 1;
}

/* Safety Algorithm */
int
safety_algorithm(int sequence[])
{
  int work[M];
  int finish[N];
  int count = 0;
  int i, j;

  for(j = 0; j < M; j++)
    work[j] = Available[j];

  for(i = 0; i < N; i++)
    finish[i] = 0;

  while(count < N) {
    int found = 0;

    for(i = 0; i < N; i++) {

      if(finish[i])
        continue;

      if(can_finish(i, work)) {

        /* Process finishes and releases resources */
        for(j = 0; j < M; j++)
          work[j] += Allocation[i][j];

        finish[i] = 1;
        sequence[count] = i;
        count++;
        found = 1;
      }
    }

    if(!found)
      return 0;
  }

  return 1;
}

/* Print a vector */
void
print_vector(int v[])
{
  printf("[ ");

  for(int j = 0; j < M; j++)
    printf("%d ", v[j]);

  printf("]");
}

/* Print current matrices */
void
print_state(void)
{
  printf("\nAllocation Matrix:\n");

  for(int i = 0; i < N; i++) {
    printf("P%d: ", i);
    print_vector(Allocation[i]);
    printf("\n");
  }

  printf("\nMax Matrix:\n");

  for(int i = 0; i < N; i++) {
    printf("P%d: ", i);
    print_vector(Max[i]);
    printf("\n");
  }

  printf("\nNeed Matrix:\n");

  for(int i = 0; i < N; i++) {
    printf("P%d: ", i);
    print_vector(Need[i]);
    printf("\n");
  }

  printf("\nAvailable: ");
  print_vector(Available);
  printf("\n");
}

/* Resource Request Algorithm */
int
resource_request(int p, int request[])
{
  int j;
  int sequence[N];

  printf("\n----------------------------------------\n");
  printf("Resource Request from P%d: ", p);
  print_vector(request);
  printf("\n");

  /* Check Request <= Need */
  for(j = 0; j < M; j++) {
    if(request[j] > Need[p][j]) {
      printf("Request rejected: exceeds Need[%d]\n", p);
      return 0;
    }
  }

  /* Check Request <= Available */
  for(j = 0; j < M; j++) {
    if(request[j] > Available[j]) {
      printf("Request rejected: exceeds Available resources\n");
      return 0;
    }
  }

  /*
   * Temporarily grant the request.
   */
  for(j = 0; j < M; j++) {
    Available[j] -= request[j];
    Allocation[p][j] += request[j];
    Need[p][j] -= request[j];
  }

  /*
   * Run safety algorithm on the temporary state.
   */
  if(safety_algorithm(sequence)) {

    printf("Request granted safely.\n");
    printf("Safe sequence after request: ");

    for(j = 0; j < N; j++) {
      printf("P%d", sequence[j]);

      if(j != N - 1)
        printf(" -> ");
    }

    printf("\n");

    return 1;
  }

  /*
   * Unsafe state: roll back.
   */
  for(j = 0; j < M; j++) {
    Available[j] += request[j];
    Allocation[p][j] -= request[j];
    Need[p][j] += request[j];
  }

  printf("Request denied — would lead to unsafe state.\n");

  return 0;
}

int
main(int argc, char *argv[])
{
  int sequence[N];

  calculate_need();

  printf("       BANKER'S ALGORITHM SIMULATION\n");
  printf("...........................................\n");

  print_state();

  /*
   * Initial safety check
   */
  printf("\nChecking initial system safety...\n");

  if(safety_algorithm(sequence)) {

    printf("System is in a SAFE state.\n");

    printf("Safe sequence: ");

    for(int i = 0; i < N; i++) {
      printf("P%d", sequence[i]);

      if(i != N - 1)
        printf(" -> ");
    }

    printf("\n");
  }
  else {
    printf("System is in an UNSAFE state.\n");
  }

  /*
   * Scenario 1:
   * P1 requests (1,0,2)
   * This request can be safely granted.
   */
  printf("\n\nSCENARIO 1: Safe Resource Request\n");

  int request1[M] = {1, 0, 2};

  resource_request(1, request1);

  /*
   * Scenario 2:
   *
   * Reset the original state because the two scenarios
   * are independent.
   */
  int original_allocation[N][M] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
  };

  int original_available[M] = {3, 3, 2};

  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      Allocation[i][j] = original_allocation[i][j];

  for(int j = 0; j < M; j++)
    Available[j] = original_available[j];

  calculate_need();

  /*
   * Scenario 2:
   * P4 requests (3,3,0).
   *
   * The request is <= Need and <= Available,
   * but granting it makes the system unsafe.
   */
  printf("\n\nSCENARIO 2: Unsafe Resource Request\n");

  int request2[M] = {3, 3, 0};

  resource_request(4, request2);

  printf("Banker's Algorithm completed\n");
  printf("...................................\n");

  exit(0);
}