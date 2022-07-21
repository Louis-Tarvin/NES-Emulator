#pragma once
#include <array>
#include "Mapper.h"

class Mapper001 : public Mapper
{
public:
    Mapper001(uint8_t prg_banks, uint8_t chr_banks, Mirroring m);
    ~Mapper001();

    uint32_t map(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    uint32_t ppu_map(uint16_t addr) override;
    uint8_t prg_ram_read(uint16_t addr) override;
    void prg_ram_write(uint16_t addr, uint8_t data) override;

private:
    std::array<uint8_t, 8192> prg_ram;
    uint8_t load = 0;
    uint8_t write_counter = 0;
    bool multiple_chr_banks = true;
    uint8_t prg_bank_mode = 3;
    uint8_t chr_bank_0 = 0;
    uint8_t chr_bank_1 = 0;
    uint8_t chr_bank_combined = 0;
    uint8_t prg_bank_0 = 0;
    uint8_t prg_bank_1 = num_prg_banks - 1;
    uint8_t prg_bank_combined = 0;
};
