#include <bitset>
#include "Ppu.h"
#include "Cartridge.h"

Ppu::Ppu()
{
    // Init palette memory to all zeros
    // for (auto &i : palettes)
    //     i = 0x00;

    colour_map[0x00] = olc::Pixel(84, 84, 84);
    colour_map[0x01] = olc::Pixel(0, 30, 116);
    colour_map[0x02] = olc::Pixel(8, 16, 144);
    colour_map[0x03] = olc::Pixel(48, 0, 136);
    colour_map[0x04] = olc::Pixel(68, 0, 100);
    colour_map[0x05] = olc::Pixel(92, 0, 48);
    colour_map[0x06] = olc::Pixel(84, 4, 0);
    colour_map[0x07] = olc::Pixel(60, 24, 0);
    colour_map[0x08] = olc::Pixel(32, 42, 0);
    colour_map[0x09] = olc::Pixel(8, 58, 0);
    colour_map[0x0A] = olc::Pixel(0, 64, 0);
    colour_map[0x0B] = olc::Pixel(0, 60, 0);
    colour_map[0x0C] = olc::Pixel(0, 50, 60);
    colour_map[0x0D] = olc::Pixel(0, 0, 0);
    colour_map[0x0E] = olc::Pixel(0, 0, 0);
    colour_map[0x0F] = olc::Pixel(0, 0, 0);

    colour_map[0x10] = olc::Pixel(152, 150, 152);
    colour_map[0x11] = olc::Pixel(8, 76, 196);
    colour_map[0x12] = olc::Pixel(48, 50, 236);
    colour_map[0x13] = olc::Pixel(92, 30, 228);
    colour_map[0x14] = olc::Pixel(136, 20, 176);
    colour_map[0x15] = olc::Pixel(160, 20, 100);
    colour_map[0x16] = olc::Pixel(152, 34, 32);
    colour_map[0x17] = olc::Pixel(120, 60, 0);
    colour_map[0x18] = olc::Pixel(84, 90, 0);
    colour_map[0x19] = olc::Pixel(40, 114, 0);
    colour_map[0x1A] = olc::Pixel(8, 124, 0);
    colour_map[0x1B] = olc::Pixel(0, 118, 40);
    colour_map[0x1C] = olc::Pixel(0, 102, 120);
    colour_map[0x1D] = olc::Pixel(0, 0, 0);
    colour_map[0x1E] = olc::Pixel(0, 0, 0);
    colour_map[0x1F] = olc::Pixel(0, 0, 0);

    colour_map[0x20] = olc::Pixel(236, 238, 236);
    colour_map[0x21] = olc::Pixel(76, 154, 236);
    colour_map[0x22] = olc::Pixel(120, 124, 236);
    colour_map[0x23] = olc::Pixel(176, 98, 236);
    colour_map[0x24] = olc::Pixel(228, 84, 236);
    colour_map[0x25] = olc::Pixel(236, 88, 180);
    colour_map[0x26] = olc::Pixel(236, 106, 100);
    colour_map[0x27] = olc::Pixel(212, 136, 32);
    colour_map[0x28] = olc::Pixel(160, 170, 0);
    colour_map[0x29] = olc::Pixel(116, 196, 0);
    colour_map[0x2A] = olc::Pixel(76, 208, 32);
    colour_map[0x2B] = olc::Pixel(56, 204, 108);
    colour_map[0x2C] = olc::Pixel(56, 180, 204);
    colour_map[0x2D] = olc::Pixel(60, 60, 60);
    colour_map[0x2E] = olc::Pixel(0, 0, 0);
    colour_map[0x2F] = olc::Pixel(0, 0, 0);

    colour_map[0x30] = olc::Pixel(236, 238, 236);
    colour_map[0x31] = olc::Pixel(168, 204, 236);
    colour_map[0x32] = olc::Pixel(188, 188, 236);
    colour_map[0x33] = olc::Pixel(212, 178, 236);
    colour_map[0x34] = olc::Pixel(236, 174, 236);
    colour_map[0x35] = olc::Pixel(236, 174, 212);
    colour_map[0x36] = olc::Pixel(236, 180, 176);
    colour_map[0x37] = olc::Pixel(228, 196, 144);
    colour_map[0x38] = olc::Pixel(204, 210, 120);
    colour_map[0x39] = olc::Pixel(180, 222, 120);
    colour_map[0x3A] = olc::Pixel(168, 226, 144);
    colour_map[0x3B] = olc::Pixel(152, 226, 180);
    colour_map[0x3C] = olc::Pixel(160, 214, 228);
    colour_map[0x3D] = olc::Pixel(160, 162, 160);
    colour_map[0x3E] = olc::Pixel(0, 0, 0);
    colour_map[0x3F] = olc::Pixel(0, 0, 0);
}

Ppu::~Ppu()
{
}

void Ppu::connect_cartridge(Cartridge *cart)
{
    cartridge = cart;
}

uint8_t Ppu::read(uint16_t addr)
{
    addr &= 0x0007;
    uint8_t data = 0;
    switch (addr)
    {
    case 0x0000:
        // PPU CTRL
        data = ppuctrl;
        break;
    case 0x0001:
        // PPU MASK
        data = ppumask;
        break;
    case 0x0002:
        // PPU STATUS
        // ppustatus |= 0b10000000;
        data = ppustatus;
        // reading clears vblank flag and resets address latch
        ppustatus &= 0b01111111;
        address_latch = false;
        break;
    case 0x0007:
        // PPU DATA
        data = data_buffer;
        data_buffer = ppu_read(ppuaddr);
        // no delay when reading palette data for some reason
        if (ppuaddr > 0x3f00)
            data = data_buffer;
        // increment PPU addr by amount specified by bit 2 of PPU CTRL
        ppuaddr += (ppuctrl & 0b00000100) ? 32 : 1;
        break;
    }
    return data;
}

void Ppu::write(uint16_t addr, uint8_t data)
{
    addr &= 0x0007;
    switch (addr)
    {
    case 0x0000:
        // PPU CTRL
        ppuctrl = data;
        break;
    case 0x0001:
        ppumask = data;
        break;
    case 0x0006:
        if (address_latch)
        {
            // write the high byte of the address
            addr_buffer = (addr_buffer & 0xFF00) | data;
            ppuaddr = (addr_buffer & 0x3FFF);
            address_latch = false;
        }
        else
        {
            // write the low byte of the address
            addr_buffer = (addr_buffer & 0x00FF) | (uint16_t)(data << 8);
            address_latch = true;
        }
        break;
    case 0x0007:
        ppu_write(ppuaddr, data);
        // increment PPU addr by amount specified by bit 2 of PPU CTRL
        ppuaddr += (ppuctrl & 0b00000100) ? 32 : 1;
        break;
    }
}

uint8_t Ppu::ppu_read(uint16_t addr)
{
    addr &= 0x3FFF;
    if (addr >= 0 && addr < 0x2000)
    {
        // Pattern table lookup
        return cartridge->ppu_read(addr);
    }
    else if (addr < 0x3F00)
    {
        // Nametable lookup
        addr &= 0x0FFF; // mirroring
        if (cartridge->mirroring == Mirroring::Horizontal)
        {
            if (addr > 0 && addr < 0x0800)
            {
                return nametables[0][addr & 0x3ff];
            }
            else if (addr < 0x1000)
            {
                return nametables[1][(addr - 0x0800) & 0x3ff];
            }
        }
        else if (cartridge->mirroring == Mirroring::Vertical)
        {
            if (addr > 0 && addr < 0x0400)
            {
                return nametables[0][addr];
            }
            else if (addr < 0x0800)
            {
                return nametables[1][addr - 0x0400];
            }
            else if (addr < 0x0C00)
            {
                return nametables[0][addr - 0x0800];
            }
            else if (addr < 0x1000)
            {
                return nametables[1][addr - 0x0C00];
            }
        }
    }
    else if (addr < 0x4000)
    {
        // Palette lookup
        addr &= 0x1f;
        return palettes[addr];
    }
    return 0; // todo
}

void Ppu::ppu_write(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
    if (addr >= 0 && addr < 0x2000)
    {
        // TODO: Pattern table write??
    }
    else if (addr < 0x3F00)
    {
        // Nametable write
        addr &= 0x0FFF; // mirroring
        if (cartridge->mirroring == Mirroring::Horizontal)
        {
            if (addr > 0 && addr < 0x0800)
            {
                nametables[0][addr & 0x3ff] = data;
            }
            else if (addr < 0x1000)
            {
                nametables[1][(addr - 0x0800) & 0x3ff] = data;
            }
        }
        else if (cartridge->mirroring == Mirroring::Vertical)
        {
            if (addr > 0 && addr < 0x0400)
            {
                nametables[0][addr] = data;
            }
            else if (addr < 0x0800)
            {
                nametables[1][addr - 0x0400] = data;
            }
            else if (addr < 0x0C00)
            {
                nametables[0][addr - 0x0800] = data;
            }
            else if (addr < 0x1000)
            {
                nametables[1][addr - 0x0C00] = data;
            }
        }
    }
    else if (addr < 0x4000)
    {
        // Palette write
        addr &= 0x1f;
        palettes[addr] = data;
    }
}

void Ppu::clock()
{
    if (scanline == 261 && scanline_cycles == 0)
    {
        // pre-render scanline: unset vblank flag
        ppustatus &= 0b01111111;
        emit_nmi = false;
    }

    if (scanline == 241 && scanline_cycles == 0)
    {
        // set vblank flag
        ppustatus |= 0b10000000;
        if (ppuctrl & 0b10000000)
        {
            emit_nmi = true; // emit NMI
        }
    }

    scanline_cycles++;
    if (scanline_cycles >= 341)
    {
        scanline_cycles = 0;
        scanline++;
        if (scanline > 261)
        {
            scanline = 0;
        }
    }
}

void Ppu::get_pattern_table(uint8_t index, uint8_t palette, olc::Sprite *sprite)
{
    uint16_t base = (index == 0) ? 0x0000 : 0x1000;
    // loop through 16x16 tiles
    for (uint8_t tile_y = 0; tile_y < 16; tile_y++)
    {
        for (uint8_t tile_x = 0; tile_x < 16; tile_x++)
        {
            uint16_t offset = tile_y * 256 + tile_x * 16;
            // loop through 8x8 pixels within tile
            for (uint8_t pixel_y = 0; pixel_y < 8; pixel_y++)
            {
                uint8_t lsb = ppu_read(base + offset + pixel_y);
                uint8_t msb = ppu_read(base + offset + pixel_y + 0x8);
                for (uint8_t pixel_x = 0; pixel_x < 8; pixel_x++)
                {
                    uint8_t pixel_colour_index = (lsb & 1) | ((msb & 1) << 1);
                    uint8_t colour_index = ppu_read(0x3F00 + (palette << 2) + pixel_colour_index) & 0x3F;
                    lsb = lsb >> 1;
                    msb = msb >> 1;
                    // olc::Pixel background = (((pixel_x + pixel_y) % 2) == 0) ? olc::BLACK : olc::DARK_GREY;
                    // olc::Pixel colour = pixel_colour_index ? olc::WHITE : background;
                    sprite->SetPixel(tile_x * 8 + (7 - pixel_x), tile_y * 8 + pixel_y, colour_map[colour_index]);
                }
            }
        }
    }
}

/// Return a string representation of the PPU for debugging
std::string Ppu::display()
{
    std::stringstream output;
    output << "\n ppuctrl:   " << std::bitset<8>(ppuctrl);
    output << "\n ppumask:   " << std::bitset<8>(ppumask);
    output << "\n ppustatus: " << std::bitset<8>(ppustatus);
    output << "\n ppuaddr:   " << std::hex << +ppuaddr;
    output << "\n addrbuffer:" << std::hex << +addr_buffer;
    output << "\n ppudata:   " << std::bitset<8>(ppudata);
    output << "\n";
    return output.str();
}