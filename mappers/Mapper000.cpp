#include "Mapper000.h"

Mapper000::Mapper000(uint8_t prg_banks, uint8_t chr_banks, Mirroring m) : Mapper(prg_banks, chr_banks, m)
{
}

Mapper000::~Mapper000()
{
}

uint32_t Mapper000::map(uint16_t addr)
{
    return addr & (num_prg_banks > 1 ? 0x7FFF : 0x3FFF);
}

uint32_t Mapper000::ppu_map(uint16_t addr)
{
    return addr;
}