#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
EchoDelayAudioProcessor::buildLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ── Echo Time (ms, free mode) ─────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "echoTime", "Echo Time",
        juce::NormalisableRange<float> (1.0f, 2000.0f, 0.1f, 0.35f),
        250.0f,
        juce::AudioParameterFloatAttributes().withLabel ("ms")));

    // ── Feedback ──────────────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "feedback", "Feedback",
        juce::NormalisableRange<float> (0.0f, 99.0f, 0.1f),
        35.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    // ── Mix ───────────────────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "mix", "Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    // ── Feedback filters ──────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "lowCut", "Low Cut",
        juce::NormalisableRange<float> (20.0f, 2000.0f, 1.0f, 0.4f),
        80.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "highCut", "High Cut",
        juce::NormalisableRange<float> (1000.0f, 20000.0f, 1.0f, 0.4f),
        8000.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    // ── Groove + Feel ─────────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "groove", "Groove",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "feel", "Feel",
        juce::NormalisableRange<float> (-50.0f, 50.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("ms")));

    // ── Mode + Sync ───────────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "mode", "Mode",
        juce::StringArray { "Single Echo", "Dual Echo", "Ping-Pong", "Rhythm Echo" },
        0));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        "syncMode", "Sync to Host",
        false));

    // Note divisions: Whole, Half, Quarter, 8th, 16th, Dot Half, Dot Quarter, Dot 8th,
    //                 Half-T, Quarter-T, 8th-T
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "noteDiv", "Note Division",
        juce::StringArray { "1/1", "1/2", "1/4", "1/8", "1/16",
                            "1/2.", "1/4.", "1/8.",
                            "1/2t", "1/4t", "1/8t" },
        3));  // default = 1/8

    // ── Saturation ────────────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "satInput", "Sat Input",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "satOutput", "Sat Output",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "satStyle", "Sat Style",
        juce::StringArray { "Clean", "Tape", "Tube", "Bit Crush" },
        1));  // default = Tape

    // ── Width + Offset + Accent ───────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "width", "Width",
        juce::NormalisableRange<float> (0.0f, 200.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "lrOffset", "L/R Offset",
        juce::NormalisableRange<float> (-50.0f, 50.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("ms")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "accent", "Accent",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    // ── Tap BPM ───────────────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "tapBpm", "Tap BPM",
        juce::NormalisableRange<float> (20.0f, 300.0f, 0.01f),
        120.0f,
        juce::AudioParameterFloatAttributes().withLabel ("bpm")));

    // ── Prime Numbers mode ────────────────────────────────────────────────
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "primeMode", "Prime Numbers",
        false));

    return layout;
}

//==============================================================================
EchoDelayAudioProcessor::EchoDelayAudioProcessor()
    : AudioProcessor (BusesProperties()
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , apvts (*this, nullptr, "Parameters", buildLayout())
{}

//==============================================================================
bool EchoDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
    auto ins = layouts.getMainInputChannelSet();
    return (ins == juce::AudioChannelSet::mono() || ins == juce::AudioChannelSet::stereo());
}

//==============================================================================
void EchoDelayAudioProcessor::prepareToPlay (double sr, int)
{
    currentSampleRate = sr;

    // Allocate delay buffers
    for (int ch = 0; ch < 2; ++ch)
    {
        delayBuffer[ch].assign (kMaxDelaySamples, 0.f);
        writePos[ch] = 0;
    }

    lpState.fill (0.f);
    hpState.fill (0.f);
    inRmsSmooth  = 0.f;
    outRmsSmooth = 0.f;
    rmsCoeff     = std::exp (-1.f / (0.05f * (float)sr));
    silentBlockCount = 0;

    // 5ms delay smoothing
    delaySmooth = std::exp (-1.f / (0.005f * (float)sr));

    smoothDelayL = 441.f;
    smoothDelayR = 441.f;
    pingPongLeft = true;

    gainHistory.fill (0.f);
    historyWritePos.store (0);
}

void EchoDelayAudioProcessor::releaseResources() {}

//==============================================================================
// Nearest prime number (for Prime Numbers mode)
static int nearestPrime (int n)
{
    if (n < 2) return 2;
    auto isPrime = [](int x) {
        if (x < 2) return false;
        for (int i = 2; i * i <= x; ++i) if (x % i == 0) return false;
        return true;
    };
    int lo = n, hi = n;
    while (!isPrime (lo) && lo > 2) --lo;
    while (!isPrime (hi)) ++hi;
    return (n - lo) <= (hi - n) ? lo : hi;
}

// Note division multiplier relative to one beat (quarter note)
static float noteDivToBeats (int div)
{
    // Indices match StringArray in buildLayout
    static const float kBeats[] = { 4.f, 2.f, 1.f, 0.5f, 0.25f,   // whole..16th
                                     3.f, 1.5f, 0.75f,               // dot half/quarter/8th
                                     4.f/3.f, 2.f/3.f, 1.f/3.f };   // triplet half/quarter/8th
    if (div < 0 || div >= 11) return 0.5f;
    return kBeats[div];
}

//==============================================================================
// Fractional delay read (linear interpolation)
float EchoDelayAudioProcessor::readDelay (int ch, float delaySamples) const noexcept
{
    const auto& buf = delayBuffer[ch];
    const int   sz  = (int)buf.size();
    if (sz == 0) return 0.f;   // guard: prepareToPlay not yet called
    delaySamples = juce::jlimit (1.f, (float)(sz - 2), delaySamples);
    int   rd        = writePos[ch] - (int)delaySamples;
    while (rd < 0) rd += sz;
    rd %= sz;
    float frac = delaySamples - std::floor (delaySamples);
    int   rd1  = (rd + 1) % sz;
    return buf[rd] * (1.f - frac) + buf[rd1] * frac;
}

void EchoDelayAudioProcessor::writeDelay (int ch, float sample) noexcept
{
    auto& buf = delayBuffer[ch];
    if (buf.empty()) return;   // guard: prepareToPlay not yet called
    buf[writePos[ch]] = sample;
    if (++writePos[ch] >= (int)buf.size()) writePos[ch] = 0;
}

//==============================================================================
// Saturation
float EchoDelayAudioProcessor::saturate (float x, int style, float driveGain) const noexcept
{
    float d = x * driveGain;
    switch (style)
    {
        case 1: // Tape – soft clip with slight 2nd harmonic
            return std::tanh (d * 1.5f) / 1.5f;

        case 2: // Tube – asymmetric soft clip
            if (d >= 0.f) return std::tanh (d * 2.f) / 2.f;
            else          return std::tanh (d * 1.2f) / 1.2f;

        case 3: // Bit Crush – 8-bit quantise
        {
            float steps = 128.f;
            return std::round (d * steps) / steps;
        }
        default: // Clean
            return juce::jlimit (-1.f, 1.f, d);
    }
}

//==============================================================================
void EchoDelayAudioProcessor::calcDelayTimes (float& delL, float& delR) const noexcept
{
    const float sr      = (float)currentSampleRate;
    bool  syncMode      = apvts.getRawParameterValue ("syncMode")->load() > 0.5f;
    float echoTimeMs    = apvts.getRawParameterValue ("echoTime")->load();
    float feel          = apvts.getRawParameterValue ("feel")->load();
    float lrOffsetMs    = apvts.getRawParameterValue ("lrOffset")->load();
    bool  prime         = apvts.getRawParameterValue ("primeMode")->load() > 0.5f;
    int   mode          = (int)apvts.getRawParameterValue ("mode")->load();

    float timeMs = echoTimeMs;

    if (syncMode)
    {
        float bpm       = apvts.getRawParameterValue ("tapBpm")->load();
        int   noteDiv   = (int)apvts.getRawParameterValue ("noteDiv")->load();
        float beatMs    = 60000.f / bpm;
        timeMs          = beatMs * noteDivToBeats (noteDiv);
    }

    timeMs = juce::jlimit (1.f, 4000.f, timeMs + feel);

    if (prime)
    {
        int ms = nearestPrime (juce::roundToInt (timeMs));
        timeMs = (float)ms;
    }

    delL = timeMs * 0.001f * sr;

    // Dual echo: right is 5/4 of left (classic "dual tape head" spacing)
    if (mode == EchoDelayAudioProcessor::DualEcho)
        delR = delL * 1.25f;
    else
        delR = delL + lrOffsetMs * 0.001f * sr;

    delL = juce::jlimit (1.f, (float)(kMaxDelaySamples - 1), delL);
    delR = juce::jlimit (1.f, (float)(kMaxDelaySamples - 1), delR);
}

//==============================================================================
void EchoDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Safety: if prepareToPlay hasn't been called yet, buffers are empty
    if (delayBuffer[0].empty() || delayBuffer[1].empty())
    {
        buffer.clear();
        return;
    }

    // Consume MIDI (tap tempo from MIDI clock is handled separately)
    for (const auto meta : midiMessages)
    {
        auto msg = meta.getMessage();
        if (msg.isMidiClock())
        {
            // 24 PPQN: accumulate and derive BPM after 24 clocks
            // (simplified – just keep for future enhancement)
        }
    }

    const int numSamples = buffer.getNumSamples();
    const int numCh      = juce::jmin (buffer.getNumChannels(), 2);
    const float sr       = (float)currentSampleRate;

    // ── Read parameters ────────────────────────────────────────────────────
    const float feedbackFrac   = apvts.getRawParameterValue ("feedback")->load() * 0.01f;
    const float mixFrac        = apvts.getRawParameterValue ("mix")->load() * 0.01f;
    const float lowCutHz       = apvts.getRawParameterValue ("lowCut")->load();
    const float highCutHz      = apvts.getRawParameterValue ("highCut")->load();
    const float groove         = apvts.getRawParameterValue ("groove")->load() * 0.01f;
    const int   mode           = (int)apvts.getRawParameterValue ("mode")->load();
    const float satInDb        = apvts.getRawParameterValue ("satInput")->load();
    const float satOutDb       = apvts.getRawParameterValue ("satOutput")->load();
    const int   satStyle       = (int)apvts.getRawParameterValue ("satStyle")->load();
    const float widthFrac      = apvts.getRawParameterValue ("width")->load() * 0.01f;
    const float accentFrac     = apvts.getRawParameterValue ("accent")->load() * 0.01f;
    const float lrOffsetMs     = apvts.getRawParameterValue ("lrOffset")->load();

    const float satInGain  = std::pow (10.f, satInDb  / 20.f);
    const float satOutGain = std::pow (10.f, satOutDb / 20.f);

    // ── 1-pole filter coefficients ──────────────────────────────────────────
    // LP for highCut feedback filter
    const float lpCoeff = 1.f - std::exp (-2.f * juce::MathConstants<float>::pi * highCutHz / sr);
    // HP for lowCut feedback filter (1-pole)
    const float hpCoeff = std::exp (-2.f * juce::MathConstants<float>::pi * lowCutHz / sr);

    // ── Target delay times ─────────────────────────────────────────────────
    float targetDelL, targetDelR;
    calcDelayTimes (targetDelL, targetDelR);

    // ── Groove: nudge even-numbered echoes (beat 2 & 4) later ────────────
    // For single-echo path we apply groove as a slight time push each other cycle.
    // Here we capture it for Rhythm Echo mode (simplified: just note groove value).

    float inSumSq  = 0.f;
    float outSumSq = 0.f;

    for (int n = 0; n < numSamples; ++n)
    {
        // Smooth delay time
        smoothDelayL = delaySmooth * smoothDelayL + (1.f - delaySmooth) * targetDelL;
        smoothDelayR = delaySmooth * smoothDelayR + (1.f - delaySmooth) * targetDelR;

        // Read input
        float inL = (numCh > 0) ? buffer.getReadPointer (0)[n] : 0.f;
        float inR = (numCh > 1) ? buffer.getReadPointer (1)[n] : inL;

        // Input saturation
        float drvIn = (satStyle == 0) ? 1.f : satInGain;
        float satL  = saturate (inL, satStyle, drvIn);
        float satR  = saturate (inR, satStyle, drvIn);

        // Read from delay lines
        float delOutL = readDelay (0, smoothDelayL);
        float delOutR = readDelay (1, smoothDelayR);

        // Feedback filter: LP then HP
        lpState[0] = lpState[0] + lpCoeff * (delOutL - lpState[0]);
        lpState[1] = lpState[1] + lpCoeff * (delOutR - lpState[1]);
        hpState[0] = hpCoeff * (hpState[0] + lpState[0] - delOutL);
        hpState[1] = hpCoeff * (hpState[1] + lpState[1] - delOutR);
        // Kill denormals / near-zero residuals that would recirculate forever
        if (std::abs (lpState[0]) < 1.e-15f) lpState[0] = 0.f;
        if (std::abs (lpState[1]) < 1.e-15f) lpState[1] = 0.f;
        if (std::abs (hpState[0]) < 1.e-15f) hpState[0] = 0.f;
        if (std::abs (hpState[1]) < 1.e-15f) hpState[1] = 0.f;
        float fbL   = lpState[0] - hpState[0];
        float fbR   = lpState[1] - hpState[1];

        // Feedback saturation (drive from input gain for warmth)
        fbL = saturate (fbL, satStyle, (satStyle == 0) ? 1.f : (satInGain * 0.5f + 0.5f));
        fbR = saturate (fbR, satStyle, (satStyle == 0) ? 1.f : (satInGain * 0.5f + 0.5f));

        // Mode-specific routing
        float writeL = 0.f, writeR = 0.f;
        float wetL   = 0.f, wetR   = 0.f;

        switch (mode)
        {
            case SingleEcho:
                // Mono sum → both delay lines
                writeL = (satL + satR) * 0.5f + fbL * feedbackFrac;
                writeR = writeL;
                wetL   = delOutL;
                wetR   = delOutL;
                break;

            case DualEcho:
                // Independent L and R
                writeL = satL + fbL * feedbackFrac;
                writeR = satR + fbR * feedbackFrac;
                wetL   = delOutL;
                wetR   = delOutR;
                break;

            case PingPong:
                // L feeds R, R feeds L (classic ping-pong)
                writeL = satL + fbR * feedbackFrac;
                writeR = satR + fbL * feedbackFrac;
                wetL   = delOutL;
                wetR   = delOutR;
                break;

            case RhythmEcho:
            {
                // 4 taps at echoTime, echoTime*groove, echoTime*2, echoTime*(2+groove)
                float t1 = smoothDelayL;
                float t2 = t1 * (1.f + groove * 0.5f);
                float t3 = t1 * 2.f;
                float t4 = t1 * (2.f + groove * 0.5f);
                t2 = juce::jlimit (1.f, (float)(kMaxDelaySamples - 1), t2);
                t3 = juce::jlimit (1.f, (float)(kMaxDelaySamples - 1), t3);
                t4 = juce::jlimit (1.f, (float)(kMaxDelaySamples - 1), t4);
                float accentBoost = 1.f + accentFrac;
                wetL = (readDelay (0, t1)
                      + readDelay (0, t2) * (0.6f + accentFrac * 0.4f) * accentBoost
                      + readDelay (0, t3) * 0.5f
                      + readDelay (0, t4) * (0.35f + accentFrac * 0.3f) * accentBoost) * 0.5f;
                wetR = wetL;
                writeL = satL + fbL * feedbackFrac;
                writeR = satR + fbR * feedbackFrac;
                break;
            }
        }

        writeDelay (0, writeL);
        writeDelay (1, writeR);

        // Output saturation + gain
        wetL = saturate (wetL, satStyle, satStyle == 0 ? satOutGain : satOutGain * 0.8f);
        wetR = saturate (wetR, satStyle, satStyle == 0 ? satOutGain : satOutGain * 0.8f);

        // Width: M/S expansion on wet signal
        float wetM = (wetL + wetR) * 0.5f;
        float wetS = (wetL - wetR) * 0.5f;
        float widthMult = widthFrac;
        float outWetL = wetM + wetS * widthMult;
        float outWetR = wetM - wetS * widthMult;

        // Mix
        float outL = inL * (1.f - mixFrac) + outWetL * mixFrac;
        float outR = inR * (1.f - mixFrac) + outWetR * mixFrac;

        if (numCh > 0) buffer.getWritePointer (0)[n] = outL;
        if (numCh > 1) buffer.getWritePointer (1)[n] = outR;

        inSumSq  += inL * inL;
        outSumSq += outL * outL;
    }

    // ── Meters ─────────────────────────────────────────────────────────────
    {
        const float rmsIn  = std::sqrt (inSumSq  / (float)numSamples);
        const float rmsOut = std::sqrt (outSumSq / (float)numSamples);

        // Per-block coefficient so the 50ms time constant is correct at any block size.
        // (rmsCoeff is a per-sample coeff; applying it once per block made decay ~2000x slower)
        const float rmsBlock = std::exp (-(float)numSamples / (0.05f * (float)currentSampleRate));
        inRmsSmooth  = rmsBlock * inRmsSmooth  + (1.f - rmsBlock) * rmsIn;
        outRmsSmooth = rmsBlock * outRmsSmooth + (1.f - rmsBlock) * rmsOut;

        auto toDb = [](float x) noexcept {
            return x > 1.e-6f ? 20.f * std::log10 (x) : -120.f;
        };
        inputLevelDb .store (juce::jlimit (-120.f, 6.f, toDb (inRmsSmooth)));
        outputLevelDb.store (juce::jlimit (-120.f, 6.f, toDb (outRmsSmooth)));

        // Silence detection: if output stays below -80 dB for 1 second, flush delay
        // buffers and filter states to prevent infinite tails and denormal accumulation.
        const int kSilenceBlocks = (int)(1.0 * currentSampleRate / std::max (1, numSamples));
        if (outRmsSmooth < 1.e-4f)   // ~-80 dB
            ++silentBlockCount;
        else
            silentBlockCount = 0;

        if (silentBlockCount > kSilenceBlocks)
        {
            for (auto& buf : delayBuffer) std::fill (buf.begin(), buf.end(), 0.f);
            lpState.fill (0.f);
            hpState.fill (0.f);
            silentBlockCount = 0;
        }

        // History: store output level for graph
        int wp = historyWritePos.load();
        gainHistory[wp] = toDb (outRmsSmooth);
        historyWritePos.store ((wp + 1) % kHistorySize);
    }
}

//==============================================================================
void EchoDelayAudioProcessor::tapTempoHit()
{
    double now = juce::Time::getMillisecondCounterHiRes();
    if (tapCount > 0 && (now - lastTapMs) > 3000.0)
        tapCount = 0;   // reset after 3s silence

    tapTimes[tapCount % kMaxTaps] = now;
    ++tapCount;
    lastTapMs = now;

    if (tapCount >= 2)
    {
        int n = std::min (tapCount, kMaxTaps);
        double sum = 0.0;
        int   idx  = tapCount - 1;
        for (int i = 1; i < n; ++i)
        {
            double a = tapTimes[(idx - i + kMaxTaps) % kMaxTaps];
            double b = tapTimes[(idx - i + 1 + kMaxTaps) % kMaxTaps];
            sum += (b - a);
        }
        double avgMs = sum / (double)(n - 1);
        float  bpm   = (float)(60000.0 / avgMs);
        bpm = juce::jlimit (20.f, 300.f, bpm);
        apvts.getParameterAsValue ("tapBpm").setValue (bpm);
        tapBpmDisplay.store (bpm);

        // Update echo time to match tap in free mode
        if (apvts.getRawParameterValue ("syncMode")->load() < 0.5f)
            apvts.getParameterAsValue ("echoTime").setValue (juce::jlimit (1.f, 2000.f, (float)avgMs));
    }
}

//==============================================================================
void EchoDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    xml->setAttribute ("editorZoom", editorZoomIndex);
    copyXmlToBinary (*xml, destData);
}

void EchoDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml && xml->hasTagName (apvts.state.getType()))
    {
        editorZoomIndex = juce::jlimit (0, 2, xml->getIntAttribute ("editorZoom", 0));
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

//==============================================================================
juce::AudioProcessorEditor* EchoDelayAudioProcessor::createEditor()
{
    return new EchoDelayAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EchoDelayAudioProcessor();
}
