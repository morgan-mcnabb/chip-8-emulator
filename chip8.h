#ifndef CHIP8_H
#define CHIP8_H

#include <string>

class Chip8
{
private:
    // the chip8 interpreter occupies the first 512 bytes of memory, so most programs
    // start at memory location 512 (0x200).
    // the uppermost 256 bytes (0xF00-0xFFF) are reserved for display refresh.
    // memory locations (0xEA0-0xEFF) were reserved for the call stack, internal use and other variables
    unsigned char memory[4096];


    // 16 8-bit data registers named V0-VF. the VF register doubles as a flag for some instrcutions.
    // in an addition operation, VF is the carry flag while in subtraction, it is the "no borrow" flag.
    // in the draw instruction, VF is set upon pixel collision.
    //
    // The address register, which is named I, is 16 bits wide and is used with several opcodes that involve
    // memory operation.
    
    unsigned char v_registers[16];
    
    // 16-bit address register
    unsigned short I;
    unsigned short program_counter;

    // chip8's stack has 16 levels
    unsigned short stack[16];
    unsigned short stack_pointer;


    // both counters count down until they reach 0
    unsigned char delay_timer; // intended to be used for timing the events of games
    unsigned char sound_timer; // when value is nonzero, a beeping sound is made.



public:

    unsigned char input_keys[16];

    // chip8's display resolution is 64x32 and is monochrome.
    unsigned char graphics[64 * 32];

    unsigned short opcode;
    bool draw_flag;

    Chip8();
    ~Chip8();

    void emulate();
    void read_rom(std::string& path);

};

#endif
