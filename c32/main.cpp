#include <iostream>
#include "bios/bios.h"
#include "cpu/cpu_runner.h"
#include <chrono>
//#include <iomanip>

using namespace std;

class Timer { // i made this class with ai for testing
public:
    std::chrono::time_point<std::chrono::steady_clock> start_tp;

    void start_time() {
        start_tp = std::chrono::steady_clock::now();
    }
    double get_time() {
        auto end_tp = std::chrono::steady_clock::now();
        std::chrono::duration<double,milli> elapsed = end_tp - start_tp;
        return elapsed.count();
    }
    void print_time() {
        std::cout << "Elapsed time: " << std::fixed << std::setprecision(3) 
                  << get_time() << "s" << std::endl;
    }
    void print_time(string msg) {
        std::cout << msg << " took " << std::fixed << std::setprecision(3) 
                  << get_time() << "s" << std::endl;
    }
};
Timer timer;
array<Reg, CACHE_SIZE> global_cache;
void init_cores(){
    for(int i = 0; i < CORE_COUNT; i++){
        cores[i] = Core(cores, i);  // properly init with real memory
        cores[i].mem->cache = &global_cache; // point all cores to the global cache
    }
}
int main(){
    #if DEBUG == 2
        timer.start_time();
    #endif
    init_cores();

    BIOS boot_loader;
    boot_loader.load_boot_config_to_core(
        &cores[
            boot_loader.boot_config["data"]["core_to_boot"].get<uint32_t>()
        ]
    );
    #if DEBUG == 2
        cout << "loaded boot config to core " << boot_loader.boot_config["data"]["core_to_boot"].get<uint32_t>() << " and initialization took an ";
        timer.print_time();
    #else
        cout << "loaded boot config to core " << boot_loader.boot_config["data"]["core_to_boot"].get<uint32_t>() << endl;
    #endif
    start_cpu(&timer);
    return 0;
}

// compile command :
//        cd "c:\Users\matt\.c\proj1\c32\" ; g++ -std=c++20 -O3 main.cpp -o main  ; if ($?) { .\main }          



