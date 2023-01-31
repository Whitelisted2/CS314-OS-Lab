#include<bits/stdc++.h>
using namespace std;
int procs_finished = 0;
int timer = 0;
vector<pair<int, int>> turn_around_time;
int max_time_len = 50;

class Process{
private:
    int pid;
    int arrival_time;
    vector<pair<int, char>> bursts;

public:
    vector<pair<int, char>> workloads;
    int ind_burst_finished;
    Process(int new_pid, int time){
        pid = new_pid;
        arrival_time = time;
        ind_burst_finished = 0;
    }

    void add_burst(int time){
        char dev;
        if(bursts.size()%2==0){ dev = 'c'; }
        else{ dev = 'i'; }
        bursts.push_back(make_pair(time, dev));
        workloads.push_back(make_pair(time, dev));
    }

    int getPID(){ return pid; }

    int getArrivalTime(){ return arrival_time; }

    void printBursts(){
        for(int i=0; i<bursts.size(); i++){
            cout << bursts[i].first << ' ' << bursts[i].second << "      ";
        }
    }
};


class Compare {
public:
    bool operator()(Process below,Process above){
        if (below.workloads[0].first > above.workloads[0].first){
            return true;
        }
        return false;
    }
};


priority_queue<Process, vector<Process>, Compare> scheduler;


class io_device{
private:
    deque<Process> queue;

public:
    void add_proc(Process new_proc){
        queue.push_back(new_proc);
    }

    void finishTasksForTime(int time_span){
        int time_used = 0;
        while(!queue.empty() && time_used < time_span){
            if(queue.front().workloads[0].first + time_used <= time_span){
                time_used += queue.front().workloads[0].first;
                if(queue.front().workloads.size()>1){
                    queue.front().workloads.erase(queue.front().workloads.begin());
                    scheduler.push(queue.front());
                }
                else{
                    turn_around_time.push_back(make_pair(queue.front().getPID(), timer+time_used-queue.front().getArrivalTime()));
                    procs_finished++;
                }
                queue.pop_front();
            }
            else{
                queue.front().workloads[0].first -= time_span-time_used;
                break;
            }
        }
    }
    
    void finishFirstTask(){
        timer += queue.front().workloads[0].first;
        if(queue.front().workloads.size()>1){
            queue.front().workloads.erase(queue.front().workloads.begin());
            scheduler.push(queue.front());
        }
        else{
            turn_around_time.push_back(make_pair(queue.front().getPID(), timer-queue.front().getArrivalTime()));
            procs_finished++;
        }
        queue.pop_front();
    }

    bool empty(){
        return queue.empty();
    }
};



int main(){
    ifstream f("inputs/process1.dat");
    string ip;
    vector<Process> all_procs;
    int pid=0;

    while(f >> ip){
        if(ip=="<pre>"){ break; }
    }

    do{
        if(ip=="-1" || ip=="<pre>"){
            pid++;
            f >> ip;
            if(ip=="</pre></body></html>"){
                break;
            }
            Process new_p = Process(pid, stoi(ip));
            all_procs.push_back(new_p);
        }
        else{
            all_procs[all_procs.size()-1].add_burst(stoi(ip));
        }
    }while(f >> ip);

    
    io_device io_dev = io_device();
    int n_procs = all_procs.size();

    while(all_procs[0].getArrivalTime()==0){
        scheduler.push(all_procs[0]);
        all_procs.erase(all_procs.begin());
    }

    while(procs_finished < n_procs){

        Process running_proc = scheduler.top();
        scheduler.pop();

        if(running_proc.workloads[0].second=='i'){
            io_dev.add_proc(running_proc);
        }
        else{
            
            if(running_proc.workloads.size()==1 && running_proc.workloads[0].first<=max_time_len){
                timer += running_proc.workloads[0].first;
                io_dev.finishTasksForTime(running_proc.workloads[0].first);
                turn_around_time.push_back(make_pair(running_proc.getPID(), timer-running_proc.getArrivalTime()));
                procs_finished++;
                running_proc.workloads.erase(running_proc.workloads.begin());
                // scheduler.push(running_proc);
            }
            else if(running_proc.workloads.size()>1 && running_proc.workloads[0].first<=max_time_len){
                timer += running_proc.workloads[0].first;
                io_dev.finishTasksForTime(running_proc.workloads[0].first);
                running_proc.workloads.erase(running_proc.workloads.begin());
                scheduler.push(running_proc);
            }
            else{
                timer += max_time_len;
                io_dev.finishTasksForTime(max_time_len);
                running_proc.workloads[0].first -= max_time_len;
                scheduler.push(running_proc);
            }

            while(all_procs.size()>0 && all_procs[0].getArrivalTime()<=timer){
                scheduler.push(all_procs[0]);
                all_procs.erase(all_procs.begin());
            }
        }

        if(scheduler.empty() && !io_dev.empty()){
            io_dev.finishFirstTask();
        }
        else if(scheduler.empty() && io_dev.empty()){
            break;
        }
    }

    for(int i=0; i<turn_around_time.size(); i++){
        cout << turn_around_time[i].first << ' ' << turn_around_time[i].second << '\n' ;
    }

    return 0;
}