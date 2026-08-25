#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <algorithm>
#include <cmath>
#include <array>
#include <atomic>

class VisualizerComponent : public juce::Component, private juce::Timer
{
public:
    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder;

    VisualizerComponent()
        : forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        fftData.fill(0.0f);
        fftBuffer.fill(0.0f);
        scopeData.fill(0.0f);
        spectrumLevels.fill(0.0f);
        startTimerHz(30);
    }

    ~VisualizerComponent() override
    {
        stopTimer();
    }

    void pushSampleBlock(const float* channelData, int numSamples)
    {
        if (channelData == nullptr || numSamples <= 0)
            return;

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // Push to scope buffer
            int sIdx = scopeIndex.load(std::memory_order_relaxed);
            scopeData[static_cast<size_t>(sIdx % scopeSize)] = sample;
            scopeIndex.store((sIdx + 1) % scopeSize, std::memory_order_relaxed);

            // Push to FFT FIFO
            if (fifoIndex < fftSize)
            {
                fifo[static_cast<size_t>(fifoIndex)] = sample;
                fifoIndex++;
            }

            if (fifoIndex >= fftSize)
            {
                if (!nextFFTBlockReady.load(std::memory_order_relaxed))
                {
                    std::copy(fifo.begin(), fifo.end(), fftBuffer.begin());
                    nextFFTBlockReady.store(true, std::memory_order_release);
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
        if (nextFFTBlockReady.load(std::memory_order_acquire))
        {
            std::copy(fftBuffer.begin(), fftBuffer.end(), fftData.begin());
            nextFFTBlockReady.store(false, std::memory_order_release);

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
        }

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // OLED Glass Display Background
        juce::ColourGradient bgGrad(juce::Colour(0xff090a0f), bounds.getX(), bounds.getY(),
                                    juce::Colour(0xff040507), bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(bgGrad);
        g.fillRoundedRectangle(bounds, 4.0f);

        // Grid lines
        g.setColour(juce::Colour(0xff181b26));
        for (float x = bounds.getX() + 20.0f; x < bounds.getRight(); x += 30.0f)
            g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());
        for (float y = bounds.getY() + 15.0f; y < bounds.getBottom(); y += 15.0f)
            g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());

        // 1. Draw FFT Spectrum Bars
        int numBars = 48;
        float barWidth = bounds.getWidth() / static_cast<float>(numBars);

        for (int i = 0; i < numBars; ++i)
        {
            int fftIndex = static_cast<int>(std::pow(static_cast<float>(i) / static_cast<float>(numBars), 2.0f) * (fftSize / 4));
            fftIndex = juce::jlimit(0, fftSize / 2 - 1, fftIndex);
            
            float level = spectrumLevels[static_cast<size_t>(fftIndex)];
            float barHeight = level * (bounds.getHeight() - 6.0f);
            
            float bx = bounds.getX() + i * barWidth;
            float by = bounds.getBottom() - barHeight - 3.0f;

            juce::ColourGradient barGrad(juce::Colour(0xff00d4ff), bx, bounds.getBottom(),
                                         juce::Colour(0xffff0055), bx, bounds.getY(), false);
            g.setGradientFill(barGrad);
            g.fillRect(bx + 1.0f, by, barWidth - 2.0f, barHeight);
        }

        // 2. Draw Oscilloscope Waveform Overlay
        juce::Path wavePath;
        int sIdx = scopeIndex.load(std::memory_order_relaxed);
        float centerY = bounds.getCentreY();
        float waveScale = (bounds.getHeight() * 0.45f);

        wavePath.startNewSubPath(bounds.getX(), centerY);
        for (int i = 0; i < scopeSize; ++i)
        {
            int readIdx = (sIdx + i) % scopeSize;
            float sample = scopeData[static_cast<size_t>(readIdx)];
            float wx = bounds.getX() + (static_cast<float>(i) / static_cast<float>(scopeSize)) * bounds.getWidth();
            float wy = centerY - sample * waveScale;
            wavePath.lineTo(wx, wy);
        }

        g.setColour(juce::Colour(0xffffffff).withAlpha(0.65f));
        g.strokePath(wavePath, juce::PathStrokeType(1.2f));

        // 3. Draw Filter Cutoff & Resonance Curve Overlay
        if (filterCutoff > 20.0f)
        {
            float normCutoff = std::log10(filterCutoff / 20.0f) / std::log10(20000.0f / 20.0f);
            normCutoff = juce::jlimit(0.0f, 1.0f, normCutoff);
            float cx = bounds.getX() + normCutoff * bounds.getWidth();

            juce::Path filterCurve;
            filterCurve.startNewSubPath(bounds.getX(), bounds.getY() + 8.0f);
            filterCurve.lineTo(cx - 15.0f, bounds.getY() + 8.0f);
            filterCurve.quadraticTo(cx, bounds.getY() + 8.0f - (filterRes * 12.0f), cx + 25.0f, bounds.getBottom());

            g.setColour(juce::Colour(0xffffaa00).withAlpha(0.7f));
            g.strokePath(filterCurve, juce::PathStrokeType(1.5f));

            g.setColour(juce::Colour(0xffffcc00));
            g.fillEllipse(cx - 3.0f, bounds.getY() + 5.0f - (filterRes * 12.0f), 6.0f, 6.0f);
        }

        // Inner glowing border
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }

private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    static constexpr int scopeSize = 256;
    std::array<float, scopeSize> scopeData;
    std::atomic<int> scopeIndex { 0 };

    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftBuffer;
    std::array<float, fftSize * 2> fftData;
    std::array<float, fftSize / 2> spectrumLevels;
    
    int fifoIndex = 0;
    std::atomic<bool> nextFFTBlockReady { false };

    float filterCutoff = 4500.0f;
    float filterRes = 0.2f;
};
