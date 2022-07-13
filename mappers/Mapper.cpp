#include "Mapper.h"

Mapper::Mapper(uint8_t prg_banks, uint8_t chr_banks)
{
    num_prg_banks = prg_banks;
    num_chr_banks = chr_banks;
}

Mapper::~Mapper()
{
}

uint32_t Mapper::map(uint16_t addr)
{
    return addr;
}

void Mapper::write(uint16_t addr, uint8_t data) {}

uint32_t Mapper::ppu_map(uint16_t addr)
{
    return addr;
}
