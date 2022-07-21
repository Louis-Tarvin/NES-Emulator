#include <bitset>
#include "Ppu.h"
#include "Cartridge.h"

Ppu::Ppu()
{
    // Init palette memory to all zeros
    for (auto &i : palettes)
        i = 0x00;

    // Init OAM to off screen
    for (auto &i : oam)
    {
        i.y_top = 0xFF;
        i.attributes = 0;
        i.tile_index = 0;
        i.x_left = 0;
    }

    // Init array which maps hex values to colours
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
    case 0x0004:
        // OAM Data
        data = ((uint8_t *)oam)[oamaddr];
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
    if (addr == 0x4014)
    {
        // Begin Direct Memory Access
        dma_page = data;
        begin_dma = true;
        return;
    }

    addr &= 0x0007;
    switch (addr)
    {
    case 0x0000:
        // PPU Control
        ppuctrl = data;
        addr_buffer.nametable_y = (ppuctrl & 0b00000010) > 0;
        addr_buffer.nametable_x = (ppuctrl & 0b00000001) > 0;
        break;
    case 0x0001:
        // PPU Mask
        ppumask = data;
        break;
    case 0x0003:
        // OAM Address
        oamaddr = data;
        break;
    case 0x0004:
        // OAM Data
        ((uint8_t *)oam)[oamaddr] = data;
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
        if (cartridge->get_mirroring() == Mirroring::Horizontal)
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
        else if (cartridge->get_mirroring() == Mirroring::Vertical)
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
        if (addr == 0x3F10)
            addr = 0x3F00;
        else if (addr == 0x3F14)
            addr = 0x3F04;
        else if (addr == 0x3F18)
            addr = 0x3F08;
        else if (addr == 0x3F1C)
            addr = 0x3F0C;
        addr &= 0x1f;
        return palettes[addr];
    }
    return 0;
}

void Ppu::ppu_write(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;
    if (addr >= 0 && addr < 0x2000)
    {
        // Write to pattern memory (CHR RAM)
        cartridge->ppu_write(addr, data);
    }
    else if (addr < 0x3F00)
    {
        // Nametable write
        addr &= 0x0FFF; // mirroring
        if (cartridge->get_mirroring() == Mirroring::Horizontal)
        {
            if (addr >= 0 && addr < 0x0800)
            {
                nametables[0][addr & 0x3FF] = data;
            }
            else if (addr < 0x1000)
            {
                nametables[1][addr & 0x3FF] = data;
            }
        }
        else if (cartridge->get_mirroring() == Mirroring::Vertical)
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
        if (addr == 0x3F10)
            addr = 0x3F00;
        else if (addr == 0x3F14)
            addr = 0x3F04;
        else if (addr == 0x3F18)
            addr = 0x3F08;
        else if (addr == 0x3F1C)
            addr = 0x3F0C;
        addr &= 0x1f;
        palettes[addr] = data;
    }
}

/// write a byte to OAM memory
void Ppu::write_oam_byte(uint8_t addr, uint8_t data)
{
    ((uint8_t *)oam)[addr] = data;
}

// Useful reference: https://www.nesdev.org/w/images/default/d/d1/Ntsc_timing.png
bool Ppu::clock()
{
    bool quarter_frame = false;
    if (scanline_cycles == 0 && (scanline == 0 || scanline == 131 || scanline == 66 || scanline == 197))
        quarter_frame = true;

    if (scanline == 261)
    {
        // Pre-render scanline
        if (scanline_cycles == 0)
        {
            ppustatus &= 0b00111111; // unset vblank and sprite zero hit flags
            emit_nmi = false;        // reset NMI flag
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
        if ((scanline_cycles >= 1 && scanline_cycles <= 257) || (scanline_cycles > 320 && scanline_cycles <= 337))
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
                // shift background registers
                pattern_shift_reg_low <<= 1;
                pattern_shift_reg_high <<= 1;
                attribute_shift_reg_low <<= 1;
                attribute_shift_reg_high <<= 1;
            }
            if (ppumask & 0b00010000 && scanline_cycles < 258)
            {
                // decrement x coordinate of visible sprites
                // when x is 0, the sprite is being rendered, so shift the registers
                for (size_t i = 0; i < 8; i++)
                {
                    if (visible_sprites[i].x_left > 0)
                    {
                        visible_sprites[i].x_left--;
                    }
                    else
                    {
                        sprite_pixels_low[i] <<= 1;
                        sprite_pixels_high[i] <<= 1;
                    }
                }
            }
        }

        // Sprite Rendering

        if (scanline_cycles == 257)
        {
            // clear visible sprites
            for (size_t i = 0; i < 8; i++)
            {
                visible_sprites[i].y_top = 0xFF;
                visible_sprites[i].tile_index = 0xFF;
                visible_sprites[i].attributes = 0xFF;
                visible_sprites[i].x_left = 0xFF;
            }
            num_visible_sprites = 0;

            sprite_zero_possible = false;

            // Find up to 8 visible sprites
            uint8_t num_sprites = 0;
            uint8_t oam_index = 0;
            while (oam_index < 64 && num_sprites < 9)
            {
                int16_t diff = ((int16_t)scanline - (int16_t)oam[oam_index].y_top);
                if (diff >= 0 && diff < ((ppuctrl & 0b00100000) ? 16 : 8))
                {
                    if (num_sprites < 8)
                    {
                        // sprite is visible next scanline, so add it to visible sprites
                        std::memcpy(&visible_sprites[num_sprites], &oam[oam_index], sizeof(SpriteData));
                        num_visible_sprites++;
                        if (oam_index == 0)
                        {
                            // check if this is sprite zero
                            sprite_zero_possible = true;
                        }
                    }
                    else
                    {
                        // sprite overflow has occured, so set relevant flag
                        ppustatus |= 0b00100000;
                    }
                    num_sprites++;
                }
                oam_index++;
            }
        }
        else if (scanline_cycles == 340)
        {
            for (size_t i = 0; i < num_visible_sprites; i++)
            {
                uint16_t sprite_pixels_addr = 0;
                if (ppuctrl & 0b00100000)
                {
                    // 8x16 tile
                    sprite_pixels_addr = (visible_sprites[i].tile_index & 0x01) ? 0x1000 : 0;

                    if (visible_sprites[i].attributes & 0b10000000)
                    {
                        // sprite is flipped vertically
                        if (scanline - visible_sprites[i].y_top < 8)
                        {
                            // add one to get next tile
                            sprite_pixels_addr |= (((uint16_t)(visible_sprites[i].tile_index & 0xFE) + 1)) << 4; // times by 16 to get tile offset
                        }
                        else
                        {
                            sprite_pixels_addr |= ((uint16_t)(visible_sprites[i].tile_index & 0xFE)) << 4; // times by 16 to get tile offset
                        }
                        sprite_pixels_addr |= 7 - ((scanline - (uint16_t)visible_sprites[i].y_top) & 0x07); // get row
                    }
                    else
                    {
                        // sprite is NOT flipped vertically
                        if (scanline - visible_sprites[i].y_top < 8)
                        {
                            sprite_pixels_addr |= ((uint16_t)(visible_sprites[i].tile_index & 0xFE)) << 4; // times by 16 to get tile offset
                        }
                        else
                        {
                            // add one to get next tile
                            sprite_pixels_addr |= (((uint16_t)(visible_sprites[i].tile_index & 0xFE) + 1)) << 4; // times by 16 to get tile offset
                        }
                        sprite_pixels_addr |= (scanline - (uint16_t)visible_sprites[i].y_top) & 0x07; // get row
                    }
                }
                else
                {
                    // 8x8 tile
                    sprite_pixels_addr = (ppuctrl & 0b00001000) ? 0x1000 : 0;
                    sprite_pixels_addr |= (uint16_t)visible_sprites[i].tile_index << 4; // times by 16 to get tile offset
                    if (visible_sprites[i].attributes & 0b10000000)
                    {
                        // sprite is flipped vertically
                        sprite_pixels_addr |= 7 - (scanline - (uint16_t)visible_sprites[i].y_top); // get row
                    }
                    else
                    {
                        sprite_pixels_addr |= scanline - (uint16_t)visible_sprites[i].y_top; // get row
                    }
                }

                sprite_pixels_low[i] = ppu_read(sprite_pixels_addr);
                sprite_pixels_high[i] = ppu_read(sprite_pixels_addr + 8);

                // check if sprite should be flipped horizontally
                if (visible_sprites[i].attributes & 0b01000000)
                {
                    // https://stackoverflow.com/a/2602885
                    auto reversebyte = [](uint8_t b)
                    {
                        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
                        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
                        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
                        return b;
                    };

                    sprite_pixels_low[i] = reversebyte(sprite_pixels_low[i]);
                    sprite_pixels_high[i] = reversebyte(sprite_pixels_high[i]);
                }
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

    // get background colour
    uint8_t background_pixel = 0;
    uint8_t background_palette = 0;
    if (ppumask & 0b00001000)
    {
        // get pixel colour from shift register
        uint8_t lsb = (pattern_shift_reg_low & (0x8000 >> fine_x)) > 0;
        uint8_t msb = (pattern_shift_reg_high & (0x8000 >> fine_x)) > 0;
        background_pixel = lsb | (msb << 1);
        // get palette from shift register
        lsb = (attribute_shift_reg_low & (0x8000 >> fine_x)) > 0;
        msb = (attribute_shift_reg_high & (0x8000 >> fine_x)) > 0;
        background_palette = lsb | (msb << 1);
    }

    // get sprite colour
    uint8_t sprite_pixel = 0;
    uint8_t sprite_palette = 0;
    bool sprite_priority = false;
    if (ppumask & 0b00010000)
    {
        sprite_zero_rendering = false;
        for (size_t i = 0; i < num_visible_sprites; i++)
        {
            if (visible_sprites[i].x_left == 0)
            {
                // get pixel colour from shift register
                uint8_t lsb = (sprite_pixels_low[i] & 0x80) > 0;
                uint8_t msb = (sprite_pixels_high[i] & 0x80) > 0;
                sprite_pixel = lsb | (msb << 1);
                if (sprite_pixel != 0)
                {
                    // the first non-transparent sprite found is the one that is drawn
                    sprite_palette = (visible_sprites[i].attributes & 0x03) + 0x04;
                    sprite_priority = (visible_sprites[i].attributes & 0x20) == 0;
                    if (i == 0)
                        sprite_zero_rendering = true;
                    break;
                }
            }
        }
    }

    // get resulting colour
    uint8_t output_colour_index = 0;
    if (sprite_pixel != 0 && background_pixel != 0)
    {
        if (sprite_priority)
        {
            // sprite is drawn
            output_colour_index = ppu_read(0x3F00 + (sprite_palette << 2) + sprite_pixel) & 0x3F;
        }
        else
        {
            // background is drawn
            output_colour_index = ppu_read(0x3F00 + (background_palette << 2) + background_pixel) & 0x3F;
        }

        if (sprite_zero_possible && sprite_zero_rendering && (ppumask & 0b00001000) && (ppumask & 0b00010000))
        {
            if (scanline_cycles >= 1 && scanline_cycles < 258)
            {
                ppustatus |= 0b01000000; // set sprite zero hit flag
            }
        }
    }
    else if (sprite_pixel != 0)
    {
        // sprite is drawn
        output_colour_index = ppu_read(0x3F00 + (sprite_palette << 2) + sprite_pixel) & 0x3F;
    }
    else if (background_pixel != 0)
    {
        // background is drawn
        output_colour_index = ppu_read(0x3F00 + (background_palette << 2) + background_pixel) & 0x3F;
    }
    else
    {
        // draw background colour
        output_colour_index = ppu_read(0x3F00);
    }

    screen.SetPixel(scanline_cycles - 1, scanline, colour_map[output_colour_index]);

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

    return quarter_frame;
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