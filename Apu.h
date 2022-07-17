#include <cstdint>

class Apu
{
private:
    // DDLC VVVV
    uint8_t pulse_1_flags_1 = 0;
    // EPPP NSSS
    uint8_t pulse_1_flags_2 = 0;
    // TTTT TTTT
    uint8_t pulse_1_flags_3 = 0;
    // LLLL LTTT
    uint8_t pulse_1_flags_4 = 0;
    uint16_t pulse_1_t = 0;

    bool pulse_1_active = false;
    uint16_t pulse_2_t = 0;
    bool pulse_2_active = false;

public:
    Apu();
    ~Apu();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void clock();
    double get_sample(float global_time);
};
