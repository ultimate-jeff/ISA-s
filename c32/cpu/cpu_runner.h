#include "jeff/global_utils.h"
#include "main.h"
#include <thread>
#include <vector>
#include <atomic>
#include <barrier>
#pragma once
using namespace std;

Core cores[CORE_COUNT];
atomic<bool> running = true;




template<typename T>
int start_cpu(T *timer){
    cout << "starting emulator " << endl;
    uint64_t active_cores = 0;
    uint64_t total_core_cycles = 0;
    int loops = 0;

    #if DEBUG == 2 or DEBUG == 3
        timer->start_time();
    #endif

    while (Util::cpu_on){
        ++loops;
        active_cores = 0;
        for(int i = 0 ; i < CORE_COUNT ; i++){
            void (Core::*execute_fn)() = cores[i].execute;
            if(cores[i].get_sf(sf_map::active).reg.data){
                active_cores++;
                (cores[i].*execute_fn)();
            }
        }
        total_core_cycles += active_cores;
        if(active_cores == 0){
            cout << "no active cores, shutting down cpu " << endl;
            Util::cpu_on = false;
        }
    }

    #if DEBUG == 2 or DEBUG == 3
        double elapsed = timer->get_time();
        double cps = total_core_cycles / elapsed;
        cout << "----- end of runtime debug info -----" << endl;
        cout << "total cycles: " << total_core_cycles << endl;
        cout << "total loops: " << loops << endl;
        cout << "elapsed time: " << elapsed << "s" << endl;
        cout << "avg active cores: " << (double)total_core_cycles / loops << endl;
        cout << "cps: " << cps << endl;
    #endif

    return 0;
};

/* compile command :
    cd "c:\Users\matt\.c\proj1\c32\" ; g++ -std=c++20 -O3 main.cpp -o main  ; if ($?) { .\main }                                    

*/

//(instruction);