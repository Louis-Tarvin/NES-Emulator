#include <sstream>
#include <iostream>
#include "Bus.h"
#include "olcPixelGameEngine.h"

class Emulator : public olc::PixelGameEngine
{
private:
    int clockspeed = 1789770;
    Bus bus;
    float frame_timer = 0;
    uint8_t selected_palette = 0;
    bool realtime = true;

public:
    Emulator() : bus(Bus("../roms/nestest.nes"))
    {
        sAppName = "NES Emulator";
    }

    bool OnUserCreate() override
    {
        bus.cpu.reset();

        return true;
    }

    bool OnUserUpdate(float dt) override
    {
        bus.controller[0] = 0x00;
        bus.controller[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;
        if (GetKey(olc::Key::P).bPressed)
            (++selected_palette) &= 0x07;
        if (GetKey(olc::Key::SPACE).bPressed)
            realtime = !realtime;

        // clear the screen
        Clear(olc::BLACK);

        if (realtime)
        {
            // run the correct number of clock cycles
            for (int i = 0; i < (clockspeed * dt); i++)
            {
                bus.clock();
            }
        }
        else
        {
            // Use enter key to step through instructions
            if (GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::SHIFT).bHeld)
            {
                // perform PPU clocks
                bus.clock();
                bus.clock();
                bus.clock();
                while (bus.cpu_executing)
                    bus.clock(); // wait for CPU to finish executing
            }
            if (GetKey(olc::Key::N).bPressed)
                bus.cpu.nmi(); // emit NMI
        }

        if (!realtime)
            DrawString(516, 0, "paused", olc::YELLOW);

        DrawString(516, 10, bus.display(), olc::WHITE);

        const int nSwatchSize = 6;
        for (int p = 0; p < 8; p++)     // For each palette
            for (int s = 0; s < 4; s++) // For each index
                FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340,
                         nSwatchSize, nSwatchSize, bus.ppu.colour_map[bus.ppu.ppu_read(0x3F00 + (p << 2) + s) & 0x3F]);

        // Draw selection reticule around selected palette
        DrawRect(516 + selected_palette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);
        // Draw Pattern Tables
        DrawString(516, 328, "Pattern Table 0:", olc::WHITE);
        olc::Sprite sprite = olc::Sprite(128, 128);
        bus.ppu.get_pattern_table(0, selected_palette, &sprite);
        DrawSprite(516, 348, &sprite);
        DrawString(648, 328, "Pattern Table 1:", olc::WHITE);
        for (uint8_t y = 0; y < 30; y++)
        {
            for (uint8_t x = 0; x < 32; x++)
            {
                uint8_t id = (u_int32_t)bus.ppu.nametables[0][y * 32 + x];
                DrawPartialSprite(x * 16, y * 16, &sprite, (id & 0x0F) << 3, ((id >> 4) & 0x0F) << 3, 8, 8, 2, 0);
            }
        }
        bus.ppu.get_pattern_table(1, selected_palette, &sprite);
        DrawSprite(648, 348, &sprite);

        return true;
    }
};

int main(int argc, char const *argv[])
{
    Emulator emulator;
    if (emulator.Construct(780, 480, 2, 2))
    {
        std::cout << "Starting emulator\n";
        emulator.Start();
    }
    return 0;
}
