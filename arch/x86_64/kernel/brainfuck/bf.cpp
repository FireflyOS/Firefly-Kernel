//Hehe :)

#include <include/stl/cstdlib/cstdint.h>
#include <include/x86_64/memory-manager/greenleafy.hpp>
#include <include/stl/cstdlib/stdio.h>
#include <include/x86_64/drivers/ps2.hpp>

uint8_t *bf_block = (uint8_t *)firefly::mm::greenleafy::use_block(0)->block;
int p = 0;

uint8_t current_key;
bool gotkey = false;
void keyboard_handle(){
    bf_block[p] = current_key;
    gotkey = true;
}
void load(const char *bf, int length){
    int i = 0;
    while(i < length){
        char cmd = bf[i];
        switch(cmd){
            case '>': {
                p++;
            }
            case '<': {
                p--;
            }
            case '+': {
                bf_block[p]++;
            }
            case '-': {
                bf_block[p]--;
            }
            case '.': {
                printf("%c", bf_block[p]);
            }
            case ',': {
                gotkey = false;
                firefly::drivers::ps2::redirect_to_app(&keyboard_handle, &current_key);
                while(!gotkey){
                    firefly::drivers::ps2::app_input();
                }
            }
        }
    }
}