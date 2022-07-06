#include <bitset>
#include "Ppu.h"
#include "Cartridge.h"

Ppu::Ppu()
{
    // Init palette memory to all zeros
    for (auto &i : palettes)
        i = 0x00;

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
        // PPU Control
        // data = ppuctrl;
        break;
    case 0x0001:
        // PPU Mask
        // data = ppumask;
        break;
    case 0x0002:
        // PPU Status
        data = ppustatus;
        // reading clears vblank flag and resets address latch
        ppustatus &= 0b01111111;
        address_latch = false;
        break;
    case 0x0007:
        // PPU Data
        data = data_buffer;
        data_buffer = ppu_read(ppuaddr.reg);
        // no delay when reading palette data for some reason
        if (ppuaddr.reg >= 0x3f00)
            data = data_buffer;
        // increment PPU addr by amount specified by bit 2 of PPU CTRL
        ppuaddr.reg += (ppuctrl & 0b00000100) ? 32 : 1;
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
        // PPU Control
        ppuctrl = data;
        addr_buffer.nametable_y = (ppuctrl & 0b00000010);
        addr_buffer.nametable_x = (ppuctrl & 0b00000001);
        break;
    case 0x0001:
        // PPU Mask
        ppumask = data;
        break;
    case 0x0005:
        // PPU Scroll
        if (address_latch)
        {
            // scroll y
            addr_buffer.fine_y = data & 0b0111;
            addr_buffer.coarse_y = data >> 3;
            address_latch = false;
        }
        else
        {
            // scroll x
            fine_x = data & 0b0111;
            addr_buffer.coarse_x = data >> 3;
            address_latch = true;
        }
        break;
    case 0x0006:
        // PPU Address
        if (address_latch)
        {
            // write the high byte of the address
            addr_buffer.reg = (addr_buffer.reg & 0xFF00) | data;
            addr_buffer.reg &= 0x3FFF;
            ppuaddr = addr_buffer;
            address_latch = false;
        }
        else
        {
            // write the low byte of the address
            addr_buffer.reg = (addr_buffer.reg & 0x00FF) | (uint16_t)(data << 8);
            address_latch = true;
        }
        break;
    case 0x0007:
        // PPU Data
        ppu_write(ppuaddr.reg, data);
        // increment PPU addr by amount specified by bit 2 of PPU CTRL
        ppuaddr.reg += (ppuctrl & 0b00000100) ? 32 : 1;
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
            if (addr >= 0 && addr < 0x0800)
            {
                return nametables[0][addr & 0x3FF];
            }
            else if (addr < 0x1000)
            {
                return nametables[1][addr & 0x3FF];
            }
        }
        else if (cartridge->mirroring == Mirroring::Vertical)
        {
            if (addr >= 0 && addr < 0x0400)
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
        // Can't write to pattern memory
    }
    else if (addr < 0x3F00)
    {
        // Nametable write
        addr &= 0x0FFF; // mirroring
        if (cartridge->mirroring == Mirroring::Horizontal)
        {
            if (addr >= 0 && addr < 0x0800)
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
            if (addr >= 0 && addr < 0x0400)
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

// Useful reference: https://www.nesdev.org/w/images/default/d/d1/Ntsc_timing.png
void Ppu::clock()
{
    if (scanline == 261)
    {
        // Pre-render scanline
        if (scanline_cycles == 0)
        {
            // unset vblank flag
            ppustatus &= 0b01111111;
            // emit_nmi = false;
        }
        else if (scanline_cycles >= 280 && scanline_cycles < 305)
        {
            if (ppumask & 0b00011000)
            {
                // transfer y address from buffer
                ppuaddr.fine_y = addr_buffer.fine_y;
                ppuaddr.coarse_y = addr_buffer.coarse_y;
                ppuaddr.nametable_y = addr_buffer.nametable_y;
            }
        }
    }
    else if (scanline >= 0 && scanline < 240)
    {
        if (scanline == 0 && scanline_cycles == 0)
            scanline_cycles = 1; // skipped

        // visible scanlines
        if ((scanline_cycles >= 2 && scanline_cycles <= 257) || (scanline_cycles > 320 && scanline_cycles <= 337))
        {
            uint8_t phase = (scanline_cycles - 1) % 8;
            if (phase == 0)
            {
                // load new data into shift registers
                pattern_shift_reg_low = (pattern_shift_reg_low & 0xFF00) | pattern_table_tile_low;
                pattern_shift_reg_high = (pattern_shift_reg_high & 0xFF00) | pattern_table_tile_high;
                attribute_shift_reg_low = (attribute_shift_reg_low & 0xFF00) | ((attribute_table_byte & 0b01) ? 0xFF : 0x00);
                attribute_shift_reg_high = (attribute_shift_reg_high & 0xFF00) | ((attribute_table_byte & 0b10) ? 0xFF : 0x00);
            }
            else if (phase == 1)
            {
                // load nametable byte
                nametable_byte = ppu_read(0x2000 | (ppuaddr.reg & 0x0FFF));
            }
            else if (phase == 3)
            {
                // load attribute table byte (last two rows of nametable)
                uint16_t attribute_table_offset = ppuaddr.coarse_x >> 2; // divide by 4
                attribute_table_offset |= (ppuaddr.coarse_y >> 2) << 3;  // divide by 4 then shift to make room for x
                attribute_table_offset |= ppuaddr.nametable_x << 10;     // index into correct nametable
                attribute_table_offset |= ppuaddr.nametable_y << 11;
                attribute_table_byte = ppu_read(0x23C0 | attribute_table_offset); // load byte
                // choose corresponding 2-bit palette index
                if ((ppuaddr.coarse_y % 4) >= 2)
                    attribute_table_byte >>= 4; // if true then its the top tiles
                if ((ppuaddr.coarse_x % 4) >= 2)
                    attribute_table_byte >>= 2; // if true then its the left tile
                attribute_table_byte &= 0x03;   // zero other bits
            }
            else if (phase == 5)
            {
                // load low pattern table tile byte
                uint16_t base = (ppuctrl & 0b00010000) ? 0x1000 : 0; // get base address of pattern table
                base += (uint16_t)nametable_byte << 4;               // times tile id by 16 (two 8 bit planes)
                pattern_table_tile_low = ppu_read(base + ppuaddr.fine_y);
            }
            else if (phase == 7)
            {
                // load high pattern table tile byte
                uint16_t base = (ppuctrl & 0b00010000) ? 0x1000 : 0; // get base address of pattern table
                base += (uint16_t)nametable_byte << 4;               // times tile id by 16 (two 8 bit planes)
                pattern_table_tile_high = ppu_read(base + ppuaddr.fine_y + 8);

                if (ppumask & 0b00011000)
                {
                    // Increment coarse_x
                    if (ppuaddr.coarse_x >= 31)
                    {
                        // crossing into other nametable
                        ppuaddr.coarse_x = 0;
                        ppuaddr.nametable_x = ~ppuaddr.nametable_x; // flip nametable X
                    }
                    else
                    {
                        ppuaddr.coarse_x++;
                    }
                }
            }

            if (scanline_cycles == 256)
            {
                if (ppumask & 0b00011000)
                {
                    // Increment fine_y
                    if (ppuaddr.fine_y < 7)
                    {
                        ppuaddr.fine_y++;
                    }
                    else
                    {
                        // reached a row boundary: increment coarse_y
                        ppuaddr.fine_y = 0;
                        if (ppuaddr.coarse_y == 29)
                        {
                            // crossing into other nametable
                            ppuaddr.coarse_y = 0;
                            ppuaddr.nametable_y = ~ppuaddr.nametable_y; // flip nametable Y
                        }
                        else if (ppuaddr.coarse_y == 31)
                        {
                            ppuaddr.coarse_y = 0;
                        }
                        else
                        {
                            ppuaddr.coarse_y++;
                        }
                    }
                }
            }
            else if (scanline_cycles == 257)
            {
                // load new data into shift registers
                pattern_shift_reg_low = (pattern_shift_reg_low & 0xFF00) | pattern_table_tile_low;
                pattern_shift_reg_high = (pattern_shift_reg_high & 0xFF00) | pattern_table_tile_high;
                attribute_shift_reg_low = (attribute_shift_reg_low & 0xFF00) | ((attribute_table_byte & 0b01) ? 0xFF : 0x00);
                attribute_shift_reg_high = (attribute_shift_reg_high & 0xFF00) | ((attribute_table_byte & 0b10) ? 0xFF : 0x00);
                // transfer x address from buffer
                if (ppumask & 0b00011000)
                {
                    ppuaddr.coarse_x = addr_buffer.coarse_x;
                    ppuaddr.nametable_x = addr_buffer.nametable_x;
                }
            }
            if (ppumask & 0b00001000)
            {
                // shift registers
                pattern_shift_reg_low <<= 1;
                pattern_shift_reg_high <<= 1;
                attribute_shift_reg_low <<= 1;
                attribute_shift_reg_high <<= 1;
            }
        }
    }
    else if (scanline == 240 && scanline_cycles == 1)
    {
        // post-render scanline
        // set vblank flag
        ppustatus |= 0b10000000;
        if (ppuctrl & 0b10000000)
        {
            emit_nmi = true; // emit NMI
        }
    }

    // render pixel to screen
    u_int8_t colour_index = 0;
    if (ppumask & 0b00001000)
    {
        // get pixel colour from shift register
        uint8_t lsb = (pattern_shift_reg_low & (0x8000 >> fine_x)) > 0;
        uint8_t msb = (pattern_shift_reg_high & (0x8000 >> fine_x)) > 0;
        uint8_t pixel = lsb | (msb << 1);
        // get palette from shift register
        lsb = (attribute_shift_reg_low & (0x8000 >> fine_x)) > 0;
        msb = (attribute_shift_reg_high & (0x8000 >> fine_x)) > 0;
        uint8_t palette = lsb | (msb << 1);
        // get resulting colour
        colour_index = ppu_read(0x3F00 + (palette << 2) + pixel) & 0x3F;
    }

    screen.SetPixel(scanline_cycles - 1, scanline, colour_map[colour_index]);

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
    output << "\n ppuaddr:   " << std::hex << +ppuaddr.reg;
    output << "\n addrbuffer:" << std::hex << +addr_buffer.reg;
    output << "\n ppudata:   " << std::bitset<8>(ppudata);
    output << "\n";
    return output.str();
}