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
    
    void setParameters(float a, float d, float s, float r, float decTensionVal, 
                       float delaySecVal = 0.0f, float holdSecVal = 0.0f, float relTensionVal = 1.0f)
    {
        delayTime = std::max(0.0f, delaySecVal);
        attackTime = std::max(0.001f, a);
        holdTime = std::max(0.0f, holdSecVal);
        decayTime = std::max(0.001f, d);
        sustainLevel = std::max(0.0f, std::min(1.0f, s));
        releaseTime = std::max(0.001f, r);
        decTension = std::max(0.1f, std::min(10.0f, decTensionVal));
        relTension = std::max(0.1f, std::min(10.0f, relTensionVal));
    }

    void noteOn()
    {
        timeInState = 0.0f;
        currentLevel = 0.0f;
        releaseLevel = 0.0f;
        state = (delayTime > 0.0005f) ? State::Delay : State::Attack;
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

        float dt = 1.0f / static_cast<float>(sampleRate > 0.0 ? sampleRate : 44100.0);
        timeInState += dt;

        switch (state)
        {
            case State::Idle:
                break;

            case State::Delay:
            {
                if (timeInState >= delayTime)
                {
                    state = State::Attack;
                    timeInState = 0.0f;
                }
                currentLevel = 0.0f;
                break;
            }

            case State::Attack:
            {
                currentLevel = timeInState / attackTime;
                if (currentLevel >= 1.0f)
                {
                    currentLevel = 1.0f;
                    state = (holdTime > 0.0005f) ? State::Hold : State::Decay;
                    timeInState = 0.0f;
                }
                break;
            }

            case State::Hold:
            {
                currentLevel = 1.0f;
                if (timeInState >= holdTime)
                {
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
                    float shapeVal = std::pow(progress, decTension);
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
                    float shapeVal = std::pow(progress, relTension);
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
    enum class State { Idle, Delay, Attack, Hold, Decay, Sustain, Release };
    State state = State::Idle;
    double sampleRate = 44100.0;
    
    float delayTime = 0.0f;
    float attackTime = 0.01f;
    float holdTime = 0.0f;
    float decayTime = 0.3f;
    float sustainLevel = 0.8f;
    float releaseTime = 0.5f;
    float decTension = 1.0f;
    float relTension = 1.0f;

    float currentLevel = 0.0f;
    float releaseLevel = 0.0f;
    float timeInState = 0.0f;
};
