#include <iostream>
#include <vector>
#include <queue>
#include <string>
using namespace std;

// first in first out page replacement algorithm impemented using queue data structure
// pops the first element from the queue when a new page is to be added
int fifo(int n, int s, vector<int> &page){
    int fault = 0;
    queue<int> memory;
    int i = 0;
    
    while(i < s){
        if(memory.size() < n){
            memory.push(page[i]);
            fault++;
            i++;
            continue;
        }

        queue<int> temp = memory;
        bool present = false;
        while(!temp.empty()){
            if(temp.front() == page[i]){
                present = true;
                break;
            }
            temp.pop();
        }
        if(present == true){
            i++;
            continue;
        }

        memory.pop();
        memory.push(page[i]);
        fault++;
        i++;
    }

    return fault;
}

// optimal page replacement algorithm removes the page that will not be required for the longest duration out of all thee existing pages
int optimal(int n, int s, vector<int> &page){
    int fault = 0;
    vector<pair<int, int>> memory;

    int i = 0;
    while(i < s){
        if(memory.size() < n){
            int p;
            for(p=i+1; p<s; p++){
                if(page[i] == page[p]){
                    break;
                }
            }
            memory.push_back({page[i], p});
            i++;
            fault++;
            continue;
        }

        bool present = false;
        for(auto j=memory.begin(); j!=memory.end(); j++){
            if((*j).first == page[i]){
                int p;
                for(p=i+1; p<s; p++){
                    if(page[i] == page[p]){
                        break;
                    }
                }
                (*j).second = p;
                present = true;
                break;
            }
        }
        if(present == true){
            i++;
            continue;
        }

        auto min_m = memory.begin();
        for(auto j=memory.begin(); j!=memory.end(); j++){
            if((*j).second > (*min_m).second){
                min_m = j;
            }
        }

        int next;
        for(next=i+1; next<s; next++){
            if(page[i] == page[next]){
                break;
            }
        }
        memory.erase(min_m);
        memory.push_back({page[i], next});
        i++;
        fault++;
    }

    return fault;
}

// least recently used page replacement algorithm
// removes the page that is the last recently accessed page
int lru(int n, int s, vector<int> &page){
    int fault = 0;
    vector<pair<int, int>> memory;

    int i = 0;
    while(i < s){
        if(memory.size() < n){
            memory.push_back({page[i], i});
            i++;
            fault++;
            continue;
        }

        auto min_m = memory.begin();
        bool present = false;
        for(auto j=memory.begin(); j!=memory.end(); j++){
            if((*j).first == page[i]){
                (*j).second = i;
                present = true;
                break;
            }
            if((*min_m).second > (*j).second){
                min_m = j;
            }
        }
        if(present == true){
            i++;
            continue;
        }

        memory.erase(min_m);
        memory.push_back({page[i], i});
        i++;
        fault++;
    }

    return fault;
}

int main(){
    int n, s;
    cin >> n;
    cin >> s;

    vector<int> page(s);
    for(int i=0; i<s; i++){
        cin >> page[i];
    }
    string algo;
    cin >> algo;
    int page_faults = 0;

    if(algo == "FIFO"){
        page_faults = fifo(n, s, page);
    }
    else if(algo == "OPTIMAL"){
        page_faults = optimal(n, s, page);
    }
    else if(algo == "LRU"){
        page_faults = lru(n, s, page);
    }

    cout << page_faults << endl;
    return 0;
}