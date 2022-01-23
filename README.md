
## ![Oven mit logo](oven-mit-diagonal.png) OvenMit Synthesizer

kenakofer's first ever synthesizer! Coming initially out of [BMusic's excellent tutoral series for lv2 synthesizers](https://www.youtube.com/watch?v=51eHCA4oCEI&list=PLkuRaNsK2AJ0D8uhRIjftgmqVW0yvDfMx), the developer decided to explore a bit. Since they didn't copy any particular features or techniques from the other open source synths out there, this is probably bad in some unforseen ways.

The existing featureset of this LV2 additive synthesizer include:
- 2 oscillators, with 5 shapes: sin, triangle, square, saw, and (band-limited) noise
- 2 ADSR envelopes, which can target the oscillator levels or frequency cutoffs
- The low pass cutoff for Osc 1 adjusts levels of individual partials
- Resonance height and width also adjust individual partials (this is uncached and inefficient at the moment)
- Osc 2 can simply add, or it can target the 1st with FM or AM or cutoff adjustments.
- Caching of the various waveforms with various partials to make everything somewhat more efficient.
- No GUI except for the basic LV2 sliders/dropdowns right now.

Things that are in-progress or coming soon, in approx. priority order:
- Unity Audio Plugin integration
- More efficient resonance
- Make noise more efficient somehow.
- Add cutoff key tracking optional
- Add mono mode with portamento
- Beat tracking
- Add a GUI. In my defense, BMusic's tutorial's haven't discussed that topic yet as of this writing