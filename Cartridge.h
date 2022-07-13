#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include "mappers/Mapper000.h"
#include "mappers/Mapper002.h"

enum class Mirroring
{
    Horizontal,
    Vertical,
    Single,
    Quad,
};

class Cartridge
{
private:
    bool persistent_memory;
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    std::shared_ptr<Mapper> mapper;

public:
    Cartridge(std::string file);
    ~Cartridge();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    uint8_t ppu_read(uint16_t addr);
    void ppu_write(uint16_t addr, uint8_t data);

    Mirroring mirroring;
};
