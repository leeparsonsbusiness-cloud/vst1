#include "PresetManager.h"

static PresetData createDefaultPreset(const char* name)
{
    PresetData p;
    p.name = name;
    
    // Osc 1
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Detune = 0.0f;
    p.osc1Level = 0.85f;
    p.unisonCount = 1;
    p.unisonDetune = 15.0f;

    // Osc 2
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Detune = 5.0f;
    p.osc2Level = 0.5f;
    p.fmDepth = 0.0f;
    p.oscSync = 0;

    // Transients
    p.transientType = 0;
    p.transientLevel = 0.0f;
    p.transientDecay = 15.0f;

    // Sub
    p.subWave = 0;
    p.subOctave = -1;
    p.subLevel = 0.0f;
    p.subDrive = 0.0f;

    // Filter
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 4500.0f;
    p.filterRes = 0.2f;
    p.filterDrive = 1.0f;
    p.filterEnvAmt = 0.4f;
    p.filterKeyTrack = 0.0f;
    p.filterLfoMod = 0.0f;
    p.formantMorph = 0.0f;

    // LFOs
    p.lfo1Rate = 2; // 1/4
    p.lfo1Wave = 0; // Sine
    p.lfo1ToCutoff = 0.0f;
    p.lfo1ToShape = 0.0f;
    
    p.lfo2Rate = 3; // 1/8
    p.lfo2Wave = 0;
    p.lfo2ToPitch = 0.0f;
    p.lfo2ToPan = 0.0f;

    // Envelopes
    p.ampAttack = 0.01f;
    p.ampDecay = 0.4f;
    p.ampSustain = 0.8f;
    p.ampRelease = 0.5f;
    p.ampDecayCurve = 1.0f;

    p.filterAttack = 0.01f;
    p.filterDecay = 0.3f;
    p.filterSustain = 0.5f;
    p.filterRelease = 0.4f;
    p.filterDecayCurve = 1.0f;

    // Legato & Pitch Drop
    p.playMode = 0;
    p.glideTime = 100.0f;
    p.pitchDropActive = 0;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 50.0f;

    // Chord
    p.chordMode = 0;
    p.chordType = 0;

    // XY
    p.trashGlossX = 0.0f;
    p.trashGlossY = 0.0f;

    // Mono Maker
    p.monoMakerActive = 1;
    p.monoMakerFreq = 120.0f;

    // Pumper
    p.pumpActive = 0;
    p.pumpDivision = 0;
    p.pumpDepth = 0.0f;
    p.pumpCurve = 2.0f;

    // FX
    p.fxDrive = 1.2f;
    p.fxChorusRate = 1.5f;
    p.fxChorusDepth = 0.35f;
    p.fxChorusMix = 0.0f; // bypassed initially
    p.fxDelayTime = 350.0f;
    p.fxDelayFeedback = 0.4f;
    p.fxDelayMix = 0.0f;
    p.fxDelayColor = 0.5f;
    
    p.fxReverbDecay = 0.5f;
    p.fxReverbDamping = 0.3f;
    p.fxReverbWidth = 0.8f;
    p.fxReverbMix = 0.0f;

    // Macros
    p.macroDrop = 0.0f;
    p.macroPunch = 0.0f;
    p.macroGrit = 0.0f;
    p.macroSpace = 0.0f;
    p.macroWidth = 0.0f;

    return p;
}

PresetManager::PresetManager()
{
    PresetData p;
    
    // ----------------------------------------------------
    // CATEGORY 1: BASSES (1-8)
    // ----------------------------------------------------
    
    // 1. Dirty Electro Saw Bass
    p = createDefaultPreset("Dirty Electro Saw Bass");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = -1;
    p.osc1Level = 0.9f;
    p.unisonCount = 3;
    p.unisonDetune = 12.0f;
    p.subLevel = 0.6f;
    p.subWave = 1; // Square
    p.subOctave = -1;
    p.subDrive = 0.5f;
    p.filterMode = 1; // LP24
    p.filterCutoff = 800.0f;
    p.filterRes = 0.55f;
    p.filterEnvAmt = 0.65f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.15f;
    p.filterSustain = 0.1f;
    p.filterRelease = 0.1f;
    p.trashGlossX = 0.6f; // heavy drive
    presets.push_back(p);
    
    // 2. Complextro Growl Bass
    p = createDefaultPreset("Complextro Growl Bass");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.85f;
    p.osc2Shape = 3.0f; // Pulse
    p.osc2Octave = -2;
    p.osc2Level = 0.6f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 50.0f;
    p.filterMode = 1; // LP24
    p.filterCutoff = 350.0f;
    p.filterRes = 0.65f;
    p.filterEnvAmt = 0.7f;
    p.filterAttack = 0.002f;
    p.filterDecay = 0.25f;
    p.filterSustain = 0.2f;
    p.trashGlossX = 0.8f;
    presets.push_back(p);
    
    // 3. Distorted Square Bass
    p = createDefaultPreset("Distorted Square Bass");
    p.osc1Shape = 3.0f; // Square
    p.osc1Octave = -1;
    p.osc1Level = 0.9f;
    p.subLevel = 0.7f;
    p.subWave = 1;
    p.subDrive = 0.8f;
    p.filterMode = 0; // LP12
    p.filterCutoff = 1100.0f;
    p.filterRes = 0.4f;
    p.trashGlossX = 0.9f;
    presets.push_back(p);
    
    // 4. Acid Squelch Bass
    p = createDefaultPreset("Acid Squelch Bass");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.9f;
    p.filterMode = 1; // LP24
    p.filterCutoff = 400.0f;
    p.filterRes = 0.92f;
    p.filterEnvAmt = 0.8f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.12f;
    p.filterSustain = 0.05f;
    p.filterDecayCurve = 4.0f;
    p.trashGlossX = 0.4f;
    presets.push_back(p);
    
    // 5. Sub Smasher
    p = createDefaultPreset("Sub Smasher");
    p.osc1Shape = 0.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.95f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 0;
    p.osc2Level = 0.4f;
    p.subLevel = 0.9f;
    p.subWave = 0;
    p.subDrive = 0.9f;
    p.filterMode = 0; // LP12
    p.filterCutoff = 300.0f;
    presets.push_back(p);
    
    // 6. Metallic FM Reeses Bass
    p = createDefaultPreset("Metallic FM Reeses Bass");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.8f;
    p.unisonCount = 5;
    p.unisonDetune = 20.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.7f;
    p.fmDepth = 0.45f;
    p.fxChorusMix = 0.45f;
    presets.push_back(p);
    
    // 7. Talking Formant Bass
    p = createDefaultPreset("Talking Formant Bass");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.9f;
    p.filterMode = 5; // Formant
    p.formantMorph = 1.5f;
    p.lfo1Rate = 2; // 1/4
    p.lfo1Wave = 1; // Triangle
    p.filterLfoMod = 0.5f;
    presets.push_back(p);
    
    // 8. Rubber Band Pluck Bass
    p = createDefaultPreset("Rubber Band Pluck Bass");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.85f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 1.5f;
    p.pitchDropTime = 80.0f;
    p.filterMode = 0; // LP12
    p.filterCutoff = 600.0f;
    p.filterRes = 0.5f;
    p.filterEnvAmt = 0.75f;
    p.filterDecay = 0.08f;
    presets.push_back(p);

    // ----------------------------------------------------
    // CATEGORY 2: LEADS (9-16)
    // ----------------------------------------------------
    
    // 9. Clarity Supersaw
    p = createDefaultPreset("Clarity Supersaw");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.unisonCount = 7;
    p.unisonDetune = 25.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.5f;
    p.filterMode = 1; // LP24
    p.filterCutoff = 4500.0f;
    p.filterRes = 0.2f;
    p.filterEnvAmt = 0.4f;
    p.trashGlossY = 0.7f; // High gloss compression
    p.fxChorusMix = 0.35f;
    p.fxDelayMix = 0.35f;
    p.fxReverbMix = 0.2f;
    presets.push_back(p);
    
    // 10. 8-Bit Glitch Lead
    p = createDefaultPreset("8-Bit Glitch Lead");
    p.osc1Shape = 3.0f; // Pulse
    p.osc1Octave = 1;
    p.osc1Level = 0.9f;
    p.osc2Shape = 3.0f;
    p.osc2Octave = 0;
    p.osc2Level = 0.5f;
    p.fxDrive = 5.0f; // heavy drive glitch
    p.trashGlossX = 0.8f; // trash mid boost/downsample style
    presets.push_back(p);
    
    // 11. Vocal Formant Lead
    p = createDefaultPreset("Vocal Formant Lead");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.8f;
    p.unisonCount = 5;
    p.unisonDetune = 18.0f;
    p.filterMode = 5; // Formant
    p.formantMorph = 0.0f; // sweeps with LFO 1
    p.lfo1Rate = 3; // 1/8 note
    p.lfo1Wave = 1; // Triangle
    p.filterLfoMod = 0.6f;
    p.fxDelayMix = 0.35f;
    p.fxReverbMix = 0.25f;
    presets.push_back(p);
    
    // 12. Stadium Anthem Lead
    p = createDefaultPreset("Stadium Anthem Lead");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 1;
    p.osc1Level = 0.9f;
    p.unisonCount = 7;
    p.unisonDetune = 30.0f;
    p.fxDelayMix = 0.45f;
    p.fxReverbMix = 0.4f; // wet lush stadium
    presets.push_back(p);
    
    // 13. Dirty Sync Lead
    p = createDefaultPreset("Dirty Sync Lead");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 1;
    p.osc2Level = 0.6f;
    p.oscSync = 1; // hard sync active
    p.filterMode = 1; // LP24
    p.filterCutoff = 1500.0f;
    p.filterEnvAmt = 0.6f;
    p.filterAttack = 0.05f;
    p.filterDecay = 0.4f;
    presets.push_back(p);
    
    // 14. Glitter Pop Screamer
    p = createDefaultPreset("Glitter Pop Screamer");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.lfo2Rate = 5; // fast 1/32 vibrato LFO
    p.lfo2Wave = 0; // Sine
    p.lfo2ToPitch = 0.2f; // Vibrato active
    p.trashGlossX = 0.7f; // presence boost
    presets.push_back(p);
    
    // 15. Laser Beam Lead
    p = createDefaultPreset("Laser Beam Lead");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 40.0f; // 40ms sweep
    p.fxChorusMix = 0.5f; // wide chorus
    presets.push_back(p);
    
    // 16. Eurodance Rave Saw
    p = createDefaultPreset("Eurodance Rave Saw");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 1;
    p.osc1Level = 0.8f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 0;
    p.osc2Level = 0.7f;
    p.trashGlossY = 0.6f; // air boost
    p.fxDelayMix = 0.4f;
    presets.push_back(p);

    // ----------------------------------------------------
    // CATEGORY 3: PLUCKS (17-23)
    // ----------------------------------------------------
    
    // 17. Zedd Punch Pluck
    p = createDefaultPreset("Zedd Punch Pluck");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.unisonCount = 5;
    p.unisonDetune = 15.0f;
    p.transientType = 0; // Click
    p.transientLevel = 0.8f;
    p.transientDecay = 15.0f;
    p.ampDecay = 0.25f;
    p.ampSustain = 0.0f;
    p.filterMode = 1; // LP24
    p.filterCutoff = 400.0f;
    p.filterRes = 0.6f;
    p.filterEnvAmt = 0.85f;
    p.filterDecay = 0.14f;
    p.filterSustain = 0.0f;
    presets.push_back(p);
    
    // 18. Trashy Pop Pluck
    p = createDefaultPreset("Trashy Pop Pluck");
    p.osc1Shape = 3.0f; // Pulse
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.ampDecay = 0.2f;
    p.ampSustain = 0.0f;
    p.trashGlossX = 0.6f; // diode drive
    p.fxDelayMix = 0.3f; // slapback
    p.fxDelayTime = 120.0f;
    presets.push_back(p);
    
    // 19. Glass Bell Pluck
    p = createDefaultPreset("Glass Bell Pluck");
    p.osc1Shape = 0.0f; // Sine
    p.osc1Octave = 1;
    p.osc1Level = 0.8f;
    p.osc2Shape = 0.0f;
    p.osc2Octave = 2;
    p.osc2Level = 0.6f;
    p.fmDepth = 0.7f; // FM bell chime
    p.ampDecay = 0.5f;
    p.ampSustain = 0.0f;
    p.fxReverbMix = 0.5f; // shimmer reverb
    presets.push_back(p);
    
    // 20. Marimba Synth Strike
    p = createDefaultPreset("Marimba Synth Strike");
    p.osc1Shape = 1.0f; // Triangle
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.transientType = 1; // Hammer strike
    p.transientLevel = 0.9f;
    p.transientDecay = 20.0f;
    p.ampDecay = 0.3f;
    p.ampSustain = 0.0f;
    presets.push_back(p);
    
    // 21. Staccato Arp Bite
    p = createDefaultPreset("Staccato Arp Bite");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 1;
    p.osc1Level = 0.9f;
    p.ampDecay = 0.15f;
    p.ampSustain = 0.0f;
    p.fxDelayMix = 0.4f; // 1/16 delay feel
    p.fxDelayTime = 250.0f;
    presets.push_back(p);
    
    // 22. Hollow Square Pluck
    p = createDefaultPreset("Hollow Square Pluck");
    p.osc1Shape = 3.0f; // 50% square
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.filterMode = 2; // BPF
    p.filterCutoff = 1000.0f;
    p.filterRes = 0.4f;
    p.filterEnvAmt = 0.8f;
    p.filterDecay = 0.1f;
    p.filterSustain = 0.0f;
    presets.push_back(p);
    
    // 23. Club Drop Pluck
    p = createDefaultPreset("Club Drop Pluck");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.unisonCount = 5;
    p.unisonDetune = 18.0f;
    p.ampDecay = 0.3f;
    p.ampSustain = 0.0f;
    p.pumpActive = 1; // 1/4 note pumping active
    p.pumpDivision = 0;
    p.pumpDepth = 0.85f;
    presets.push_back(p);

    // ----------------------------------------------------
    // CATEGORY 4: KEYS & CHORDS (24-30)
    // ----------------------------------------------------
    
    // 24. Euphoric Pop Chords
    p = createDefaultPreset("Euphoric Pop Chords");
    p.osc1Shape = 3.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.lfo1Rate = 2;
    p.lfo1Wave = 1; // Triangle
    p.lfo1ToShape = 0.4f; // PWM stabs
    p.fxChorusMix = 0.45f;
    presets.push_back(p);
    
    // 25. Radio Piano-Synth Hybrid
    p = createDefaultPreset("Radio Piano-Synth Hybrid");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.transientType = 1; // Hammer transient
    p.transientLevel = 0.8f;
    p.transientDecay = 30.0f;
    presets.push_back(p);
    
    // 26. Pumping Synth Brass
    p = createDefaultPreset("Pumping Synth Brass");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.8f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 0;
    p.osc2Level = 0.6f;
    p.filterMode = 0; // LP12
    p.filterCutoff = 300.0f;
    p.filterEnvAmt = 0.7f;
    p.filterDecay = 0.3f;
    p.pumpActive = 1;
    p.pumpDivision = 1; // 1/8 pump
    p.pumpDepth = 0.75f;
    presets.push_back(p);
    
    // 27. Wobbly Lo-Fi Keys
    p = createDefaultPreset("Wobbly Lo-Fi Keys");
    p.osc1Shape = 1.0f; // Triangle
    p.osc1Octave = 0;
    p.osc1Level = 0.7f;
    p.osc2Shape = 0.0f; // Sine
    p.osc2Octave = 0;
    p.osc2Level = 0.7f;
    p.lfo2Rate = 1; // slow wobble
    p.lfo2Wave = 0;
    p.lfo2ToPitch = 0.15f; // gentle pitch flutter
    p.trashGlossX = 0.3f; // tape saturation
    presets.push_back(p);
    
    // 28. Anthem Organ Stab
    p = createDefaultPreset("Anthem Organ Stab");
    p.osc1Shape = 0.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.osc2Shape = 0.0f;
    p.osc2Octave = 1; // octave up
    p.osc2Level = 0.6f;
    p.subLevel = 0.5f;
    p.subWave = 0;
    p.trashGlossX = 0.5f; // organ distortion drive
    presets.push_back(p);
    
    // 29. Bright EDM Piano Stab
    p = createDefaultPreset("Bright EDM Piano Stab");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.transientType = 1; // Hammer strike
    p.transientLevel = 0.95f;
    p.trashGlossY = 0.8f; // upward compression punch
    presets.push_back(p);
    
    // 30. Velvet Neo-Pop Chords
    p = createDefaultPreset("Velvet Neo-Pop Chords");
    p.osc1Shape = 1.0f; // Triangle
    p.osc1Octave = 0;
    p.osc1Level = 0.8f;
    p.filterMode = 0; // LP12
    p.filterCutoff = 1000.0f;
    p.filterRes = 0.1f;
    p.unisonCount = 5;
    p.unisonDetune = 20.0f;
    p.fxChorusMix = 0.3f;
    presets.push_back(p);

    // ----------------------------------------------------
    // CATEGORY 5: PADS & TEXTURES (31-35)
    // ----------------------------------------------------
    
    // 31. Lush Sidechain Pad
    p = createDefaultPreset("Lush Sidechain Pad");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.unisonCount = 7;
    p.unisonDetune = 25.0f;
    p.ampAttack = 0.8f; // slow attack
    p.pumpActive = 1;
    p.pumpDivision = 0; // 1/4 note pumping
    p.pumpDepth = 0.85f;
    presets.push_back(p);
    
    // 32. Shimmer Dream Pad
    p = createDefaultPreset("Shimmer Dream Pad");
    p.osc1Shape = 1.0f; // Triangle
    p.osc1Octave = 0;
    p.osc1Level = 0.8f;
    p.ampAttack = 1.5f;
    p.fxReverbMix = 0.9f; // 90% wet plate reverb
    p.fxReverbDecay = 0.9f;
    presets.push_back(p);
    
    // 33. Dark Cinema Drone
    p = createDefaultPreset("Dark Cinema Drone");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.9f;
    p.unisonCount = 5;
    p.unisonDetune = 18.0f;
    p.filterMode = 5; // Formant
    p.formantMorph = 0.0f;
    p.lfo1Rate = 1; // very slow filter sweep LFO
    p.lfo1Wave = 1; // Triangle
    p.filterLfoMod = 0.7f;
    presets.push_back(p);
    
    // 34. Retro Vapor Sweep
    p = createDefaultPreset("Retro Vapor Sweep");
    p.osc1Shape = 3.0f; // PWM style
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.lfo1Rate = 1;
    p.lfo1Wave = 0;
    p.lfo1ToShape = 0.5f;
    p.filterMode = 0; // LP12
    p.filterCutoff = 600.0f;
    p.filterLfoMod = 0.4f; // slow sweep
    p.fxChorusMix = 0.6f;
    presets.push_back(p);
    
    // 35. Glacier Atmosphere
    p = createDefaultPreset("Glacier Atmosphere");
    p.osc1Shape = 1.0f; // Triangle
    p.osc1Octave = 1;
    p.osc1Level = 0.8f;
    p.lfo2Rate = 0; // slow drift
    p.lfo2Wave = 0;
    p.lfo2ToPitch = 0.1f;
    p.fxReverbMix = 0.7f;
    presets.push_back(p);

    // ----------------------------------------------------
    // CATEGORY 6: TRANSITIONS & FX (36-40)
    // ----------------------------------------------------
    
    // 36. Hyperpop Bubble FX
    p = createDefaultPreset("Hyperpop Bubble FX");
    p.osc1Shape = 0.0f; // Sine
    p.osc1Octave = 1;
    p.osc1Level = 0.9f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 40.0f; // 40ms bubble drop
    p.filterMode = 1; // LP24
    p.filterCutoff = 2000.0f;
    p.filterRes = 0.8f;
    presets.push_back(p);
    
    // 37. Tension Noise Riser
    p = createDefaultPreset("Tension Noise Riser");
    p.osc1Shape = 3.0f;
    p.osc1Level = 0.0f; // white noise only
    p.transientType = 0;
    p.transientLevel = 0.85f;
    p.transientDecay = 100.0f;
    p.filterMode = 3; // HPF
    p.filterCutoff = 100.0f;
    p.filterEnvAmt = 0.9f; // sweeps up
    p.filterAttack = 3.0f; // 3 second sweep
    p.fxDelayMix = 0.6f;
    p.fxDelayFeedback = 0.8f;
    presets.push_back(p);
    
    // 38. Sub Drop Boom
    p = createDefaultPreset("Sub Drop Boom");
    p.osc1Shape = 0.0f; // Sine
    p.osc1Octave = -1;
    p.osc1Level = 0.95f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 100.0f; // 100ms sweep
    p.ampDecay = 1.5f; // long sub tail
    p.ampSustain = 0.0f;
    presets.push_back(p);
    
    // 39. Downlifter Laser Fall
    p = createDefaultPreset("Downlifter Laser Fall");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 1;
    p.osc1Level = 0.85f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 100.0f;
    p.lfo1Rate = 4; // synced LFO gate
    p.lfo1Wave = 3; // Square LFO gate
    p.lfo1ToCutoff = 0.8f;
    presets.push_back(p);
    
    // 40. Pre-Drop Impact
    p = createDefaultPreset("Pre-Drop Impact");
    p.osc1Shape = 2.0f;
    p.osc1Octave = -1;
    p.osc1Level = 0.9f;
    p.transientType = 3; // 808 Tock
    p.transientLevel = 0.9f;
    p.ampDecay = 0.4f;
    p.ampSustain = 0.0f;
    p.fxReverbMix = 0.6f;
    p.fxReverbDecay = 0.8f;
    presets.push_back(p);

    // ----------------------------------------------------
    // CATEGORY 7: BILLBOARD HITS & ANTHEMS (41-46)
    // ----------------------------------------------------

    // 41. Safe and Sound (Capital Cities Brass Lead)
    p = createDefaultPreset("Safe and Sound (Brass Lead)");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.unisonCount = 5;
    p.unisonDetune = 14.0f;
    p.osc2Shape = 3.0f; // Pulse
    p.osc2Octave = -1;
    p.osc2Level = 0.6f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 3800.0f;
    p.filterRes = 0.25f;
    p.filterEnvAmt = 0.45f;
    p.filterAttack = 0.015f;
    p.filterDecay = 0.35f;
    p.filterSustain = 0.6f;
    p.filterRelease = 0.3f;
    p.ampAttack = 0.01f;
    p.ampDecay = 0.4f;
    p.ampSustain = 0.85f;
    p.ampRelease = 0.4f;
    p.fxChorusMix = 0.25f;
    p.fxDelayTime = 180.0f;
    p.fxDelayMix = 0.20f;
    p.fxReverbMix = 0.25f;
    presets.push_back(p);

    // 42. Beautiful Now (Zedd Anthem Lead)
    p = createDefaultPreset("Beautiful Now (Zedd Anthem Lead)");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.95f;
    p.unisonCount = 7;
    p.unisonDetune = 32.0f;
    p.osc2Shape = 2.0f; // Saw
    p.osc2Octave = 1;
    p.osc2Detune = 7.0f;
    p.osc2Level = 0.7f;
    p.transientType = 3; // 808 Tock punch
    p.transientLevel = 0.4f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 9500.0f;
    p.filterRes = 0.20f;
    p.filterDrive = 2.2f;
    p.ampAttack = 0.001f;
    p.ampDecay = 1.2f;
    p.ampSustain = 0.90f;
    p.ampRelease = 0.5f;
    p.fxChorusRate = 1.6f;
    p.fxChorusMix = 0.45f;
    p.fxDelayTime = 375.0f;
    p.fxDelayFeedback = 0.45f;
    p.fxDelayMix = 0.35f;
    p.fxReverbDecay = 0.7f;
    p.fxReverbMix = 0.35f;
    p.macroPunch = 0.6f;
    presets.push_back(p);

    // 43. Glad You Came (The Wanted Accordion Pluck)
    p = createDefaultPreset("Glad You Came (Club Accordion Pluck)");
    p.osc1Shape = 2.4f; // Square/Saw hybrid
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.unisonCount = 3;
    p.unisonDetune = 10.0f;
    p.osc2Shape = 3.0f; // Square
    p.osc2Octave = 1;
    p.osc2Level = 0.6f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 2400.0f;
    p.filterRes = 0.3f;
    p.filterEnvAmt = 0.65f;
    p.filterAttack = 0.002f;
    p.filterDecay = 0.18f;
    p.filterSustain = 0.2f;
    p.filterRelease = 0.25f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.22f;
    p.ampSustain = 0.25f;
    p.ampRelease = 0.25f;
    p.fxChorusMix = 0.30f;
    p.fxDelayTime = 220.0f;
    p.fxDelayMix = 0.30f;
    p.fxReverbMix = 0.20f;
    presets.push_back(p);

    // 44. Let It Rock (Kevin Rudolf Distorted Lead)
    p = createDefaultPreset("Let It Rock (Distorted Rock Lead)");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.9f;
    p.unisonCount = 5;
    p.unisonDetune = 20.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 0;
    p.osc2Detune = 12.0f;
    p.osc2Level = 0.75f;
    p.fmDepth = 0.25f;
    p.subWave = 1; // Square
    p.subOctave = -1;
    p.subLevel = 0.65f;
    p.subDrive = 0.65f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 5500.0f;
    p.filterRes = 0.35f;
    p.filterDrive = 4.5f;
    p.ampAttack = 0.005f;
    p.ampDecay = 0.5f;
    p.ampSustain = 0.85f;
    p.ampRelease = 0.35f;
    p.fxDrive = 3.5f;
    p.fxChorusMix = 0.20f;
    p.fxReverbMix = 0.25f;
    p.trashGlossX = 0.7f;
    presets.push_back(p);

    // 45. Right Round (Flo Rida Club Pulse Stab)
    p = createDefaultPreset("Right Round (Club Pulse Stab)");
    p.osc1Shape = 2.85f; // Pulse Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.95f;
    p.unisonCount = 5;
    p.unisonDetune = 18.0f;
    p.osc2Shape = 2.0f; // Saw
    p.osc2Octave = -1;
    p.osc2Level = 0.8f;
    p.subWave = 0; // Sine sub
    p.subOctave = -1;
    p.subLevel = 0.5f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 3200.0f;
    p.filterRes = 0.35f;
    p.filterEnvAmt = 0.55f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.22f;
    p.filterSustain = 0.3f;
    p.filterRelease = 0.25f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.3f;
    p.ampSustain = 0.5f;
    p.ampRelease = 0.3f;
    p.pumpActive = 1;
    p.pumpDepth = 0.55f;
    p.fxChorusMix = 0.35f;
    p.fxDelayTime = 250.0f;
    p.fxDelayMix = 0.25f;
    p.fxReverbMix = 0.25f;
    presets.push_back(p);

    // 46. Blow (Kesha Glitter Squelch Lead)
    p = createDefaultPreset("Blow (Glitter Squelch Lead)");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.95f;
    p.unisonCount = 7;
    p.unisonDetune = 28.0f;
    p.osc2Shape = 3.0f; // Square
    p.osc2Octave = 1;
    p.osc2Level = 0.6f;
    p.fmDepth = 0.35f;
    p.subWave = 1;
    p.subOctave = -1;
    p.subLevel = 0.55f;
    p.subDrive = 0.5f;
    p.filterMode = 5; // Vowel Formant
    p.formantMorph = 1.2f;
    p.filterRes = 0.5f;
    p.filterDrive = 3.0f;
    p.playMode = 1; // Mono Legato for pitch sliding
    p.glideTime = 65.0f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.4f;
    p.ampSustain = 0.9f;
    p.ampRelease = 0.3f;
    p.fxChorusMix = 0.40f;
    p.fxDelayTime = 300.0f;
    p.fxDelayMix = 0.30f;
    p.fxReverbMix = 0.25f;
    p.trashGlossX = 0.4f;
    presets.push_back(p);

    // 47. Blow - Dirty Squelch Bass (Verse/Drop)
    p = createDefaultPreset("Blow - Dirty Squelch Bass");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = -1;
    p.osc1Level = 0.95f;
    p.unisonCount = 3;
    p.unisonDetune = 12.0f;
    p.osc2Shape = 1.0f; // Square
    p.osc2Octave = -2;
    p.osc2Level = 0.70f;
    p.subWave = 0;
    p.subOctave = -2;
    p.subLevel = 0.85f;
    p.subDrive = 0.60f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 2200.0f;
    p.filterRes = 0.45f;
    p.filterDrive = 4.5f;
    p.filterEnvAmt = 0.55f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.18f;
    p.filterSustain = 0.20f;
    p.playMode = 1; // Mono Legato
    p.glideTime = 45.0f;
    p.trashGlossX = 0.55f; // Grit
    p.fxDrive = 2.5f;
    p.fxChorusMix = 0.20f;
    presets.push_back(p);

    // 48. Blow - Pumping Anthem Chords (Pre/Chorus)
    p = createDefaultPreset("Blow - Pumping Anthem Chords");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.90f;
    p.unisonCount = 7; // Max Unison Supersaw
    p.unisonDetune = 35.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.65f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 12000.0f;
    p.filterRes = 0.15f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.50f;
    p.ampSustain = 0.90f;
    p.ampRelease = 0.40f;
    p.pumpActive = 1; // Sidechain Pumper
    p.pumpDepth = 0.80f;
    p.pumpDivision = 0; // 1/4 note pump
    p.fxChorusMix = 0.50f;
    p.fxDelayMix = 0.25f;
    p.fxReverbMix = 0.35f;
    p.trashGlossY = 0.45f; // Gloss
    presets.push_back(p);

    // 49. Blow - Screaming Laser Lead (Chorus Hook)
    p = createDefaultPreset("Blow - Screaming Laser Lead");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 1;
    p.osc1Level = 0.95f;
    p.unisonCount = 5;
    p.unisonDetune = 20.0f;
    p.osc2Shape = 1.0f; // Square Sync
    p.osc2Octave = 0;
    p.osc2Level = 0.60f;
    p.oscSync = 1;
    p.filterMode = 5; // Formant Vowel
    p.formantMorph = 0.85f;
    p.filterRes = 0.40f;
    p.filterDrive = 2.0f;
    p.playMode = 1; // Mono Legato
    p.glideTime = 55.0f;
    p.ampAttack = 0.001f;
    p.ampRelease = 0.35f;
    p.fxDelayTime = 333.0f; // 1/8d delay
    p.fxDelayMix = 0.35f;
    p.fxReverbMix = 0.30f;
    p.trashGlossX = 0.35f;
    presets.push_back(p);

    // 50. Blow - Staccato Party Pluck (Intro/Verse Riff)
    p = createDefaultPreset("Blow - Staccato Party Pluck");
    p.osc1Shape = 1.0f; // Square
    p.osc1Octave = 0;
    p.osc1Level = 0.90f;
    p.unisonCount = 3;
    p.unisonDetune = 10.0f;
    p.osc2Shape = 2.0f; // Saw
    p.osc2Octave = 0;
    p.osc2Level = 0.50f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 1800.0f;
    p.filterRes = 0.35f;
    p.filterEnvAmt = 0.70f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.12f;
    p.filterSustain = 0.0f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.20f;
    p.ampSustain = 0.0f;
    p.ampRelease = 0.10f;
    p.fxDelayTime = 250.0f; // 1/8 note stereo bounce
    p.fxDelayMix = 0.40f;
    p.fxReverbMix = 0.20f;
    presets.push_back(p);

    // 51. Blow - Glitter Shimmer FX (Transitions)
    p = createDefaultPreset("Blow - Glitter Shimmer FX");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 1;
    p.unisonCount = 7;
    p.unisonDetune = 40.0f;
    p.filterCutoff = 8000.0f;
    p.ampAttack = 0.8f;
    p.ampRelease = 2.0f;
    p.fxReverbDecay = 0.95f;
    p.fxReverbMix = 0.70f;
    presets.push_back(p);

    // 52. Blow - Heavy Sub Smasher (Drop Anchor)
    p = createDefaultPreset("Blow - Heavy Sub Smasher");
    p.osc1Shape = 0.0f; // Sine
    p.osc1Octave = -2;
    p.osc1Level = 0.90f;
    p.subWave = 0;
    p.subOctave = -2;
    p.subLevel = 1.0f;
    p.subDrive = 0.8f;
    p.filterCutoff = 350.0f;
    p.monoMakerActive = 1;
    p.monoMakerFreq = 160.0f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.30f;
    p.ampSustain = 0.85f;
    p.ampRelease = 0.10f;
    presets.push_back(p);

    // 53. Beautiful Now - Euphoric Drop Chords
    p = createDefaultPreset("Beautiful Now - Euphoric Drop Chords");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.95f;
    p.unisonCount = 7; // Max Supersaw
    p.unisonDetune = 38.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.70f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 14000.0f;
    p.filterRes = 0.12f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.45f;
    p.ampSustain = 0.92f;
    p.ampRelease = 0.35f;
    p.pumpActive = 1; // 1/4 note pumping
    p.pumpDepth = 0.75f;
    p.pumpDivision = 0;
    p.trashGlossY = 0.60f; // High gloss polish
    p.fxChorusMix = 0.45f;
    p.fxDelayMix = 0.20f;
    p.fxReverbMix = 0.35f;
    presets.push_back(p);

    // 54. Beautiful Now - Soaring Festival Lead
    p = createDefaultPreset("Beautiful Now - Soaring Festival Lead");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 1;
    p.osc1Level = 0.95f;
    p.unisonCount = 5;
    p.unisonDetune = 22.0f;
    p.osc2Shape = 1.0f; // Square Sync
    p.osc2Octave = 0;
    p.osc2Level = 0.65f;
    p.oscSync = 1;
    p.playMode = 1; // Mono Legato
    p.glideTime = 60.0f;
    p.filterMode = 0;
    p.filterCutoff = 16000.0f;
    p.ampAttack = 0.001f;
    p.ampRelease = 0.35f;
    p.fxDelayTime = 375.0f; // 1/8d delay
    p.fxDelayMix = 0.35f;
    p.fxReverbDecay = 0.75f;
    p.fxReverbMix = 0.30f;
    p.trashGlossY = 0.50f;
    presets.push_back(p);

    // 55. Beautiful Now - Clockwork Bell Pluck
    p = createDefaultPreset("Beautiful Now - Clockwork Bell Pluck");
    p.osc1Shape = 1.0f; // Square
    p.osc1Octave = 0;
    p.osc1Level = 0.85f;
    p.unisonCount = 3;
    p.unisonDetune = 12.0f;
    p.osc2Shape = 0.0f; // Sine
    p.osc2Octave = 1;
    p.osc2Level = 0.60f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 2800.0f;
    p.filterRes = 0.30f;
    p.filterEnvAmt = 0.65f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.15f;
    p.filterSustain = 0.0f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.22f;
    p.ampSustain = 0.0f;
    p.ampRelease = 0.12f;
    p.fxDelayTime = 234.0f; // 1/8 note sync
    p.fxDelayMix = 0.35f;
    p.fxReverbMix = 0.25f;
    presets.push_back(p);

    // 56. Beautiful Now - Rolling Progressive Bass
    p = createDefaultPreset("Beautiful Now - Rolling Progressive Bass");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = -1;
    p.osc1Level = 0.95f;
    p.osc2Shape = 1.0f; // Square
    p.osc2Octave = -2;
    p.osc2Level = 0.70f;
    p.subWave = 0; // Sine Sub
    p.subOctave = -2;
    p.subLevel = 0.90f;
    p.subDrive = 0.40f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 1800.0f;
    p.filterRes = 0.30f;
    p.filterDrive = 2.5f;
    p.filterEnvAmt = 0.50f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.18f;
    p.filterSustain = 0.30f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.25f;
    p.ampSustain = 0.80f;
    p.ampRelease = 0.15f;
    p.monoMakerActive = 1;
    p.monoMakerFreq = 160.0f;
    presets.push_back(p);

    // 57. Beautiful Now - Cinematic Emotional Pad
    p = createDefaultPreset("Beautiful Now - Cinematic Emotional Pad");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.90f;
    p.unisonCount = 7;
    p.unisonDetune = 30.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 1;
    p.osc2Level = 0.50f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 3500.0f;
    p.filterRes = 0.10f;
    p.ampAttack = 0.65f; // Soft swell
    p.ampDecay = 1.50f;
    p.ampSustain = 0.95f;
    p.ampRelease = 1.80f;
    p.fxChorusMix = 0.60f;
    p.fxReverbDecay = 0.90f;
    p.fxReverbMix = 0.55f;
    p.trashGlossY = 0.40f;
    presets.push_back(p);

    // 58. Beautiful Now - Emotional Breakdown Piano
    p = createDefaultPreset("Beautiful Now - Emotional Breakdown Piano");
    p.osc1Shape = 1.0f; // Square / Pulse
    p.osc1Octave = 0;
    p.osc1Level = 0.90f;
    p.unisonCount = 3;
    p.unisonDetune = 10.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.50f;
    p.filterMode = 0;
    p.filterCutoff = 8000.0f;
    p.filterRes = 0.15f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.60f;
    p.ampSustain = 0.40f;
    p.ampRelease = 0.30f;
    p.trashGlossY = 0.55f;
    p.fxChorusMix = 0.35f;
    p.fxReverbMix = 0.30f;
    presets.push_back(p);

    // 59. Beautiful Now - Stadium Punch Kick (Sub Synth)
    p = createDefaultPreset("Beautiful Now - Stadium Punch Kick");
    p.osc1Shape = 0.0f; // Sine
    p.osc1Octave = -2;
    p.osc1Level = 1.0f;
    p.pitchDropActive = 1;
    p.pitchDropOctaves = 2.0f;
    p.pitchDropTime = 40.0f;
    p.subWave = 0;
    p.subOctave = -2;
    p.subLevel = 1.0f;
    p.subDrive = 0.70f;
    p.filterCutoff = 250.0f;
    p.monoMakerActive = 1;
    p.monoMakerFreq = 160.0f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.32f;
    p.ampSustain = 0.0f;
    p.ampRelease = 0.05f;
    presets.push_back(p);

    // 60. Beautiful Now - White Noise Tension Sweep
    p = createDefaultPreset("Beautiful Now - White Noise Tension Sweep");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 1;
    p.unisonCount = 7;
    p.unisonDetune = 50.0f;
    p.filterMode = 3; // Highpass
    p.filterCutoff = 3500.0f;
    p.filterRes = 0.45f;
    p.ampAttack = 2.0f;
    p.ampRelease = 1.5f;
    p.fxReverbDecay = 0.95f;
    p.fxReverbMix = 0.75f;
    presets.push_back(p);

    // 61. Right Round - Club Pulse Synth Stab (Main Hook)
    p = createDefaultPreset("Right Round - Club Pulse Synth Stab");
    p.osc1Shape = 1.0f; // Square
    p.osc1Octave = 0;
    p.osc1Level = 0.95f;
    p.unisonCount = 5;
    p.unisonDetune = 18.0f;
    p.osc2Shape = 2.0f; // Saw
    p.osc2Octave = 0;
    p.osc2Level = 0.65f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 3200.0f;
    p.filterRes = 0.35f;
    p.filterEnvAmt = 0.60f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.22f;
    p.filterSustain = 0.15f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.35f;
    p.ampSustain = 0.30f;
    p.ampRelease = 0.15f;
    p.fxChorusMix = 0.40f;
    p.fxDelayTime = 238.0f; // 1/8 note sync
    p.fxDelayMix = 0.30f;
    p.fxReverbMix = 0.25f;
    p.trashGlossY = 0.50f;
    presets.push_back(p);

    // 62. Right Round - Dirty Electro Slap Bass
    p = createDefaultPreset("Right Round - Dirty Electro Slap Bass");
    p.osc1Shape = 1.0f; // Square
    p.osc1Octave = -1;
    p.osc1Level = 0.95f;
    p.osc2Shape = 2.0f; // Saw
    p.osc2Octave = -2;
    p.osc2Level = 0.60f;
    p.subWave = 0; // Sine Sub
    p.subOctave = -2;
    p.subLevel = 0.90f;
    p.subDrive = 0.50f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 1600.0f;
    p.filterRes = 0.30f;
    p.filterEnvAmt = 0.55f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.16f;
    p.filterSustain = 0.10f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.20f;
    p.ampSustain = 0.0f;
    p.ampRelease = 0.08f;
    p.playMode = 1; // Mono Legato
    p.glideTime = 40.0f;
    p.monoMakerActive = 1;
    p.monoMakerFreq = 160.0f;
    p.trashGlossX = 0.40f;
    presets.push_back(p);

    // 63. Right Round - Anthemic Octave Club Lead
    p = createDefaultPreset("Right Round - Anthemic Octave Club Lead");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 1;
    p.osc1Level = 0.95f;
    p.unisonCount = 7;
    p.unisonDetune = 30.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = 0;
    p.osc2Level = 0.70f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 15000.0f;
    p.ampAttack = 0.001f;
    p.ampRelease = 0.30f;
    p.fxDelayTime = 357.0f; // 1/8d
    p.fxDelayMix = 0.35f;
    p.fxReverbMix = 0.30f;
    p.trashGlossY = 0.60f;
    presets.push_back(p);

    // 64. Right Round - Pumping Stadium Chords
    p = createDefaultPreset("Right Round - Pumping Stadium Chords");
    p.osc1Shape = 2.0f;
    p.osc1Octave = 0;
    p.osc1Level = 0.90f;
    p.unisonCount = 7;
    p.unisonDetune = 35.0f;
    p.osc2Shape = 1.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.65f;
    p.filterCutoff = 12000.0f;
    p.pumpActive = 1;
    p.pumpDepth = 0.80f;
    p.pumpDivision = 0; // 1/4 note pump
    p.ampAttack = 0.001f;
    p.ampDecay = 0.45f;
    p.ampSustain = 0.90f;
    p.ampRelease = 0.30f;
    p.fxChorusMix = 0.45f;
    p.fxReverbMix = 0.35f;
    presets.push_back(p);

    // 65. Right Round - Glitch Squelch Arp (Ear Candy)
    p = createDefaultPreset("Right Round - Glitch Squelch Arp");
    p.osc1Shape = 1.0f; // Square
    p.osc1Octave = 1;
    p.osc1Level = 0.90f;
    p.osc2Shape = 0.0f; // Sine Sync
    p.osc2Octave = 1;
    p.osc2Level = 0.50f;
    p.filterMode = 1; // LPF 24
    p.filterCutoff = 2400.0f;
    p.filterRes = 0.50f;
    p.filterEnvAmt = 0.70f;
    p.filterAttack = 0.001f;
    p.filterDecay = 0.12f;
    p.filterSustain = 0.0f;
    p.ampAttack = 0.001f;
    p.ampDecay = 0.18f;
    p.ampSustain = 0.0f;
    p.ampRelease = 0.08f;
    p.fxDelayTime = 238.0f;
    p.fxDelayMix = 0.35f;
    presets.push_back(p);

    // 66. Right Round - 80s Retro Analog Brass
    p = createDefaultPreset("Right Round - 80s Retro Analog Brass");
    p.osc1Shape = 2.0f; // Saw
    p.osc1Octave = 0;
    p.osc1Level = 0.90f;
    p.unisonCount = 5;
    p.unisonDetune = 20.0f;
    p.osc2Shape = 2.0f;
    p.osc2Octave = -1;
    p.osc2Level = 0.65f;
    p.filterMode = 0; // LPF 12
    p.filterCutoff = 4800.0f;
    p.filterRes = 0.25f;
    p.filterEnvAmt = 0.40f;
    p.filterAttack = 0.02f; // Brass swell
    p.filterDecay = 0.40f;
    p.filterSustain = 0.60f;
    p.ampAttack = 0.02f;
    p.ampDecay = 0.40f;
    p.ampSustain = 0.75f;
    p.ampRelease = 0.30f;
    p.fxChorusMix = 0.50f;
    p.fxReverbMix = 0.30f;
    presets.push_back(p);

    // Populate name array
    for (const auto& preset : presets)
        presetNames.push_back(preset.name);
}

int PresetManager::getNumPresets() const
{
    return static_cast<int>(presets.size());
}

juce::String PresetManager::getPresetName(int index) const
{
    if (index >= 0 && index < getNumPresets())
        return presets[static_cast<size_t>(index)].name;
    return {};
}

const std::vector<std::string>& PresetManager::getPresetNames() const
{
    return presetNames;
}

void PresetManager::applyPreset(int index, juce::AudioProcessorValueTreeState& apvts)
{
    if (index < 0 || index >= getNumPresets())
        return;
        
    const auto& p = presets[static_cast<size_t>(index)];
    
    // Safety store of index
    if (auto* param = apvts.getParameter("preset"))
        param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(static_cast<float>(index)));

    // Helper lambda to assign parameters safely
    auto setParam = [&](const juce::String& paramId, float value) {
        if (auto* param = apvts.getParameter(paramId))
        {
            float norm = param->getNormalisableRange().convertTo0to1(value);
            param->setValueNotifyingHost(norm);
        }
    };

    setParam("osc1_shape", p.osc1Shape);
    setParam("osc1_octave", static_cast<float>(p.osc1Octave));
    setParam("osc1_detune", p.osc1Detune);
    setParam("osc1_level", p.osc1Level);
    setParam("unison_count", static_cast<float>(p.unisonCount));
    setParam("unison_detune", p.unisonDetune);

    setParam("osc2_shape", p.osc2Shape);
    setParam("osc2_octave", static_cast<float>(p.osc2Octave));
    setParam("osc2_detune", p.osc2Detune);
    setParam("osc2_level", p.osc2Level);
    setParam("osc_fm_depth", p.fmDepth);
    setParam("osc_sync", static_cast<float>(p.oscSync));

    setParam("transient_type", static_cast<float>(p.transientType));
    setParam("transient_level", p.transientLevel);
    setParam("transient_decay", p.transientDecay);

    setParam("sub_wave", static_cast<float>(p.subWave));
    setParam("sub_octave", static_cast<float>(p.subOctave));
    setParam("sub_level", p.subLevel);
    setParam("sub_drive", p.subDrive);

    setParam("filter_mode", static_cast<float>(p.filterMode));
    setParam("filter_cutoff", p.filterCutoff);
    setParam("filter_res", p.filterRes);
    setParam("filter_drive", p.filterDrive);
    setParam("filter_env_amt", p.filterEnvAmt);
    setParam("filter_key_track", p.filterKeyTrack);
    setParam("filter_lfo_mod", p.filterLfoMod);
    setParam("formant_morph", p.formantMorph);

    setParam("lfo1_rate", static_cast<float>(p.lfo1Rate));
    setParam("lfo1_wave", static_cast<float>(p.lfo1Wave));
    setParam("lfo1_to_cutoff", p.lfo1ToCutoff);
    setParam("lfo1_to_shape", p.lfo1ToShape);

    setParam("lfo2_rate", static_cast<float>(p.lfo2Rate));
    setParam("lfo2_wave", static_cast<float>(p.lfo2Wave));
    setParam("lfo2_to_pitch", p.lfo2ToPitch);
    setParam("lfo2_to_pan", p.lfo2ToPan);

    setParam("amp_attack", p.ampAttack);
    setParam("amp_decay", p.ampDecay);
    setParam("amp_sustain", p.ampSustain);
    setParam("amp_release", p.ampRelease);
    setParam("amp_decay_curve", p.ampDecayCurve);

    setParam("filter_attack", p.filterAttack);
    setParam("filter_decay", p.filterDecay);
    setParam("filter_sustain", p.filterSustain);
    setParam("filter_release", p.filterRelease);
    setParam("filter_decay_curve", p.filterDecayCurve);

    setParam("play_mode", static_cast<float>(p.playMode));
    setParam("glide_time", p.glideTime);
    setParam("pitch_drop_active", static_cast<float>(p.pitchDropActive));
    setParam("pitch_drop_octaves", p.pitchDropOctaves);
    setParam("pitch_drop_time", p.pitchDropTime);

    setParam("chord_mode", static_cast<float>(p.chordMode));
    setParam("chord_type", static_cast<float>(p.chordType));

    setParam("trash_gloss_x", p.trashGlossX);
    setParam("trash_gloss_y", p.trashGlossY);

    setParam("mono_maker_active", static_cast<float>(p.monoMakerActive));
    setParam("mono_maker_frequency", p.monoMakerFreq);

    setParam("pump_active", static_cast<float>(p.pumpActive));
    setParam("pump_division", static_cast<float>(p.pumpDivision));
    setParam("pump_depth", p.pumpDepth);
    setParam("pump_curve", p.pumpCurve);

    setParam("fx_drive", p.fxDrive);
    setParam("fx_chorus_rate", p.fxChorusRate);
    setParam("fx_chorus_depth", p.fxChorusDepth);
    setParam("fx_chorus_mix", p.fxChorusMix);
    setParam("fx_delay_time", p.fxDelayTime);
    setParam("fx_delay_feedback", p.fxDelayFeedback);
    setParam("fx_delay_mix", p.fxDelayMix);
    setParam("fx_delay_color", p.fxDelayColor);
    setParam("fx_reverb_decay", p.fxReverbDecay);
    setParam("fx_reverb_damping", p.fxReverbDamping);
    setParam("fx_reverb_width", p.fxReverbWidth);
    setParam("fx_reverb_mix", p.fxReverbMix);

    setParam("macro_drop", p.macroDrop);
    setParam("macro_punch", p.macroPunch);
    setParam("macro_grit", p.macroGrit);
    setParam("macro_space", p.macroSpace);
    setParam("macro_width", p.macroWidth);
}

void PresetManager::saveUserPreset(const juce::String& category, const juce::String& name)
{
    if (presetAPVTS == nullptr) 
        return;
        
    juce::File userDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                            .getChildFile("KeshaAndZeddSynth")
                            .getChildFile("UserPresets")
                            .getChildFile(category);
    if (!userDir.exists())
        userDir.createDirectory();
        
    juce::File presetFile = userDir.getChildFile(name + ".json");
    
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("name", name);
    obj->setProperty("category", category);
    
    // Save all parameters as raw floats
    for (auto* param : presetAPVTS->processor.getParameters())
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            float val = presetAPVTS->getRawParameterValue(rangedParam->paramID)->load();
            obj->setProperty(rangedParam->paramID, val);
        }
    }
    
    juce::var v(obj.get());
    presetFile.replaceWithText(juce::JSON::toString(v));
}

bool PresetManager::loadUserPreset(const juce::File& file)
{
    if (presetAPVTS == nullptr || !file.existsAsFile()) 
        return false;
        
    juce::var parsed = juce::JSON::parse(file);
    if (parsed.isVoid()) 
        return false;
        
    if (auto* obj = parsed.getDynamicObject())
    {
        for (auto* param : presetAPVTS->processor.getParameters())
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                if (obj->hasProperty(rangedParam->paramID))
                {
                    float val = static_cast<float>(obj->getProperty(rangedParam->paramID));
                    float normalizedVal = rangedParam->getNormalisableRange().convertTo0to1(val);
                    rangedParam->setValueNotifyingHost(normalizedVal);
                }
            }
        }
        return true;
    }
    return false;
}
