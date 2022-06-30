#include "Mapper000.h"

Mapper000::Mapper000(uint8_t prg_banks, uint8_t chr_banks) : Mapper(prg_banks, chr_banks)
{
}

Mapper000::~Mapper000()
{
}

uint16_t Mapper000::map(uint16_t addr)
{
    return addr & (num_prg_banks > 1 ? 0x7FFF : 0x3FFF);
}

uint16_t Mapper000::ppu_map(uint16_t addr)
{
    return addr;
}