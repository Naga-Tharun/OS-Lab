#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <climits>
#include <iomanip>
using namespace std;

#define long unsigned int int

void sorter(vector<pair<int, int>> &a){
    int n = a.size();
    for(int i=0; i<n-1; i++){
        for(int j=i+1; j<n; j++){
            if(a[i].first > a[j].first){
                pair<int, int> temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
    return;
}

class table {
    public:
    int n_process;
    vector<int> arrival_time;
    vector<int> burst_time;
    vector<int> priority;
    int time_quantum;

    table(){
        n_process = 0;
        time_quantum = 0;
    }

    table(int n, vector<int> a_t, vector<int> b_t){
        n_process = n;
        for(int i=0; i<n; i++){
            arrival_time.push_back(a_t[i]);
            burst_time.push_back(b_t[i]);
        }
    }
    
    table(int n, vector<int> a_t, vector<int> b_t, vector<int> p_t, int t_q){
        n_process = n;
        for(int i=0; i<n; i++){
            arrival_time.push_back(a_t[i]);
            burst_time.push_back(b_t[i]);
            priority.push_back(p_t[i]);
        }
        time_quantum = t_q;
    }

    void printing(vector<int> wait_time, vector<int> response_time, vector<int> turn_around_time, int c_t, int min_a){
        for(int i=0; i<n_process; i++){
            cout << wait_time[i] << " ";
        }
        cout << endl;
        for(int i=0; i<n_process; i++){
            cout << response_time[i] << " ";
        }
        cout << endl;
        for(int i=0; i<n_process; i++){
            cout << turn_around_time[i] << " ";
        }
        float throughput = (float)n_process/(c_t-min_a);
        cout << endl;
        cout << fixed;
        cout << setprecision(4);
        cout << throughput << endl;
    }

    void fcfs(){
        vector<int> wait_time(n_process);
        vector<int> complete_time(n_process);
        vector<int> turn_around_time(n_process);
        vector<int> response_time(n_process);
        vector<int> start_time(n_process, -1);

        vector<pair<int, int>> order;
        for(int i=0; i<n_process; i++){
            pair<int, int> temp = {arrival_time[i], i};
            order.push_back(temp);
        }
        sorter(order);
        int min_a = order[0].second;
        int c_t = 0;
        for(int i=0; i<n_process; i++){
            if(start_time[order[i].second] == -1){
                start_time[order[i].second] = c_t;
            }
            c_t += burst_time[order[i].second];
            complete_time[order[i].second] = c_t;
            turn_around_time[order[i].second] = c_t - arrival_time[order[i].second];
            wait_time[order[i].second] = turn_around_time[order[i].second] - burst_time[order[i].second];
            response_time[order[i].second] = start_time[order[i].second]-arrival_time[order[i].second];
        }

        printing(wait_time, response_time, turn_around_time, c_t, min_a);
    }

    void shortest_job_first(){
        vector<int> wait_time(n_process);
        vector<int> complete_time(n_process);
        vector<int> turn_around_time(n_process);
        vector<int> response_time(n_process);
        vector<int> visited(n_process, 0);
        vector<int> start_time(n_process, -1);
        
        int count = n_process;
        int min_a = INT_MAX;
        for(int i=0; i<count; i++){
            min_a = min(min_a, arrival_time[i]);
        }
        int c_t = 0;
        while(count--){
            vector<int> a_at;
            for(int i=0; i<n_process; i++){
                if(arrival_time[i] <= c_t && visited[i] != 1){
                    a_at.push_back(i);
                }
            }
            if(a_at.size() == 0){
                c_t++;
                count++;
                continue;
            }
            int min_bt = INT_MAX;
            int min_bt_index;
            for(int i=0; i<a_at.size(); i++){
                if(burst_time[a_at[i]] == min_bt){
                    if(arrival_time[a_at[i]] < arrival_time[min_bt_index]){
                        min_bt_index = a_at[i];
                    }
                }
                else if(burst_time[a_at[i]] < min_bt){
                    min_bt = burst_time[a_at[i]];
                    min_bt_index = a_at[i];
                }
            }
            visited[min_bt_index] = 1;
            if(start_time[min_bt_index] == -1){
                start_time[min_bt_index] = c_t;
            }
            c_t += burst_time[min_bt_index];
            complete_time[min_bt_index] = c_t;
            turn_around_time[min_bt_index] = c_t - arrival_time[min_bt_index];
            wait_time[min_bt_index] = turn_around_time[min_bt_index] - burst_time[min_bt_index];
            response_time[min_bt_index] = start_time[min_bt_index]-arrival_time[min_bt_index];
        }

        printing(wait_time, response_time, turn_around_time, c_t, min_a);
    }

    void shortest_remaining_time_first(){
        vector<int> wait_time(n_process);
        vector<int> complete_time(n_process);
        vector<int> turn_around_time(n_process);
        vector<int> response_time(n_process);
        vector<int> visited(n_process, 0);
        vector<int> start_time(n_process, -1);

        vector<int> r_t(n_process);
        for(int i=0; i<n_process; i++){
            r_t[i] = burst_time[i];
        }
        
        int c_t = 0;
        int min_at = INT_MAX;
        int min_at_i = 0;
        for(int i=0; i<n_process; i++){
            if(arrival_time[i] < min_at){
                min_at = arrival_time[i];
                min_at_i = i;
            }
        }
        int t_bt = min_at;
        for(int i=0; i<n_process; i++){
            t_bt += burst_time[i];
        }
        int max_at = INT_MIN;
        for(int i=0; i<n_process; i++){
            if(arrival_time[i] > max_at){
                max_at = arrival_time[i];
            }
        }
        t_bt += max_at;
        int count = n_process;
        queue<int> pending;
        vector<int> pending_exist(n_process, 0);
        c_t= min_at;
        while(t_bt--){
            if(count == 0){
                break;
            }
            for(int i=0; i<n_process; i++){
                if(arrival_time[i] <= c_t && visited[i] != 1 && pending_exist[i] != 1){
                    pending.push(i);
                    pending_exist[i] = 1;
                }
            }
            int min_bt = INT_MAX;
            int min_bt_i;
            int p = pending.size();
            for(int i=0; i<p; i++){
                int x = pending.front();
                pending.pop();
                if(r_t[x] < min_bt){
                    min_bt = r_t[x];
                    min_bt_i = x;
                }
                pending.push(x);
            }
            r_t[min_bt_i]--;
            if(start_time[min_bt_i] == -1){
                start_time[min_bt_i] = c_t;
            }
            c_t++;
            if(r_t[min_bt_i] == 0){
                count--;
                for(int i=0; i<p; i++){
                    int x = pending.front();
                    pending.pop();
                    if(x != min_bt_i){
                        pending.push(x);
                    }
                }
                visited[min_bt_i] = 1;
                complete_time[min_bt_i] = c_t;
                turn_around_time[min_bt_i] = c_t - arrival_time[min_bt_i];
                wait_time[min_bt_i] = turn_around_time[min_bt_i] - burst_time[min_bt_i];
                response_time[min_bt_i] = start_time[min_bt_i]-arrival_time[min_bt_i];
            }
        }

        printing(wait_time, response_time, turn_around_time, c_t, min_at);
    }

    void priority_scheduling(){
        vector<int> wait_time(n_process);
        vector<int> complete_time(n_process);
        vector<int> turn_around_time(n_process);
        vector<int> response_time(n_process);
        vector<int> visited(n_process, 0);
        vector<int> start_time(n_process, -1);
        
        vector<int> r_t(n_process);
        for(int i=0; i<n_process; i++){
            r_t[i] = burst_time[i];
        }

        int min_at = INT_MAX;
        int min_at_i = 0;
        for(int i=0; i<n_process; i++){
            if(arrival_time[i] < min_at){
                min_at = arrival_time[i];
                min_at_i = i;
            }
        }
        int c_t = min_at;
        int count = n_process;
        queue<int> pending;
        vector<int> pending_exist(count, 0);
        while(count != 0){
            for(int i=0; i<n_process; i++){
                if(arrival_time[i] <= c_t && visited[i] != 1 && pending_exist[i] != 1){
                    pending.push(i);
                    pending_exist[i] = 1;
                }
            }
            int min_pt = INT_MAX;
            int min_pt_i;
            int p = pending.size();
            for(int i=0; i<p; i++){
                int x = pending.front();
                pending.pop();
                if(priority[x] < min_pt){
                    min_pt = priority[x];
                    min_pt_i = x;
                }
                pending.push(x);
            }
            r_t[min_pt_i]--;
            if(start_time[min_pt_i] == -1){
                start_time[min_pt_i] = c_t;
            }
            c_t++;
            if(r_t[min_pt_i] == 0){
                count--;
                for(int i=0; i<p; i++){
                    int x = pending.front();
                    pending.pop();
                    if(x != min_pt_i){
                        pending.push(x);
                    }
                }
                visited[min_pt_i] = 1;
                complete_time[min_pt_i] = c_t;
                turn_around_time[min_pt_i] = c_t - arrival_time[min_pt_i];
                wait_time[min_pt_i] = turn_around_time[min_pt_i] - burst_time[min_pt_i];
                response_time[min_pt_i] = start_time[min_pt_i]-arrival_time[min_pt_i];
            }
        }

        printing(wait_time, response_time, turn_around_time, c_t, min_at);
    }

    void round_robin(){
        vector<int> wait_time(n_process);
        vector<int> complete_time(n_process);
        vector<int> turn_around_time(n_process);
        vector<int> response_time(n_process);
        vector<int> visited(n_process, 0);
        vector<int> start_time(n_process, -1);
        
        vector<int> r_t(n_process);
        for(int i=0; i<n_process; i++){
            r_t[i] = burst_time[i];
        }
        queue<int> pending;
        int count = n_process;
        int min_at = INT_MAX;
        int min_at_i = 0;
        for(int i=0; i<n_process; i++){
            if(arrival_time[i] < min_at){
                min_at = arrival_time[i];
                min_at_i = i;
            }
        }
        vector<pair<int, int>> order;
        for(int i=0; i<n_process; i++){
            pair<int, int> temp = {arrival_time[i], i};
            order.push_back(temp);
        }
        sorter(order);

        vector<int> pending_exist(count, 0);
        int c_t = min_at;
        int prev = -1;
        while(count != 0){
            for(int i=0; i<n_process; i++){
                if(arrival_time[order[i].second] <= c_t && visited[order[i].second] != 1 && order[i].second != prev && pending_exist[order[i].second] != 1){
                    pending.push(order[i].second);
                    pending_exist[order[i].second] = 1;
                }
            }
            if(prev != -1){
                pending.push(prev);
            }
            int x = pending.front();
            pending.pop();
            if(start_time[x] == -1){
                start_time[x] = c_t;
            }
            c_t += min(time_quantum, r_t[x]);
            r_t[x] = r_t[x] - min(time_quantum, r_t[x]);
            if(r_t[x] == 0){
                count--;
                visited[x] = 1;
                complete_time[x] = c_t;
                turn_around_time[x] = c_t - arrival_time[x];
                wait_time[x] = turn_around_time[x] - burst_time[x];
                response_time[x] = start_time[x]-arrival_time[x];
                prev = -1;
            }
            else{
                prev = x;
            }
        }
        printing(wait_time, response_time, turn_around_time, c_t, min_at);

    }

};

int main(){
    int x;
    cin >> x;

    int n;
    cin >> n;
    vector<int> a_t(n);
    vector<int> b_t(n);
    for(int i=0; i<n; i++){
        cin >> a_t[i];
    }
    for(int i=0; i<n; i++){
        cin >> b_t[i];
    }

    if(x == 1){
        table values(n, a_t, b_t);
        values.fcfs();
    }
    else if(x == 2){
        table values(n, a_t, b_t);
        values.shortest_job_first();
    }
    else if(x == 3){
        table values(n, a_t, b_t);
        values.shortest_remaining_time_first();
    }
    else if(x == 4){
        vector<int> p_t(n);
        for(int i=0; i<n; i++){
            cin >> p_t[i];
        }
        table values(n, a_t, b_t, p_t, 0);
        values.priority_scheduling();
    }
    else if(x == 5){
        int t_q;
        vector<int> p_t(n);
        cin >> t_q;
        table values(n, a_t, b_t, p_t , t_q);
        values.round_robin();
    }

    return 0;
}