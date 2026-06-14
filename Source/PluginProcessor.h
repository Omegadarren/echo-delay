#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <cmath>

//==============================================================================
/**
 * Echo Delay  –  Analog-style stereo delay inspired by classic tape/bucket-brigade units.
 *
 * Signal Path:
 *   Input → [saturation input stage] → delay lines → [feedback filters] →
 *   [saturation feedback stage] → Mix → [output gain]
 *
 * Modes:
 *   Single Echo   – mono delay, centre-panned
 *   Dual Echo     – L and R independent delays (left uses time, right uses time*ratio)
 *   Ping-Pong     – signal bounces L→R→L
 *   Rhythm Echo   – up to 4 tap pattern using groove subdivision
 *
 * Timing:
 *   Free mode: 1–2000 ms
 *   Sync mode: note values synced to host BPM (whole, half, quarter, 8th, 16th,
 *              dotted, and triplet variants)
 *   Tap Tempo: click to set BPM manually
 *
 * Groove / Feel:
 *   Groove (0–100%): shuffle/swing amount applied to even taps
 *   Feel  (-50..+50 ms): nudge all echoes earlier (rushin) or later (draggin)
 *
 * Feedback filters:
 *   LowCut (20–1000 Hz)  – HPF on feedback loop
 *   HighCut (1k–20 kHz)  – LPF on feedback loop
 *
 * Saturation:
 *   Input gain trim, Output gain trim, Style (Clean/Tape/Tube/Bit Crush)
 *
 * Width + Offset:
 *   Width (0–200%): stereo spread of wet signal
 *   L/R Offset (-50..+50 ms): nudge Right channel relative to Left
 *   Accent (0–100%): emphasise beats 2 & 4 (velocity-like emphasis)
 *
 * Prime Numbers mode (Tweak):
 *   Locks delay time to nearest prime-number millisecond for dense, non-phasing echoes.
 *
 * APVTS parameter IDs:
 *   "echoTime"   "feedback"    "mix"
 *   "lowCut"     "highCut"
 *   "groove"     "feel"
 *   "mode"       "syncMode"    "noteDiv"
 *   "satInput"   "satOutput"   "satStyle"
 *   "width"      "lrOffset"    "accent"
 *   "tapBpm"     "primeMode"
 */

class EchoDelayAudioProcessor : public juce::AudioProcessor
{
public:
    // ── Constants ─────────────────────────────────────────────────────────────
    static constexpr int    kMaxDelaySamples = 192000 * 4;  // 4 s @ 192 kHz
    static constexpr int    kHistorySize     = 512;
    static constexpr int    kNumTaps         = 4;           // Rhythm Echo taps

    enum Mode { SingleEcho = 0, DualEcho, PingPong, RhythmEcho };
    enum SatStyle { Clean = 0, Tape, Tube, BitCrush };

    EchoDelayAudioProcessor();
    ~EchoDelayAudioProcessor() override = default;

    void prepareToPlay    (double sampleRate, int samplesPerBlock) override;
    void releaseResources () override;
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock     (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName()  const override { return JucePlugin_Name; }
    bool acceptsMidi()            const override { return true; }
    bool producesMidi()           const override { return false; }
    bool isMidiEffect()           const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms()                            override { return 1; }
    int getCurrentProgram()                         override { return 0; }
    void setCurrentProgram(int)                     override {}
    const juce::String getProgramName(int)          override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout buildLayout();

    // ── Public state for UI ────────────────────────────────────────────────
    juce::AudioProcessorValueTreeState apvts;

    std::atomic<float> inputLevelDb  { -120.f };
    std::atomic<float> outputLevelDb { -120.f };
    std::atomic<float> tapBpmDisplay { 120.f  };  // current effective BPM

    // Gain history ring buffer for the graph display
    std::array<float, kHistorySize> gainHistory {};
    std::atomic<int>                historyWritePos { 0 };

    // Tap-tempo state (UI calls tapTempoHit())
    void tapTempoHit();

    int editorZoomIndex { 0 };

private:
    // ── DSP state ─────────────────────────────────────────────────────────────
    double currentSampleRate { 44100.0 };

    // Stereo delay buffers
    std::array<std::vector<float>, 2> delayBuffer;
    std::array<int, 2>                writePos { 0, 0 };

    // Feedback filter states (1-pole LP + HP per channel)
    std::array<float, 2> lpState { 0.f, 0.f };
    std::array<float, 2> hpState { 0.f, 0.f };

    // Meters
    float inRmsSmooth    { 0.f };
    float outRmsSmooth   { 0.f };
    float rmsCoeff       { 0.f };
    int   silentBlockCount { 0 };

    // Tap-tempo
    static constexpr int   kMaxTaps      = 8;
    std::array<double, kMaxTaps> tapTimes {};
    int    tapCount  { 0 };
    double lastTapMs { 0.0 };

    // Ping-pong phase state
    bool   pingPongLeft { true };

    // Smooth delay time to avoid zipper noise
    float smoothDelayL { 441.f };
    float smoothDelayR { 441.f };
    float delaySmooth  { 0.f  };   // coeff set in prepareToPlay

    // Fractional delay (linear interpolation)
    float readDelay (int channel, float delaySamples) const noexcept;
    void  writeDelay (int channel, float sample) noexcept;

    // Saturation
    float saturate (float x, int style, float drive) const noexcept;

    // Returns effective delay in samples for L and R
    void  calcDelayTimes (float& delL, float& delR) const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EchoDelayAudioProcessor)
};
