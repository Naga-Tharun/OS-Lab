#include <iostream>
#include <cstdio>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char *argv[]){
    if(argc == 1){
        //Processor type
        {
            FILE *fp;
            fp = fopen("/proc/cpuinfo", "r");
            char c;
            int line = 0;
            int x = 0;
            cout << "\nProcessor type";
            while (line != 5){
                c = fgetc(fp);
                if(line == 4 && line < 5){
                    if(c == ':' && x == 0){
                        x++;
                    }
                    if(x == 1){
                        cout << c;
                    }
                }
                if(c == '\n'){
                    line++;
                }
            }
            fclose(fp);
        }

        //Kernel version
        {
            FILE *fp;
            fp = fopen("/proc/version", "r");
            char c;
            int x = 0;
            cout << "Kernel version:";
            while (c != EOF){
                c = fgetc(fp);
                if(c == ' '){
                    x++;
                }
                if(x == 2){
                    cout << c;
                }
                if(x == 3){
                    cout << endl;
                    break;
                }
            }
            fclose(fp);
        }

        //Amount of memory configured into this computer
        {
            FILE *fp;
            fp = fopen("/proc/meminfo", "r");
            char c;
            int x = 0;
            while (c != EOF){
                c = fgetc(fp);
                if(c == '\n'){
                    x++;
                }
                if(x == 2){
                    cout << endl;
                    break;
                }
                cout << c;
            }
            fclose(fp);
        }

        //Amount of time since the system was last booted
        {
            FILE *fp;
            fp = fopen("/proc/uptime", "r");
            char c;
            cout << "Time since last boot: ";
            while (c != EOF){
                c = fgetc(fp);
                if(c == ' '){
                    cout << " seconds" << endl;
                    break;
                }
                cout << c;
            }
            fclose(fp);
        }

    }

    else{
        
        int read_rate = strtol(argv[1], nullptr, 10);
        int printout_rate = strtol(argv[2], nullptr, 10);

        long long int user = 0, system = 0, idle = 0;
        long long int context_switches = 0, process_created = 0;
        long long int avg_read = 0, avg_write = 0;
        int count = 0;
        long long int prev_context = 0, prev_process = 0;

        while(true)
        { 
            long long int x1, x2, y, z;
            std::this_thread::sleep_for(std::chrono::seconds(read_rate));
            count += read_rate;
            {   //percent of time the processor spend in user mode and system mode
                //percent of time the system is idle
                FILE *fp;
                fp = fopen("/proc/stat", "r");
                char c;
                int p = 0;
                while (c != EOF){
                    c = fgetc(fp);
                    if(c == ' '){
                        p++;
                    }
                    if(p == 1){
                        fscanf(fp, "%lld %lld %lld %lld", &x1, &x2, &y, &z);
                        break;
                    }
                }
                user += x1+x2;
                system += y;
                idle += z;
                fclose(fp);
                if(count >= printout_rate){
                    long long int sum = user+system+idle;
                    float p_user = (float)(user*100)/sum;
                    float p_system = (float)(system*100)/sum;
                    float p_idle = (float)(idle*100)/sum;
                    cout << "\nPercentage of time processor spent in user mode: " << p_user << "%" << endl;
                    cout << "Percentage of time processor spent in system mode: " << p_system << "%" << endl;
                    cout << "Percentage of time processor is idle: " << p_idle << "%" << endl;
                }
            }

            //The amount and percentage of free memory
            if(count >= printout_rate){
                FILE *fp;
                fp = fopen("/proc/meminfo", "r");
                char c;
                int x = 0;
                char txt[50];
                char txx[50];
                long long int total;
                long long int fre;
                while (c != EOF){
                    c = fgetc(fp);
                    if(c == '\n'){
                        x++;
                    }
                    if(x == 0){
                        fscanf(fp, "%s %lld %s", txt, &total, txx);
                    }
                    if(x == 1){
                        fscanf(fp, "%s %lld %s", txt, &fre, txx);
                    }
                    if(x == 2){
                        break;
                    }
                }
                fclose(fp);
                float p_free = (float)(fre*100)/total;
                cout << "The amount of free memory: " << fre << " KB" << endl;
                cout << "The percentage of free memory: " << p_free << "%"<< endl;
            }

            {   //the rate (number of sectors per second) of disk read/write in the system
                FILE *fp;
                fp = fopen("/proc/diskstats", "r");
                char c;
                int line = 0;
                while (line != 24){
                    c = fgetc(fp);
                    if(c == '\n'){
                        line++;
                    }
                }
                long long int a;
                char b[10];
                long long int read_sector, read_time;
                long long int write_sector, write_time;
                fscanf(fp, "%lld %lld %s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", 
                &a, &a, b, &a, &a, &read_sector, &read_time, &a, &a, &write_sector, &write_time, &a, &a, &a, &a, &a, &a, &a, &a, &a);
                long long int n = printout_rate/read_rate;
                avg_read += ((read_sector*1000)/read_time)/n;
                if(write_sector != 0){
                    avg_write += ((write_sector*1000)/write_time)/n;
                }
                fclose(fp);
                if(count >= printout_rate){
                    cout << "The rate(number of sectors per second) of disk read in system: " << avg_read << endl;
                    cout << "The rate(number of sectors per second) of disk write in system: " << avg_write << endl;
                }
            }

            {   //the rate (number per second) of context switches in the kernel
                FILE *fp;
                fp = fopen("/proc/stat", "r");
                char c;
                long long int p = 0, q;
                char c_s[10];
                while (c != EOF){
                    c = fgetc(fp);
                    if(c == '\n'){
                        p++;
                    }
                    if(p == 18){
                        fscanf(fp, "%s %lld", c_s, &q);
                        break;
                    }
                }
                fclose(fp);
                long long int n = printout_rate/read_rate;
                long long int temp = q;
                q = q-prev_context;
                prev_context = temp;
                context_switches += q/n;
                if(count >= printout_rate){
                    cout << "The rate(number per second) of context_switches in kernel: " << context_switches << endl;
                }
            }

            //the rate (number per second) of the process creations in the system
            {
                FILE *fp;
                fp = fopen("/proc/stat", "r");
                char c;
                long long int p = 0, q;
                char c_s[20];
                while (c != EOF){
                    c = fgetc(fp);
                    if(c == '\n'){
                        p++;
                    }
                    if(p == 20){
                        fscanf(fp, "%s %lld", c_s, &q);
                        break;
                    }
                }
                fclose(fp);
                long long int n = printout_rate/read_rate;
                process_created += q;
                if(count >= printout_rate){
                    float ans = (float)(process_created/q)/n;
                    cout << "The rate(number per second) of process creations in system: " << ans << endl;
                }
            }

            if(count >= printout_rate){
                cout << endl;
                count = 0;
                user = 0;
                system = 0;
                idle = 0;
                context_switches = 0;
                process_created = 0;
                prev_context = 0;
                prev_process = 0;
                avg_read = 0;
                avg_write = 0;
            }
        }
    }

    return 0;
}