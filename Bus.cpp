#include <string>
#include <iostream>
#include <sstream>
#include "Bus.h"

Bus::Bus(std::string cart_file) : cartridge(Cartridge(cart_file))
{
    // Init memory to all zeros
    for (auto &i : mem)
        i = 0x00;

    cpu.connect_bus(*this);
    ppu.connect_cartridge(&cartridge);
}

Bus::~Bus() {}

uint8_t Bus::read(uint16_t addr)
{
    if (addr >= 0 && addr <= 0x1FFF)
    {
        return mem[addr & 0x07FF];
    }
    else if (addr <= 0x3FFF)
    {
        return ppu.read(addr & 0x0007);
    }
    else if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        return cartridge.read(addr);
    }

    return 0;
}

void Bus::write(uint16_t addr, uint8_t data)
{
    if (addr > 0 && addr <= 0x1FFF)
    {
        mem[addr & 0x07FF] = data;
    }
}

void Bus::clock()
{
    if (cpu_cycle_delay == 0)
    {
        cpu.clock();
        cpu_cycle_delay = 3;
    }
    ppu.clock();
    cpu_cycle_delay--;
}

/// Return a string representation of the bus for debugging
std::string Bus::display()
{
    std::stringstream output;
    output << "mem: ";
    for (int i = 0; i < 16; i++)
    {
        output << std::hex << +mem[i];
        output << " ";
    }
    output << cpu.display();
    return output.str();
}
