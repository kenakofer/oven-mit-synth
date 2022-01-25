## OvenMit Synthesizer

<img align="left" src="oven-mit-diagonal-big.png">

kenakofer's first ever synthesizer! Coming initially out of [BMusic's excellent tutoral series for lv2 synthesizers](https://www.youtube.com/watch?v=51eHCA4oCEI&list=PLkuRaNsK2AJ0D8uhRIjftgmqVW0yvDfMx), the developer decided to explore a bit. Since they didn't copy any particular features or techniques from the other open source synths out there, this is probably bad in some unforseen ways.

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
- No GUI except for the basic LV2 sliders/dropdowns right now.


Things that are in-progress or coming soon, in approx. priority order:
- Unity Audio Plugin integration
- Add cutoff key tracking optional
- Beat tracking
- The noise could be cleaner, both in code and spectrum
- Add a GUI. In my defense, BMusic's tutorial's haven't discussed that topic yet as of this writing