// scheduler
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

int m;                                 // global variable for size of hash table
int time;
int j;
int time_quantum = 2;
#define MAX_PROCESSES 100
#define MAX_BURSTS 100
#define MAX_TIME 100000

struct processNode {
    int pid;
    int val;
    int type;
    int age;
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
    int in_cQueue;
    int response_time;
    int service_time;
    float penalty_ratio;
    float response_ratio;
    int status; // 0: not arrived yet , 1: ready , 2: running , 3: blocked (IO), 4: completed
};
struct processLogNode processLog[MAX_PROCESSES];

struct io{
    int idle; // 0: idle, 1: active
    int curr_pid;
    int time_left;
};
struct io ioDevice;

struct cpuQueueNode{
    int pid;
    int time_left;
    struct cpuQueueNode* next;
};
struct cpuQueueNode *cpuQueueHead = NULL;

struct cp{
    int idle; // 0: idle, 1: active
    int curr_pid;
    int time_left;
};
struct cp cpu;

void enqueue(int ipid, int ival)
{
    struct cpuQueueNode *newnode = malloc(sizeof(struct cpuQueueNode));
    if(newnode != NULL)                             // check if memory is available
    {
        newnode->pid = ipid;
        newnode->next = NULL;                       // initialise newnode->next to be NULL
        newnode->time_left = ival;
    }
    else{
        printf("\nMemory Insufficient!");
        exit(4);
    }
    
    if(cpuQueueHead == NULL)                               // no-collision case
    {
        cpuQueueHead = newnode;
        cpuQueueHead->next = cpuQueueHead;
    }
    else{                                           // collision case; insert at end of linked list
        struct cpuQueueNode *tail;
        tail = cpuQueueHead;
        while(tail->next != cpuQueueHead)                   // get last element
        {
            tail = tail->next;
        }
        tail->next = newnode;                       // place newnode after (old) tail, before head
        newnode->next = cpuQueueHead;
    }
}

void insert_node(struct processNode **head, int ival, char itype)
{
    struct processNode *newnode = malloc(sizeof(struct processNode));
    if(newnode != NULL)                             // check if memory is available
    {
        newnode->type = itype;                      // put data in
        newnode->val = ival;
        newnode->age = 0;
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

void delete_begin()
{
    if(cpuQueueHead == NULL)                        // if CLL is empty
    {
        printf("\nThe List is EMPTY!\n");
    }
    else
    {
        struct cpuQueueNode *del;
        del = cpuQueueHead;
        if(cpuQueueHead == cpuQueueHead->next)              // if CLL has one node
        {
            cpuQueueHead->next = NULL;
            free(del);
        }
        else
        {
            struct cpuQueueNode *tail;
            tail = cpuQueueHead;
            while(tail->next != cpuQueueHead)       // get element just before head in CLL (i.e. tail)
            {
                tail = tail->next;
            }
            cpuQueueHead = cpuQueueHead->next;              // change head, then make tail point to new head
            tail->next = cpuQueueHead;
            free(del);
        }
    }
    return;
}

int in_cQueue(int ipid){ // function to check presence of process in cQueue
    if(cpuQueueHead == NULL){
        return 0;
    } else {
        struct cpuQueueNode *tail;
        tail = cpuQueueHead;
        if(tail->pid == ipid){
            return 1;
        }
        while(tail->next != cpuQueueHead)                   // get last element
        {
            tail = tail->next;
            if(tail->pid == ipid){
                return 1;
            }
        }
        if(tail->pid == ipid){
            return 1;
        }
        return 0;
    }
}

void update(struct processNode** proc) {
    for(int r=0; r<j; r++){
        if(processLog[r].arrival_time > time){ // since we're given that the arrival times are in non-descending order
            break;
        }
        if(in_cQueue(r) && (r != cpu.curr_pid)){
            processLog[r].waiting_time++; // if in cQueue but not running yet
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
    fprintf(fp," circular queue contains : ");
    struct cpuQueueNode *tail;
    tail = cpuQueueHead;
    if(tail != NULL){
        while(tail->next != cpuQueueHead){
            fprintf(fp, "%d -> ", tail->pid);
            tail = tail->next;
        }
        fprintf(fp, "%d \n", tail->pid);
    } else{
        fprintf(fp, "NULL \n");
    }
    fclose(fp);
    return;
}

int find_min(int a, int b){
    if(a<b){
        return a;
    }
    return b;
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
        processLog[j].response_time = -1;
        processLog[j].first_time = -1;
        processLog[j].service_time = 0;
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
            processLog[j].service_time += temp;
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
        // check all processes:
        // they may be in circular queue (cpu ready/running)
        // they may be in io (blocked)
        // they may be ready for io (stagnant in LL)
        // they may be cpu ready and not in cQueue (enqueue them)
        // they may be completed fully
        for(int k=0; k<j; k++){
            if(proc[k] != NULL){
                if(proc[k]->type == 'C' && !in_cQueue(k)){
                    enqueue(k, proc[k]->val);
                } // else all cpu ready processes are in cQueue
                if(proc[k]->type == 'I' && !in_cQueue(k)){
                    proc[k]->age++;
                }
            }
        }

        // this is almost round robin (very very tiny things that seem to differ ...)
        if(cpu.idle){
            // allot a process for a time quantum, by picking from the circular queue
            if(cpuQueueHead == NULL){
                printf("No CPU processes in cQueue ...\n");
            } else{
                int pid_next = cpuQueueHead->pid;
                printf("At time %d , %c job pid: %d\n", time, 'C', pid_next);
                cpu.curr_pid = pid_next;
                cpu.time_left = find_min(time_quantum, cpuQueueHead->time_left);
                cpuQueueHead->time_left -= cpu.time_left; // cpu takes some time off of the head
                proc[cpu.curr_pid]->val -= cpu.time_left;
                cpu.idle = 0;
                processLog[pid_next].status = 2; //running


                if(processLog[pid_next].first_time == -1){
                    processLog[pid_next].first_time = time; // time when process first gets cpu time
                    printf("First time process %d got cpu is %d\n", pid_next, time);
                }
                cpu.time_left--;

                if(cpu.time_left == 0){
                    cpu.idle = 1;
                    processLog[cpu.curr_pid].status = 1; // ready
                    
                    if(cpuQueueHead->time_left == 0){ // if head time left is 0, get out of cQueue
                        delete_begin();
                        proc[cpu.curr_pid] = proc[cpu.curr_pid]->next; // get out of the LL also
                    }
                    // cpuQueueHead = cpuQueueHead->next; // for next pass
                    // cpu.curr_pid = -1;
                }
            }
            // keep time_left as time_quantum
        } else {
            // update all processes waiting in the circular queue
            update(proc);
            // update time left for the cpu task
            cpu.time_left--;
            // if cpu task ends, make it idle, remove the process from cQueue
            if(cpu.time_left == 0){
                cpu.idle = 1;
                processLog[cpu.curr_pid].status = 1; // ready
                
                if(cpuQueueHead->time_left == 0){ // if head time left is 0, get out of cQueue
                    delete_begin();
                    proc[cpu.curr_pid] = proc[cpu.curr_pid]->next; // get out of the LL also
                }
                cpuQueueHead = cpuQueueHead->next; // for next pass
                // cpu.curr_pid = -1;
            }
        }

        if(ioDevice.idle){
            // allot an IO process, such that cpu burst of same pid is not in circular queue
            int min_age = INT_MAX;
            int pid_next = -1;
            for(int k=0; k<j; k++){
                if(k!=cpu.curr_pid && proc[k]->type == 'I'){
                    if(proc[k]->age < min_age){
                        min_age = proc[k]->age;
                        pid_next = k;
                    }
                }
            }
            if(pid_next == -1){
                // no io stuff waiting
            } else {
                // do io
                printf("\n");
                ioDevice.curr_pid = pid_next;
                ioDevice.idle = 0;
                ioDevice.time_left = proc[pid_next]->val;
                processLog[pid_next].status = 3; // blocked
                ioDevice.time_left--;
                if(ioDevice.time_left == 0) { // common
                    ioDevice.idle = 1;
                    // processLog[pid_next].status = 4; // done
                    // processLog[pid_next].turnaround_time = 1;
                } 
            }

        } else {
            // update time for process in IO
            ioDevice.time_left--;
            if(ioDevice.time_left == 0){
                ioDevice.idle = 1;
                processLog[ioDevice.curr_pid].status = 1; // ready
            }
            // if io process done, then change status of the pid to allow cpu
        }

        printstuff(proc);

        time++;
    }

    for(int k=0; k<j; k++){
        processLog[k].turnaround_time = processLog[k].last_time - processLog[k].arrival_time;
        processLog[k].response_time = processLog[k].first_time - processLog[k].arrival_time;
        // waiting time (W) has been calculated along the way!
        // service time (S) was calculated at insertion
        // response ratio, R = (W+S)/S
        // penalty ratio, P = 1/R
        processLog[k].response_ratio = (float)(processLog[k].waiting_time + processLog[k].service_time)/(float)processLog[k].service_time;
        processLog[k].penalty_ratio = 1.0/processLog[k].response_ratio;
    }

    FILE *stats;
    stats = fopen("stats.txt", "w");
    float tt, wt, rt, pt;
    for(int k=0; k<j; k++){
        fprintf(stats, "______________________________________\n");
        fprintf(stats, "Process PID %d :\n", k);
        tt += (float)processLog[k].turnaround_time;
        fprintf(stats, "Turnaround Time = %d\n", processLog[k].turnaround_time);
        wt += (float)processLog[k].waiting_time;
        fprintf(stats, "Waiting Time = %d\n", processLog[k].waiting_time);
        rt += (float)processLog[k].response_time;
        fprintf(stats, "Response Time = %d\n", processLog[k].response_time);
        pt += processLog[k].penalty_ratio;
        fprintf(stats, "Penalty Ratio = %f\n", processLog[k].penalty_ratio);
    }
    tt /= j;
    wt /= j;
    rt /= j;
    pt /= j;
    fprintf(stats, "______________________________________\n");
    fprintf(stats, "System Averages\n");
    fprintf(stats, "Avg Turnaround Time = %f\n", tt);
    fprintf(stats, "Avg Waiting Time = %f\n", wt);
    fprintf(stats, "Avg Response Time = %f\n", rt);
    fprintf(stats, "Avg Penalty Ratio = %f\n", pt);
    fclose(stats);
    return 0;
}
