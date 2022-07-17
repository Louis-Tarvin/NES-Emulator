#include <cstdint>
#include <array>
#include <string>
#include "Cartridge.h"
#include "Cpu.h"
#include "Ppu.h"
#include "Apu.h"

class Bus
{
public:
    Bus(std::string cart_file);
    ~Bus();

    Cpu cpu;
    Ppu ppu;
    Apu apu;
    std::array<uint8_t, 2048> mem;
    Cartridge cartridge;
    uint8_t controller[2];
    bool cpu_executing = false;

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    bool clock();

    std::string display();

private:
    uint8_t cpu_cycle_delay = 0;
    uint8_t controller_state[2];

    bool even_cycle = true;
    bool dma_enabled = false;
    uint8_t dma_addr = 0;
    uint8_t dma_data = 0;
    double time_per_sample = 1.0 / 44100.0;
    double time_per_clock = 1.0 / 5369318.0;
    double audio_timer = 0;
};
