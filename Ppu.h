#include <cstdint>
#include "olcPixelGameEngine.h"

class Cartridge;

class Ppu
{
private:
    Cartridge *cartridge;

    // uint8_t patterntables[2][4096];
    uint8_t palettes[32];

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
            uint16_t nametable : 2;
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

public:
    uint8_t nametables[2][1024];
    Ppu();
    ~Ppu();

    void connect_cartridge(Cartridge *cartridge);
    // reading and writing to main bus
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    // reading and writing to PPU bus
    uint8_t ppu_read(uint16_t addr);
    void ppu_write(uint16_t addr, uint8_t data);

    void get_pattern_table(uint8_t index, uint8_t palette, olc::Sprite *sprite);
    std::string display();

    void clock();

    olc::Pixel colour_map[0x40];
    bool emit_nmi = false;
};
