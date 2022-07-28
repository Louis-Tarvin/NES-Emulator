#include <cstdint>

class Bus;

class Apu
{
private:
    Bus *bus;

    bool half_frame = false;
    bool sequencer_mode = false;
    bool irq_inhibit = false;
    uint8_t clock_timer = 0;

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

    // Triangle channel variables
    bool triangle_active = false;
    uint8_t triangle_counter_reload = 0;
    bool triangle_control_flag = false;
    bool triangle_reload_flag = false;
    uint16_t triangle_t = 0;
    uint8_t triangle_length_counter = 0;
    uint8_t triangle_linear_counter = 0;
    bool triangle_was_off = false;

    // Noise channel variables
    bool noise_active = false;
    bool noise_halt = false;
    bool noise_loop = false;
    uint16_t noise_period = 0;
    uint16_t noise_timer = 0;
    uint8_t noise_length_counter = 0;
    envelope noise_envelope;
    uint16_t noise_shift_register = 1;

    // DMC channel variables
    bool dmc_active = false;
    bool dmc_irq_enable = false;
    bool dmc_loop = false;
    uint16_t dmc_rate = 0;
    uint16_t dmc_timer = 0;
    uint8_t dmc_output_level = 0;
    uint16_t dmc_sample_addr = 0;
    uint16_t dmc_current_addr = 0;
    uint16_t dmc_sample_length = 0;
    uint16_t dmc_bytes_remaining = 0;
    uint8_t dmc_sample_buffer = 0;
    uint8_t dmc_sample_buffer_counter = 0;

    void update_envelope(envelope *env);

public:
    Apu();
    ~Apu();

    void connect_bus(Bus &bus);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void quarter_frame();
    void clock();
    double get_sample(float global_time);
};
