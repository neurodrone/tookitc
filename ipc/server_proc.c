/**
 * @author Vaibhav Bhembre
 * @version 2013/09/03
 */

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CENTRAL_MAILBOX 1200
#define NUM_PROCESSES 4

struct {
    long priority;
    int temp;
    int pid;
    int stable;
} msgp, cmbox;

int main(int argc, char *argv[]) {
    int uid, init_temp, unstable, msqid[NUM_PROCESSES], temp_array[NUM_PROCESSES];
    int mailbox_id, length, result, status;
    int i;

    if (argc != 2) {
        printf("Usage: Too few arguments ./central Temp.\n");
        return 1;
    }

    printf("Starting server...\n");

    uid = 0;
    init_temp = atoi(argv[1]);
    unstable = 1;

    mailbox_id = msgget(CENTRAL_MAILBOX, 0600 | IPC_CREAT);

    for (i = 0; i < NUM_PROCESSES; i++) {
        msqid[i] = msgget((CENTRAL_MAILBOX + i + 1), 0600 | IPC_CREAT);
    }

    msgp.priority = 1;
    msgp.pid = uid;
    msgp.temp = init_temp;
    msgp.stable = 1;

    length = sizeof (msgp) - sizeof (long);

    while (unstable == 1) {
        int sumtemp = 0;
        int stable = 1;

        for (i = 0; i < NUM_PROCESSES; i++) {
            result = msgrcv(mailbox_id, &cmbox, length, 1, 0);
            if (temp_array[cmbox.pid - 1] != cmbox.temp) {
                stable = 0;
                temp_array[cmbox.pid - 1] = cmbox.temp;
            }
            sumtemp += cmbox.temp;
        }
        printf("sumtemp: %d\n", sumtemp);

        if (stable) {
            printf("Temperature stabilized: %d\n", msgp.temp);
            unstable = 0;
            msgp.stable = 0;
        } else {
            int newtemp = (2 * msgp.temp + sumtemp) / 6;
            printf("newtemp: %d\n", newtemp);
            msgp.temp = newtemp;
        }

        for (i = 0; i < NUM_PROCESSES; i++) {
            result = msgsnd(msqid[i], &msgp, length, 0);
        }
    }

    printf("\nShutting down server...\n");
    
    status = msgctl(mailbox_id, IPC_RMID, 0);
    if (status != 0) {
        fprintf(stderr, "Error closing mailbox.\n");
    }

    return 0;
}

