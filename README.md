# ThetaSynth

## About

![](thetasynth.png)

A simple software synthesizer developed with C++, using framework "[WDL-OL/IPlug](https://github.com/olilarkin/wdl-ol)".

This repository is a result of my personal interest and was created following articles found on [Making Audio Plugins - Martin Finke's Blog](https://www.martin-finke.de/tags/making_audio_plugins.html).

**NOTICE**: This repository contains C++ source code and GUI components **only**.

## GUI Components

All the GUI resources were **created by me**.

You can find and view the exported PNG files of the metallic knob [here](gui_knob).

Please note that the images may appear slightly jagged as they aren't the original versions.

A `.knob` file requires [KnobMan/JKnobMan](https://www.g200kg.com/jp/software/knobman.html).

## Knowledge

This software requires a variety of knowledge, including:

- Audio plugin framework: [WDL-OL/IPlug](https://github.com/olilarkin/wdl-ol)
- Finite state machine (for ADSR envelope generator)
- Fourier transform (for generating waveforms)
- Observer pattern using `<functional>` (C++ Technical Report 1)
- Realtime monitoring of MIDI signals using [pbhogan/Signals](https://github.com/pbhogan/Signals)
