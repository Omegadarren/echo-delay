#include "PluginEditor.h"

constexpr float       EchoDelayAudioProcessorEditor::kZoomFactors[];
constexpr const char* EchoDelayAudioProcessorEditor::kZoomLabels[];

//==============================================================================
//  Colour palette
//==============================================================================
static const juce::Colour kBg       {  20,  21,  32 };
static const juce::Colour kPanel    {  14,  15,  24 };
static const juce::Colour kHeader   {  22,  54,  98 };
static const juce::Colour kAccent   {  65, 145, 210 };
static const juce::Colour kTextMain { 225, 238, 255 };
static const juce::Colour kTextDim  { 115, 152, 195 };
static const juce::Colour kDivider  {  48,  82, 124 };
static const juce::Colour kGold     { 220, 190,  55 };
static const juce::Colour kOrange   { 200, 100,  45 };
static const juce::Colour kGreen    {  55, 210, 110 };
static const juce::Colour kRed      { 220,  60,  60 };

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
        g.strokePath (arc, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
        float fe = startA + pos * (endA - startA);
        juce::Path fill;
        fill.addCentredArc (cx, cy, r, r, 0.f, startA, fe, true);
        g.setColour (fillCol.withAlpha (0.80f));
        g.strokePath (fill, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));
        const float kr = r * 0.62f;
        juce::ColourGradient grad (juce::Colour (90, 98, 118), cx - kr * 0.4f, cy - kr * 0.5f,
                                    juce::Colour (28, 30, 42),  cx + kr * 0.4f, cy + kr * 0.5f, false);
        g.setGradientFill (grad);
        g.fillEllipse (cx - kr, cy - kr, kr * 2, kr * 2);
        g.setColour (kDivider.withAlpha (0.5f));
        g.drawEllipse (cx - kr, cy - kr, kr * 2, kr * 2, 1.f);
        juce::ColourGradient rim (juce::Colours::white.withAlpha (0.18f), cx, cy - kr,
                                   juce::Colours::transparentBlack, cx, cy + kr, false);
        g.setGradientFill (rim);
        g.fillEllipse (cx - kr, cy - kr, kr * 2, kr * 2);
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

    juce::Font getLabelFont (juce::Label&)       override { return juce::Font (11.f); }
    juce::Font getComboBoxFont (juce::ComboBox&) override { return juce::Font (11.f, juce::Font::bold); }
    juce::Font getPopupMenuFont()                override { return juce::Font (12.f); }
};

//==============================================================================
//  Helpers
//==============================================================================
static void drawVUBar (juce::Graphics& g, float x, float y, float w, float h, float db)
{
    g.setColour (juce::Colour (3, 5, 12));
    g.fillRoundedRectangle (x, y, w, h, 2.f);
    g.setColour (kDivider.withAlpha (0.22f));
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
    g.setColour (kTextDim.withAlpha (0.25f));
    g.fillRect (x, y + h * (1.f - 60.f / 66.f), w, 0.8f);
}

static void drawPanel (juce::Graphics& g, juce::Rectangle<int> r,
                        const juce::String& title, juce::Colour accent)
{
    g.setColour (kPanel.withAlpha (0.65f));
    g.fillRoundedRectangle (r.toFloat(), 6.f);
    g.setColour (kDivider.withAlpha (0.28f));
    g.drawRoundedRectangle (r.toFloat().reduced (0.5f), 5.5f, 0.8f);
    g.setColour (accent.withAlpha (0.38f));
    g.fillRoundedRectangle ((float)r.getX() + 4.f, (float)r.getY() - 2.f,
                             (float)r.getWidth() - 8.f, 3.f, 1.5f);
    if (title.isNotEmpty())
    {
        g.setFont (juce::Font (8.5f, juce::Font::bold));
        g.setColour (kTextDim.withAlpha (0.50f));
        g.drawText (title, r.getX(), r.getY() + 4, r.getWidth(), 12,
                    juce::Justification::centred, false);
    }
}

//==============================================================================
//  Convenience
//==============================================================================
static void setupKnob (juce::Slider& s, juce::Label& l,
                        const juce::String& labelText,
                        const juce::String& suffix,
                        const juce::String& tooltip,
                        juce::Component& parent)
{
    s.setSliderStyle (juce::Slider::Rotary);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 13);
    s.setTextValueSuffix (suffix);
    s.setTooltip (tooltip);
    parent.addAndMakeVisible (s);

    l.setText (labelText, juce::dontSendNotification);
    l.setFont (juce::Font (8.5f));
    l.setColour (juce::Label::textColourId, kTextDim);
    l.setJustificationType (juce::Justification::centred);
    parent.addAndMakeVisible (l);
}

//==============================================================================
//  Constructor
//==============================================================================
EchoDelayAudioProcessorEditor::EchoDelayAudioProcessorEditor (EchoDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    laf = std::make_unique<EchoDelayLAF>();
    setLookAndFeel (laf.get());

    // ── Main knobs ────────────────────────────────────────────────────────
    setupKnob (mixSlider,       mixLabel,       "MIX",       " %",  "Wet/Dry mix",                *this);
    setupKnob (echoTimeSlider,  echoTimeLabel,  "ECHO TIME", " ms", "Delay time",                  *this);
    setupKnob (feedbackSlider,  feedbackLabel,  "FEEDBACK",  " %",  "Repeats",                     *this);
    setupKnob (lowCutSlider,    lowCutLabel,    "LOW CUT",   " Hz", "High-pass on feedback path",  *this);
    setupKnob (highCutSlider,   highCutLabel,   "HIGH CUT",  " Hz", "Low-pass on feedback path",   *this);
    setupKnob (grooveSlider,    grooveLabel,    "GROOVE",    " %",  "Shuffle / swing amount",      *this);
    setupKnob (feelSlider,      feelLabel,      "FEEL",      " ms", "Rushin (−) or Draggin (+)",   *this);
    setupKnob (satInputSlider,  satInputLabel,  "INPUT",     " dB", "Saturation drive",            *this);
    setupKnob (satOutputSlider, satOutputLabel, "OUTPUT",    " dB", "Saturation output trim",      *this);
    setupKnob (widthSlider,     widthLabel,     "WIDTH",     " %",  "Stereo spread of wet signal", *this);
    setupKnob (lrOffsetSlider,  lrOffsetLabel,  "L/R OFFSET"," ms", "Right channel delay offset",  *this);
    setupKnob (accentSlider,    accentLabel,    "ACCENT",    " %",  "Beats 2 & 4 emphasis",        *this);

    // Tint filter knobs gold, sat knobs orange
    lowCutSlider  .setColour (juce::Slider::rotarySliderFillColourId, kGold);
    highCutSlider .setColour (juce::Slider::rotarySliderFillColourId, kGold);
    satInputSlider .setColour (juce::Slider::rotarySliderFillColourId, kOrange.brighter (0.3f));
    satOutputSlider.setColour (juce::Slider::rotarySliderFillColourId, kOrange.brighter (0.3f));
    grooveSlider   .setColour (juce::Slider::rotarySliderFillColourId, kGold.withAlpha (0.75f));
    feelSlider     .setColour (juce::Slider::rotarySliderFillColourId, kGold.withAlpha (0.75f));

    // ── Attachments ────────────────────────────────────────────────────────
    mixAtt       = std::make_unique<SliderAtt> (p.apvts, "mix",       mixSlider);
    echoTimeAtt  = std::make_unique<SliderAtt> (p.apvts, "echoTime",  echoTimeSlider);
    feedbackAtt  = std::make_unique<SliderAtt> (p.apvts, "feedback",  feedbackSlider);
    lowCutAtt    = std::make_unique<SliderAtt> (p.apvts, "lowCut",    lowCutSlider);
    highCutAtt   = std::make_unique<SliderAtt> (p.apvts, "highCut",   highCutSlider);
    grooveAtt    = std::make_unique<SliderAtt> (p.apvts, "groove",    grooveSlider);
    feelAtt      = std::make_unique<SliderAtt> (p.apvts, "feel",      feelSlider);
    satInputAtt  = std::make_unique<SliderAtt> (p.apvts, "satInput",  satInputSlider);
    satOutputAtt = std::make_unique<SliderAtt> (p.apvts, "satOutput", satOutputSlider);
    widthAtt     = std::make_unique<SliderAtt> (p.apvts, "width",     widthSlider);
    lrOffsetAtt  = std::make_unique<SliderAtt> (p.apvts, "lrOffset",  lrOffsetSlider);
    accentAtt    = std::make_unique<SliderAtt> (p.apvts, "accent",    accentSlider);

    // ── Sync toggle ───────────────────────────────────────────────────────
    syncToggle.setClickingTogglesState (true);
    syncToggle.setTooltip ("Sync echo time to host BPM");
    addAndMakeVisible (syncToggle);
    syncAtt = std::make_unique<ButtonAtt> (p.apvts, "syncMode", syncToggle);

    // ── Note division ─────────────────────────────────────────────────────
    noteDivCombo.addItemList ({ "1/1", "1/2", "1/4", "1/8", "1/16",
                                "1/2.", "1/4.", "1/8.",
                                "1/2t", "1/4t", "1/8t" }, 1);
    noteDivCombo.setTooltip ("Note division for sync mode");
    addAndMakeVisible (noteDivCombo);
    noteDivLabel.setText ("NOTE", juce::dontSendNotification);
    noteDivLabel.setFont (juce::Font (8.f));
    noteDivLabel.setColour (juce::Label::textColourId, kTextDim);
    noteDivLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (noteDivLabel);
    noteDivAtt = std::make_unique<ComboAtt> (p.apvts, "noteDiv", noteDivCombo);

    // ── Prime mode ────────────────────────────────────────────────────────
    primeModeToggle.setClickingTogglesState (true);
    primeModeToggle.setTooltip ("Lock delay to nearest prime ms");
    addAndMakeVisible (primeModeToggle);
    primeModeAtt = std::make_unique<ButtonAtt> (p.apvts, "primeMode", primeModeToggle);

    // ── Mode combo ────────────────────────────────────────────────────────
    modeCombo.addItemList ({ "Single Echo", "Dual Echo", "Ping-Pong", "Rhythm Echo" }, 1);
    addAndMakeVisible (modeCombo);
    modeLabel.setText ("MODE", juce::dontSendNotification);
    modeLabel.setFont (juce::Font (8.5f));
    modeLabel.setColour (juce::Label::textColourId, kTextDim);
    modeLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (modeLabel);
    modeAtt = std::make_unique<ComboAtt> (p.apvts, "mode", modeCombo);

    // ── Tap tempo ─────────────────────────────────────────────────────────
    tapButton.setTooltip ("Tap to set tempo");
    tapButton.onClick = [this] { processorRef.tapTempoHit(); };
    addAndMakeVisible (tapButton);

    bpmDisplay.setFont (juce::Font (16.f, juce::Font::bold));
    bpmDisplay.setColour (juce::Label::textColourId, kGold);
    bpmDisplay.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (bpmDisplay);

    // ── Saturation style ──────────────────────────────────────────────────
    satStyleCombo.addItemList ({ "Clean", "Tape", "Tube", "Bit Crush" }, 1);
    addAndMakeVisible (satStyleCombo);
    satStyleLabel.setText ("STYLE", juce::dontSendNotification);
    satStyleLabel.setFont (juce::Font (8.5f));
    satStyleLabel.setColour (juce::Label::textColourId, kTextDim);
    satStyleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (satStyleLabel);
    satStyleAtt = std::make_unique<ComboAtt> (p.apvts, "satStyle", satStyleCombo);

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
    float bpm = processorRef.apvts.getRawParameterValue ("tapBpm")->load();
    bpmDisplay.setText (juce::String (bpm, 1) + " BPM", juce::dontSendNotification);
    repaint();
}

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
//  resized()
//  ─────────────────────────────────────────────────────────────────────────────
//  kBaseW=840  kBaseH=540
//
//  ┌─ Header ──────────────────────────────────────── y=0..50 ─┐
//  ├─ Left DELAY panel x=12 w=574 h=276 ──── y=52..328 ────────┤ ┌─ Tap  y=52 h=186 ─┐
//  │  5 knobs (row y=72): Mix EchoT Fb LoCut HiCut             │ │ TAP btn + BPM      │
//  │  Sync / Note / Prime row (y=181)                          │ │ Groove   Feel      │
//  └───────────────────────────────────────────────────────────┘ └─ Sat  y=246 h=82 ──┘
//  ┌─ Bottom strip x=12 w=816 h=148 ─── y=334..482 ────────────────────────────────────┐
//  │  Width   L/R Offset   Accent                                                       │
//  └────────────────────────────────────────────────────────────────────────────────────┘
//==============================================================================
void EchoDelayAudioProcessorEditor::resized()
{
    const int W = kBaseW;

    // Header
    zoomButtonBounds = { 8, 12, 38, 26 };
    modeLabel.setBounds (W - 196, 10, 36, 12);
    modeCombo.setBounds (W - 196, 22, 148, 24);

    // ── Layout constants ──────────────────────────────────────────────────
    static constexpr int kW  = 64;   // knob widget width
    static constexpr int kH  = 88;   // knob height (includes 13px textbox)
    static constexpr int kLH = 13;   // label height

    // ── Left DELAY panel: 5 knobs evenly spaced ───────────────────────────
    static constexpr int LP_X = 12;
    static constexpr int LP_W = 574;
    static constexpr int kRowY = 72;
    static constexpr int sp = LP_W / 5;   // 114

    auto colCx = [](int i) { return LP_X + sp / 2 + i * sp; };

    auto placeKnob = [&](juce::Slider& s, juce::Label& l, int col, int y)
    {
        const int cx = colCx (col);
        s.setBounds (cx - kW / 2, y,      kW, kH);
        l.setBounds (cx - 40,     y + kH, 80, kLH);
    };

    placeKnob (mixSlider,      mixLabel,      0, kRowY);
    placeKnob (echoTimeSlider, echoTimeLabel, 1, kRowY);
    placeKnob (feedbackSlider, feedbackLabel, 2, kRowY);
    placeKnob (lowCutSlider,   lowCutLabel,   3, kRowY);
    placeKnob (highCutSlider,  highCutLabel,  4, kRowY);

    // Sync / Note / Prime row
    const int ctrlY = kRowY + kH + kLH + 10;   // 72+88+13+10 = 183
    const int c1    = colCx (1);                 // Echo Time col centre
    const int c2    = colCx (2);                 // Feedback col centre

    syncToggle.setBounds      (c1 - 60, ctrlY, 50, 22);
    noteDivLabel.setBounds    (c1 - 4,  ctrlY - 5, 70, 12);
    noteDivCombo.setBounds    (c1 - 4,  ctrlY + 7, 84, 22);
    primeModeToggle.setBounds (c2 - 36, ctrlY, 72, 22);

    // ── Right section (x=594, w=238) ─────────────────────────────────────
    static constexpr int RP_X = LP_X + LP_W + 8;  // 594
    static constexpr int RP_W = W - RP_X - 8;     // 238

    // Column centres inside right panel
    const int rcx1 = RP_X + RP_W / 4;       // left knob: ~653
    const int rcx2 = RP_X + RP_W * 3 / 4;  // right knob: ~772

    // TAP TEMPO panel (y=52..238)
    static constexpr int tapY = 52;
    tapButton .setBounds (RP_X + (RP_W - 76) / 2, tapY + 14, 76, 32);
    bpmDisplay.setBounds (RP_X + 6, tapY + 52, RP_W - 12, 22);

    // Groove + Feel (y=130)
    static constexpr int gfY = tapY + 82;
    grooveSlider.setBounds (rcx1 - kW / 2, gfY,      kW, kH);
    grooveLabel .setBounds (rcx1 - 40,     gfY + kH, 80, kLH);
    feelSlider  .setBounds (rcx2 - kW / 2, gfY,      kW, kH);
    feelLabel   .setBounds (rcx2 - 40,     gfY + kH, 80, kLH);

    // SATURATION panel: STYLE selector at top, knobs below
    static constexpr int satY = gfY + kH + kLH + 10;
    satStyleLabel.setBounds (RP_X + 22, satY + 10,     RP_W - 44, 12);
    satStyleCombo.setBounds (RP_X + 22, satY + 22,     RP_W - 44, 22);
    satInputSlider .setBounds (rcx1 - kW / 2, satY + 50, kW, kH);
    satInputLabel  .setBounds (rcx1 - 40,     satY + 50 + kH, 80, kLH);
    satOutputSlider.setBounds (rcx2 - kW / 2, satY + 50, kW, kH);
    satOutputLabel .setBounds (rcx2 - 40,     satY + 50 + kH, 80, kLH);

    // WIDTH / L-R OFFSET / ACCENT — below left DELAY panel (574px wide, spacious)
    static constexpr int wlaY = 52 + 162 + 6;  // 220
    const int bcx1 = LP_X + LP_W / 6;           // 107
    const int bcx2 = LP_X + LP_W / 2;           // 299
    const int bcx3 = LP_X + LP_W * 5 / 6;       // 490

    widthSlider   .setBounds (bcx1 - kW / 2, wlaY + 14, kW, kH);
    widthLabel    .setBounds (bcx1 - 40,     wlaY + 14 + kH, 80, kLH);
    lrOffsetSlider.setBounds (bcx2 - kW / 2, wlaY + 14, kW, kH);
    lrOffsetLabel .setBounds (bcx2 - 40,     wlaY + 14 + kH, 80, kLH);
    accentSlider  .setBounds (bcx3 - kW / 2, wlaY + 14, kW, kH);
    accentLabel   .setBounds (bcx3 - 40,     wlaY + 14 + kH, 80, kLH);
}

//==============================================================================
void EchoDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    const int W = kBaseW, H = kBaseH;
    (void)H;

    // Background
    g.setColour (kBg);
    g.fillAll();

    // Header gradient
    {
        juce::ColourGradient hdr (kHeader.brighter (0.05f), 0.f, 0.f,
                                   kHeader.darker   (0.25f), 0.f, 50.f, false);
        g.setGradientFill (hdr);
        g.fillRect (0, 0, W, 50);
        g.setColour (kDivider.withAlpha (0.45f));
        g.fillRect (0, 49, W, 1);
    }

    // Zoom button
    {
        auto zb = zoomButtonBounds;
        g.setColour (kPanel.withAlpha (0.65f));
        g.fillRoundedRectangle (zb.toFloat(), 5.f);
        g.setColour (kDivider.withAlpha (0.55f));
        g.drawRoundedRectangle (zb.toFloat().reduced (0.5f), 4.5f, 0.8f);
        g.setFont (juce::Font (10.5f, juce::Font::bold));
        g.setColour (kTextMain);
        g.drawText (kZoomLabels[zoomIndex], zb, juce::Justification::centred, false);
    }

    // Title
    {
        juce::Font tf (20.f, juce::Font::bold);
        g.setFont (tf);
        const juce::String p1 = "ECHO", p2 = " DELAY";
        float w1 = tf.getStringWidthFloat (p1), w2 = tf.getStringWidthFloat (p2);
        float sx = (W - w1 - w2) * 0.5f;
        g.setColour (kTextMain.withAlpha (0.72f));
        g.drawText (p1, (int)sx,        15, (int)w1 + 4, 20, juce::Justification::centredLeft, false);
        g.setColour (kAccent);
        g.drawText (p2, (int)(sx + w1), 15, (int)w2 + 4, 20, juce::Justification::centredLeft, false);
    }

    // Mode label
    g.setFont (juce::Font (8.f));
    g.setColour (kTextDim.withAlpha (0.50f));
    g.drawText ("MODE", W - 198, 10, 36, 12, juce::Justification::centredLeft, false);

    // Version
    g.setFont (juce::Font (8.5f));
    g.setColour (kTextDim.withAlpha (0.45f));
    g.drawText ("v1.0", W - 48, 36, 40, 10, juce::Justification::centredRight, false);

    // VU meters
    {
        const float vuY = 54.f, vuH = 276.f, vuW = 10.f;
        drawVUBar (g, 0.f, vuY, vuW, vuH, processorRef.inputLevelDb.load());
        g.setFont (juce::Font (7.f, juce::Font::bold));
        g.setColour (kTextDim.withAlpha (0.38f));
        g.drawText ("IN",  0, (int)(vuY + vuH + 3), 12, 10, juce::Justification::centred, false);
        drawVUBar (g, (float)(W - (int)vuW), vuY, vuW, vuH, processorRef.outputLevelDb.load());
        g.drawText ("OUT", W - 14, (int)(vuY + vuH + 3), 14, 10, juce::Justification::centred, false);
    }

    // ── Left DELAY panel ─────────────────────────────────────────────────
    drawPanel (g, { 12, 52, 574, 162 }, "DELAY", kAccent);

    // ── WIDTH / L-R OFFSET / ACCENT panel — below left panel ─────────────
    drawPanel (g, { 12, 220, 574, 112 }, "WIDTH  /  L-R OFFSET  /  ACCENT",
                kAccent.withAlpha (0.55f));

    // Subtle vertical divider between delay knobs and filter knobs (after Feedback)
    {
        const int sp = 574 / 5;
        const float sepX = 12.f + sp * 2.5f;
        g.setColour (kDivider.withAlpha (0.14f));
        g.fillRect (sepX, 66.f, 0.8f, 260.f);
    }

    // ── Right panels ─────────────────────────────────────────────────────
    const int RP_X = 594, RP_W = W - RP_X - 8;

    // Static constants matching resized()
    const int kH = 88, kLH = 13;
    const int tapY = 52;
    const int gfY  = tapY + 82;
    const int tapPanelH = gfY + kH + kLH + 8 - tapY;  // 191

    drawPanel (g, { RP_X, tapY, RP_W, tapPanelH }, "TAP TEMPO", kGold);

    const int satY      = gfY + kH + kLH + 10;
    const int satPanelH = 50 + kH + kLH + 8;
    drawPanel (g, { RP_X, satY, RP_W, satPanelH }, "SATURATION", kOrange);

    // Echo time readout (under Echo Time knob in delay panel)
    {
        const int sp2 = 574 / 5;
        const int etCx = 12 + sp2 / 2 + sp2;  // col 1 centre
        bool  sync = processorRef.apvts.getRawParameterValue ("syncMode")->load() > 0.5f;
        float et   = processorRef.apvts.getRawParameterValue ("echoTime")->load();
        juce::String etTxt = sync ? "(synced)" : (juce::String (et, 1) + " ms");
        g.setFont (juce::Font (8.f));
        g.setColour (kAccent.withAlpha (0.50f));
        g.drawText (etTxt, etCx - 40, 52 + 162 - 16, 80, 12,
                    juce::Justification::centred, false);
    }
}
