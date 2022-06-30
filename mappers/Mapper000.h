#include "Mapper.h"

class Mapper000 : public Mapper
{
public:
    Mapper000(uint8_t prg_banks, uint8_t chr_banks);
    ~Mapper000();

    uint16_t map(uint16_t addr) override;
    uint16_t ppu_map(uint16_t addr) override;
};
