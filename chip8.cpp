#include "chip8.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstring>

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
    delay_timer = 0;
    sound_timer = 0;
}

Chip8::~Chip8()
{

}

void Chip8::read_rom(std::string& path)
{
    std::ifstream input("IBMLOGO.ch8", std::ios::binary);
    std::vector<char> bytes((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
    
    if(4096 - PROGRAM_STARTING_ADDRESS >= bytes.size())
    {
        for(unsigned short index = 0; index < bytes.size(); index++)
        {
            memory[index + PROGRAM_STARTING_ADDRESS] = bytes[index];
        }
    }

    input.close();
}

void Chip8::emulate()
{
    unsigned short register_offset;
    opcode = memory[program_counter] << 8 | memory[program_counter + 1];

    program_counter += 2;
    
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
            break;

        case 0x4000:
            break;

        case 0x5000:
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
            break;

        case 0x9000:
            break;

        case 0xA000:
            I = opcode & 0x0FFF;
            program_counter += 2;
            break;

        case 0xB000:
            break;

        case 0xC000:
            break;

        case 0xD000:
            break;

        case 0xE000:
            {
                register_offset = (opcode & 0x0F00) >> 8;
                unsigned short x = v_registers[register_offset] & 64;
                register_offset = (opcode & 0x00F0) >> 4;
                unsigned short y = v_registers[register_offset] & 32;
                v_registers[0x0F] = 0;
                unsigned char rows = opcode & 0x000F;
                unsigned char sprite;

                for(unsigned char row = 0; row < rows; row++)
                {
                    sprite = memory[I + row];
                    for(unsigned char pixel = 0; pixel < 8; pixel++)
                    {
                        if((sprite & 0x80) > 0)
                        {
                            if(graphics[pixel + x + y + row] == 1)
                                v_registers[0x0F] = 1;

                            graphics[pixel + x + y + row] ^= 1;
                        }

                    }
                }
                draw_flag = true;
                program_counter += 2;
            }
            break;

        case 0xF000:
            break;

        default:
            break;

    }

}












