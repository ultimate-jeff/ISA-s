#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

#include "config.h"

using namespace std;
using json = nlohmann::json;
class Core;

json load_json(string path){
    ifstream file(path);
    json data;
    if(!file.is_open()){
        cout << "failed to open file at path " << path << endl;
    }
    else{
        file >> data;
        cout << "successfully loaded json file at path " << path << endl;
    }
    file.close();
    return data;
}


class BIOS{
    public:
        json boot_config;

        BIOS(){
            boot_config = load_json(BOOT_PATH);
        }
        void load_boot_config_to_core(Core* core){
            load_memory(boot_config, core , &Core::set_reg, "regs");
            load_memory(boot_config, core , &Core::set_stack, "stack");
            load_memory(boot_config, core , &Core::set_sf, "sf");
            load_memory(boot_config, core , &Core::set_cash, "cache");
        }
    private:
        void load_memory(json core_config, Core* core ,void(Core::*set_mem)(uint32_t addr, uint32_t value),string mem_name){
            for (int i = 0 ; i < core_config["preload"][mem_name]["data"].size() ; i++){
                (core->*set_mem)(i, core_config["preload"][mem_name]["data"][i].get<uint32_t>());
            }
        }

};
