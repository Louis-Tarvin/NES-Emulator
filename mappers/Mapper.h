#include <cstdint>

class Mapper
{
public:
    Mapper(uint8_t prg_banks, uint8_t chr_banks);
    ~Mapper();

    virtual uint16_t map(uint16_t addr);
    virtual uint16_t ppu_map(uint16_t addr);

protected:
    uint8_t num_prg_banks;
    uint8_t num_chr_banks;
};
