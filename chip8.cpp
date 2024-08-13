#include <fstream>
#include "chip8.h"
#include <random>

using namespace std;

uint8_t fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {}

void Chip8::init(){
    pc = 0x200;
    for (int i = 0; i < 80; i++) {
        mem[0x50 + i] = fontset[i];
    }
    srand (time(NULL));
}

void Chip8::load(char const* filename){
    ifstream file(filename, ios::binary | ios::ate);
    if (file.is_open()) {
        streampos size = file.tellg();
        char* insts = new char[size];

        file.seekg(0, ios::beg);
        file.read(insts, size);
        file.close();

        for (int i = 0; i < size; i++) {
            mem[0x200 + i] = insts[i];
        }

        delete[] insts;
    }
}

void Chip8::process() {
    switch(opcode & 0xF000) {
        case 0x00E0: {
            for (int i = 0; i < 64*32; i++) {
                display[i] = 0;
            }
            break;
        }
        case 0x00EE: {
            pc = stack[sp];
            sp--;
            break;
        }
        case 0x1000: {
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x2000: {
            sp++;
            stack[sp] = pc;
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x3000: {
            if (regs[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc+=2;
            }
            break;
        }
        case 0x4000: {
            if (regs[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc+=2;
            }
            break;
        }
        case 0x5000: {
            if (regs[(opcode & 0x0F00) >> 8] == regs[(opcode & 0x00F0) >> 4]) {
                pc+=2;
            }
            break;
        }
        case 0x6000: {
            regs[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            break;
        }
        case 0x7000: {
            regs[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            break;
        }
        case 0x8000: {
            switch(opcode & 0x000F) {
                case 0x0000: {
                    regs[(opcode & 0x0F00) >> 8] = regs[(opcode & 0x00F0) >> 4];
                    break;
                }
                case 0x0001: {
                    regs[(opcode & 0x0F00) >> 8] |= regs[(opcode & 0x00F0) >> 4];
                    break;
                }
                case 0x0002: {
                    regs[(opcode & 0x0F00) >> 8] &= regs[(opcode & 0x00F0) >> 4];
                    break;
                }
                case 0x0003: {
                    regs[(opcode & 0x0F00) >> 8] ^= regs[(opcode & 0x00F0) >> 4];
                    break;
                }
                case 0x0004: {
                    int sum = regs[(opcode & 0x0F00) >> 8] + regs[(opcode & 0x00F0) >> 4];
                    if (sum > 255) {
                        regs[15] = 1;
                    } else {
                        regs[15] = 0;
                    }
                    regs[(opcode & 0x0F00) >> 8] = sum & 0x00FF;
                    break;
                }
                case 0x0005: {
                    if (regs[(opcode & 0x0F00) >> 8] > regs[(opcode & 0x00F0) >> 4]) {
                        regs[15] = 1;
                    } else {
                        regs[15] = 0;
                    }
                    regs[(opcode & 0x0F00) >> 8] -= regs[(opcode & 0x00F0) >> 4];
                    break;
                }
                case 0x0006: {
                    if (regs[(opcode & 0x0F00) >> 8] & 0x0001 == 1) {
                        regs[15] = 1;
                    } else {
                        regs[15] = 0;
                    }
                    regs[(opcode & 0x0F00) >> 8] /= 2;
                    break;
                }
                case 0x0007: {
                    if (regs[(opcode & 0x0F00) >> 8] < regs[(opcode & 0x00F0) >> 4]) {
                        regs[15] = 1;
                    } else {
                        regs[15] = 0;
                    }
                    regs[(opcode & 0x0F00) >> 8] = regs[(opcode & 0x00F0) >> 4] - regs[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x000E: {
                    if (regs[(opcode & 0x0F00) >> 8] & 0x8000 == 1) {
                        regs[15] = 1;
                    } else {
                        regs[15] = 0;
                    }
                    regs[(opcode & 0x0F00) >> 8] *= 2;
                    break;
                }
            }
            break;
        }
        case 0x9000: {
            if (regs[(opcode & 0x0F00) >> 8] != regs[(opcode & 0x00F0) >> 4]) {
                pc += 2;
            }
            break;
        }
        case 0xA000: {
            idx = opcode & 0x0FFF;
            break;
        }
        case 0xB000: {
            pc = (opcode & 0x0FFF) + regs[0];
            break;
        }
        case 0xC000: {
            regs[(opcode & 0x0F00) >> 8] = (rand() % (0xFF+1)) & (opcode & 0x00FF);
            break;
        }
        case 0xD000: {
            bool check = false;
            for (int i = 0; i < (opcode & 0x000F); i++) {
                if (((regs[(opcode & 0x0F00) >> 8] % 64) + (i%8)) > 63) {
                    continue;
                }
                if (((regs[(opcode & 0x00F0) >> 4] % 32) + (i/8)) > 31) {
                    continue;
                }
                if ((display[(regs[(opcode & 0x0F00) >> 8] % 64) + (regs[(opcode & 0x00F0) >> 4] % 32)*64 + 64*(i/8) + (i%8)]) == 1) {
                    regs[15] = 1;
                    check = true;
                }
                display[(regs[(opcode & 0x0F00) >> 8] % 64) + (regs[(opcode & 0x00F0) >> 4] % 32)*64 + 64*(i/8) + (i%8)] ^= mem[idx + i];
            }
            if (!check) {
                regs[15] = 0;
            }
            break;
        }
        case 0xE000: {
            switch(opcode & 0x000F) {
                case 0x000E: {
                    if (keys[(opcode & 0x0F00) >> 8]) {
                        pc += 2;
                    }
                    break;
                }
                case 0x0001: {
                    if (!keys[(opcode & 0x0F00) >> 8]) {
                        pc += 2;
                    }
                    break;
                }
            }
            break;
        }
        case 0xF000: {
            switch(opcode & 0x00FF) {
                case 0x0007: {
                    regs[(opcode & 0x0F00) >> 8] = delay_timer;
                    break;
                }
                case 0x000A: {
                    bool check = false;
                    for (int i = 0; i < 16; i++) {
                        if (keys[i]) {
                                regs[(opcode & 0x0F00) >> 8] = keys[i];
                                check = true;
                                break;
                        }
                    }
                    if (!check) {
                        pc -= 2;
                    }
                    break;
                }
                case 0x0015: {
                    delay_timer = regs[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x0018: {
                    sound_timer = regs[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x001E: {
                    idx += regs[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x0029: {
                    idx = 0x50 + (5 * (regs[(opcode & 0x0F00) >> 8] * 5));
                    break;
                }
                case 0x0033: {
                    mem[idx + 2] = regs[(opcode & 0x0F00) >> 8] % 10;
                    mem[idx + 1] = mem[idx + 2] % 10;
                    mem[idx] = mem[idx + 1] % 10;
                    break;
                }
                case 0x0055: {
                    for (int i = 0; i < ((opcode & 0x0F00) >> 8); i++) {
                        mem[idx + i] = regs[i];
                    }
                    break;
                }
                case 0x0065: {
                    for (int i = 0; i < ((opcode & 0x0F00) >> 8); i++) {
                            regs[i] = mem[idx + i];
                    }
                    break;
                }
            }
            break;
        }
    }
}

void Chip8::cycle() {
    opcode = (mem[pc] << 8) | mem[pc + 1];
    pc += 2;
    process();
    if (delay_timer > 0) {
        delay_timer--;
    }
    if (sound_timer > 0) {
        sound_timer--;
    }
}