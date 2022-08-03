#include "Mapper.h"

Mapper::Mapper(uint8_t prg_banks, uint8_t chr_banks, Mirroring m)
{
    num_prg_banks = prg_banks;
    num_chr_banks = chr_banks;
    mirroring = m;
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

uint8_t Mapper::prg_ram_read(uint16_t addr)
{
    return 0;
}

void Mapper::prg_ram_write(uint16_t addr, uint8_t data) {}

Mirroring Mapper::get_mirroring()
{
    return mirroring;
}

void Mapper::clock() {}
