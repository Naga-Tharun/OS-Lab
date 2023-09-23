#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <limits.h>
using namespace std;

#define memory_size 1024;

// For data collection
int p_c = 0;
int h_c = 0;
int hole_c = 0;
int process_c = 0;
int partitions = 0;
// 

class process_mem{
    public:
        int process_id;
        int start_address;
        int end_address;
        int process_size;

        process_mem(){

        }

        process_mem(int pid, int s_ad, int e_ad){
            process_id = pid;
            start_address = s_ad;
            end_address = e_ad;
            process_size = e_ad-s_ad+1;
        }
};

class hole_mem{
    public:
        int hole_id;
        int start_address;
        int end_address;
        int hole_size;

        hole_mem(){

        }

        hole_mem(int hid, int s_ad, int e_ad){
            hole_id = hid;
            start_address = s_ad;
            end_address = e_ad;
            hole_size = e_ad-s_ad+1;
        }

        void updateSize(){
            hole_size = end_address-start_address+1;
        }
};

class data_collector{
    public:
        long frag;
        long hole;
        long n_hole_part;
        long max_frag;
        long min_frag;
        long n_jobs;
        long max_n_jobs;
        long min_n_jobs;
        long job_size;
        long n_holes;
        long max_holes;
        long min_holes;
        long max_part;
        long max_evict;

        data_collector()
        {
            frag = 0;
            hole = 0;
            n_hole_part = 0;
            max_frag = INT_MIN;
            min_frag = INT_MAX;
            n_jobs = 0;
            max_n_jobs = 0;
            min_n_jobs = INT_MAX;
            job_size = 0;
            n_holes = 0;
            max_holes = 0;
            min_holes = INT_MAX;
            max_part = INT_MIN;
            max_evict = INT_MIN;
        }

        void collector(hole_mem new_hole, vector<hole_mem> h_list, vector<process_mem> p_list){
            frag += new_hole.hole_size;
            if (new_hole.hole_size > max_frag){
                max_frag = new_hole.hole_size;
            }
            if (new_hole.hole_size < min_frag){
                min_frag = new_hole.hole_size;
            }
        }

        void collector(vector<hole_mem> h_list, vector<process_mem> p_list){
            if(p_c >= 1000){
                hole_c += h_list.size();
            }
            for (auto j=h_list.begin(); j!=h_list.end(); j++){
                hole += (*j).hole_size;
            }
            n_jobs += p_list.size();
            if (p_list.size() > max_n_jobs){
                max_n_jobs = p_list.size();
            }
            if (p_list.size() < min_n_jobs){
                min_n_jobs = p_list.size();
            }
            if(p_c >= 1000){
                process_c += p_list.size();
            }
            for(auto j=p_list.begin(); j!=p_list.end(); j++){
                job_size += (*j).process_size;
            }
            n_holes += h_list.size();
            if(h_list.size() > max_holes){
                max_holes = h_list.size();
            }
            if (h_list.size()<min_holes){
                min_holes = h_list.size();
            }
        }

        void display(){
            cout << "average fragmentation: " << (float)frag / (10 * 1024) << "%" << endl;
            cout << "average hole size: " << (float)hole / (hole_c) << endl;
            cout << "average holes examined to satisfy partition: " << (float)n_hole_part / (hole_c) << endl;
            cout << "highest fragmentation: " << (float)max_frag / (10.24) << "%" << endl;
            cout << "lowest fragmentation: " << (float)min_frag / (10.24) << "%" << endl;
            cout << "average number of jobs: " << (float)n_jobs / (p_c-1000) << endl;
            cout << "highest number of jobs: " << max_n_jobs << endl;
            cout << "lowest number of jobs: " << min_n_jobs << endl;
            cout << "average job size: " << (float)job_size / (process_c) << endl;
            cout << "average number of holes: " << (float)n_holes / 1000 << endl;
            cout << "highest holes recorded: " << max_holes << endl;
            cout << "lowest holes recorded: " << min_holes << endl;
            cout << "largest number of partitions created in a row without evictions: " << max_part << endl;
            cout << "largest number of evictions to satisfy a single partition request: " << max_evict << endl;
            cout << endl;
        }
};

data_collector data_c;
int allocateJobSize(int profile){
    int num;
    if(profile == 1){
        num = rand()%(1024)+1;
    }
    else if(profile == 2){
        num = rand()%(100)+1;
    }
    else if(profile == 3){
        num = rand()%(501)+500;
    }

    return num;
}

bool compare_hole(hole_mem h1, hole_mem h2){
    return (h1.start_address < h2.start_address);
}

bool compare_process(process_mem p1, process_mem p2){
    return (p1.start_address < p2.start_address);
}

void randomProcessRemove(vector<process_mem> &p_list, vector<hole_mem> &h_list){
    int rand_p = rand()%(p_list.size());
    auto i = p_list.begin();
    int x = 0;

    hole_mem new_hole;
    new_hole.hole_id = h_c++;

    for(i=p_list.begin(); i!=p_list.end(); i++){
        if(x == rand_p){
            new_hole.start_address = (*i).start_address;
            new_hole.end_address = (*i).end_address;
            new_hole.hole_size = (*i).process_size;
            p_list.erase(i);
            break;
        }
        x++;
    }

    h_list.push_back(new_hole);
    return;
}

void mergeHoles(vector<hole_mem> &h_list){
    sort(h_list.begin(), h_list.end(), compare_hole);

    if(h_list.size() == 1){
        return;
    }

    auto i = h_list.begin();
    while(i!=h_list.end()-1){
        auto j = i+1;
        if((*i).end_address == (*j).start_address-1){
            (*i).end_address = (*j).end_address;
            (*i).updateSize();
            h_list.erase(j);
        }
        else{
            i++;
        }
    }

    return;
}

void bestFit(vector<process_mem> &p_list, vector<hole_mem> &h_list, int new_p_size){

    bool inserted = false;

    // For data collection
    int new_p_counter = 0;
    int evictions = 0;

    partitions++;
    if(data_c.max_part < partitions){
        data_c.max_part = partitions;
    }
    // 
    
    process_mem new_process;
    new_process.process_id = p_c++;

    while(!inserted){
        if(h_list.size() != 0){
            
            sort(p_list.begin(), p_list.end(), compare_process);
            sort(h_list.begin(), h_list.end(), compare_hole);

            int p_len = p_list.size();
            int h_len = h_list.size();

            // For data collection
            if(new_p_counter == 0){
                new_p_counter = 1;
            }

            if(evictions > data_c.max_evict){
                data_c.max_evict = evictions;
            }
            //

            for(auto i=h_list.begin(); i!=h_list.end(); i++){

                // For data collection
                if(new_p_counter == 1){
                    data_c.n_hole_part++;
                }
                // 

                if((*i).hole_size == new_p_size){
                    new_process.start_address = (*i).start_address;
                    new_process.end_address = (*i).end_address;
                    new_process.process_size = (*i).hole_size;
                    h_list.erase(i);
                    p_list.push_back(new_process);
                    inserted = true;

                    // Data collection
                    if(p_c >= 1000){
                        data_c.collector(h_list, p_list);
                    }
                    // 

                    return;
                }
            }

            auto h_min = h_list.begin();
            while(h_min != h_list.end()-1 && (*h_min).hole_size < new_p_size){
                h_min++;
            }

            for(auto i=h_list.begin(); i!=h_list.end(); i++){
                if((*i).hole_size > new_p_size){
                    if((*h_min).hole_size > (*i).hole_size){
                        h_min = i;
                    }
                }
            }

            if((*h_min).hole_size >= new_p_size){

                // For data collection
                if(new_p_counter == 1){
                    data_c.n_hole_part++;
                }
                // 

                new_process.start_address = (*h_min).start_address;
                new_process.end_address = (*h_min).start_address+new_p_size-1;
                new_process.process_size = new_p_size;
                hole_mem new_hole = hole_mem(h_c++, new_process.end_address+1, (*h_min).end_address);
                h_list.erase(h_min);
                h_list.push_back(new_hole);
                p_list.push_back(new_process);
                inserted = true;

                // Data collection
                if(p_c >= 1000){
                    data_c.collector(h_list, p_list);
                    data_c.collector(new_hole, h_list, p_list);
                }
                // 

                return;
            }

        }
        // Data collection
        partitions = 0;
        evictions++;
        new_p_counter++;
        data_c.n_hole_part++;
        // 

        randomProcessRemove(p_list, h_list);
        mergeHoles(h_list);
    }
}

void worstFit(vector<process_mem> &p_list, vector<hole_mem> &h_list, int new_p_size){

    bool inserted = false;

    // For data collection
    int new_p_counter = 0;
    int evictions = 0;

    partitions++;
    if(data_c.max_part < partitions){
        data_c.max_part = partitions;
    }
    // 
    
    process_mem new_process;
    new_process.process_id = p_c++;
    
    while(!inserted){
        if(h_list.size() != 0){
            
            sort(p_list.begin(), p_list.end(), compare_process);
            sort(h_list.begin(), h_list.end(), compare_hole);

            int p_len = p_list.size();
            int h_len = h_list.size();

            // For data collection
            if(new_p_counter == 0){
                new_p_counter = 1;
            }

            if(evictions > data_c.max_evict){
                data_c.max_evict = evictions;
            }
            //

            for(auto i=h_list.begin(); i!=h_list.end(); i++){

                // For data collection
                if(new_p_counter == 1){
                    data_c.n_hole_part++;
                }
                // 

                if((*i).hole_size == new_p_size){
                    new_process.start_address = (*i).start_address;
                    new_process.end_address = (*i).end_address;
                    new_process.process_size = (*i).hole_size;
                    h_list.erase(i);
                    p_list.push_back(new_process);
                    inserted = true;

                    // Data collection
                    if(p_c >= 1000){
                        data_c.collector(h_list, p_list);
                    }
                    // 

                    return;
                }
            }

            auto h_max = h_list.begin();
            while(h_max != h_list.end()-1 && (*h_max).hole_size < new_p_size){
                h_max++;
            }

            for(auto i=h_list.begin(); i!=h_list.end(); i++){
                if((*i).hole_size > new_p_size){
                    if((*h_max).hole_size < (*i).hole_size){
                        h_max = i;
                    }
                }
            }

            if((*h_max).hole_size >= new_p_size){

                // For data collection
                if(new_p_counter == 1){
                    data_c.n_hole_part++;
                }
                // 

                new_process.start_address = (*h_max).start_address;
                new_process.end_address = (*h_max).start_address+new_p_size-1;
                new_process.process_size = new_p_size;
                hole_mem new_hole = hole_mem(h_c++, new_process.end_address+1, (*h_max).end_address);
                h_list.erase(h_max);
                h_list.push_back(new_hole);
                p_list.push_back(new_process);
                inserted = true;

                // Data collection
                if(p_c >= 1000){
                    data_c.collector(h_list, p_list);
                    data_c.collector(new_hole, h_list, p_list);
                }
                // 

                return;
            }

        }
        // Data colelction
        partitions = 0;
        evictions++;
        new_p_counter++;
        data_c.n_hole_part++;
        // 

        randomProcessRemove(p_list, h_list);
        mergeHoles(h_list);
    }
}



void firstFit(vector<process_mem> &p_list, vector<hole_mem> &h_list, int new_p_size){

    bool inserted = false;
    
    // For data collection
    int new_p_counter = 0;
    int evictions = 0;

    partitions++;
    if(data_c.max_part < partitions){
        data_c.max_part = partitions;
    }
    // 
    
    process_mem new_process;
    new_process.process_id = p_c++;

    while(!inserted){
        if(h_list.size() != 0){
            
            sort(p_list.begin(), p_list.end(), compare_process);
            sort(h_list.begin(), h_list.end(), compare_hole);

            int p_len = p_list.size();
            int h_len = h_list.size();

            
            // For data collection
            if(new_p_counter == 0){
                new_p_counter = 1;
            }

            if(evictions > data_c.max_evict){
                data_c.max_evict = evictions;
            }
            // 

            for(auto i=h_list.begin(); i!=h_list.end(); i++){

                // For data collection
                if(new_p_counter == 1){
                    data_c.n_hole_part++;
                }
                // 

                if((*i).hole_size == new_p_size){
                    new_process.start_address = (*i).start_address;
                    new_process.end_address = (*i).end_address;
                    new_process.process_size = (*i).hole_size;
                    h_list.erase(i);
                    p_list.push_back(new_process);
                    inserted = true;

                    // Data collection
                    if(p_c >= 1000){
                        data_c.collector(h_list, p_list);
                    }
                    // 

                    return;
                }
                else if((*i).hole_size > new_p_size){
                    new_process.start_address = (*i).start_address;
                    new_process.end_address = (*i).start_address+new_p_size-1;
                    new_process.process_size = new_p_size;
                    hole_mem new_hole = hole_mem(h_c++, new_process.end_address+1, (*i).end_address);
                    h_list.erase(i);
                    h_list.push_back(new_hole);
                    p_list.push_back(new_process);
                    inserted = true;
                    
                    // Data collection
                    if(p_c >= 1000){
                        data_c.collector(h_list, p_list);
                        data_c.collector(new_hole, h_list, p_list);
                    }
                    // 

                    return;
                }
            }

        }
        // For data collection
        partitions = 0;
        evictions++;
        new_p_counter++;
        data_c.n_hole_part++;
        // 

        // if we exit the for loop then the holes list does not have a sufficient szed hole to fit the new process
        // so we need to remove a process randomly and check if we can merge any of the holes and do it repeatedly till we fit the process
        randomProcessRemove(p_list, h_list);
        mergeHoles(h_list);
        
    }

}

int main(int argc, char** argv){

    srand(time(0));
    int profile, policy;

    if(argc <= 2){
        cout << "Error: Value for job profile and Replacement policy not given" << endl;
        return 0;
    }
    
    profile = strtol(argv[1], nullptr, 10);
    policy = strtol(argv[2], nullptr, 10);

    if(profile > 3 || profile < 1 || policy < 1 || policy > 3){
        cout << "Error: Invalid value of either profile or policy given" << endl;
        return 0;
    }

    vector<process_mem> process_list;
    vector<hole_mem> hole_list;

    hole_mem intital_mem = hole_mem(h_c++, 1, 1024);
    hole_list.push_back(intital_mem);

    for(int i=0; i<2000; i++){
        int new_process_size = allocateJobSize(profile);
        if(policy == 1){
            bestFit(process_list, hole_list, new_process_size);
        }
        else if(policy == 2){
            worstFit(process_list, hole_list, new_process_size);
        }
        else if(policy == 3){
            firstFit(process_list, hole_list, new_process_size);
        }
    }

    data_c.display();

    return 0;
}