#include <fstream>
#include <iostream>
#include "Cartridge.h"

Cartridge::Cartridge(std::string file)
{
    struct Header
    {
        char identification[4];
        uint8_t pgr_rom_size;
        uint8_t chr_rom_size;
        uint8_t flags_1;
        uint8_t flags_2;
        uint8_t pgr_ram_size;
        uint8_t tv_system_1;
        uint8_t tv_system_2;
        char unused[5];
    };

    std::ifstream ifs(file, std::fstream::binary);

    std::cout << "loading cartridge: " << file << "\n";

    if (ifs.is_open())
    {
        // Read the header
        Header header;
        ifs.read((char *)&header, sizeof(Header));

        // Verify header
        if (header.identification[0] != 'N' ||
            header.identification[1] != 'E' ||
            header.identification[2] != 'S' ||
            header.identification[3] != '\032')
        {
            std::cout << "Invalid .nes file\n";
            exit(1);
        }

        if (header.flags_1 & 0b00000100)
        {
            // skip trainer data
            ifs.seekg(512, std::ios_base::cur);
        }

        mirroring = (header.flags_1 & 1) ? Mirroring::Vertical : Mirroring::Horizontal;

        // Read PGR ROM
        prg_rom.resize(16384 * header.pgr_rom_size);
        ifs.read((char *)prg_rom.data(), prg_rom.size());

        // for (auto byte : prg_rom)
        // {
        //     std::cout << +byte << " ";
        // }

        // Read CHR ROM
        if (header.chr_rom_size)
        {
            chr_rom.resize(8192 * header.chr_rom_size);
        }
        else
        {
            chr_rom.resize(8192);
        }
        ifs.read((char *)chr_rom.data(), chr_rom.size());

        ifs.close();

        mapper = std::make_shared<Mapper000>(header.pgr_rom_size, header.chr_rom_size);
    }
    else
    {
        std::cout << "failed to load cartridge\n";
    }
}

Cartridge::~Cartridge()
{
}

uint8_t Cartridge::read(uint16_t addr)
{
    uint16_t mapped_addr = mapper->map(addr);
    return prg_rom[mapped_addr];
}

uint8_t Cartridge::ppu_read(u_int16_t addr)
{
    uint16_t mapped_addr = mapper->ppu_map(addr);
    return chr_rom[mapped_addr];
}