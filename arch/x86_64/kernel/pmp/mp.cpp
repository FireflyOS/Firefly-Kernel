//pceudo multiprocessing

#include <x86_64/pmp/mp.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

namespace firefly::kernel::mp {
    int aa = 0;
    Process processes[0xFF];
    unsigned char processes_used[0xFF];

    uint8_t make(void (*func)(Process *process), unsigned char run_one_time, char *nid) {
        //search free processes
        unsigned char i = 0;
        while(processes[i].used == 1) i++;
        
        //use process
        processes[i].used = 1;
        processes[i].onetime = run_one_time;
        processes[i].func = func;
        processes[i].block = firefly::mm::greenleafy::use_block(0);
        processes[i].nid = nid;

        return i;
    }
    uint8_t make(void (*func)(Process *process), unsigned char run_one_time){
        uint8_t id = make(func, run_one_time, (char *)"unknown process");
        firefly::kernel::io::legacy::writeTextSerial("Warning: process %d with unknown name id was made.\n", id);
        return id;
    }

    void run(){
        unsigned char i = 0;
        while(true) {
            //firefly::kernel::io::legacy::writeTextSerial("mp aa: %d\n", aa++);
            if(processes[i].used == 1) {
                processes[i].func(&processes[i]);
                if(processes[i].onetime == 1) {
                    processes[i].used = 0;
                    firefly::mm::greenleafy::delete_block(processes[i].block->block_number);
                }
            }
            i++;
        }
    }
    void close(uint8_t id){
        processes[id].used = 0;
        firefly::mm::greenleafy::delete_block(processes[id].block->block_number);
        return;
    }
}