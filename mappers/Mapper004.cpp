#include "Mapper004.h"

Mapper004::Mapper004(uint8_t prg_banks, uint8_t chr_banks, Mirroring m) : Mapper(prg_banks, chr_banks, m)
{
    prg_bank[2] = num_prg_banks * 2 - 2;
    prg_bank[3] = num_prg_banks * 2 - 1;
}

Mapper004::~Mapper004()
{
}

uint32_t Mapper004::map(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        if (prg_bank_mode)
        {
            uint8_t index = 0;
            if (addr < 0xA000)
                index = 2;
            else if (addr < 0xC000)
                index = 1;
            else if (addr < 0xE000)
                index = 0;
            else
                index = 3;
            return prg_bank[index] * 0x2000 + (addr & 0x1FFF);
        }
        else
        {
            uint8_t index = 0;
            if (addr < 0xA000)
                index = 0;
            else if (addr < 0xC000)
                index = 1;
            else if (addr < 0xE000)
                index = 2;
            else
                index = 3;
            return prg_bank[index] * 0x2000 + (addr & 0x1FFF);
        }
    }
    return 0;
}

void Mapper004::write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr < 0xA000)
    {
        if (addr % 2 == 0)
        {
            // even -> bank select
            bank_select = (data & 0b00000111);
            prg_bank_mode = (data & 0b01000000);
            chr_inversion = (data & 0b10000000);
        }
        else
        {
            // odd -> bank data
            if (bank_select <= 0b101)
            {
                chr_bank[bank_select] = data;
            }
            else if (bank_select == 0b110)
            {
                prg_bank[0] = data;
            }
            else if (bank_select == 0b111)
            {
                prg_bank[1] = data;
            }
        }
    }
    else if (addr < 0xC000)
    {
        if (addr % 2 == 0)
        {
            // even -> mirroring
            if (data & 0b01)
                mirroring = Mirroring::Horizontal;
            else
                mirroring = Mirroring::Vertical;
        }
    }
    else if (addr < 0xE000)
    {
        if (addr % 2 == 0)
        {
            // even -> irq latch
            irq_latch = data;
        }
        else
        {
            // odd -> irq reload
            irq_counter = irq_latch;
        }
    }
    else if (addr <= 0xFFFF)
    {
        if (addr % 2 == 0)
        {
            // even -> irq disable
            irq_enable = false;
        }
        else
        {
            // odd -> irq enable
            irq_enable = true;
        }
    }
}

uint32_t Mapper004::ppu_map(uint16_t addr)
{
    if (chr_inversion)
    {
        if (addr < 0x0400)
            return chr_bank[2] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x0800)
            return chr_bank[3] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x0C00)
            return chr_bank[4] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x1000)
            return chr_bank[5] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x1800)
            return chr_bank[0] * 0x0400 + (addr & 0x07FF);
        else if (addr < 0x2000)
            return chr_bank[1] * 0x0400 + (addr & 0x07FF);
    }
    else
    {
        if (addr < 0x0800)
            return chr_bank[0] * 0x0400 + (addr & 0x07FF);
        else if (addr < 0x1000)
            return chr_bank[1] * 0x0400 + (addr & 0x07FF);
        else if (addr < 0x1400)
            return chr_bank[2] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x1800)
            return chr_bank[3] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x1C00)
            return chr_bank[4] * 0x0400 + (addr & 0x03FF);
        else if (addr < 0x2000)
            return chr_bank[5] * 0x0400 + (addr & 0x03FF);
    }
    return 0;
}

uint8_t Mapper004::prg_ram_read(uint16_t addr)
{
    if (addr >= 0x6000 && addr < 0x8000)
    {
        // Reading PRG RAM
        return prg_ram[addr & 0x1FFF];
    }
    return 0;
}

void Mapper004::prg_ram_write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr < 0x8000)
    {
        // Reading PRG RAM
        prg_ram[addr & 0x1FFF] = data;
    }
}

void Mapper004::clock()
{
    if (irq_counter == 0)
        irq_counter = irq_latch;
    else
        irq_counter--;

    if (irq_counter == 0 && irq_enable)
        emit_irq = true;
}