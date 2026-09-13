#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 5
#define M 3

/*
 * Allocation[i][j] = 1 if process Pi holds resource Rj.
 */
int Allocation[N][M];

/*
 * Request[i][j] = 1 if process Pi is waiting for resource Rj.
 */
int Request[N][M];

/*
 * Wait-for graph.
 *
 * Graph[i][j] = 1 means Pi is waiting for a resource
 * currently held by Pj.
 */
int Graph[N][N];

/*
 * Build the wait-for graph from Allocation and Request.
 */
void
build_graph(void)
{
  int i, j, r;

  /* Clear graph */
  for(i = 0; i < N; i++) {
    for(j = 0; j < N; j++) {
      Graph[i][j] = 0;
    }
  }

  /*
   * For every process Pi:
   * If Pi requests resource Rr,
   * find the process Pj holding Rr.
   *
   * Then add edge:
   *
   * Pi -> Pj
   */
  for(i = 0; i < N; i++) {

    for(r = 0; r < M; r++) {

      if(Request[i][r]) {

        for(j = 0; j < N; j++) {

          if(Allocation[j][r]) {

            if(i != j)
              Graph[i][j] = 1;
          }
        }
      }
    }
  }
}

/*
 * Print the wait-for graph.
 */
void
print_graph(void)
{
  int i, j;

  printf("\nWait-for Graph:\n");

  for(i = 0; i < N; i++) {

    printf("P%d -> ", i);

    int found = 0;

    for(j = 0; j < N; j++) {

      if(Graph[i][j]) {

        printf("P%d ", j);
        found = 1;
      }
    }

    if(!found)
      printf("none");

    printf("\n");
  }
}

/*
 * DFS cycle detection.
 *
 * state:
 * 0 = not visited
 * 1 = currently in DFS path
 * 2 = completely processed
 */
int state[N];
int path[N];
int path_len;

int
dfs(int u)
{
  int v;

  state[u] = 1;
  path[path_len++] = u;

  for(v = 0; v < N; v++) {

    if(!Graph[u][v])
      continue;

    /*
     * Back edge to a process already in
     * the current DFS path = cycle.
     */
    if(state[v] == 1) {

      int start = 0;

      while(start < path_len && path[start] != v)
        start++;

      printf("\nDeadlock detected!\n");
      printf("Cycle: ");

      for(int i = start; i < path_len; i++)
        printf("P%d -> ", path[i]);

      printf("P%d\n", v);

      return 1;
    }

    if(state[v] == 0) {

      if(dfs(v))
        return 1;
    }
  }

  path_len--;
  state[u] = 2;

  return 0;
}

/*
 * Detect a cycle in the complete wait-for graph.
 */
int
detect_deadlock(void)
{
  int i;

  for(i = 0; i < N; i++)
    state[i] = 0;

  path_len = 0;

  for(i = 0; i < N; i++) {

    if(state[i] == 0) {

      if(dfs(i))
        return 1;
    }
  }

  return 0;
}

/*
 * Print allocation and request matrices.
 */
void
print_matrices(void)
{
  int i, j;

  printf("\nAllocation Matrix:\n");

  for(i = 0; i < N; i++) {

    printf("P%d: ", i);

    for(j = 0; j < M; j++)
      printf("%d ", Allocation[i][j]);

    printf("\n");
  }

  printf("\nRequest Matrix:\n");

  for(i = 0; i < N; i++) {

    printf("P%d: ", i);

    for(j = 0; j < M; j++)
      printf("%d ", Request[i][j]);

    printf("\n");
  }
}

/*
 * Scenario 1:
 * No deadlock.
 *
 * P0 holds R0 and requests R1.
 * P1 holds R1 and requests R2.
 * P2 holds R2.
 *
 * This creates:
 *
 * P0 -> P1 -> P2
 *
 * No cycle.
 */
void
scenario_no_deadlock(void)
{
  printf("\n========================================\n");
  printf("SCENARIO 1: NO DEADLOCK\n");
  printf("========================================\n");

  int allocation[N][M] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {0, 0, 0},
    {0, 0, 0}
  };

  int request[N][M] = {
    {0, 1, 0},
    {0, 0, 1},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
  };

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      Allocation[i][j] = allocation[i][j];
      Request[i][j] = request[i][j];
    }
  }

  print_matrices();

  build_graph();
  print_graph();

  if(!detect_deadlock())
    printf("\nResult: No deadlock detected.\n");
}

/*
 * Scenario 2:
 * Deadlock involving 3 processes.
 *
 * P0 holds R0 and requests R1.
 * P1 holds R1 and requests R2.
 * P2 holds R2 and requests R0.
 *
 * Therefore:
 *
 * P0 -> P1
 * P1 -> P2
 * P2 -> P0
 *
 * This creates a cycle.
 */
void
scenario_deadlock(void)
{
  printf("\n========================================\n");
  printf("SCENARIO 2: DEADLOCK\n");
  printf("========================================\n");

  int allocation[N][M] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {0, 0, 0},
    {0, 0, 0}
  };

  int request[N][M] = {
    {0, 1, 0},
    {0, 0, 1},
    {1, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
  };

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      Allocation[i][j] = allocation[i][j];
      Request[i][j] = request[i][j];
    }
  }

  print_matrices();

  build_graph();
  print_graph();

  if(!detect_deadlock())
    printf("\nResult: No deadlock detected.\n");
}

int
main(int argc, char *argv[])
{
  printf("========================================\n");
  printf("   DEADLOCK DETECTION USING RAG\n");
  printf("========================================\n");

  scenario_no_deadlock();

  scenario_deadlock();

  printf("\n========================================\n");
  printf("Deadlock detection completed\n");
  printf("========================================\n");

  exit(0);
}