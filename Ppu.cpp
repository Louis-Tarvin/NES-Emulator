#include "Ppu.h"
#include "Cartridge.h"

Ppu::Ppu()
{
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
    return 0;
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
    }
}

uint8_t Ppu::ppu_read(uint16_t addr)
{
    if (addr >= 0 && addr < 0x2000)
    {
        return cartridge->ppu_read(addr);
    }
    return 0; // todo
}

void Ppu::clock()
{
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
                    uint8_t colour_index = (lsb & 1) + (msb & 1);
                    lsb = lsb >> 1;
                    msb = msb >> 1;
                    olc::Pixel colour = colour_index ? olc::WHITE : olc::DARK_GREY;
                    sprite->SetPixel(tile_x * 8 + (7 - pixel_x), tile_y * 8 + pixel_y, colour);
                }
            }
        }
    }
}