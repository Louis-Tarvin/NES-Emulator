#include "Mapper002.h"

Mapper002::Mapper002(uint8_t prg_banks, uint8_t chr_banks, Mirroring m) : Mapper(prg_banks, chr_banks, m)
{
}

Mapper002::~Mapper002()
{
}

uint32_t Mapper002::map(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0xBFFF)
    {
        return selected_bank * 0x4000 + (addr & 0x3FFF);
    }
    else if (addr >= 0xC000 && addr <= 0xFFFF)
    {
        return (num_prg_banks - 1) * 0x4000 + (addr & 0x3FFF);
    }
    return 0;
}

void Mapper002::write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        selected_bank = data & 0b00001111;
    }
}

uint32_t Mapper002::ppu_map(uint16_t addr)
{
    return addr;
}