#pragma once
namespace firefly::kernel::fs::custom {
    namespace formats {
        constexpr int no_format = 0;
        constexpr int text = 1;
    }
    #pragma pack(push, 1)
    struct f {
        int id;
        int size = 0;
        char name[64];
        char data[65536];
        int is_used;
        int format;
    };
    struct structure {
        char name[64];
        int file_count = 0;
        //will implement it later
        int directories;
        
        f files[256];
    };
    #pragma pack(pop)
    extern structure filesystem;
    void init(const char *name);
    f *make_file(const char *name, int format);
    void remove_file(int id);
}