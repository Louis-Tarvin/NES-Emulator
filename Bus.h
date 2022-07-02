#include <cstdint>
#include <array>
#include <string>
#include "Cartridge.h"
#include "Cpu.h"
#include "Ppu.h"

class Bus
{
public:
    Bus(std::string cart_file);
    ~Bus();

    Cpu cpu;
    Ppu ppu;
    std::array<uint8_t, 2048> mem;
    Cartridge cartridge;
    uint8_t controller[2];

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void clock();

    std::string display();

private:
    uint8_t cpu_cycle_delay = 0;
    uint8_t controller_state[2];
};
