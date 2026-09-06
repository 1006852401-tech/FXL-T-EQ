// ==========================================================
//  Seesaw Tilt EQ - Faust DSP Source Code
//  Generates AU, VST3, LV2, WebAudio, Max/MSP, and C++
// ==========================================================
declare name "SeesawTiltEQ";
declare version "1.0.0";
declare author "Audio DSP Team";
declare description "Single-knob tilt equalizer (boost highs / cut lows to the right, vice-versa to the left)";

import("stdfaust.lib");

// --- Parameters ---
// Central rotary knob: 0 dB is centered/neutral (default: 0 dB, range: -6 to +6 dB)
tilt_gain = vslider("Tilt [unit:dB][style:knob]", 0, -6, 6, 0.05) : si.smoo;
pivot_freq = hslider("Pivot Frequency [unit:Hz]", 650, 200, 2000, 1) : si.smoo;
bypass = checkbox("Bypass");

// --- Seesaw Tilt DSP Core ---
// Symmetrical dual-shelf filter:
// Turning right (> 0): cuts low (-tilt_gain), boosts high (+tilt_gain)
// Turning left (< 0): boosts low (+|tilt_gain|), cuts high (-|tilt_gain|)
tilt_unit = fi.low_shelf(-tilt_gain, pivot_freq) : fi.high_shelf(tilt_gain, pivot_freq);

// --- Stereo In/Out Processing with Smooth Bypass ---
process = ba.bypass2(bypass, (tilt_unit, tilt_unit));
