#pragma once
#include <cstdint>

enum class Mirroring
{
    Horizontal,
    Vertical,
    Single,
    Quad,
};

class Mapper
{
public:
    Mapper(uint8_t prg_banks, uint8_t chr_banks, Mirroring mirroring);
    ~Mapper();

    /// Map the CPU address to the PGR ROM address
    virtual uint32_t map(uint16_t addr);

    /// Write to the mapper. Allows the mapper state to be changed
    virtual void write(uint16_t addr, uint8_t data);

    /// Map the PPU address to the CHR ROM address
    virtual uint32_t ppu_map(uint16_t addr);

    // Read from PRG RAM
    virtual uint8_t prg_ram_read(uint16_t addr);

    // Write to PRG RAM
    virtual void prg_ram_write(uint16_t addr, uint8_t data);

    /// Get the Mirroring mode of the cartridge
    virtual Mirroring get_mirroring();

protected:
    uint8_t num_prg_banks;
    uint8_t num_chr_banks;
    Mirroring mirroring;
};
