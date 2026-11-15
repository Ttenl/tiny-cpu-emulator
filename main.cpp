#include <cstddef>
#include <fstream>
#include <iostream>


#define CF_BIT  0
#define EOF_BIT 2
#define ZF_BIT  4
#define OF_BIT  5

#define SET_FLAG(f, b)  ((f) |=  (1 << (b)))
#define CLR_FLAG(f, b)  ((f) &= ~(1 << (b)))
#define GET_FLAG(f, b)  (((f) >> (b)) & 1)

void u_flags(unsigned char& eflags, unsigned char a, unsigned char b,
    unsigned short result, bool is_add)
{
    unsigned char res = (unsigned char)result;

    if (res == 0)
        SET_FLAG(eflags, ZF_BIT);
    else
        CLR_FLAG(eflags, ZF_BIT);

    if (result > 255)
        SET_FLAG(eflags, CF_BIT);
    else
        CLR_FLAG(eflags, CF_BIT);

    if (is_add)
    {
        if ((a < 128 && b < 128 && res >= 128) || (a >= 128 && b >= 128 && res < 128))
            SET_FLAG(eflags, OF_BIT);
        else
            CLR_FLAG(eflags, OF_BIT);
    }
    else
    {
        if ((a < 128 && b >= 128 && res >= 128) || (a >= 128 && b < 128 && res < 128))
            SET_FLAG(eflags, OF_BIT);
        else
            CLR_FLAG(eflags, OF_BIT);
    }
}

void l_flags(unsigned char& eflags, unsigned char result)
{
    if (result == 0)
        SET_FLAG(eflags, ZF_BIT);
    else
        CLR_FLAG(eflags, ZF_BIT);

    CLR_FLAG(eflags, CF_BIT);
    CLR_FLAG(eflags, OF_BIT);
}


void IN(unsigned short IR, unsigned char regs[], unsigned char& eflags, std::ifstream& input_file)
{
    int dest = (IR >> 8) & 7;
    char c;
    if (input_file.get(c))
    {
        regs[dest] = (unsigned char)c;
        CLR_FLAG(eflags, EOF_BIT);
    }
    else
    {
        SET_FLAG(eflags, EOF_BIT);
    }
}

void OUT(unsigned short IR, unsigned char regs[], std::ofstream& output_file)
{
    int src = (IR >> 4) & 7;
    output_file.put((char)regs[src]);
}

void INC(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    unsigned short result = regs[dest] + 1;
    u_flags(eflags, regs[dest], 1, result, true);
    regs[dest] = (unsigned char)result;
}

void DEC(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    unsigned short result = regs[dest] + 255;
    u_flags(eflags, regs[dest], 1, result, false);
    regs[dest] = (unsigned char)result;
}

void ADD(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int mode = (IR >> 7) & 1;
    unsigned char a, b;

    if (mode == 0)
    {
        a = regs[dest];
        b = regs[(IR >> 4) & 7];
    }
    else
    {
        a = regs[(IR >> 4) & 7];
        b = regs[IR & 7];
    }

    unsigned short result = a + b;
    u_flags(eflags, a, b, result, true);
    regs[dest] = (unsigned char)result;
}

void SUB(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int mode = (IR >> 7) & 1;
    unsigned char a, b;

    if (mode == 0)
    {
        a = regs[dest];
        b = regs[(IR >> 4) & 7];
    }
    else
    {
        a = regs[(IR >> 4) & 7];
        b = regs[IR & 7];
    }

    unsigned short result = a + (256 - b);
    u_flags(eflags, a, b, result, false);
    regs[dest] = (unsigned char)result;
}

void XOR_OP(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int mode = (IR >> 7) & 1;
    unsigned char result;

    if (mode == 0)
        result = regs[dest] ^ regs[(IR >> 4) & 7];
    else
        result = regs[(IR >> 4) & 7] ^ regs[IR & 7];

    regs[dest] = result;
    l_flags(eflags, result);
}

void OR_OP(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int mode = (IR >> 7) & 1;
    unsigned char result;

    if (mode == 0)
        result = regs[dest] | regs[(IR >> 4) & 7];
    else
        result = regs[(IR >> 4) & 7] | regs[IR & 7];

    regs[dest] = result;
    l_flags(eflags, result);
}

void AND_OP(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int mode = (IR >> 7) & 1;
    unsigned char result;

    if (mode == 0)
        result = regs[dest] & regs[(IR >> 4) & 7];
    else
        result = regs[(IR >> 4) & 7] & regs[IR & 7];

    regs[dest] = result;
    l_flags(eflags, result);
}

void LSR(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int src = (IR >> 4) & 7;
    unsigned char val = regs[src];

    if (val & 1)
        SET_FLAG(eflags, CF_BIT);
    else
        CLR_FLAG(eflags, CF_BIT);

    regs[dest] = val >> 1;

    if (regs[dest] == 0)
        SET_FLAG(eflags, ZF_BIT);
    else
        CLR_FLAG(eflags, ZF_BIT);

    CLR_FLAG(eflags, OF_BIT);
}

void LSL(unsigned short IR, unsigned char regs[], unsigned char& eflags)
{
    int dest = (IR >> 8) & 7;
    int src = (IR >> 4) & 7;
    unsigned char val = regs[src];

    if (val & 0x80)
        SET_FLAG(eflags, CF_BIT);
    else
        CLR_FLAG(eflags, CF_BIT);

    regs[dest] = (unsigned char)(val << 1);

    if (regs[dest] == 0)
        SET_FLAG(eflags, ZF_BIT);
    else
        CLR_FLAG(eflags, ZF_BIT);

    CLR_FLAG(eflags, OF_BIT);
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: tiny_cpu_emulator <program.bin> <input.txt> <output.txt>\n";
        return 1;
    }

    unsigned char regs[8] = {};
    unsigned char RAM[256] = {};
    unsigned char eflags = 0b00000010;

    std::ifstream bin_file(argv[1], std::ios::binary);
    std::ifstream input_file(argv[2], std::ios::binary);
    if (!bin_file || !input_file)
    {
        std::cerr << "Could not open the program or input file.\n";
        return 1;
    }

    char symbol;
    std::size_t programSize = 0;
    while (bin_file.get(symbol))
    {
        if (programSize == sizeof RAM)
        {
            std::cerr << "Program exceeds 256 bytes.\n";
            return 1;
        }
        RAM[programSize++] = static_cast<unsigned char>(symbol);
    }
    if (bin_file.bad() || programSize == 0 || programSize % 2 != 0)
    {
        std::cerr << "Program must contain complete 16-bit instructions.\n";
        return 1;
    }

    std::ofstream output_file(argv[3], std::ios::binary);
    if (!output_file)
    {
        std::cerr << "Could not open the output file.\n";
        return 1;
    }

    int PC = 0;
    bool running = true;

    while (running)
    {
        if (PC < 0 || PC % 2 != 0 || static_cast<std::size_t>(PC) + 1 >= programSize)
        {
            std::cerr << "Invalid instruction address: " << PC << '\n';
            return 1;
        }

        unsigned short IR = static_cast<unsigned short>((RAM[PC] << 8) | RAM[PC + 1]);
        PC += 2;

        int opcode = (IR >> 11) & 0x1F;
        int offset = IR & 0xFF;
        if (offset >= 128)
            offset -= 256;

        switch (opcode)
        {
        case 0x00: break;
        case 0x01: IN(IR, regs, eflags, input_file); break;
        case 0x02: OUT(IR, regs, output_file); break;
        case 0x03: regs[(IR >> 8) & 7] = regs[(IR >> 4) & 7]; break;
        case 0x04: regs[(IR >> 8) & 7] = (unsigned char)(IR & 0xFF); break;
        case 0x05:
            PC += offset - 2;
            break;
        case 0x06:
            if (GET_FLAG(eflags, ZF_BIT))
                PC += offset - 2;
            break;
        case 0x07:
            if (!GET_FLAG(eflags, ZF_BIT))
                PC += offset - 2;
            break;
        case 0x08:
            if (GET_FLAG(eflags, CF_BIT))
                PC += offset - 2;
            break;
        case 0x09:
            if (GET_FLAG(eflags, OF_BIT))
                PC += offset - 2;
            break;
        case 0x0A:
            if (GET_FLAG(eflags, EOF_BIT))
                PC += offset - 2;
            break;
        case 0x0B: running = false; break;
        case 0x0C: INC(IR, regs, eflags); break;
        case 0x0D: DEC(IR, regs, eflags); break;
        case 0x0E: ADD(IR, regs, eflags); break;
        case 0x0F: SUB(IR, regs, eflags); break;
        case 0x10: XOR_OP(IR, regs, eflags); break;
        case 0x11: OR_OP(IR, regs, eflags); break;
        case 0x12: AND_OP(IR, regs, eflags); break;
        case 0x13: LSR(IR, regs, eflags); break;
        case 0x14: LSL(IR, regs, eflags); break;
        default:
            std::cerr << "Unknown opcode: " << opcode << '\n';
            return 1;
        }
    }

    output_file.close();
    if (input_file.bad() || !output_file)
    {
        std::cerr << "Could not read the input or write the output.\n";
        return 1;
    }
    return 0;
}
