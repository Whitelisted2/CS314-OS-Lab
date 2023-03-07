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

int find_next(struct processNode** proc, char itype) {
    int least_bt = INT_MAX;
    int least_bt_ind = -1;
    int least_age = INT_MAX;
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
        // if(time == 1425){
        //             printf("%d %c\n", proc[6]->val, itype);
        //             if(itype == 'I'){
        //                 exit(0);
        //             }
        //         }
        // if((itype == 'C' && r != ioDevice.curr_pid) || (itype == 'I' && r != cpu.curr_pid)){
            if(proc[r]->val < least_bt && proc[r]->type == itype && (time==0 || r != cpu.curr_pid)){ /////////
                // second_bt = least_bt;
                // second_bt_ind = least_bt_ind;
                least_bt = proc[r]->val;
                least_bt_ind = r;
                if(itype == 'I' && proc[r]->age < least_age){
                    least_age = proc[r]->age;
                    least_bt = proc[r]->val;
                    least_bt_ind = r;
                }
            }
        // }
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
    return least_bt_ind;
}

void update(struct processNode** proc) {
    for(int r=0; r<j; r++){
        if(processLog[r].arrival_time > time){ // since we're given that the arrival times are in non-descending order
            break;
        }
        if(proc[r]!= NULL){
            if(proc[r]->type == 'C'){
                processLog[r].waiting_time++;
            }
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
        
        int flag_pid = -1;
        if(cpu.idle){
            // select next process to run
            
            int pid_next = find_next(proc, 'C');
            if(pid_next == -1 || (pid_next == ioDevice.curr_pid && ioDevice.time_left>=1)){
                printf("No CPU processes waiting at time %d ...\n", time);
                // break;
            } else {
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
            }
        } else {
            // increment waiting time of all arrived processes
            update(proc);
            
            flag_pid = cpu.curr_pid; // flag_pid stores the pid of currently-in-cpu process
            
            // progress current process in cpu by 1 time unit
            if(cpu.time_left == 1){
                cpu.idle = 1;                           // cpu relinquished
                processLog[cpu.curr_pid].status = 1;
                cpu.curr_pid = -1;
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

                // if(time == 111){
                //     printf("%d %d\n", pid_next, flag_pid);
                // }
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
        printstuff(proc);

        flag_pid = -1;
        time++;
        
        // check if done
        int is_done = 1;
        for(int k=0; k<j; k++)
        {
            if(proc[k] != NULL){ // any null things
                is_done = 0;
                if(proc[k]->type == 'I'){
                    proc[k]->age++;
                }
                // break;
            } else{
                if(processLog[k].last_time < 0){
                    processLog[k].last_time = time-1+cpu.time_left; // record when a process is done
                    printf("Process %d completed at time %d\n", k, time-1+cpu.time_left);
                }
                processLog[k].status = 4;
            }
        }
        if(is_done == 1){
            done = 1;
        }
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
