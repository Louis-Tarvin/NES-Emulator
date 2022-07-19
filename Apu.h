#include <cstdint>

class Apu
{
private:
    bool half_frame = false;

    union sweep
    {
        struct
        {
            uint8_t shift : 3;
            uint8_t negate : 1;
            uint8_t period : 3;
            uint8_t enabled : 1;
        };
        uint8_t reg = 0;
    };

    struct envelope
    {
        bool start;
        bool constant_volume;
        bool loop;
        uint8_t volume;
        uint8_t divider;
        uint8_t decay_level;
        uint16_t amplitude;
    };

    // Pulse channel 1 variables
    bool pulse_1_active = false;
    float pulse_1_offset = 3.14159;
    bool pulse_1_halt = false;
    sweep pulse_1_sweep;
    uint8_t pulse_1_sweep_timer = 1;
    bool pulse_1_sweep_mute = false;
    uint8_t pulse_1_length_counter = 0;
    uint16_t pulse_1_t = 0;
    envelope pulse_1_envelope;

    // Pulse channel 2 variables
    bool pulse_2_active = false;
    float pulse_2_offset = 3.14159;
    bool pulse_2_halt = false;
    sweep pulse_2_sweep;
    uint8_t pulse_2_sweep_timer = 1;
    bool pulse_2_sweep_mute = false;
    uint8_t pulse_2_length_counter = 0;
    uint16_t pulse_2_t = 0;
    envelope pulse_2_envelope;

    void update_envelope(envelope *env);

public:
    Apu();
    ~Apu();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void quarter_frame();
    double get_sample(float global_time);
};
