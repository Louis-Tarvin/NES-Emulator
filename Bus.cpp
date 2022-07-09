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
    else if (addr == 0x4016)
    {
        auto data = (controller_state[0] & 0x80) > 0;
        controller_state[0] <<= 1;
        return data;
    }

    return 0;
}

void Bus::write(uint16_t addr, uint8_t data)
{
    if (addr >= 0 && addr < 0x2000)
    {
        mem[addr & 0x07FF] = data;
    }
    else if (addr < 0x4000 || addr == 0x4014)
    {
        ppu.write(addr, data);
    }
    else if (addr == 0x4016)
    {
        controller_state[0] = controller[0];
    }
}

void Bus::clock()
{
    cpu_executing = true;
    if (cpu_cycle_delay == 0)
    {
        if (!dma_enabled)
        {
            cpu.clock();
        }
        else
        {
            // OAM Direct Memory Access
            if (even_cycle)
            {
                // read
                dma_data = read(((u_int16_t)ppu.dma_page << 8) | dma_addr);
            }
            else
            {
                // write
                ppu.write_oam_byte(dma_addr, dma_data);
                dma_addr++;
                if (dma_addr == 0)
                {
                    // DMA finished
                    dma_enabled = false;
                }
            }
            even_cycle = !even_cycle;
        }
        cpu_cycle_delay = 3;
        if (cpu.just_completed)
            cpu_executing = false;
    }
    ppu.clock();

    // check if the PPU has triggered a non-maskable interrupt
    if (ppu.emit_nmi)
    {
        ppu.emit_nmi = false;
        cpu.nmi();
    }

    // check if OAM DMA register was written to
    if (ppu.begin_dma)
    {
        ppu.begin_dma = false;
        dma_enabled = true;
        dma_addr = 0x00;
        even_cycle = true;
    }

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
    output << ppu.display();
    return output.str();
}
