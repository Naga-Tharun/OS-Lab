#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// function to check if enough resources are available for the process
bool can_allocate(int process, vector<int> avaliable, vector<vector<int>> need){
    // checks if resources are available or not
    for(int i=0; i<need.size(); i++){
        // if the need for process is greater than the available resources then we return false
        if(need[process][i] > avaliable[i]){
            return false;
        }
    }
    // if all the resource needs are satisfied and available then we return true
    return true;
}

// prints the safe sequences of the processes provided
void print_sequence(vector<vector<int>> safe_seq_list){
    // the total count of safe sequences
    cout << safe_seq_list.size() << endl;
    for(int i=0; i<safe_seq_list.size(); i++){
        for(int j=0; j<safe_seq_list[i].size(); j++){
            cout << safe_seq_list[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

// finds the safe sequences using recursion and adds them to a matrix
void find_safe_seq(vector<vector<int>> max_req, vector<vector<int>> allocated, vector<vector<int>> need, vector<int> available, vector<bool> &visited, vector<int> &safe_seq, vector<vector<int>> &safe_seq_list){
    // going thorgh all the process to find the path choosing the process at the instant or choosing the process later in the sequence
    int n = allocated.size();
    for(int i=0; i<n; i++){
        // checking whether the process is visited and whether resources can be allocated for the process and suficient resources exist or not
        if (visited[i] == false && can_allocate(i, available, need) == true){
            // if resources exist we mark the process as visited to prevent it from entering the sequence again
            visited[i] = true;
            // then we relase the resources allocated for thr process and add them to the available resources
            for(int j=0; j<n; j++){
                available[j] += allocated[i][j];
            }
            // we add the resourse to the sequence
            safe_seq.push_back(i);
            // we then find the other processes for the present sequence
            // we go through the processes again using recursion to get different sequences of the processes
            find_safe_seq(max_req, allocated, need, available, visited, safe_seq, safe_seq_list);
            // we then remove the process from the sequence and also make it unvisited
            // we do this to find other possible safe sequence orders
            safe_seq.pop_back();
            visited[i] = false;
            // we also deallocate the available memory of the process and reset it to the available process by making the process pending
            for(int j=0; j<n; j++){
                available[j] -= allocated[i][j];
            }
        }
    }

    // then we will check if the sequence has all the processes visited
    // if all processes are visited then we add the sequence to list of sequences
    if(safe_seq.size() == n){
        safe_seq_list.push_back(safe_seq);
    }
}

int main(){
    // fstream used for reading input form a file
    fstream fio;
    fio.open("input.txt");
    string line;
    
    // m is the number of resources
    // n is the number of processes
    int m, n;
    cin >> m >> n;

    // taking the value of m from the input file as string and converting to int
    // taking the value of n from the input file as string and converting to int
    {
        // using getline to take the string from file
        getline(fio, line);
        string num = "";
        // converting the string to m and n
        for(int j=0; j<line.size(); j++){
            if(line[j] == ' '){
                int x = stoi(num);
                m = x;
                num = "";
            }
            else{
                num += line[j];
            }
        }
        int x = stoi(num);
        n = x;
    }

    // the number of instances of each resource available
    vector<int> available;
    // the matrix of maximum requirements of resouces for each process
    vector<vector<int>> max_req;
    // the matrix for allocated resources for each process
    vector<vector<int>> allocated;
    // the matrix for storing the number of resources still needed for the process to be executed
    vector<vector<int>> need(n, vector<int>(m, 0));

    // getting the available array from the input file
    {
        // using getline to take the string from file
        getline(fio, line);
        string num = "";
        // converting the string to an array of integers
        for(int j=0; j<line.size(); j++){
            if(line[j] == ' '){
                int x = stoi(num);
                available.push_back(x);
                num = "";
            }
            else{
                num += line[j];
            }
        }
        int x = stoi(num);
        // adding each integer to available array
        available.push_back(x);
    }

    // getting the matrix max_req data from the input file
    for(int i=0; i<n; i++){
        getline(fio, line);
        // using getline to get each linne as a string
        vector<int> row;
        string num = "";
        // converting the string to array of integers
        for(int j=0; j<line.size(); j++){
            if(line[j] == ' '){
                int x = stoi(num);
                row.push_back(x);
                num = "";
            }
            else{
                num += line[j];
            }
        }
        // adding the array to the max_req matrix
        int x = stoi(num);
        row.push_back(x);
        max_req.push_back(row);
    }

    // getting the matrix allocated data from the input file
    for(int i=0; i<n; i++){
        // using getline to get each linne as a string
        getline(fio, line);
        vector<int> row;
        string num = "";
        // converting the string to array of integers
        for(int j=0; j<line.size(); j++){
            if(line[j] == ' '){
                int x = stoi(num);
                row.push_back(x);
                num = "";
            }
            else{
                num += line[j];
            }
        }
         // adding the array to the allocated matrix
        int x = stoi(num);
        row.push_back(x);
        allocated.push_back(row);
    }

    //finding the available resources by subtracting the allocated resources
    for(int j=0; j<m; j++){
        int sum_resource = 0;
        for(int i=0; i<n; i++){
            sum_resource += allocated[i][j];
        }
        available[j] -= sum_resource;
    }
    
    // finding the needed resources for each process to run
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            need[i][j] = max_req[i][j] - allocated[i][j];
        }
    }

    // vector used for storing the safe sequence
    vector<int> safe_seq;
    // vector for keeping note of visited processes
    vector<bool> visited(n, false);
    // matrix stores the possible safe sequences
    vector<vector<int>> safe_seq_list;

    // function to find safe sequences using recursion
    find_safe_seq(max_req, allocated, need, available, visited, safe_seq, safe_seq_list);

    // printing the count of total safe sequences and the safe sequences
    print_sequence(safe_seq_list);

    // closing the input file
    fio.close();
    return 0;
}