# 2402mc11_Assign-6-
Deadlock Handling and Synchronization in xv6

bankers.c – Implements the Banker's Algorithm. It checks whether the system is in a safe state, generates a safe sequence, and tests resource requests that are either safely granted or denied.

deadlockdetect.c – Implements deadlock detection using a Resource Allocation Graph / Wait-For Graph. It detects cycles and reports the processes involved in a deadlock.

resourceorder.c – Demonstrates deadlock prevention using resource ordering. Processes acquire locks in a fixed global order to eliminate circular wait.

resourceorder_bad.c / resourceorder_fixed.c – Versions showing the deadlocking approach and the corrected resource-ordering approach.

syncdeadlock.c – Demonstrates synchronization with multiple resource types and limited resource instances while using resource ordering to prevent deadlock.

Objective
The main objective is to understand how operating systems handle resource allocation, synchronization, deadlock detection, and deadlock prevention using practical xv6 programs. The programs were tested with multiple processes and repeated execution cycles. The output logs demonstrate safe execution, resource sharing, deadlock detection, and successful completion without deadlock where prevention/avoidance is applied.
