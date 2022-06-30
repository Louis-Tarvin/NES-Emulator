#include <sstream>
#include <iostream>
#include "Bus.h"
#include "olcPixelGameEngine.h"

class Emulator : public olc::PixelGameEngine
{
private:
    Bus bus;
    float frame_timer = 0;

public:
    Emulator() : bus(Bus("../nestest.nes"))
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
        Clear(olc::BLACK);
        if (frame_timer <= 0)
        {
            frame_timer += 1.0f / 60.0f;
            bus.clock();
        }
        frame_timer -= dt;

        DrawString(0, 0, bus.display(), olc::WHITE);

        // Draw Pattern Tables
        DrawString(516, 338, "Pattern Table 0:", olc::WHITE);
        olc::Sprite sprite = olc::Sprite(128, 128);
        bus.ppu.get_pattern_table(0, 0, &sprite);
        DrawSprite(516, 348, &sprite);
        DrawString(648, 338, "Pattern Table 1:", olc::WHITE);
        bus.ppu.get_pattern_table(1, 0, &sprite);
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
