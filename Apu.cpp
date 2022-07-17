#include <cmath>
#include "Apu.h"

const float PI = 3.14159;

Apu::Apu()
{
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
        data = pulse_1_flags_1;
        break;
    case 0x4001:
        data = pulse_1_flags_2;
        break;
    case 0x4002:
        data = pulse_1_flags_3;

        break;
    case 0x4003:
        data = pulse_1_flags_4;

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
        pulse_1_flags_1 = data;
        break;
    case 0x4001:
        pulse_1_flags_2 = data;
        break;
    case 0x4002:
        pulse_1_flags_3 = data;
        pulse_1_t = (pulse_1_t & 0xFF00) | data;
        pulse_1_active = pulse_1_t >= 8;
        break;
    case 0x4003:
        pulse_1_flags_4 = data;
        pulse_1_t = (pulse_1_t & 0x00FF) | ((data & 0b00000111) << 8);
        pulse_1_active = pulse_1_t >= 8;
        break;
    case 0x4006:
        pulse_2_t = (pulse_2_t & 0xFF00) | data;
        pulse_2_active = pulse_2_t >= 8;
        break;
    case 0x4007:
        pulse_2_t = (pulse_2_t & 0x00FF) | ((data & 0b00000111) << 8);
        pulse_2_active = pulse_2_t >= 8;
        break;
    default:
        break;
    }
}

void Apu::clock()
{
}

double pulse_sample(float global_time, uint16_t t)
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
        b += -approxsin(c - PI * n) / n;
    }

    return (a - b);
}

double Apu::get_sample(float global_time)
{
    double pulse_1_sample = pulse_1_active ? pulse_sample(global_time, pulse_1_t) * 0.1 : 0.0;
    double pulse_2_sample = pulse_2_active ? pulse_sample(global_time, pulse_2_t) * 0.1 : 0.0;

    return (pulse_1_sample + pulse_2_sample) * 0.5;
}