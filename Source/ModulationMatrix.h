#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class ModulationMatrix
{
public:
    enum Source { NoneSrc = 0, LFO1, LFO2, AmpEnv, FilterEnv, Velocity, ModWheel, KeyTrack };
    enum Target { NoneTgt = 0, Osc1Pitch, Osc2Pitch, Osc1Shape, Osc2Shape, Cutoff, Resonance, FMDepth, ChorusMix, DelayMix, ReverbMix };

    struct Slot
    {
        int source = 0;  // Source enum
        int target = 0;  // Target enum
        float depth = 0.0f; // -1.0 to +1.0
    };

    ModulationMatrix()
    {
        slots.resize(8);
    }

    ~ModulationMatrix() = default;

    Slot& getSlot(int index) { return slots[static_cast<size_t>(juce::jlimit(0, 7, index))]; }
    const Slot& getSlot(int index) const { return slots[static_cast<size_t>(juce::jlimit(0, 7, index))]; }

    float getModulationForTarget(Target target, float lfo1, float lfo2, float ampEnv, float filterEnv, float vel, float modWheel, float keyTrack)
    {
        float totalMod = 0.0f;
        for (const auto& slot : slots)
        {
            if (slot.target == static_cast<int>(target) && slot.source != 0 && std::abs(slot.depth) > 0.001f)
            {
                float srcVal = 0.0f;
                switch (slot.source)
                {
                    case LFO1: srcVal = lfo1; break;
                    case LFO2: srcVal = lfo2; break;
                    case AmpEnv: srcVal = ampEnv; break;
                    case FilterEnv: srcVal = filterEnv; break;
                    case Velocity: srcVal = vel; break;
                    case ModWheel: srcVal = modWheel; break;
                    case KeyTrack: srcVal = keyTrack; break;
                }
                totalMod += srcVal * slot.depth;
            }
        }
        return totalMod;
    }

private:
    std::vector<Slot> slots;
};
