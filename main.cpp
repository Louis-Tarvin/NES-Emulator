#include <sstream>
#include <iostream>
#include "Bus.h"
#include "olcPixelGameEngine.h"

class Emulator : public olc::PixelGameEngine
{
private:
    int clockspeed = 1789770 * 3;
    Bus bus;
    float frame_timer = 0;
    uint8_t selected_palette = 0;
    uint8_t selected_nametable = 0;
    bool realtime = true;

public:
    Emulator() : bus(Bus("../roms/Backward_Branch.nes"))
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
        // Controller inputs
        bus.controller[0] = 0x00;
        bus.controller[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
        bus.controller[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

        // Cycle pattern table palette
        if (GetKey(olc::Key::P).bPressed)
            (++selected_palette) &= 0x07;

        // Cycle visible nametable
        if (GetKey(olc::Key::TAB).bPressed)
            selected_nametable = 1 - selected_nametable;

        // Pause realtime execution
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

        // Draw Pattern Tables
        DrawString(516, 338, "Pattern Table 0:", olc::WHITE);
        olc::Sprite sprite = olc::Sprite(128, 128);
        bus.ppu.get_pattern_table(0, selected_palette, &sprite);
        DrawSprite(516, 348, &sprite);
        DrawString(648, 338, "Pattern Table 1:", olc::WHITE);
        bus.ppu.get_pattern_table(1, selected_palette, &sprite);
        DrawSprite(648, 348, &sprite);
        for (uint8_t y = 0; y < 30; y++)
        {
            for (uint8_t x = 0; x < 32; x++)
            {
                uint8_t id = (u_int32_t)bus.ppu.nametables[selected_nametable][y * 32 + x];
                DrawPartialSprite((x * 8) + 516, (y * 8) + 98, &sprite, (id & 0x0F) << 3, ((id >> 4) & 0x0F) << 3, 8, 8, 1, 0);
            }
        }

        // Draw main screen
        DrawSprite(0, 0, &bus.ppu.screen, 2);

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
