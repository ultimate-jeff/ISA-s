#include "core/chip.h"

array<Reg, CACHE_SIZE> cache;
array<Reg, PORT_SIZE> ports;

array<Device*, AVAILABLE_CIDS> cores;

namespace Init_Cores{

    void gen_cores(){
        for(uint16_t i = 0 ; i < CORE_COUNT ; i++){
            cores[i] = new Core(&cache, &ports, cores);
        }
    }

    void memory(){
        for(int i = 0 ; i < AVAILABLE_CIDS ; i++){;
            if(cores[i] != nullptr){
                cores[i]->mem.cache = &cache;
                cores[i]->mem.ports = &ports;
                cores[i]->cores = cores;
            }
        }
    }
    void init_system_flags(uint32_t device_type, uint32_t core_id){
        cores[core_id]->set_sf(sf_map::core_id,core_id);
        cores[core_id]->set_sf(sf_map::reg_size,REG_SIZE);
        cores[core_id]->set_sf(sf_map::stack_size,STACK_SIZE);
        cores[core_id]->set_sf(sf_map::cache_size,CACHE_SIZE);
        cores[core_id]->set_sf(sf_map::csd,CALL_STACK_DEPTH);
        cores[core_id]->set_sf(sf_map::device_type,device_type);
    }
    void system_flags(uint32_t device_type = 0){
        for(int i = 0 ; i < CORE_COUNT ; i++){
            init_system_flags(0,i);
        }
    }

    void init(){
        memory();
        system_flags();
    }
}
namespace Core_Runner{

    void Run(){

    }

}



void Clock(){

}
void Init(){

}
void decuple(){
    
}


int main(){
    Init_Cores::gen_cores();
    Init_Cores::init();
 
    for(int i = 0 ; i < AVAILABLE_CIDS ; i++){
        std::cout << "CID is initalized " <<  i << " -> " << (cores[i] != nullptr) << std::endl;
    }
    return 0;
}

namespace api_tools{
    uint32_t get_avalable_cid(){
        for(uint32_t i = 0 ; i < AVAILABLE_CIDS ; i++){
            if(cores[i] != nullptr){
                return i;
            }
        }
        return UINT32_MAX;
    }
}

namespace API{
    #define api extern "C" __declspec(dllexport)

    api void* get_device(uint16_t addr){
        return cores[addr & CORE_MASK];
    }
    api array<Device*, AVAILABLE_CIDS> get_cid_bus(){
        return cores;
    }
    api void add_device(void* new_device,uint32_t device_type){
        Device* d = static_cast<Device*>(new_device);
        uint32_t cid = api_tools::get_avalable_cid();

        if(cid > AVAILABLE_CIDS){
            cout << "could not add device at " << d << " all avalable Cid's are taken" << endl;
            return;
        }
        Init_Cores::init_system_flags(device_type,cid);

        cores[cid] = d;
    }
    api void remove_device(uint16_t cid){
        if(cid <= AVAILABLE_CIDS){
            cores[cid] = nullptr;
        }
    }
    api void clock_device(uint16_t cid){
        Device* d = static_cast<Device*>(get_device(cid));
        d->exacute();
    }
    api void release_device(uint16_t cid){
        Device* d = static_cast<Device*>(get_device(cid));
        d->release();
    }
    api void attach_device(uint16_t cid){
        Device* d = static_cast<Device*>(get_device(cid));
        d->attach();
    }


}





