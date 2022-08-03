#pragma once
#include <array>
#include "Mapper.h"

class Mapper004 : public Mapper
{
public:
    Mapper004(uint8_t prg_banks, uint8_t chr_banks, Mirroring m);
    ~Mapper004();

    uint32_t map(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    uint32_t ppu_map(uint16_t addr) override;
    uint8_t prg_ram_read(uint16_t addr) override;
    void prg_ram_write(uint16_t addr, uint8_t data) override;
    void clock() override;

private:
    std::array<uint8_t, 8192> prg_ram;
    bool chr_inversion = false;
    uint8_t prg_bank_mode = 0;
    uint8_t bank_select = 0;
    uint8_t chr_bank[6];
    uint8_t prg_bank[4];
    bool irq_enable = true;
    uint8_t irq_latch = 0;
    uint8_t irq_counter = 0;
};
