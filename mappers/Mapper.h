#pragma once
#include <cstdint>

class Mapper
{
public:
    Mapper(uint8_t prg_banks, uint8_t chr_banks);
    ~Mapper();

    /// Map the CPU address to the PGR ROM address
    virtual uint32_t map(uint16_t addr);

    /// Write to the mapper. Allows the mapper state to be changed
    virtual void write(uint16_t addr, uint8_t data);

    /// Map the PPU address to the CHR ROM address
    virtual uint32_t ppu_map(uint16_t addr);

protected:
    uint8_t num_prg_banks;
    uint8_t num_chr_banks;
};
