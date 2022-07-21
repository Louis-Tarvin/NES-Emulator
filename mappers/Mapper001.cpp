#include "Mapper001.h"

Mapper001::Mapper001(uint8_t prg_banks, uint8_t chr_banks, Mirroring m) : Mapper(prg_banks, chr_banks, m)
{
}

Mapper001::~Mapper001()
{
}

uint32_t Mapper001::map(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        if (prg_bank_mode == 0 || prg_bank_mode == 1)
        {
            // 32kb mode
            return prg_bank_combined * 0x8000 + (addr & 0x7FFF);
        }
        else
        {
            // 16kb mode
            if (addr < 0xC000)
                return prg_bank_0 * 0x4000 + (addr & 0x3FFF);
            else
                return prg_bank_1 * 0x4000 + (addr & 0x3FFF);
        }
    }
    return 0;
}

void Mapper001::write(uint16_t addr, uint8_t data)
{
    // This mapper is written to serially, one bit at a time
    if (data & 0b10000000)
    {
        load = 0;
        write_counter = 0;
        prg_bank_mode = 0b11;
    }
    else
    {
        load >>= 1;
        load |= (data & 1) << 4;
        write_counter++;
        if (write_counter >= 5)
        {
            // after 5 bits the data is written
            switch ((addr >> 13) & 0x3)
            {
            case 0b00:
                // setting control register

                // set mirroring mode
                switch (load & 0b00011)
                {
                case 0b00:
                    // one screen - lower bank
                    mirroring = Mirroring::Single;
                    break;
                case 0b01:
                    // one screen - upper bank
                    mirroring = Mirroring::Single;
                    break;
                case 0b10:
                    // vertical
                    mirroring = Mirroring::Vertical;
                    break;
                case 0b11:
                    // horizontal
                    mirroring = Mirroring::Horizontal;
                    break;
                }

                // set PRG ROM bank mode
                prg_bank_mode = (load & 0b01100) >> 2;
                // set CHR ROM bank mode
                multiple_chr_banks = (load & 0b10000) > 0;
                break;
            case 0b01:
                // set CHR bank 0
                if (multiple_chr_banks)
                {
                    // 4kb bank
                    chr_bank_0 = load & 0x1F;
                }
                else
                {
                    // 8kb bank
                    chr_bank_combined = load & 0x1E;
                }
                break;
            case 0b10:
                // set CHR bank 1
                if (multiple_chr_banks)
                {
                    chr_bank_1 = load & 0x1F;
                }
                break;
            case 0b11:
                // set PRG banks
                if (prg_bank_mode == 0 || prg_bank_mode == 1)
                {
                    // 32kb mode
                    prg_bank_combined = (load & 0b1110) >> 1;
                }
                else if (prg_bank_mode == 2)
                {
                    // 16kb mode - first bank is fixed
                    prg_bank_0 = 0;
                    prg_bank_1 = (load & 0x0F);
                }
                else if (prg_bank_mode == 3)
                {
                    // 16kb mode - second bank is fixed
                    prg_bank_0 = (load & 0x0F);
                    prg_bank_1 = num_prg_banks - 1;
                }
                break;
            }

            load = 0;
            write_counter = 0;
        }
    }
}

uint32_t Mapper001::ppu_map(uint16_t addr)
{
    if (multiple_chr_banks)
    {
        if (addr <= 0x0FFF)
        {
            return chr_bank_0 * 0x1000 + (addr & 0x0FFF);
        }
        else if (addr <= 0x1FFF)
        {
            return chr_bank_1 * 0x1000 + (addr & 0x0FFF);
        }
    }
    else
    {
        return chr_bank_combined * 0x2000 + (addr & 0x1FFF);
    }
    return 0;
}

uint8_t Mapper001::prg_ram_read(uint16_t addr)
{
    if (addr >= 0x6000 && addr < 0x8000)
    {
        // Reading PRG RAM
        return prg_ram[addr & 0x1FFF];
    }
    return 0;
}

void Mapper001::prg_ram_write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr < 0x8000)
    {
        // Reading PRG RAM
        prg_ram[addr & 0x1FFF] = data;
    }
}