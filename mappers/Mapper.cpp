#include "Mapper.h"

Mapper::Mapper(uint8_t prg_banks, uint8_t chr_banks)
{
    num_prg_banks = prg_banks;
    num_chr_banks = chr_banks;
}

Mapper::~Mapper()
{
}

uint16_t Mapper::map(uint16_t addr)
{
    return addr;
}

uint16_t Mapper::ppu_map(uint16_t addr)
{
    return addr;
}
