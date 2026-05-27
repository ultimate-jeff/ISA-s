#include <iostream>
#include "bios/bios.h"
#include "cpu/cpu_runner.h"

using namespace std;


int main(){
    BIOS boot_loader;
    boot_loader.load_boot_config_to_core(
        &cores[
            boot_loader.boot_config["data"]["core_to_boot"].get<uint32_t>()
        ]
    );
    start_cpu();
    return 0;
}