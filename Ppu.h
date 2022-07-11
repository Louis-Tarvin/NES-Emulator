#include <cstdint>
#include "olcPixelGameEngine.h"

class Cartridge;

class Ppu
{
private:
    Cartridge *cartridge;

    struct SpriteData
    {
        uint8_t y_top;
        uint8_t tile_index;
        uint8_t attributes;
        uint8_t x_left;
    };

    // Internal memory
    uint8_t palettes[32];

    SpriteData visible_sprites[8];

    // Registers
    uint8_t ppuctrl = 0;
    uint8_t ppumask = 0;
    uint8_t ppustatus = 0;
    uint8_t oamaddr = 0;
    uint8_t oamdata = 0;
    uint8_t ppuscroll = 0;
    uint8_t ppudata = 0;
    uint8_t oamdma = 0;

    union vram_addr
    {
        struct
        {
            uint16_t coarse_x : 5;
            uint16_t coarse_y : 5;
            uint16_t nametable_x : 1;
            uint16_t nametable_y : 1;
            uint16_t fine_y : 3;
            uint16_t unused : 1;
        };
        uint16_t reg = 0x0000;
    };

    // Internal registers
    bool address_latch = false;
    uint8_t data_buffer = 0;
    vram_addr ppuaddr;
    vram_addr addr_buffer;
    uint8_t fine_x = 0;
    uint16_t scanline = 0;
    uint16_t scanline_cycles = 0;
    uint8_t num_visible_sprites = 0;

    // Internal latches for background rendering
    uint8_t nametable_byte = 0;
    uint8_t attribute_table_byte = 0;
    uint8_t pattern_table_tile_low = 0;
    uint8_t pattern_table_tile_high = 0;
    uint16_t pattern_shift_reg_low = 0;
    uint16_t pattern_shift_reg_high = 0;
    uint16_t attribute_shift_reg_low = 0;
    uint16_t attribute_shift_reg_high = 0;

    // Internal latches for sprite rendering
    uint8_t sprite_pixels_low[8];
    uint8_t sprite_pixels_high[8];
    bool sprite_zero_possible = false;
    bool sprite_zero_rendering = false;

public:
    Ppu();
    ~Ppu();

    SpriteData oam[64];
    uint8_t nametables[2][1024];

    void connect_cartridge(Cartridge *cartridge);

    // reading and writing to main bus
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

    // reading and writing to PPU bus
    uint8_t ppu_read(uint16_t addr);
    void ppu_write(uint16_t addr, uint8_t data);

    // useful for debugging
    void get_pattern_table(uint8_t index, uint8_t palette, olc::Sprite *sprite);
    std::string display();

    void clock();
    void write_oam_byte(uint8_t addr, uint8_t data);

    olc::Pixel colour_map[0x40];
    olc::Sprite screen = olc::Sprite(256, 240);
    bool emit_nmi = false;
    bool begin_dma = false;
    uint8_t dma_page = 0;
};
