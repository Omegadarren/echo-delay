#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Ui/PlateLookAndFeel.h"


//==============================================================================
class EchoDelayAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    explicit EchoDelayAudioProcessorEditor (EchoDelayAudioProcessor&);
    ~EchoDelayAudioProcessorEditor() override;

    void paint  (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    void visibilityChanged()      override;
    void parentHierarchyChanged() override;
    void timerCallback()          override;

    // ── Layout constants ───────────────────────────────────────────────────
    static constexpr int kBaseW = 840;
    static constexpr int kBaseH = 420;
    static constexpr float       kZoomFactors[] = { 1.0f, 1.5f, 2.0f };
    static constexpr const char* kZoomLabels[]  = { "1x", "1.5x", "2x" };

private:
    EchoDelayAudioProcessor& processorRef;
    std::unique_ptr<PlateUi::PlateLookAndFeel> laf;

    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAtt  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    // ── Main section ───────────────────────────────────────────────────────
    juce::Slider echoTimeSlider, feedbackSlider, mixSlider;
    juce::Label  echoTimeLabel,  feedbackLabel,  mixLabel;
    std::unique_ptr<SliderAtt> echoTimeAtt, feedbackAtt, mixAtt;

    // ── Sync + note division ───────────────────────────────────────────────
    juce::TextButton syncToggle { "SYNC" };
    juce::ComboBox   noteDivCombo;
    juce::Label      noteDivLabel;
    std::unique_ptr<ButtonAtt> syncAtt;
    std::unique_ptr<ComboAtt>  noteDivAtt;

    // ── Tap tempo ──────────────────────────────────────────────────────────
    juce::TextButton tapButton { "TAP" };
    juce::Label      bpmDisplay;

    // ── Groove + Feel ──────────────────────────────────────────────────────
    juce::Slider grooveSlider, feelSlider;
    juce::Label  grooveLabel,  feelLabel;
    std::unique_ptr<SliderAtt> grooveAtt, feelAtt;

    // ── Feedback filters ──────────────────────────────────────────────────
    juce::Slider lowCutSlider, highCutSlider;
    juce::Label  lowCutLabel,  highCutLabel;
    std::unique_ptr<SliderAtt> lowCutAtt, highCutAtt;

    // ── Mode ──────────────────────────────────────────────────────────────
    juce::ComboBox modeCombo;
    juce::Label    modeLabel;
    std::unique_ptr<ComboAtt> modeAtt;

    // ── Saturation section ────────────────────────────────────────────────
    juce::Slider satInputSlider, satOutputSlider;
    juce::Label  satInputLabel,  satOutputLabel;
    std::unique_ptr<SliderAtt> satInputAtt, satOutputAtt;

    juce::ComboBox satStyleCombo;
    juce::Label    satStyleLabel;
    std::unique_ptr<ComboAtt> satStyleAtt;

    // ── Bottom strip ──────────────────────────────────────────────────────
    juce::Slider widthSlider, lrOffsetSlider, accentSlider;
    juce::Label  widthLabel,  lrOffsetLabel,  accentLabel;
    std::unique_ptr<SliderAtt> widthAtt, lrOffsetAtt, accentAtt;

    // ── Tweak ─────────────────────────────────────────────────────────────
    juce::TextButton primeModeToggle { "PRIME" };
    std::unique_ptr<ButtonAtt> primeModeAtt;

    // ── Zoom ──────────────────────────────────────────────────────────────
    int  zoomIndex     { 0 };
    bool lastSyncState { false };
    juce::Rectangle<int> zoomButtonBounds;

    juce::TooltipWindow tooltipWindow { this, 600 };   // 600 ms hover delay

    void applyZoom();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EchoDelayAudioProcessorEditor)
};
