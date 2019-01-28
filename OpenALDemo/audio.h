#pragma once

enum
{
    AUDIO_WAVEFORM_PULSE_12_5,
    AUDIO_WAVEFORM_PULSE_25,
    AUDIO_WAVEFORM_PULSE_50,
    AUDIO_WAVEFORM_PULSE_75,
    AUDIO_WAVEFORM_PULSE_TRIANGLE,
    AUDIO_WAVEFORM_MAX
};

bool audioInit();
void audioWaveForm(int _waveform);
void audioLength(unsigned int _millisecond);
void audioDecay(float _decay);
void audioSweep(float _sweep);
void audioPitchTarget(float _pitchTarget);
void audioFreq(float _freq);
void audioPlay();
void audioStop();
void audioUpdate();
