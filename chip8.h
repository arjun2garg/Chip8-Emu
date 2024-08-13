#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <cstdint>

class Chip8 {
    private:
        uint8_t regs[16];
        uint8_t mem[4096];
        uint16_t idx;
        uint16_t pc;
        uint16_t stack[16];
        uint8_t sp;
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint16_t opcode;

        void init();
        void process();
    public:
        
        Chip8();
        void load(char const* filename);
        void cycle();
        uint8_t keys[16];
        uint32_t display[64*32];
};

#endif