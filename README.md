## OvenMit Synthesizer

<img align="left" src="oven-mit-diagonal-big.png">

kenakofer's first ever synthesizer! Coming initially out of [BMusic's excellent tutoral series for lv2 synthesizers](https://www.youtube.com/watch?v=51eHCA4oCEI&list=PLkuRaNsK2AJ0D8uhRIjftgmqVW0yvDfMx). Since this doesn't copy any particular features or techniques from the other open source synths out there, it's probably flawed in some unforseen ways.

Currently only successfully built and tested on x86_64 GNU/Linux, with other platforms coming soon.

----------
----------

The existing featureset of this LV2 additive synthesizer include:

- 2 oscillators, with 5 shapes: sin, triangle, square, saw, and (band-limited) noise
- 2 ADSR envelopes, which can target the oscillator levels or frequency cutoffs
- The low/high pass cutoff for Osc 1 adjusts levels of individual partials
- Resonance height and width also adjust individual partials
- All waveforms/cutoff/resonance/noise samples are cached, so DSP stays low
- Osc 2 adds by default, or it can target the 1st with FM or AM or cutoff adjustments.
- Polyphonic mode: Play all 128 keys if you want!
- Monophonic mode: optional portamento, auto portamento, and legato modes in any combination.
- No GUI except for what's provided by the LV2 host (I've used it with Jalv and Ardour with ease, and there's an branch that makes it slightly less terrible in LMMS)
- **Unity Audio Plugin**, with as many virtual synth instances as your DSP can handle.
    - See Native.cs for the interface, and Plugin_OvenMit.cpp for the plugin specific code, which handles configuration and scheduling of note events.
    - You can pretty easily connect the interface to a c# Midi reader if you want to synthesize music live in your project.
    - I'll probably link a bare-bones Unity project to demo this someday. Add an issue here if you'd like it sooner!


Things that are in-progress or coming soon, in approx. priority order:
- Get the borked Windows builds sorted out
    - I'm compiling on Linux, and am hoping I can get cross-compiling to actually work. I don't really understand mingw for cross compiling yet, and would love help.
    - I will eventually cave and build on Windows if I have to.
- The noise could be cleaner, both in code and spectrum
- Add a GUI. BMusic's tutorial's haven't discussed that topic yet as of this writing, so I'm
- Work correctly with rates other than 44.1k
- Mac build?