#include <cmath>
#include "Apu.h"

const float PI = 3.14159;

uint8_t length_lookup[] = {10, 254, 20, 2, 40, 4, 80, 6,
                           160, 8, 60, 10, 14, 12, 26, 14,
                           12, 16, 24, 18, 48, 20, 96, 22,
                           192, 24, 72, 26, 16, 28, 32, 30};

Apu::Apu()
{
    pulse_1_envelope.amplitude = 0;
    pulse_2_envelope.amplitude = 0;
}

Apu::~Apu()
{
}

uint8_t Apu::read(uint16_t addr)
{
    uint8_t data = 0;
    switch (addr)
    {
    case 0x4000:
        break;
    case 0x4001:
        data = pulse_1_sweep.reg;
        break;
    case 0x4002:
        break;
    case 0x4003:
        break;
    default:
        break;
    }
    return data;
}

void Apu::write(uint16_t addr, uint8_t data)
{
    switch (addr)
    {
    case 0x4000:
        pulse_1_halt = (data & 0b00100000);
        pulse_1_envelope.volume = (data & 0x0F);
        pulse_1_envelope.constant_volume = (data & 0b00010000) > 0;
        switch ((data & 11000000) >> 6)
        {
        case 0b00:
            pulse_1_offset = 0.7854;
            break;
        case 0b01:
            pulse_1_offset = 1.5708;
            break;
        case 0b10:
            pulse_1_offset = 3.14159;
            break;
        case 0b11:
            pulse_1_offset = 4.71239;
            break;
        }
        break;
    case 0x4001:
        pulse_1_sweep.reg = data;
        break;
    case 0x4002:
        pulse_1_t = (pulse_1_t & 0xFF00) | data;
        pulse_1_active = pulse_1_t >= 8;
        break;
    case 0x4003:
        pulse_1_length_counter = length_lookup[(data & 0b11111000) >> 3];
        pulse_1_t = (pulse_1_t & 0x00FF) | ((data & 0b00000111) << 8);
        pulse_1_active = pulse_1_t >= 8;
        // pulse_1_envelope.start = true;
        break;
    case 0x4004:
        pulse_2_halt = (data & 0b00100000);
        pulse_2_envelope.volume = (data & 0x0F);
        pulse_2_envelope.constant_volume = (data & 0b00010000) > 0;
        switch ((data & 11000000) >> 6)
        {
        case 0b00:
            pulse_2_offset = 0.7854;
            break;
        case 0b01:
            pulse_2_offset = 1.5708;
            break;
        case 0b10:
            pulse_2_offset = 3.14159;
            break;
        case 0b11:
            pulse_2_offset = 4.71239;
            break;
        }
        break;
    case 0x4005:
        pulse_2_sweep.reg = data;
        break;
    case 0x4006:
        pulse_2_t = (pulse_2_t & 0xFF00) | data;
        pulse_2_active = pulse_2_t >= 8;
        break;
    case 0x4007:
        pulse_2_length_counter = length_lookup[(data & 0b11111000) >> 3];
        pulse_2_t = (pulse_2_t & 0x00FF) | ((data & 0b00000111) << 8);
        pulse_2_active = pulse_2_t >= 8;
        // pulse_2_envelope.start = true;
        break;
    default:
        break;
    }
}

void Apu::update_envelope(envelope env)
{
    if (env.start)
    {
        env.start = false;
        env.decay_level = 15;
        env.divider = env.volume;
    }
    else
    {
        if (env.divider == 0)
        {
            env.divider = env.volume;
            if (env.decay_level == 0)
            {
                if (env.loop)
                    env.decay_level = 15;
            }
            else
                env.decay_level--;
        }
        else
            env.divider--;
    }

    if (env.constant_volume)
        env.amplitude = env.volume;
    else
        env.amplitude = env.decay_level;
}

void Apu::quarter_frame()
{
    update_envelope(pulse_1_envelope);
    update_envelope(pulse_2_envelope);

    if (half_frame)
    {
        // This code is run every other quarter frame (every half frame)
        half_frame = false;

        // length counting
        if (pulse_1_length_counter > 0)
        {
            if (!pulse_1_halt)
                pulse_1_length_counter--;
            if (pulse_1_length_counter == 0)
                pulse_1_active = false;
        }
        if (pulse_2_length_counter > 0)
        {
            if (!pulse_2_halt)
                pulse_2_length_counter--;
            if (pulse_2_length_counter == 0)
                pulse_2_active = false;
        }

        // frequency sweeping
        if (pulse_1_sweep.enabled)
        {
            pulse_1_sweep_timer--;
            if (pulse_1_sweep_timer == 0)
            {
                pulse_1_sweep_timer = pulse_1_sweep.period;
                int delta = pulse_1_t >> pulse_1_sweep.shift;
                if (pulse_1_sweep.negate)
                    delta *= -1;
                int target_t = (int)pulse_1_t + delta;
                if (target_t > 0x7FF || target_t < 8)
                {
                    pulse_1_active = false;
                }
                else
                {
                    pulse_1_t = target_t;
                }
            }
        }
        if (pulse_2_sweep.enabled)
        {
            pulse_2_sweep_timer--;
            if (pulse_2_sweep_timer == 0)
            {
                pulse_2_sweep_timer = pulse_2_sweep.period;
                int delta = pulse_1_t >> pulse_2_sweep.shift;
                if (pulse_2_sweep.negate)
                    delta *= -1;
                int target_t = (int)pulse_2_t + delta;
                if (target_t > 0x7FF || target_t < 8)
                {
                    pulse_2_active = false;
                }
                else
                {
                    pulse_2_t = target_t;
                }
            }
        }
    }
    else
    {
        half_frame = true;
    }
}

double pulse_sample(float global_time, uint16_t t, float offset)
{
    float frequency = 1789773.0 / (16.0 * (double)(t + 1));

    // https://youtu.be/1xlCVBIF_ig
    auto approxsin = [](float x)
    {
        float j = x * 0.15915;
        j = j - (int)j;
        return 20.785 * j * (j - 0.5) * (j - 1.0f);
    };

    float a, b = 0;
    // approximate a pulse wave using sin waves (sounds better)
    for (double n = 1; n < 20; n++)
    {
        double c = n * frequency * 2.0 * PI * global_time;
        a += -approxsin(c) / n;
        b += -approxsin(c - offset * n) / n;
    }

    return (a - b);
}

double Apu::get_sample(float global_time)
{
    double pulse_1_amplitude = (((double)(pulse_1_envelope.amplitude - 1)) / 160.0);
    double pulse_1_sample = pulse_1_active ? pulse_sample(global_time, pulse_1_t, pulse_1_offset) * pulse_1_amplitude : 0.0;
    double pulse_2_amplitude = (((double)(pulse_2_envelope.amplitude - 1)) / 160.0);
    double pulse_2_sample = pulse_2_active ? pulse_sample(global_time, pulse_2_t, pulse_2_offset) * pulse_2_amplitude : 0.0;

    return (pulse_1_sample + pulse_2_sample) * 0.5;
}