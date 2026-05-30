#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

#include "config.h"

using namespace std;
using json = nlohmann::json;
class Core;

json load_json(string path){
    ifstream file(path);
    json data = json::object();
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
string lower_case(string text){
    for(int i = 0 ; i < text.length() ; i++){
        if((int)text[i] >= 65 && (int)text[i] <= 90){
            text[i]+=32;
        }
    }
    return text;
}

namespace pre_loader{

string accepted_formats[2] = {"c32\0","C32\0"};

struct C32_section{
    uint32_t start = 0;
    uint32_t size = 0;
};
struct C32_file{
    char magic[4] = {'C','3','2','\0'};
    uint32_t main_version = 1;
    uint32_t secondary_version = 0;
    uint32_t entry_point = 0;
    C32_section reg_size;
    C32_section stack_size;
    C32_section cache_size;
    C32_section sf_size;
    uint32_t spassing = 0;
};
struct Pre_load{
    vector<uint32_t> regs;
    vector<uint32_t> stack;
    vector<uint32_t> cache;
    vector<uint32_t> sf;
    uint32_t entry_point = 0;
};

ifstream load_bin(string path){
    ifstream file(path,ios::binary);
    if(!file.is_open()){
        cout << "failed to open : " << path << endl;
        exit(1);
    }
    return file;
}
bool is_corect_type(C32_file& header){
    for(int i = 0 ; i < sizeof(accepted_formats)/sizeof(accepted_formats[0]) ; i++){
        int corect = 0;
        for(int j = 0 ; j < 4 ; j++){
            if(header.magic[j] == accepted_formats[i][j]){
                corect++;
            }
        }
        if(corect == 4){
            return true;
        }
    }
    return false;
}
vector<uint32_t> loade_bin_to_vector(ifstream& file , uint32_t start , uint32_t size){
    vector<uint32_t> data;
    data.reserve(size);
    file.seekg(start);
    for(uint32_t i = 0 ; i < size ; i++){
        uint32_t value;
        file.read((char*)&value , sizeof(uint32_t));
        data.push_back(value);
    }
    return data;
}

Pre_load loade_c32_bin(string path){
    ifstream file = load_bin(path);
    C32_file header;
    Pre_load data;
    file.read((char*)&header,sizeof(C32_file));
    if(!is_corect_type(header)){
        cout << "incompatable type of " << header.magic << endl;
        return Pre_load{};
    }
    data.entry_point = header.entry_point;
    data.regs = loade_bin_to_vector(file , header.reg_size.start,header.reg_size.size);
    data.stack = loade_bin_to_vector(file , header.stack_size.start,header.stack_size.size);
    data.cache = loade_bin_to_vector(file , header.cache_size.start , header.cache_size.size);
    data.sf = loade_bin_to_vector(file,header.sf_size.start , header.sf_size.size);

    return data;
}

};

class BIOS{
    public:
        json boot_config;

        BIOS(){
            boot_config = load_json(BOOT_PATH);
        }
        template<typename T>
        void load_boot_config_to_core(T* core){
            cout << "loading boot config to core " << endl;
            string boot_method = boot_config["data"]["boot_method"];
            boot_method = lower_case(boot_method);
            if(boot_method == "preload" || boot_method == "p"){
                load_mem_from_boot_json(core);
            }
            else if(boot_method == "bin" || boot_method == "binary" || boot_method == "b"){
                pre_loader::Pre_load data;
                data = pre_loader::loade_c32_bin(boot_config["binary"]["binary_path"]);
                loade_mem_from_boot_bin(core,data);
            }
            
            
        }
    private:
        template<typename T>
        void load_mem_from_boot_json(T* core){
            load_memory(boot_config, core, &T::set_reg, "regs");
            load_memory(boot_config, core, &T::set_stack, "stack");
            load_memory(boot_config, core, &T::set_sf, "sf");
            load_memory(boot_config, core, &T::set_cash, "cache");
        }
        template<typename T>
        void load_memory(json core_config, T* core ,void(T::*set_mem)(uint32_t addr, uint32_t value),string mem_name){
            for (int i = 0 ; i < core_config["preload"][mem_name]["data"].size() ; i++){
                (core->*set_mem)(i, core_config["preload"][mem_name]["data"][i].get<uint32_t>());
            }
        }
        template<typename T>
        void loade_mem_from_boot_bin(T* core,pre_loader::Pre_load data){
            load_memory(boot_config,core, &T::set_reg , data.regs,"regs");
            load_memory(boot_config,core, &T::set_stack, data.stack,"stack");
            load_memory(boot_config,core, &T::set_cash , data.cache,"cache");
            load_memory(boot_config,core, &T::set_sf , data.sf,"sf");
        }
        template<typename T>
        void load_memory(json core_config, T* core , void(T::*set_mem)(uint32_t addr, uint32_t value) , vector<uint32_t> data ,string mem_unit){
            for(int i = 0 ; i < data.size() && i < core_config["preload"][mem_unit]["size"] ; i++){
                (core->*set_mem)(i,data[i]);
            }
        }

};
