#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <algorithm>
#include <cmath>

class VisualizerComponent : public juce::Component, private juce::Timer
{
public:
    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder;

    VisualizerComponent()
        : forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        std::fill(fftData.begin(), fftData.end(), 0.0f);
        std::fill(scopeData.begin(), scopeData.end(), 0.0f);
        startTimerHz(30);
    }

    ~VisualizerComponent() override
    {
        stopTimer();
    }

    void pushSampleBlock(const float* channelData, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // Push to scope buffer
            scopeData[static_cast<size_t>(scopeIndex)] = sample;
            scopeIndex = (scopeIndex + 1) % scopeSize;

            // Push to FFT FIFO
            if (fifoIndex < fftSize)
            {
                fifo[static_cast<size_t>(fifoIndex)] = sample;
                fifoIndex++;
            }

            if (fifoIndex >= fftSize)
            {
                if (!nextFFTBlockReady)
                {
                    std::fill(fftData.begin(), fftData.end(), 0.0f);
                    std::copy(fifo.begin(), fifo.end(), fftData.begin());
                    nextFFTBlockReady = true;
                }
                fifoIndex = 0;
            }
        }
    }

    void setFilterCutoffAndRes(float cutoffHz, float resonance)
    {
        filterCutoff = cutoffHz;
        filterRes = resonance;
    }

    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            window.multiplyWithWindowingTable(fftData.data(), fftSize);
            forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

            for (int i = 0; i < fftSize / 2; ++i)
            {
                float level = juce::Decibels::gainToDecibels(fftData[static_cast<size_t>(i)], -100.0f);
                float normLevel = juce::jmap(level, -80.0f, 0.0f, 0.0f, 1.0f);
                normLevel = juce::jlimit(0.0f, 1.0f, normLevel);
                
                // Smooth spectrum bars
                spectrumLevels[static_cast<size_t>(i)] = spectrumLevels[static_cast<size_t>(i)] * 0.7f + normLevel * 0.3f;
            }
            nextFFTBlockReady = false;
        }

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // 1. OLED Screen Background (Deep Black-Blue)
        g.setColour(juce::Colour(0xff090a0e));
        g.fillRoundedRectangle(bounds, 5.0f);

        float w = bounds.getWidth();
        float h = bounds.getHeight();

        // 2. Subtle CRT Gridlines
        g.setColour(juce::Colour(0xff161922));
        for (float gx = 40.0f; gx < w; gx += 50.0f)
            g.drawVerticalLine(static_cast<int>(gx), 2.0f, h - 2.0f);
        for (float gy = 15.0f; gy < h; gy += 20.0f)
            g.drawHorizontalLine(static_cast<int>(gy), 2.0f, w - 2.0f);

        // 3. Draw Spectrum Analyzer (Analog Lab Amber & Cyan Glow)
        juce::Path spectrumPath;
        spectrumPath.startNewSubPath(0.0f, h);

        int numBins = fftSize / 2;
        for (int i = 0; i < numBins; ++i)
        {
            float binNorm = static_cast<float>(i) / static_cast<float>(numBins);
            float logX = std::pow(binNorm, 0.45f) * w;
            float binH = spectrumLevels[static_cast<size_t>(i)] * (h - 8.0f);
            float y = h - binH;

            if (i == 0)
                spectrumPath.startNewSubPath(logX, y);
            else
                spectrumPath.lineTo(logX, y);
        }
        spectrumPath.lineTo(w, h);
        spectrumPath.lineTo(0.0f, h);
        spectrumPath.closeSubPath();

        juce::ColourGradient specGrad(juce::Colour(0x9900d4ff), 0.0f, h,
                                       juce::Colour(0x99ffaa00), w, 0.0f, false);
        g.setGradientFill(specGrad);
        g.fillPath(spectrumPath);

        // 4. Draw Oscilloscope Waveform Trace
        juce::Path scopePath;
        float scopeStep = w / static_cast<float>(scopeSize);

        for (int i = 0; i < scopeSize; ++i)
        {
            float x = i * scopeStep;
            float sample = scopeData[static_cast<size_t>(i)];
            float y = h * 0.5f - sample * (h * 0.42f);

            if (i == 0)
                scopePath.startNewSubPath(x, y);
            else
                scopePath.lineTo(x, y);
        }

        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.85f));
        g.strokePath(scopePath, juce::PathStrokeType(1.2f));

        // 5. Filter Cutoff Cursor
        float cutoffNorm = (std::log10(std::max(20.0f, filterCutoff)) - std::log10(20.0f)) / (std::log10(20000.0f) - std::log10(20.0f));
        float cutoffX = juce::jlimit(0.0f, 1.0f, cutoffNorm) * w;
        float resH = (filterRes * 0.35f) * h;

        g.setColour(juce::Colour(0xffffaa00));
        g.drawVerticalLine(static_cast<int>(cutoffX), 4.0f, h - 4.0f);
        g.fillEllipse(cutoffX - 3.5f, h * 0.5f - resH - 3.5f, 7.0f, 7.0f);
    }

private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    std::array<float, fftSize / 2> spectrumLevels {};
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    static constexpr int scopeSize = 256;
    std::array<float, scopeSize> scopeData {};
    int scopeIndex = 0;

    float filterCutoff = 1000.0f;
    float filterRes = 0.0f;
};
