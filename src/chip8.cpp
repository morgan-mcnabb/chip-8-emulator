#include "chip8.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstring>
#include <iostream>

const unsigned short PROGRAM_STARTING_ADDRESS = 0x200;

unsigned char font_set[80] = 
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

Chip8::Chip8()
{

    for(unsigned short i = 0; i < 80; i++)
        memory[i] = font_set[i];


    opcode = 0;
    program_counter = PROGRAM_STARTING_ADDRESS;
    stack_pointer = 0;
    I = 0;
    std::memset(graphics, 0, sizeof(graphics));
    std::memset(stack, 0, sizeof(stack));
    delay_timer = 0;
    sound_timer = 0;
}

Chip8::~Chip8()
{

}

void Chip8::read_rom(std::string& path)
{
    std::ifstream input("../roms/HIDDEN", std::ios::binary | std::ios::ate);

    if(input.is_open())
    {
        std::streampos size = input.tellg();
        char* buffer = new char[size];

        input.seekg(0, std::ios::beg);
        input.read(buffer, size);
        input.close();

        for(unsigned short i = 0; i < size; i++)
            memory[PROGRAM_STARTING_ADDRESS + i] = (unsigned char)buffer[i];

        delete[] buffer;
    }

    input.close();
}

void Chip8::emulate()
{
    unsigned short register_offset;
    opcode = memory[program_counter] << 8 | memory[program_counter + 1];

    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x00FF)
            {
                case 0x00E0:
                    std::memset(graphics, 0, sizeof(graphics));
                    program_counter += 2;
                    break;

                case 0x00EE:
                    stack_pointer--;
                    program_counter = stack[stack_pointer];
                    program_counter +=2;
                    break;
            }

            break;

        case 0x1000:
            program_counter = opcode & 0xFFF;
            break;

        case 0x2000:
            stack[stack_pointer] = program_counter;
            stack_pointer++;
            program_counter = opcode & 0x0FFF;
            break;

        case 0x3000:
            if(v_registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                program_counter += 2;

            program_counter += 2;

            break;

        case 0x4000:
            if(v_registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                program_counter += 2;

            program_counter += 2;
            break;

        case 0x5000:
            if(v_registers[(opcode & 0x0F00) >> 8] == v_registers[(opcode & 0x00F0) >> 8])
                program_counter += 2;

            program_counter += 2;
            break;

        case 0x6000:
            // shift it down by a byte since the register offset is located in the upper byte
            // of the 16-bit opcode
            register_offset = (opcode & 0x0F00) >> 8;
            v_registers[register_offset] = opcode & 0x00FF;
            program_counter += 2;
            break;

        case 0x7000:
            register_offset = (opcode & 0x0F00) >> 8;
            v_registers[register_offset] += opcode & 0x00FF;
            program_counter += 2;
            break;

        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x00:
                    v_registers[(opcode & 0x0F00) >> 8] = v_registers[(opcode & 0x00F0) >> 4];

                    break;

                case 0x01:
                    v_registers[(opcode & 0x0F00) >> 8] |= v_registers[(opcode & 0x00F0) >> 4];
                    break;

                case 0x02:
                    v_registers[(opcode & 0x0F00) >> 8] &= v_registers[(opcode & 0x00F0) >> 4];
                    break;

                case 0x03:
                    v_registers[(opcode & 0x0F00) >> 8] ^= v_registers[(opcode & 0x00F0) >> 4];
                    break;

                case 0x04:
                    v_registers[(opcode & 0x0F00) >> 8] += v_registers[(opcode & 0x00F0) >> 4];
                    v_registers[0x0F] = (0xFF - v_registers[(opcode & 0x0F00) >> 8]) < v_registers[(opcode & 0x00F0) >> 4];

                    break;

                case 0x05:
                    v_registers[0x0F] = v_registers[(opcode & 0x0F00) >> 8] < v_registers[(opcode & 0x00F0) >> 4];
                    v_registers[(opcode & 0x0F00) >> 8] -= v_registers[(opcode & 0x00F0) >> 4];
                    break;

                case 0x06:
                    v_registers[0x0F] = v_registers[(opcode & 0x0F00) >> 8] & 0x01;
                    v_registers[(opcode & 0x0F00) >> 8] = (v_registers[(opcode & 0x0F00) >> 8]) >> 1; 
                    break;

                case 0x07:
                    v_registers[0x0F] = v_registers[(opcode &0x0F00) >> 8] < v_registers[(opcode & 0x00F0) >> 4];  

                    v_registers[(opcode & 0x0F00) >> 8] = v_registers[(opcode & 0x00F0) >> 4] - v_registers[(opcode & 0x0F00) >> 8];
                    break;

                case 0x0E:
                    v_registers[0x0F] = (v_registers[(opcode & 0x0F00) >> 8]) >> 7;
                    v_registers[(opcode & 0x0F00) >> 8] = (v_registers[(opcode & 0x0F00) >> 8]) << 1;
                    break;
            } 
            program_counter += 2;
            break;

        case 0x9000:
            if(v_registers[(opcode & 0x0F00) >> 8] != v_registers[(opcode & 0x00F0) >> 4])
                program_counter += 2;

            program_counter += 2;
            break;

        case 0xA000:
            I = opcode & 0x0FFF;
            program_counter += 2;
            break;

        case 0xB000:
            program_counter = v_registers[0x00] + (opcode & 0x0FFF);
            break;

        case 0xC000:
            v_registers[(opcode & 0x0F00) >> 8] = (rand() && (opcode & 0x00FF));
            program_counter += 2;
            break;

        case 0xD000:
            {
                unsigned short x = v_registers[(opcode & 0x0F00) >> 8] & 63;
                unsigned short y = v_registers[(opcode & 0x00F0) >> 4] & 31;
                unsigned short height = opcode & 0x000F;
                unsigned short pixel_row;

                v_registers[0x0F] = 0;
                for(int yline = 0; yline < height; yline++)
                {
                    pixel_row = memory[I + yline];
                    for(int xline = 0; xline < 8; xline++)
                    {
                        if((pixel_row & (0x80 >> xline)) != 0)
                        {
                            if(graphics[((x + xline + ((y + yline)) * 64))] == 1)
                            {
                                v_registers[0x0F] = 1;                            
                            }

                            graphics[(x + xline + ((y + yline)) * 64)] ^= 1;
                        }
                    }
                }
                draw_flag = true;
                program_counter += 2;

            }
            break;

        case 0xE000:
            switch(opcode & 0x00FF)
            {
                case 0x9E:
                    // if key == VX, skip instruction
                    if(input_keys[v_registers[(opcode & 0x0F00) >> 8]] != 0)
                        program_counter += 2;
                    break;

                case 0xA1:
                    // if key != VX, skip instruction
                    if(input_keys[v_registers[(opcode & 0x0F00) >> 8]] == 0)
                        program_counter += 2;
                    break;
            }
            break;

        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x07:
                    v_registers[(opcode & 0xF00) >> 8] = delay_timer;
                    break;

                case 0x0A:
                    {
                        bool pressed = false;
                        for(int i = 0; i < 16; i++)
                        {
                            if(input_keys[i] != 0)
                            {
                                v_registers[(opcode & 0x0F00) >> 8] = i;
                                pressed = true;
                            }
                        }

                        if(!pressed)
                            return;
                    }
                    break;

                case 0x15:
                    delay_timer = v_registers[(opcode & 0x0F00) >> 8];
                    break;

                case 0x18:
                    sound_timer = v_registers[(opcode & 0x0F00) >> 8];
                    break;

                case 0x1E:
                    v_registers[0x0F] = (I + v_registers[(opcode & 0x0F00) >> 8]) > 0xFFF;
                    I += v_registers[(opcode & 0x0F00) >> 8];
                    break;

                case 0x29:                    
                    I = v_registers[(opcode * 0x0F00) >> 8] * 0x5;
                    break;

                case 0x33:
                    memory[I] = v_registers[(opcode & 0x0F00) >> 8] / 100;
                    memory[I+1] = (v_registers[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I+2] = v_registers[(opcode & 0x0F00) >> 8] % 10;
                    break;

                case 0x55:
                    for(int vx = 0; vx <= (opcode & 0x0F00) >> 8; vx++)
                        memory[I+vx] = v_registers[vx];

                    break;

                case 0x65:
                    for(int vx = 0; vx <= (opcode & 0x0F00) >> 8; vx++)
                        v_registers[vx] = memory[I+vx];

                            break;
            }

            program_counter += 2;
            break;

        default:
            break;

    }

}












