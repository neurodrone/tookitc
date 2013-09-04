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

struct {
    long priority;
    int temp;
    int pid;
    int stable;
} msgp, cmbox;

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("Usage: Too few arguments ./central Temp UID.\n");
        return 1;
    }

    int unstable = 1;
    int result, length, status;

    int init_temp = atoi(argv[1]);
    int uid = atoi(argv[2]);

    int mailbox_id = msgget(CENTRAL_MAILBOX, 0600 | IPC_CREAT);

    int msqid = msgget((CENTRAL_MAILBOX + uid), 0600 | IPC_CREAT);

    cmbox.priority = 1;
    cmbox.pid = uid;
    cmbox.temp = init_temp;
    cmbox.stable = 1;

    length = sizeof (msgp) - sizeof (long);

    while (unstable == 1) {
        result = msgsnd(mailbox_id, &cmbox, length, 0);

        result = msgrcv(msqid, &msgp, length, 1, 0);

        if (msgp.stable == 0) {
            unstable = 0;
            printf("Process %d, Temp: %d\n", cmbox.pid, cmbox.temp);
        } else {
            int newtemp = (cmbox.temp * 3 + 2 * msgp.temp) / 5;
            printf("newtemp: %d\n", newtemp);
            cmbox.temp = newtemp;
        }
    }

    status = msgctl(msqid, IPC_RMID, 0);

    if (status != 0) {
        fprintf(stderr, "Error closing mailbox\n");
    }

    return 0;
}

