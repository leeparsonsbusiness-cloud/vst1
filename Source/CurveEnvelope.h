#pragma once
#include <cmath>
#include <algorithm>

class CurveEnvelope
{
public:
    CurveEnvelope() = default;
    ~CurveEnvelope() = default;

    void setSampleRate(double sRate) 
    { 
        sampleRate = sRate; 
    }
    
    void setParameters(float a, float d, float s, float r, float decayCurve)
    {
        attackTime = std::max(0.001f, a);
        decayTime = std::max(0.001f, d);
        sustainLevel = std::max(0.0f, std::min(1.0f, s));
        releaseTime = std::max(0.001f, r);
        curve = std::max(0.1f, std::min(10.0f, decayCurve));
    }

    void noteOn()
    {
        state = State::Attack;
        currentLevel = 0.0f;
        releaseLevel = 0.0f;
        timeInState = 0.0f;
    }

    void noteOff()
    {
        state = State::Release;
        releaseLevel = currentLevel;
        timeInState = 0.0f;
    }

    float getNextSample()
    {
        if (state == State::Idle)
            return 0.0f;

        float dt = 1.0f / static_cast<float>(sampleRate);
        timeInState += dt;

        switch (state)
        {
            case State::Idle:
                break;
            case State::Attack:
            {
                currentLevel = timeInState / attackTime;
                if (currentLevel >= 1.0f)
                {
                    currentLevel = 1.0f;
                    state = State::Decay;
                    timeInState = 0.0f;
                }
                break;
            }
            case State::Decay:
            {
                float progress = timeInState / decayTime;
                if (progress >= 1.0f)
                {
                    currentLevel = sustainLevel;
                    state = State::Sustain;
                    timeInState = 0.0f;
                }
                else
                {
                    // Power curve: progress^curve shifts from 0 to 1
                    float shapeVal = std::pow(progress, curve);
                    currentLevel = 1.0f - (1.0f - sustainLevel) * shapeVal;
                }
                break;
            }
            case State::Sustain:
            {
                currentLevel = sustainLevel;
                break;
            }
            case State::Release:
            {
                float progress = timeInState / releaseTime;
                if (progress >= 1.0f)
                {
                    currentLevel = 0.0f;
                    state = State::Idle;
                }
                else
                {
                    // Release follows the same curve shape starting from releaseLevel
                    float shapeVal = std::pow(progress, curve);
                    currentLevel = releaseLevel * (1.0f - shapeVal);
                }
                break;
            }
            default:
                break;
        }

        return currentLevel;
    }

    bool isActive() const 
    { 
        return state != State::Idle; 
    }
    
    void reset() 
    { 
        state = State::Idle; 
        currentLevel = 0.0f; 
        timeInState = 0.0f;
    }

private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State state = State::Idle;
    double sampleRate = 44100.0;
    
    float attackTime = 0.01f;
    float decayTime = 0.3f;
    float sustainLevel = 0.8f;
    float releaseTime = 0.5f;
    float curve = 1.0f;

    float currentLevel = 0.0f;
    float releaseLevel = 0.0f;
    float timeInState = 0.0f;
};
