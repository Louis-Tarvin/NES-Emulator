#include <cstdint>
#include "olcPixelGameEngine.h"

class Cartridge;

class Ppu
{
private:
    Cartridge *cartridge;

    uint8_t patterntables[2][4096];
    uint8_t nametables[2][1024];

    uint8_t ppuctrl = 0;
    uint8_t ppumask = 0;
    uint8_t ppustatus = 0;
    uint8_t oamaddr = 0;
    uint8_t oamdata = 0;
    uint8_t ppuscroll = 0;
    uint8_t ppuaddr = 0;
    uint8_t ppudata = 0;
    uint8_t oamdma = 0;

public:
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

    void clock();
};
