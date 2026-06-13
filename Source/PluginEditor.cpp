#include "PluginEditor.h"

constexpr float       EchoDelayAudioProcessorEditor::kZoomFactors[];
constexpr const char* EchoDelayAudioProcessorEditor::kZoomLabels[];

//==============================================================================
//  Colour palette – matching the Tremolo / Musical EQ theme
//==============================================================================
static const juce::Colour kBg        {  20,  21,  32 };
static const juce::Colour kPanel     {  14,  15,  24 };
static const juce::Colour kHeader    {  22,  54,  98 };
static const juce::Colour kAccent    {  65, 145, 210 };
static const juce::Colour kTextMain  { 225, 238, 255 };
static const juce::Colour kTextDim   { 115, 152, 195 };
static const juce::Colour kDivider   {  48,  82, 124 };
static const juce::Colour kTapActive { 220, 190,  55 };   // gold flash on tap
static const juce::Colour kGreen     {  55, 210, 110 };
static const juce::Colour kRed       { 220,  60,  60 };

//==============================================================================
//  Custom LookAndFeel
//==============================================================================
class EchoDelayLAF : public juce::LookAndFeel_V4
{
public:
    EchoDelayLAF()
    {
        setColour (juce::Slider::thumbColourId,               kAccent);
        setColour (juce::Slider::rotarySliderFillColourId,    kAccent);
        setColour (juce::Slider::rotarySliderOutlineColourId, kDivider);
        setColour (juce::Slider::textBoxTextColourId,         kTextDim);
        setColour (juce::Slider::textBoxBackgroundColourId,   kPanel.darker (0.3f));
        setColour (juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxHighlightColourId,    kAccent.withAlpha (0.4f));
        setColour (juce::Label::textColourId,                 kTextMain);
        setColour (juce::Label::backgroundColourId,           juce::Colours::transparentBlack);
        setColour (juce::ComboBox::textColourId,              kTextMain);
        setColour (juce::ComboBox::backgroundColourId,        kPanel);
        setColour (juce::ComboBox::outlineColourId,           kDivider);
        setColour (juce::ComboBox::arrowColourId,             kTextDim);
        setColour (juce::PopupMenu::backgroundColourId,       juce::Colour (14, 16, 26));
        setColour (juce::PopupMenu::textColourId,             kTextMain);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, kAccent.withAlpha (0.28f));
        setColour (juce::TextButton::buttonColourId,          kPanel);
        setColour (juce::TextButton::buttonOnColourId,        kAccent.withAlpha (0.28f));
        setColour (juce::TextButton::textColourOnId,          kTextMain);
        setColour (juce::TextButton::textColourOffId,         kTextDim.withAlpha (0.55f));
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                           float pos, float startA, float endA, juce::Slider& s) override
    {
        const float cx = x + w * 0.5f, cy = y + h * 0.5f;
        const float r  = juce::jmin (w, h) * 0.5f - 4.f;

        juce::Colour fillCol = s.findColour (juce::Slider::rotarySliderFillColourId);

        juce::Path arc;
        arc.addCentredArc (cx, cy, r, r, 0.f, startA, endA, true);
        g.setColour (kDivider.withAlpha (0.35f));
        g.strokePath (arc, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        float fe = startA + pos * (endA - startA);
        juce::Path fill;
        fill.addCentredArc (cx, cy, r, r, 0.f, startA, fe, true);
        g.setColour (fillCol.withAlpha (0.80f));
        g.strokePath (fill, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        const float kr = r * 0.62f;
        juce::ColourGradient grad (juce::Colour (90, 98, 118), cx - kr * 0.4f, cy - kr * 0.5f,
                                    juce::Colour (28, 30, 42),  cx + kr * 0.4f, cy + kr * 0.5f, false);
        g.setGradientFill (grad); g.fillEllipse (cx-kr, cy-kr, kr*2, kr*2);
        g.setColour (kDivider.withAlpha (0.5f)); g.drawEllipse (cx-kr, cy-kr, kr*2, kr*2, 1.f);
        juce::ColourGradient rim (juce::Colours::white.withAlpha (0.18f), cx, cy-kr,
                                   juce::Colours::transparentBlack, cx, cy+kr, false);
        g.setGradientFill (rim); g.fillEllipse (cx-kr, cy-kr, kr*2, kr*2);

        float a = startA + pos * (endA - startA);
        g.setColour (kTextMain.withAlpha (0.9f));
        g.drawLine (cx + (kr * 0.25f) * std::sin (a), cy - (kr * 0.25f) * std::cos (a),
                    cx + (kr - 4.f)   * std::sin (a), cy - (kr - 4.f)   * std::cos (a), 1.8f);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&,
                                bool, bool) override
    {
        auto bounds = b.getLocalBounds().toFloat().reduced (0.5f);
        g.setColour (b.getToggleState() ? kAccent.withAlpha (0.28f) : kPanel);
        g.fillRoundedRectangle (bounds, 5.f);
        g.setColour (b.getToggleState() ? kAccent : kDivider.withAlpha (0.55f));
        g.drawRoundedRectangle (bounds.reduced (0.5f), 4.5f, 1.f);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& b, bool, bool) override
    {
        g.setFont (juce::Font (10.f, juce::Font::bold));
        g.setColour (b.getToggleState() ? kTextMain : kTextDim.withAlpha (0.6f));
        g.drawText (b.getButtonText(), b.getLocalBounds(), juce::Justification::centred, false);
    }

    juce::Font getLabelFont (juce::Label&) override { return juce::Font (11.5f); }
    juce::Font getComboBoxFont (juce::ComboBox&) override { return juce::Font (11.f, juce::Font::bold); }
    juce::Font getPopupMenuFont() override { return juce::Font (12.5f); }
};

//==============================================================================
//  Helpers
//==============================================================================
static void drawVUBar (juce::Graphics& g, float x, float y, float w, float h, float db)
{
    g.setColour (juce::Colour (3, 5, 12));
    g.fillRoundedRectangle (x, y, w, h, 2.f);
    g.setColour (kDivider.withAlpha (0.25f));
    g.drawRoundedRectangle (x, y, w, h, 2.f, 0.6f);

    float norm = juce::jlimit (0.f, 1.f, (juce::jlimit (-60.f, 6.f, db) + 60.f) / 66.f);
    if (norm > 0.002f)
    {
        float fillH = norm * h;
        juce::ColourGradient grad (kRed, x, y, kGreen, x, y + h, false);
        grad.addColour (0.55, kGreen);
        grad.addColour (0.72, juce::Colour (225, 200, 50));
        grad.addColour (0.86, juce::Colour (220, 120, 50));
        g.setGradientFill (grad);
        g.fillRoundedRectangle (x, y + h - fillH, w, fillH, 2.f);
    }
    float y0 = y + h * (1.f - 60.f / 66.f);
    g.setColour (kTextDim.withAlpha (0.30f));
    g.fillRect (x, y0, w, 0.8f);
}

static void drawSectionPanel (juce::Graphics& g, juce::Rectangle<int> r,
                               const juce::String& title, juce::Colour accent)
{
    g.setColour (kPanel.withAlpha (0.70f));
    g.fillRoundedRectangle (r.toFloat(), 6.f);
    g.setColour (kDivider.withAlpha (0.30f));
    g.drawRoundedRectangle (r.toFloat().reduced (0.5f), 5.5f, 0.8f);
    // Top accent bar
    g.setColour (accent.withAlpha (0.40f));
    g.fillRoundedRectangle ((float)r.getX() + 4.f, (float)r.getY() - 2.f,
                             (float)r.getWidth() - 8.f, 3.f, 1.5f);
    // Section title
    g.setFont (juce::Font (8.5f, juce::Font::bold));
    g.setColour (kTextDim.withAlpha (0.55f));
    g.drawText (title, r.getX(), r.getY() + 4, r.getWidth(), 12,
                juce::Justification::centred, false);
}

//==============================================================================
//  Constructor
//==============================================================================
EchoDelayAudioProcessorEditor::EchoDelayAudioProcessorEditor (EchoDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    laf = std::make_unique<EchoDelayLAF>();
    setLookAndFeel (laf.get());

    // ── Echo Time ─────────────────────────────────────────────────────────
    echoTimeSlider.setTextValueSuffix (" ms");
    echoTimeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    echoTimeSlider.setTooltip ("Echo delay time in milliseconds (or synced note value)");
    addAndMakeVisible (echoTimeSlider);
    echoTimeLabel.setText ("ECHO TIME", juce::dontSendNotification);
    echoTimeLabel.setFont (juce::Font (8.5f));
    echoTimeLabel.setColour (juce::Label::textColourId, kTextDim);
    echoTimeLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (echoTimeLabel);
    echoTimeAtt = std::make_unique<SliderAtt> (p.apvts, "echoTime", echoTimeSlider);

    // ── Feedback ──────────────────────────────────────────────────────────
    feedbackSlider.setTextValueSuffix (" %");
    feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    feedbackSlider.setTooltip ("Feedback amount — how many echoes repeat");
    addAndMakeVisible (feedbackSlider);
    feedbackLabel.setText ("FEEDBACK", juce::dontSendNotification);
    feedbackLabel.setFont (juce::Font (8.5f));
    feedbackLabel.setColour (juce::Label::textColourId, kTextDim);
    feedbackLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (feedbackLabel);
    feedbackAtt = std::make_unique<SliderAtt> (p.apvts, "feedback", feedbackSlider);

    // ── Mix ───────────────────────────────────────────────────────────────
    mixSlider.setTextValueSuffix (" %");
    mixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    mixSlider.setTooltip ("Wet/Dry mix");
    addAndMakeVisible (mixSlider);
    mixLabel.setText ("MIX", juce::dontSendNotification);
    mixLabel.setFont (juce::Font (8.5f));
    mixLabel.setColour (juce::Label::textColourId, kTextDim);
    mixLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (mixLabel);
    mixAtt = std::make_unique<SliderAtt> (p.apvts, "mix", mixSlider);

    // ── Sync toggle ────────────────────────────────────────────────────────
    syncToggle.setClickingTogglesState (true);
    syncToggle.setTooltip ("Sync echo time to host BPM");
    addAndMakeVisible (syncToggle);
    syncAtt = std::make_unique<ButtonAtt> (p.apvts, "syncMode", syncToggle);

    // ── Note division ──────────────────────────────────────────────────────
    noteDivCombo.addItemList ({ "1/1", "1/2", "1/4", "1/8", "1/16",
                                "1/2.", "1/4.", "1/8.",
                                "1/2t", "1/4t", "1/8t" }, 1);
    noteDivCombo.setTooltip ("Note division for sync mode");
    addAndMakeVisible (noteDivCombo);
    noteDivLabel.setText ("NOTE", juce::dontSendNotification);
    noteDivLabel.setFont (juce::Font (8.5f));
    noteDivLabel.setColour (juce::Label::textColourId, kTextDim);
    noteDivLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (noteDivLabel);
    noteDivAtt = std::make_unique<ComboAtt> (p.apvts, "noteDiv", noteDivCombo);

    // ── Tap tempo ──────────────────────────────────────────────────────────
    tapButton.setTooltip ("Tap to set tempo");
    tapButton.onClick = [this] { processorRef.tapTempoHit(); };
    addAndMakeVisible (tapButton);

    bpmDisplay.setText ("120.0 BPM", juce::dontSendNotification);
    bpmDisplay.setFont (juce::Font (15.f, juce::Font::bold));
    bpmDisplay.setColour (juce::Label::textColourId, kTapActive);
    bpmDisplay.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (bpmDisplay);

    // ── Groove + Feel ──────────────────────────────────────────────────────
    grooveSlider.setTextValueSuffix (" %");
    grooveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    grooveSlider.setTooltip ("Groove amount — adds shuffle/swing to echo rhythm");
    addAndMakeVisible (grooveSlider);
    grooveLabel.setText ("GROOVE", juce::dontSendNotification);
    grooveLabel.setFont (juce::Font (8.5f));
    grooveLabel.setColour (juce::Label::textColourId, kTextDim);
    grooveLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (grooveLabel);
    grooveAtt = std::make_unique<SliderAtt> (p.apvts, "groove", grooveSlider);

    feelSlider.setTextValueSuffix (" ms");
    feelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    feelSlider.setTooltip ("Feel — negative = rushin (earlier), positive = draggin (later)");
    addAndMakeVisible (feelSlider);
    feelLabel.setText ("FEEL", juce::dontSendNotification);
    feelLabel.setFont (juce::Font (8.5f));
    feelLabel.setColour (juce::Label::textColourId, kTextDim);
    feelLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (feelLabel);
    feelAtt = std::make_unique<SliderAtt> (p.apvts, "feel", feelSlider);

    // ── Filters ────────────────────────────────────────────────────────────
    lowCutSlider.setTextValueSuffix (" Hz");
    lowCutSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    lowCutSlider.setTooltip ("Low Cut — HPF on the feedback path");
    addAndMakeVisible (lowCutSlider);
    lowCutLabel.setText ("LOW CUT", juce::dontSendNotification);
    lowCutLabel.setFont (juce::Font (8.5f));
    lowCutLabel.setColour (juce::Label::textColourId, kTextDim);
    lowCutLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (lowCutLabel);
    lowCutAtt = std::make_unique<SliderAtt> (p.apvts, "lowCut", lowCutSlider);

    highCutSlider.setTextValueSuffix (" Hz");
    highCutSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    highCutSlider.setTooltip ("High Cut — LPF on the feedback path");
    addAndMakeVisible (highCutSlider);
    highCutLabel.setText ("HIGH CUT", juce::dontSendNotification);
    highCutLabel.setFont (juce::Font (8.5f));
    highCutLabel.setColour (juce::Label::textColourId, kTextDim);
    highCutLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (highCutLabel);
    highCutAtt = std::make_unique<SliderAtt> (p.apvts, "highCut", highCutSlider);

    // ── Mode ───────────────────────────────────────────────────────────────
    modeCombo.addItemList ({ "Single Echo", "Dual Echo", "Ping-Pong", "Rhythm Echo" }, 1);
    modeCombo.setTooltip ("Echo mode");
    addAndMakeVisible (modeCombo);
    modeLabel.setText ("MODE", juce::dontSendNotification);
    modeLabel.setFont (juce::Font (8.5f));
    modeLabel.setColour (juce::Label::textColourId, kTextDim);
    modeLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (modeLabel);
    modeAtt = std::make_unique<ComboAtt> (p.apvts, "mode", modeCombo);

    // ── Saturation ─────────────────────────────────────────────────────────
    satInputSlider.setTextValueSuffix (" dB");
    satInputSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    satInputSlider.setTooltip ("Saturation input drive");
    addAndMakeVisible (satInputSlider);
    satInputLabel.setText ("INPUT", juce::dontSendNotification);
    satInputLabel.setFont (juce::Font (8.5f));
    satInputLabel.setColour (juce::Label::textColourId, kTextDim);
    satInputLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (satInputLabel);
    satInputAtt = std::make_unique<SliderAtt> (p.apvts, "satInput", satInputSlider);

    satOutputSlider.setTextValueSuffix (" dB");
    satOutputSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    satOutputSlider.setTooltip ("Saturation output level");
    addAndMakeVisible (satOutputSlider);
    satOutputLabel.setText ("OUTPUT", juce::dontSendNotification);
    satOutputLabel.setFont (juce::Font (8.5f));
    satOutputLabel.setColour (juce::Label::textColourId, kTextDim);
    satOutputLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (satOutputLabel);
    satOutputAtt = std::make_unique<SliderAtt> (p.apvts, "satOutput", satOutputSlider);

    satStyleCombo.addItemList ({ "Clean", "Tape", "Tube", "Bit Crush" }, 1);
    satStyleCombo.setTooltip ("Saturation character");
    addAndMakeVisible (satStyleCombo);
    satStyleLabel.setText ("STYLE", juce::dontSendNotification);
    satStyleLabel.setFont (juce::Font (8.5f));
    satStyleLabel.setColour (juce::Label::textColourId, kTextDim);
    satStyleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (satStyleLabel);
    satStyleAtt = std::make_unique<ComboAtt> (p.apvts, "satStyle", satStyleCombo);

    // ── Bottom strip ───────────────────────────────────────────────────────
    widthSlider.setTextValueSuffix (" %");
    widthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    widthSlider.setTooltip ("Stereo width of the wet signal");
    addAndMakeVisible (widthSlider);
    widthLabel.setText ("WIDTH", juce::dontSendNotification);
    widthLabel.setFont (juce::Font (8.5f));
    widthLabel.setColour (juce::Label::textColourId, kTextDim);
    widthLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (widthLabel);
    widthAtt = std::make_unique<SliderAtt> (p.apvts, "width", widthSlider);

    lrOffsetSlider.setTextValueSuffix (" ms");
    lrOffsetSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    lrOffsetSlider.setTooltip ("L/R offset — nudge right channel delay time");
    addAndMakeVisible (lrOffsetSlider);
    lrOffsetLabel.setText ("L/R OFFSET", juce::dontSendNotification);
    lrOffsetLabel.setFont (juce::Font (8.5f));
    lrOffsetLabel.setColour (juce::Label::textColourId, kTextDim);
    lrOffsetLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (lrOffsetLabel);
    lrOffsetAtt = std::make_unique<SliderAtt> (p.apvts, "lrOffset", lrOffsetSlider);

    accentSlider.setTextValueSuffix (" %");
    accentSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    accentSlider.setTooltip ("Accent — emphasise beats 2 & 4 in Rhythm Echo mode");
    addAndMakeVisible (accentSlider);
    accentLabel.setText ("ACCENT", juce::dontSendNotification);
    accentLabel.setFont (juce::Font (8.5f));
    accentLabel.setColour (juce::Label::textColourId, kTextDim);
    accentLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (accentLabel);
    accentAtt = std::make_unique<SliderAtt> (p.apvts, "accent", accentSlider);

    // ── Prime mode toggle ─────────────────────────────────────────────────
    primeModeToggle.setClickingTogglesState (true);
    primeModeToggle.setTooltip ("Prime Numbers — lock delay to nearest prime ms "
                                "for dense non-phasing echoes");
    addAndMakeVisible (primeModeToggle);
    primeModeAtt = std::make_unique<ButtonAtt> (p.apvts, "primeMode", primeModeToggle);

    zoomIndex = p.editorZoomIndex;
    setSize (kBaseW, kBaseH);
    applyZoom();
    startTimerHz (30);
}

EchoDelayAudioProcessorEditor::~EchoDelayAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

//==============================================================================
void EchoDelayAudioProcessorEditor::visibilityChanged()      { if (isVisible()) applyZoom(); }
void EchoDelayAudioProcessorEditor::parentHierarchyChanged() { applyZoom(); }
void EchoDelayAudioProcessorEditor::applyZoom()
{
    if (getPeer()) setScaleFactor (kZoomFactors[zoomIndex]);
}

void EchoDelayAudioProcessorEditor::timerCallback()
{
    // Update BPM display
    float bpm = processorRef.apvts.getRawParameterValue ("tapBpm")->load();
    bpmDisplay.setText (juce::String (bpm, 1) + " BPM", juce::dontSendNotification);
    repaint();
}

//==============================================================================
void EchoDelayAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    if (zoomButtonBounds.contains (e.position.toInt()))
    {
        zoomIndex = (zoomIndex + 1) % 3;
        processorRef.editorZoomIndex = zoomIndex;
        applyZoom();
        repaint();
    }
}

//==============================================================================
void EchoDelayAudioProcessorEditor::resized()
{
    const int W = kBaseW;

    zoomButtonBounds = { 8, 12, 38, 26 };

    // ── Header ────────────────────────────────────────────────────────────
    // Mode combo (right of header)
    modeLabel .setBounds (W - 200, 8,  50, 12);
    modeCombo .setBounds (W - 200, 20, 150, 24);

    // ── VU meters: fixed left and right edges ─────────────────────────────
    // (painted in paint(), positioned here as conceptual anchors)

    // ── Main section: y=52, three knob columns ────────────────────────────
    // Layout: [VU-IN 20px] [MIX | ECHO TIME | FEEDBACK | LOWCUT | HIGHCUT] [VU-OUT 20px]
    // 5 knobs across 900px centre area
    const int kY  = 60;    // knob row top
    const int kH  = 90;    // knob height
    const int kW  = 64;    // knob width
    const int kTB = 13;    // text box height

    // Column centres (evenly spaced in the main area)
    const int col1 = 90;    // Mix
    const int col2 = 230;   // Echo Time
    const int col3 = 390;   // Feedback
    const int col4 = 530;   // Low Cut
    const int col5 = 670;   // High Cut

    auto placeKnob = [&](juce::Slider& s, juce::Label& l, int cx, int y)
    {
        s.setBounds (cx - kW/2, y,      kW, kH);
        l.setBounds (cx - 40,   y + kH, 80, 13);
    };

    placeKnob (mixSlider,      mixLabel,      col1, kY);
    placeKnob (echoTimeSlider, echoTimeLabel, col2, kY);
    placeKnob (feedbackSlider, feedbackLabel, col3, kY);
    placeKnob (lowCutSlider,   lowCutLabel,   col4, kY);
    placeKnob (highCutSlider,  highCutLabel,  col5, kY);

    // ── Sync + Note div (below Echo Time knob) ────────────────────────────
    syncToggle  .setBounds (col2 - 54, kY + kH + 20, 48, 22);
    noteDivLabel.setBounds (col2 + 6,  kY + kH + 16, 60, 12);
    noteDivCombo.setBounds (col2 + 6,  kY + kH + 28, 80, 22);

    // ── Prime mode (below Feedback) ───────────────────────────────────────
    primeModeToggle.setBounds (col3 - 36, kY + kH + 20, 72, 22);

    // ── TAP TEMPO section: right side of main strip ────────────────────────
    const int tapX = 740;
    tapButton .setBounds (tapX,      kY,       80, 36);
    bpmDisplay.setBounds (tapX - 10, kY + 44,  100, 22);

    // ── Groove + Feel (below Tap area) ────────────────────────────────────
    placeKnob (grooveSlider, grooveLabel, tapX + 20,  kY + 80);
    placeKnob (feelSlider,   feelLabel,   tapX + 120, kY + 80);

    // ── SATURATION section: right panel, y=52 ─────────────────────────────
    // Placed in a right column at x=800
    const int satX = 800;
    const int satY = 60;
    placeKnob (satInputSlider,  satInputLabel,  satX,      satY);
    placeKnob (satOutputSlider, satOutputLabel, satX + 90, satY);
    satStyleLabel.setBounds (satX - 10, satY + kH + 22, 110, 12);
    satStyleCombo.setBounds (satX - 10, satY + kH + 34, 110, 22);

    // ── Bottom strip: Width, L/R Offset, Accent ────────────────────────────
    const int botY = 490;
    const int botCol1 = 180, botCol2 = 390, botCol3 = 600;
    placeKnob (widthSlider,    widthLabel,    botCol1, botY);
    placeKnob (lrOffsetSlider, lrOffsetLabel, botCol2, botY);
    placeKnob (accentSlider,   accentLabel,   botCol3, botY);
}

//==============================================================================
void EchoDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    const int W = kBaseW, H = kBaseH;

    // ── Background ────────────────────────────────────────────────────────
    g.setColour (kBg);
    g.fillAll();

    // ── Header gradient ───────────────────────────────────────────────────
    {
        juce::ColourGradient hdr (kHeader.brighter (0.05f), 0.f, 0.f,
                                   kHeader.darker   (0.25f), 0.f, 50.f, false);
        g.setGradientFill (hdr);
        g.fillRect (0, 0, W, 50);
        g.setColour (kDivider.withAlpha (0.45f));
        g.fillRect (0, 49, W, 1);
    }

    // ── Zoom button ───────────────────────────────────────────────────────
    {
        auto& zb = zoomButtonBounds;
        g.setColour (kPanel.withAlpha (0.65f));
        g.fillRoundedRectangle (zb.toFloat(), 5.f);
        g.setColour (kDivider.withAlpha (0.55f));
        g.drawRoundedRectangle (zb.toFloat().reduced (0.5f), 4.5f, 0.8f);
        g.setFont (juce::Font (10.5f, juce::Font::bold));
        g.setColour (kTextMain);
        g.drawText (kZoomLabels[zoomIndex], zb, juce::Justification::centred, false);
    }

    // ── Title ─────────────────────────────────────────────────────────────
    {
        juce::Font tf (20.f, juce::Font::bold);
        g.setFont (tf);
        const juce::String p1 = "ECHO", p2 = " DELAY";
        float w1 = tf.getStringWidthFloat (p1), w2 = tf.getStringWidthFloat (p2);
        float sx = (W - w1 - w2) * 0.5f;
        g.setColour (kTextMain.withAlpha (0.72f));
        g.drawText (p1, (int)sx,        16, (int)w1 + 4, 20, juce::Justification::centredLeft, false);
        g.setColour (kAccent);
        g.drawText (p2, (int)(sx + w1), 16, (int)w2 + 4, 20, juce::Justification::centredLeft, false);
    }

    // Mode label in header
    g.setFont (juce::Font (9.f));
    g.setColour (kTextDim.withAlpha (0.55f));
    g.drawText ("MODE", kBaseW - 204, 8, 42, 12, juce::Justification::centredLeft, false);

    // Version
    g.setFont (juce::Font (8.5f));
    g.setColour (kTextDim.withAlpha (0.50f));
    g.drawText ("v1.0", W - 52, 36, 40, 10, juce::Justification::centredRight, false);

    // ── VU meters (left = input, right = output) ──────────────────────────
    {
        const float vuH = 320.f;
        drawVUBar (g, 6.f, 56.f, 18.f, vuH, processorRef.inputLevelDb.load());
        g.setFont (juce::Font (7.5f, juce::Font::bold));
        g.setColour (kTextDim.withAlpha (0.40f));
        g.drawText ("IN", 6, (int)(56 + vuH + 4), 18, 12, juce::Justification::centred, false);

        drawVUBar (g, (float)(W - 24), 56.f, 18.f, vuH, processorRef.outputLevelDb.load());
        g.drawText ("OUT", W - 24, (int)(56 + vuH + 4), 18, 12, juce::Justification::centred, false);
    }

    // ── Main knob area panel ──────────────────────────────────────────────
    drawSectionPanel (g, { 28, 52, 700, 290 }, "DELAY", kAccent);

    // Subdivider between filters and main
    g.setColour (kDivider.withAlpha (0.18f));
    g.fillRect (490.f, 68.f, 0.6f, 260.f);

    // Sync/Note label sub-header
    g.setFont (juce::Font (8.f, juce::Font::bold));
    g.setColour (kTextDim.withAlpha (0.40f));
    g.drawText ("TIME SYNC", 152, 168, 120, 12, juce::Justification::centred, false);

    // ── Tap Tempo panel ───────────────────────────────────────────────────
    drawSectionPanel (g, { 730, 52, 202, 200 }, "TAP TEMPO", kTapActive);

    // ── Saturation panel ──────────────────────────────────────────────────
    drawSectionPanel (g, { 730, 262, 202, 120 }, "SATURATION", juce::Colour (180, 100, 55));

    // ── Bottom strip panel ────────────────────────────────────────────────
    drawSectionPanel (g, { 28, 476, 884, 130 }, "WIDTH  /  OFFSET  /  ACCENT",
                       kAccent.withAlpha (0.60f));

    // Current echo time readout (inside delay panel)
    {
        float etMs = processorRef.apvts.getRawParameterValue ("echoTime")->load();
        bool  sync = processorRef.apvts.getRawParameterValue ("syncMode")->load() > 0.5f;
        juce::String etTxt = sync ? "(synced)" : (juce::String (etMs, 1) + " ms");
        g.setFont (juce::Font (8.5f));
        g.setColour (kAccent.withAlpha (0.65f));
        g.drawText (etTxt, 155, 264, 150, 12, juce::Justification::centred, false);
    }

    // Prime mode indicator text
    {
        bool prime = processorRef.apvts.getRawParameterValue ("primeMode")->load() > 0.5f;
        if (prime)
        {
            g.setFont (juce::Font (7.5f, juce::Font::italic));
            g.setColour (kTapActive.withAlpha (0.60f));
            g.drawText ("PRIME", 340, 268, 60, 12, juce::Justification::centred, false);
        }
    }
}
