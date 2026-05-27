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





int start_cpu(){
    cout << "starting emulator " << endl;

    for(int i = 0 ; i < CORE_COUNT ; i++){
        cores[i] = Core(cores , i);
    }
    cout << "created all cores " << endl;

    while (Util::cpu_on){
        for(int i = 0 ; i < CORE_COUNT ; i++){
            void (Core::*execute_fn)() = cores[i].execute;
            if(cores[i].get_sf(sf_map::active).reg.data){
                cout << "core " << i << " executing instruction at clk " << cores[i].get_sf(sf_map::clk).reg.data << endl;
                (cores[i].*execute_fn)();
            }
        }
    }
    
    
    


    return 0;
};

/* compile command :
    cd "c:\Users\matt\.c\proj1\c32\" ; g++ -std=c++20 -O3 main.cpp -o main  ; if ($?) { .\main }                                    

*/

//(instruction);