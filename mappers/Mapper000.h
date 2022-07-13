#pragma once
#include "Mapper.h"

class Mapper000 : public Mapper
{
public:
    Mapper000(uint8_t prg_banks, uint8_t chr_banks);
    ~Mapper000();

    uint32_t map(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override {}
    uint32_t ppu_map(uint16_t addr) override;
};
