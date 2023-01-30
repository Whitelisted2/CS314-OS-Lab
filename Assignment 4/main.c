// scheduler
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

int m;                                 // global variable for size of hash table
int time;
int j;
#define MAX_PROCESSES 100
#define MAX_BURSTS 100
#define MAX_TIME 100000

struct processNode {
    int pid;
    int val;
    int type;
    struct processNode *next;
};


struct processLogNode {
    int pid;
    int arrival_time;
    int waiting_time;
    int num_bursts;
    int first_time;
    int last_time;
    int turnaround_time;
    int status; // 0: not arrived yet , 1: ready , 2: running , 3: blocked (IO), 4: completed
};
struct processLogNode processLog[MAX_PROCESSES];

struct io{
    int idle; // 0: idle, 1: active
    int curr_pid;
    int time_left;
};
struct io ioDevice;

struct ioQueueNode{
    int pid;
    struct ioQueueNode* next;
};
struct ioQueueNode *ioQueueHead = NULL;

struct cp{
    int idle; // 0: idle, 1: active
    int curr_pid;
    int time_left;
};
struct cp cpu;

void enqueue(int ipid)
{
    struct ioQueueNode *newnode = malloc(sizeof(struct ioQueueNode));
    if(newnode != NULL)                             // check if memory is available
    {
        newnode->pid = ipid;
        newnode->next = NULL;                       // initialise newnode->next to be NULL
    }
    else{
        printf("\nMemory Insufficient!");
        exit(4);
    }
    
    if(ioQueueHead == NULL)                               // no-collision case
    {
        ioQueueHead = newnode;
    }
    else{                                           // collision case; insert at end of linked list
        struct ioQueueNode *tail;
        tail = ioQueueHead;
        while(tail->next != NULL)                   // get last element
        {
            tail = tail->next;
        }
        tail->next = newnode;                       // place newnode after (old) tail
        newnode->next = NULL;
    }
}

void insert_node(struct processNode **head, int ival, char itype)
{
    struct processNode *newnode = malloc(sizeof(struct processNode));
    if(newnode != NULL)                             // check if memory is available
    {
        newnode->type = itype;                      // put data in
        newnode->val = ival;
        newnode->next = NULL;                       // initialise newnode->next to be NULL
    }
    else{
        printf("\nMemory Insufficient!");
        exit(3);
    }
    
    if(*head == NULL)                               // no-collision case
    {
        *head = newnode;
    }
    else{                                           // collision case; insert at end of linked list
        struct processNode *tail;
        tail = *head;
        while(tail->next != NULL)                   // get last element
        {
            tail = tail->next;
        }
        tail->next = newnode;                       // place newnode after (old) tail
        newnode->next = NULL;
    }
}

int find_next(struct processNode** proc, char itype) {
    int least_bt = INT_MAX;
    int least_bt_ind = -1;
    // int second_bt = INT_MAX;
    // int second_bt_ind = -1;
    // go through all processes
    for(int r=0; r<j; r++){
        if(processLog[r].arrival_time > time){ // since we're given that the arrival times are in non-descending order
            break;
        }
        if(proc[r] == NULL) {
            continue;
        }
        if(proc[r]->val < least_bt && proc[r]->type == itype){
            // second_bt = least_bt;
            // second_bt_ind = least_bt_ind;
            least_bt = proc[r]->val;
            least_bt_ind = r;
        }
    }
    // if(cpu.curr_pid == least_bt_ind && itype == 'I'){
    //     least_bt_ind = second_bt_ind;
    // }

    // if(time == 111) {
    //     printf("%d %c ----\n", least_bt, itype);
    // }
    
    // if(least_bt_ind != -1){
    //     printf("At time %d , shortest %c job pid: %d , with burst %d\n", time, itype, least_bt_ind, least_bt);
    // }
    if(itype == 'I'){
        //
    }
    return least_bt_ind;
}

void update(struct processNode** proc) {
    for(int r=0; r<j; r++){
        if(processLog[r].arrival_time > time){ // since we're given that the arrival times are in non-descending order
            break;
        }
        if(processLog[r].status == 1){ // neither running nor blocked.
            processLog[r].waiting_time++; // update waiting time of all ready processes
        }
    }
}

void printstuff(struct processNode** proc) {
    FILE *fp;
    fp = fopen("output.txt", "a");
    fprintf(fp, "_________________ time = %d _____________________\n", time);
    fprintf(fp, "in the cpu: %d time left for process %d, while io idle:%d \n", cpu.time_left, cpu.curr_pid, ioDevice.idle);
    for(int r=0; r<j; r++){
        struct processNode *tail;
        tail = proc[r];
        if(tail != NULL){
            while(tail->next != NULL)                   // get last element
            {
                fprintf(fp, "%d%c -> ", tail->val, tail->type);
                tail = tail->next;
            }
            fprintf(fp, "%d%c -> ", tail->val, tail->type);
        }
        fprintf(fp, " NULL \n");
    }
    fclose(fp);
    return;
}

int main(int argc, char *argv[])
{
    FILE *fpw;
    fpw = fopen("output.txt", "w");
    fclose(fpw);
    if(argc>2)                         // argc contains argument count. Argument count should be 2 for execution to proceed in this case
    {                                  // the arguments are stored using pointer array *argv[] 
        printf("too many arguments. enter proper input format.");
        exit(1);
    }
    else if(argc<2)
    {
        printf("too few arguments. enter proper input format.");
        exit(2);
    }
 
    m = MAX_PROCESSES;
 
    // we want an array of linked lists to maintain a dictionary 
    // (rather, array of pointers to {the heads of} linked lists)
    struct processNode *proc[m];
    
    // initialise the heads of all the linked lists to point to NULL
    int i;
    for(i=0; i<m; i++)
    {
        proc[i] = NULL;
    }

    // Now, read the input file
    FILE *fp;
    fp = fopen(argv[1], "r");
    j = 0;
    int arrival;
    while(fscanf(fp, "%d", &arrival) != EOF)                 // for each process
    {
        // save process info in the array of structures
        processLog[j].pid = j;
        processLog[j].arrival_time = arrival;
        processLog[j].waiting_time = 0;
        processLog[j].turnaround_time = -1;
        processLog[j].first_time = -1;
        processLog[j].last_time = -1;
        processLog[j].status = 0;       // 0: not arrived yet , 1: ready , 2: running , 3: blocked (IO), 4: completed
        int n = 0;                                          // we will count this soon
        int temp = 0;
        while(fscanf(fp, "%d", &temp) != EOF ){
            if(temp == -1){                                 // reached end of line
                break;
            }
            if(n % 2 == 0){
                // insert cpu burst node to jth LL
                insert_node(&proc[j], temp, 'C');           // insert into hash table
            } else {
                // insert io burst node to jth LL
                insert_node(&proc[j], temp, 'I');           // insert into hash table
            }
            n++;
            temp = 0;
        }
        processLog[i].num_bursts = n;
        j++;
    }
    fclose(fp);
    int num_procs = j;
    // will scan thru instead of sorting ...
    // Our dictionary is now ready! ... (Might bring in priority queue if necessary)
    

    // initialize cpu, io devices
    cpu.idle = 1;
    ioDevice.idle = 1;
    int done = 0;
    time = 0;
    // find_next(proc, 'C');
    while(!done){
        printstuff(proc);
        int flag_pid = -1;
        if(cpu.idle){
            // select next process to run
            
            int pid_next = find_next(proc, 'C');
            if(pid_next == -1){
                printf("No CPU processes waiting at time %d ...\n", time);
                break;
            }
            if(pid_next != -1){
                printf("At time %d , shortest %c job pid: %d , with burst %d\n", time, 'C', pid_next, proc[pid_next]->val);
            }
            cpu.curr_pid = pid_next;
            cpu.idle = 0;
            cpu.time_left = proc[pid_next]->val;
            processLog[pid_next].status = 2; // running

            // delete the node from the LL, and check if process first time. if yes, calc response time
            if(processLog[pid_next].first_time == -1){
                processLog[pid_next].first_time = time; // time when process first gets cpu time
                printf("First time process %d got cpu is %d\n", pid_next, time);
            }
            
            proc[pid_next] = proc[pid_next]->next;
            flag_pid = pid_next; // pid at which cpu stuff just happened
            
            // update whatever happened in this 1 time unit
            cpu.time_left--;
            // printf("%d\n", cpu.time_left);

            if(cpu.time_left == 0) { // very rare case
                cpu.idle = 1;
                // processLog[pid_next].status = 4; // done
                // processLog[pid_next].turnaround_time = 1;
            }
        } else {
            // increment waiting time of all arrived processes
            update(proc);
            flag_pid = cpu.curr_pid; // flag_pid stores the pid of currently-in-cpu process
            
            // progress current process in cpu by 1 time unit
            if(cpu.time_left == 1){
                cpu.idle = 1;                           // cpu relinquished
                // if(proc[cpu.curr_pid] != NULL){ // next must be IO type /******************/
                //     // if(proc[cpu.curr_pid]->next->type == 'I'){
                //         enqueue(proc[cpu.curr_pid]->val); // enqueue
                //     // }
                // }
                // flag_pid = -1;
                // processLog[cpu.curr_pid].status = 4;    // process completed
                // save turnaround time
                // processLog[cpu.curr_pid].turnaround_time = time - processLog[cpu.curr_pid].arrival_time + 1;
                // printf("%d\n", processLog[cpu.curr_pid].turnaround_time);
            }
            cpu.time_left--;
        }
        
        if(ioDevice.idle){
            // select next io process to run
            int pid_next = find_next(proc, 'I');
            // int pid_next = ioQueueHead->pid; // get from queue
            // ioQueueHead = ioQueueHead->next;
            
            if(pid_next == -1 || flag_pid == pid_next){ // t=111, flagpid=3, pidnext is coming 3
                // printf("No IO processes waiting at time %d ...\n", time);
                if(time == 111){
                    printf("%d %d\n", pid_next, flag_pid);
                }
            } else {
                
                if(pid_next != -1){
                    printf("At time %d , shortest %c job pid: %d , with burst %d\n", time, 'I', pid_next, proc[pid_next]->val);
                }
                ioDevice.curr_pid = pid_next;
                ioDevice.idle = 0;
                ioDevice.time_left = proc[pid_next]->val;
                processLog[pid_next].status = 3; // blocked for io

                // delete the node from the LL
                proc[pid_next] = proc[pid_next]->next;
                

                // update whatever happened in this 1 time unit
                ioDevice.time_left--;
                // printf("%d\n", ioDevice.time_left);

                if(ioDevice.time_left == 0) { // common
                    ioDevice.idle = 1;
                    // processLog[pid_next].status = 4; // done
                    // processLog[pid_next].turnaround_time = 1;
                } 
            }


        } else {
            if(ioDevice.time_left == 1){
                ioDevice.idle = 1;                           // io relinquished
                processLog[ioDevice.curr_pid].status = 1;    // process ready
                // save turnaround time
                // processLog[cpu.curr_pid].turnaround_time = time - processLog[cpu.curr_pid].arrival_time + 1;
                // printf("%d\n", processLog[cpu.curr_pid].turnaround_time);
            }
            ioDevice.time_left--;
        }
        // if(flag_pid != -1) {
        //     if(proc[flag_pid] == NULL) {
        //         printf("Process %d finished at time %d\n", flag_pid, time);
        //     }
        // }
        flag_pid = -1;
        time++;
    }



    return 0;
}
