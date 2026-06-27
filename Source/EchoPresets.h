#pragma once

//==============================================================================
/**
 * EchoPresets.h  –  50 factory presets for Echo Delay.
 *
 * Fields per preset (in order):
 *   name        – display name
 *   echoTime    – delay time in ms (1–2000, used when syncMode=false)
 *   feedback    – feedback amount 0–99 %
 *   mix         – wet/dry mix 0–100 %
 *   lowCut      – feedback HPF 20–2000 Hz
 *   highCut     – feedback LPF 1000–20000 Hz
 *   groove      – shuffle amount 0–100 %
 *   feel        – timing nudge −50..+50 ms
 *   mode        – 0=Single Echo, 1=Dual Echo, 2=Ping-Pong, 3=Rhythm Echo
 *   syncMode    – false=free ms, true=lock to host BPM
 *   noteDiv     – 0=1/1 1=1/2 2=1/4 3=1/8 4=1/16 5=1/2. 6=1/4. 7=1/8.
 *                 8=1/2t 9=1/4t 10=1/8t (used when syncMode=true)
 *   satInput    – pre-saturation gain −24..+24 dB
 *   satOutput   – post-saturation gain −24..+24 dB
 *   satStyle    – 0=Clean 1=Tape 2=Tube 3=Bit Crush
 *   width       – stereo width 0–200 %
 *   lrOffset    – R-channel nudge −50..+50 ms
 *   accent      – beat-2-and-4 emphasis 0–100 %
 *   primeMode   – snap delay to nearest prime ms
 */

namespace EchoPresets
{

struct EchoPreset
{
    const char* name;
    float echoTime;
    float feedback;
    float mix;
    float lowCut;
    float highCut;
    float groove;
    float feel;
    int   mode;
    bool  syncMode;
    int   noteDiv;
    float satInput;
    float satOutput;
    int   satStyle;
    float width;
    float lrOffset;
    float accent;
    bool  primeMode;
};

//                         name                  ms    fb    mx   lc     hc    gr    fl  md  sy  nd  si    so   ss    wd   lr    ac    pm
inline constexpr EchoPreset kPresets[50] =
{
    // ── VOCALS ───────────────────────────────────────────────────────────────
    { "Lead Vocal Air",    220.f, 28.f, 28.f,  80.f, 10000.f,  0.f,  0.f, 0, false,  3,  1.f, -1.f, 1, 110.f,  0.f,  0.f, false },
    { "BVox Wash",         360.f, 50.f, 38.f,  60.f,  9000.f,  0.f,  0.f, 2, false,  3,  1.f, -1.f, 1, 160.f,  5.f,  0.f, false },
    { "Intimate Ballad",   180.f, 35.f, 30.f,  60.f, 12000.f,  0.f,  3.f, 0, false,  3,  1.f, -1.f, 1, 100.f,  0.f,  0.f, false },
    { "Pop Slap",           80.f, 15.f, 22.f,  80.f, 16000.f,  0.f,  0.f, 0, false,  3,  1.f,  0.f, 0, 100.f,  0.f,  0.f, false },
    { "R&B Tail",          420.f, 45.f, 35.f,  40.f,  8000.f,  0.f,  0.f, 0, false,  3,  2.f, -1.f, 2, 120.f,  0.f,  0.f, false },
    { "Ethereal Chorus",   380.f, 55.f, 42.f,  50.f, 11000.f,  8.f,  5.f, 2, false,  3,  2.f, -2.f, 1, 180.f, 12.f,  0.f, false },
    { "Gospel Hall",       480.f, 60.f, 45.f,  40.f,  9000.f,  0.f,  0.f, 0, false,  3,  2.f, -2.f, 1, 150.f,  0.f,  0.f, false },
    { "Verb-Style Long",   650.f, 65.f, 40.f,  30.f, 10000.f,  0.f,  0.f, 2, false,  3,  2.f, -2.f, 1, 160.f,  8.f,  0.f, false },
    { "Radio Voice",       130.f, 20.f, 20.f, 200.f,  4000.f,  0.f, -2.f, 0, false,  3,  3.f,  2.f, 3,  80.f,  0.f,  0.f, false },
    { "Whisper Space",     550.f, 45.f, 30.f,  30.f, 12000.f,  0.f,  0.f, 2, false,  3,  1.f, -1.f, 1, 140.f,  0.f,  0.f, true  },

    // ── GUITAR ───────────────────────────────────────────────────────────────
    { "Country Slapback",  130.f,  5.f, 25.f, 100.f, 16000.f,  0.f,  0.f, 0, false,  3,  2.f,  0.f, 1, 100.f,  0.f,  0.f, false },
    { "Blues Lead",        300.f, 38.f, 32.f,  80.f, 10000.f,  8.f,  3.f, 0, false,  3,  2.f, -1.f, 1, 100.f,  0.f,  0.f, false },
    { "U2 Dotted 8th",     250.f, 40.f, 38.f,  60.f, 10000.f,  0.f,  0.f, 0, true,   7,  2.f, -1.f, 1, 140.f,  0.f, 15.f, false },
    { "Clean Arpeggio",    420.f, 42.f, 35.f,  40.f, 15000.f,  0.f,  0.f, 0, false,  3,  0.f,  0.f, 0, 100.f,  0.f,  0.f, false },
    { "Ambient Swells",    800.f, 70.f, 50.f,  30.f,  8000.f,  0.f,  0.f, 2, false,  3,  2.f, -2.f, 1, 180.f,  0.f,  0.f, false },
    { "Funky Stutter",     250.f, 30.f, 35.f, 100.f, 16000.f, 65.f,  0.f, 3, true,   4,  3.f,  0.f, 0, 100.f,  0.f, 45.f, false },
    { "Heavy Metal Lead",  200.f, 20.f, 20.f, 150.f, 12000.f,  0.f,  0.f, 0, false,  3,  4.f,  2.f, 2,  80.f,  0.f,  0.f, false },
    { "Jazz Chord",        380.f, 40.f, 30.f,  60.f,  8000.f,  0.f, -5.f, 0, false,  3,  2.f, -2.f, 1,  80.f,  0.f,  0.f, false },
    { "Fingerpicked Folk", 160.f, 22.f, 25.f,  80.f, 14000.f,  0.f,  0.f, 0, false,  3,  1.f, -1.f, 0, 100.f,  0.f,  0.f, false },
    { "Post-Rock Build",   900.f, 75.f, 55.f,  30.f,  9000.f,  0.f,  0.f, 2, false,  3,  2.f, -2.f, 1, 180.f, 15.f,  0.f, false },

    // ── KEYS & SYNTH ─────────────────────────────────────────────────────────
    { "Electric Piano",    350.f, 42.f, 38.f,  60.f,  9000.f,  0.f,  0.f, 0, false,  3,  2.f, -2.f, 1, 120.f,  0.f,  0.f, false },
    { "Dreamy Pads",      1000.f, 65.f, 50.f,  30.f,  8000.f,  0.f,  0.f, 2, false,  3,  2.f, -2.f, 1, 180.f,  0.f,  0.f, true  },
    { "Organ Room",        200.f, 35.f, 28.f,  80.f, 10000.f,  0.f,  0.f, 0, false,  3,  3.f, -1.f, 2, 100.f,  0.f,  0.f, false },
    { "Harpsichord Echo",  250.f, 25.f, 28.f, 120.f, 16000.f,  0.f,  0.f, 0, false,  3,  0.f,  0.f, 0,  80.f,  0.f,  0.f, false },
    { "Rhodes Splash",     280.f, 38.f, 35.f,  60.f, 10000.f,  0.f,  3.f, 1, false,  3,  2.f, -1.f, 1, 110.f,  8.f,  0.f, false },
    { "Retro Arpeggio",    250.f, 50.f, 42.f,  80.f, 14000.f,  0.f,  0.f, 0, true,   3,  2.f, -1.f, 0, 100.f,  0.f, 20.f, false },
    { "Ping-Pong Seq",     250.f, 55.f, 45.f,  60.f, 12000.f,  0.f,  0.f, 2, true,   4,  2.f, -1.f, 1, 160.f,  0.f, 25.f, false },
    { "Pad Atmosphere",   1200.f, 72.f, 55.f,  25.f,  7000.f,  0.f,  0.f, 2, false,  3,  2.f, -3.f, 1, 200.f,  0.f,  0.f, false },
    { "Honky-Tonk Piano",  150.f, 28.f, 25.f, 120.f, 12000.f,  5.f,  8.f, 0, false,  3,  4.f,  2.f, 3,  80.f,  0.f,  0.f, false },
    { "Synth Lead Air",    320.f, 40.f, 35.f,  60.f, 14000.f,  0.f,  0.f, 2, false,  3,  2.f, -1.f, 1, 150.f,  0.f,  0.f, false },

    // ── DRUMS & PERCUSSION ───────────────────────────────────────────────────
    { "Snare Snap",         80.f,  0.f, 18.f, 200.f, 18000.f,  0.f,  0.f, 0, false,  3,  1.f,  0.f, 0, 100.f,  0.f,  0.f, false },
    { "Drum Room",         180.f, 25.f, 20.f,  60.f, 12000.f,  0.f,  0.f, 0, false,  3,  2.f, -1.f, 1, 100.f,  0.f,  0.f, false },
    { "Perc Groove Swing", 250.f, 35.f, 30.f, 100.f, 16000.f, 60.f,  0.f, 3, true,   3,  2.f,  0.f, 0, 100.f,  0.f, 35.f, false },
    { "Kick Depth",        120.f, 15.f, 15.f,  20.f,  3000.f,  0.f,  0.f, 0, false,  3,  3.f,  0.f, 2,  60.f,  0.f,  0.f, false },
    { "Rimshot Rockabilly",100.f,  8.f, 20.f, 150.f, 16000.f,  0.f,  0.f, 0, false,  3,  3.f,  1.f, 1,  80.f,  0.f,  0.f, false },
    { "Hi-Hat Glitter",     80.f, 20.f, 18.f, 400.f, 18000.f, 15.f,  0.f, 2, false,  3,  0.f,  0.f, 0, 120.f,  5.f,  0.f, false },
    { "Latin Clave",       250.f, 25.f, 25.f, 100.f, 16000.f, 30.f,  0.f, 3, true,   4,  1.f,  0.f, 0,  80.f,  0.f, 40.f, false },
    { "Tribal Ambience",   500.f, 55.f, 35.f,  40.f, 10000.f,  0.f,  0.f, 2, false,  3,  3.f,  0.f, 2, 160.f,  0.f,  0.f, false },

    // ── BASS ─────────────────────────────────────────────────────────────────
    { "Dub Bass",          330.f, 65.f, 45.f,  20.f,  1800.f,  0.f,  0.f, 0, false,  3,  4.f,  3.f, 2,  60.f, -8.f,  0.f, false },
    { "Upright Slap",      100.f, 10.f, 22.f,  60.f,  5000.f,  0.f,  5.f, 0, false,  3,  2.f, -1.f, 1,  60.f,  0.f,  0.f, false },
    { "Synth Bass Seq",    250.f, 40.f, 35.f,  30.f,  6000.f,  0.f,  0.f, 3, true,   4,  3.f,  0.f, 2,  80.f,  0.f, 30.f, false },
    { "Bass Harmonic",     280.f, 30.f, 28.f,  30.f,  4000.f,  0.f,  0.f, 1, false,  3,  2.f,  0.f, 1,  80.f,  0.f,  0.f, false },
    { "Sub Dub Madness",   440.f, 75.f, 50.f,  20.f,  1500.f,  0.f,  0.f, 0, false,  3,  5.f,  4.f, 2,  60.f,-12.f,  0.f, false },

    // ── SPECIAL ──────────────────────────────────────────────────────────────
    { "Studio Clean",      250.f, 35.f, 30.f,  80.f, 16000.f,  0.f,  0.f, 0, false,  3,  0.f,  0.f, 0, 100.f,  0.f,  0.f, false },
    { "Vintage Tape",      360.f, 45.f, 38.f,  60.f,  7000.f,  0.f, -3.f, 0, false,  3,  3.f, -2.f, 1, 100.f,  0.f,  0.f, false },
    { "Long Space",        900.f, 70.f, 50.f,  30.f,  9000.f,  0.f,  0.f, 2, false,  3,  2.f, -2.f, 1, 180.f,  0.f,  0.f, false },
    { "Prime Dense",       400.f, 55.f, 40.f,  60.f, 10000.f,  0.f,  0.f, 0, false,  3,  2.f, -1.f, 1, 100.f,  0.f,  0.f, true  },
    { "Glitch Stutter",    250.f, 50.f, 45.f, 100.f, 16000.f, 70.f,  0.f, 3, true,   4,  8.f,  4.f, 3, 100.f,  0.f, 60.f, false },
    { "Analog Warmth",     300.f, 40.f, 35.f,  60.f,  8000.f,  0.f,  0.f, 0, false,  3,  3.f,  2.f, 2, 100.f,  0.f,  0.f, false },
    { "Infinite Pad",     1500.f, 92.f, 60.f,  20.f,  7000.f,  0.f,  0.f, 2, false,  3,  2.f, -3.f, 1, 200.f,  0.f,  0.f, true  },
};

inline constexpr int kNumPresets = 50;

// Category boundaries (start index of each group)
inline constexpr struct { const char* name; int start; int count; } kCategories[] =
{
    { "VOCALS",           0,  10 },
    { "GUITAR",          10,  10 },
    { "KEYS & SYNTH",    20,  10 },
    { "DRUMS & PERC",    30,   8 },
    { "BASS",            38,   5 },
    { "SPECIAL",         43,   7 },
};

inline constexpr int kNumCategories = 6;

} // namespace EchoPresets
