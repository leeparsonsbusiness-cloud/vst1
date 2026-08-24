#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class PostFX
{
public:
    PostFX();
    ~PostFX() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts, 
                 double playHeadTimeSeconds, double bpm, double ppqPosition, bool isHostPlaying);

private:
    // Simple Biquad class for peaking EQ, shelving EQ, and HPF
    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
        float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
        void reset() { x1 = x2 = y1 = y2 = 0.0f; }
        
        float process(float in)
        {
            float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
            x2 = x1; x1 = in;
            y2 = y1; y1 = out;
            return out;
        }
        
        void setPeakEQ(float sRate, float frequency, float Q, float gainDB)
        {
            float gain = std::pow(10.0f, gainDB / 40.0f);
            float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sRate;
            float alpha = std::sin(w0) / (2.0f * Q);
            float cosw0 = std::cos(w0);
            
            float a0 = 1.0f + alpha / gain;
            b0 = (1.0f + alpha * gain) / a0;
            b1 = (-2.0f * cosw0) / a0;
            b2 = (1.0f - alpha * gain) / a0;
            a1 = (-2.0f * cosw0) / a0;
            a2 = (1.0f - alpha / gain) / a0;
        }
        
        void setHighShelf(float sRate, float frequency, float gainDB)
        {
            float gain = std::pow(10.0f, gainDB / 40.0f);
            float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sRate;
            float cosw0 = std::cos(w0);
            float sinw0 = std::sin(w0);
            float alpha = sinw0 / 2.0f * std::sqrt(2.0f);
            
            float a0 = (gain + 1.0f) - (gain - 1.0f) * cosw0 + 2.0f * std::sqrt(gain) * alpha;
            b0 = (gain * ((gain + 1.0f) + (gain - 1.0f) * cosw0 + 2.0f * std::sqrt(gain) * alpha)) / a0;
            b1 = (-2.0f * gain * ((gain - 1.0f) + (gain + 1.0f) * cosw0)) / a0;
            b2 = (gain * ((gain + 1.0f) + (gain - 1.0f) * cosw0 - 2.0f * std::sqrt(gain) * alpha)) / a0;
            a1 = (2.0f * ((gain - 1.0f) - (gain + 1.0f) * cosw0)) / a0;
            a2 = ((gain + 1.0f) - (gain - 1.0f) * cosw0 - 2.0f * std::sqrt(gain) * alpha) / a0;
        }

        void setHPF(float sRate, float frequency, float Q)
        {
            float w0 = 2.0f * juce::MathConstants<float>::pi * frequency / sRate;
            float cosw0 = std::cos(w0);
            float alpha = std::sin(w0) / (2.0f * Q);
            
            float a0 = 1.0f + alpha;
            b0 = ((1.0f + cosw0) / 2.0f) / a0;
            b1 = -(1.0f + cosw0) / a0;
            b2 = ((1.0f + cosw0) / 2.0f) / a0;
            a1 = (-2.0f * cosw0) / a0;
            a2 = (1.0f - alpha) / a0;
        }
    };

    struct Filter1stOrder
    {
        float y1 = 0.0f;
        void reset() { y1 = 0.0f; }
        
        float processLP(float in, float alpha)
        {
            float out = in * (1.0f - alpha) + y1 * alpha;
            y1 = out;
            return out;
        }
    };

    // Peaking EQ for 2.5 kHz boost (Trash)
    Biquad trashEQ_L, trashEQ_R;
    // High-shelf EQ for 14 kHz boost (Gloss)
    Biquad glossEQ_L, glossEQ_R;

    // 2-band Crossover LPF/HPF filters (for OTT)
    Filter1stOrder crossoverLPF_L, crossoverLPF_R;
    
    // Envelope trackers for OTT
    float envLowL = 0.0f, envLowR = 0.0f;
    float envHighL = 0.0f, envHighR = 0.0f;

    // Mono maker HPF at 120 Hz for Side channel
    Biquad sideHPF;

    // HPF for "The Drop / Build" Macro
    Biquad dropHPF_L, dropHPF_R;

    // Reverb module
    juce::dsp::Reverb reverb;

    // Chorus module
    juce::dsp::Chorus<float> chorus;

    // Ping-Pong Delay circular buffers
    juce::AudioBuffer<float> delayBufferL;
    juce::AudioBuffer<float> delayBufferR;
    int delayWritePosL = 0;
    int delayWritePosR = 0;
    int delayBufferLength = 0;

    double sampleRate = 44100.0;
    double internalTime = 0.0;
};
