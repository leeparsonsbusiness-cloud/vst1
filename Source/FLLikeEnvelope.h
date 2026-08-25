#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <algorithm>

class FLLikeEnvelope
{
public:
    enum Stage { Idle = 0, Delay, Attack, Hold, Decay, Sustain, Release };

    struct Parameters
    {
        float delaySec = 0.0f;
        float attackSec = 0.01f;
        float holdSec = 0.0f;
        float decaySec = 0.4f;
        float sustainLevel = 0.8f;
        float releaseSec = 0.5f;
        float decTension = 1.0f; // 0.1 (log/curved) to 1.0 (linear) to 4.0 (steep exp)
        float relTension = 1.0f;
    };

    FLLikeEnvelope() = default;
    ~FLLikeEnvelope() = default;

    void setSampleRate(double sRate) { sampleRate = sRate; }

    void trigger(const Parameters& newParams)
    {
        params = newParams;
        stage = Delay;
        stageSamples = 0;
        currentVal = 0.0f;
        releaseStartVal = 0.0f;

        if (params.delaySec <= 0.0001f)
            advanceToAttack();
    }

    void noteOff()
    {
        if (stage != Idle && stage != Release)
        {
            stage = Release;
            stageSamples = 0;
            releaseStartVal = currentVal;
        }
    }

    float getNextSample()
    {
        if (stage == Idle)
            return 0.0f;

        double sRate = std::max(22050.0, sampleRate);

        if (stage == Delay)
        {
            stageSamples++;
            if (stageSamples >= static_cast<int>(params.delaySec * sRate))
                advanceToAttack();
            return 0.0f;
        }

        if (stage == Attack)
        {
            int totalAttackSamples = std::max(1, static_cast<int>(params.attackSec * sRate));
            float progress = static_cast<float>(stageSamples) / static_cast<float>(totalAttackSamples);
            currentVal = juce::jlimit(0.0f, 1.0f, progress);
            stageSamples++;

            if (stageSamples >= totalAttackSamples)
            {
                currentVal = 1.0f;
                stage = Hold;
                stageSamples = 0;
            }
            return currentVal;
        }

        if (stage == Hold)
        {
            int totalHoldSamples = static_cast<int>(params.holdSec * sRate);
            stageSamples++;
            if (stageSamples >= totalHoldSamples)
            {
                stage = Decay;
                stageSamples = 0;
            }
            return 1.0f;
        }

        if (stage == Decay)
        {
            int totalDecaySamples = std::max(1, static_cast<int>(params.decaySec * sRate));
            float progress = static_cast<float>(stageSamples) / static_cast<float>(totalDecaySamples);
            progress = juce::jlimit(0.0f, 1.0f, progress);

            // Apply tension curvature
            float curvedProgress = std::pow(progress, params.decTension);
            currentVal = 1.0f - curvedProgress * (1.0f - params.sustainLevel);
            stageSamples++;

            if (stageSamples >= totalDecaySamples)
            {
                currentVal = params.sustainLevel;
                stage = Sustain;
                stageSamples = 0;
            }
            return currentVal;
        }

        if (stage == Sustain)
        {
            currentVal = params.sustainLevel;
            return currentVal;
        }

        if (stage == Release)
        {
            int totalReleaseSamples = std::max(1, static_cast<int>(params.releaseSec * sRate));
            float progress = static_cast<float>(stageSamples) / static_cast<float>(totalReleaseSamples);
            progress = juce::jlimit(0.0f, 1.0f, progress);

            // Apply tension curvature to release tail
            float curvedProgress = std::pow(progress, params.relTension);
            currentVal = releaseStartVal * (1.0f - curvedProgress);
            stageSamples++;

            if (stageSamples >= totalReleaseSamples || currentVal <= 0.0001f)
            {
                currentVal = 0.0f;
                stage = Idle;
            }
            return currentVal;
        }

        return 0.0f;
    }

    bool isActive() const { return stage != Idle; }

private:
    void advanceToAttack()
    {
        stage = Attack;
        stageSamples = 0;
    }

    double sampleRate = 44100.0;
    Stage stage = Idle;
    int stageSamples = 0;
    float currentVal = 0.0f;
    float releaseStartVal = 0.0f;
    Parameters params;
};
