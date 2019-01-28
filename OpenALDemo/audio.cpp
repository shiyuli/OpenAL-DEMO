#include <stddef.h>
#include <time.h>
#include <stdio.h>

#include <alc.h>
#include <al.h>

#include "audio.h"

#pragma comment(lib, "OpenAL32.lib")

#define DEFAULT_GAIN (.1f)
#define DEFAULT_FREQ (440)

static ALuint sid; // source id
static int waveform; // enum AUDIO_WAVEFORM
static ALuint buffers[AUDIO_WAVEFORM_MAX];

static unsigned int length;
static unsigned int start;
static float decay;
static float gain;
static float sweep;
static float pitchTarget;
static float pitch;
static float freq = DEFAULT_FREQ;

bool audioInit()
{
    ALCdevice *device = alcOpenDevice(NULL); // const ALCchar *devicename
    if (device == NULL)
        return false;

    ALCcontext *context = alcCreateContext(
        device, // ALCdevice *device
        NULL    // const ALCint* attrlist
    );

    if (context == NULL)
        return false;

    alcMakeContextCurrent(context); // ALCcontext *context

    alGenBuffers(
        AUDIO_WAVEFORM_MAX, // ALsizei n
        buffers             // ALuint* buffers
    );

    // pulse wave data
    unsigned char pulse_data[][8] = {
        { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // AUDIO_WAVEFORM_PULSE_12_5
        { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // AUDIO_WAVEFORM_PULSE_25
        { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }, // AUDIO_WAVEFORM_PULSE_50
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 }  // AUDIO_WAVEFORM_PULSE_75
    };

    for (int i(AUDIO_WAVEFORM_PULSE_12_5); i <= AUDIO_WAVEFORM_PULSE_75; ++i)
    {
        const ALsizei pulse_data_size = sizeof pulse_data[0];
        alBufferData(
            buffers[i],            // ALuint bid
            AL_FORMAT_MONO8,       // ALenum format
            pulse_data[i],         // const ALvoid* data
            pulse_data_size,       // ALsizei size
            pulse_data_size * 440  // ALsizei freq
        );
    }

    unsigned char triangle_data[] = {
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const ALsizei triangle_data_size = sizeof triangle_data;
    alBufferData(
        buffers[AUDIO_WAVEFORM_PULSE_TRIANGLE], // ALuint bid
        AL_FORMAT_MONO8,                        // ALenum format
        triangle_data,                          // const ALvoid* data
        triangle_data_size,                     // ALsizei size
        triangle_data_size * 440                // ALsizei freq
    );

    alGenSources(
        1,   // ALsizei n
        &sid // ALuint* sources
    );

    alSourcei(
        sid,        // ALuint sid
        AL_LOOPING, // ALenum param
        AL_TRUE     // ALint value
    );

    return true;
}

void audioWaveForm(int _waveform)
{
    waveform = _waveform;
}

void audioLength(unsigned int _millisecond)
{
    length = _millisecond;
}

void audioDecay(float _decay)
{
    decay = _decay;
}

void audioSweep(float _sweep)
{
    sweep = _sweep;
}

void audioPitchTarget(float _pitchTarget)
{
    pitchTarget = _pitchTarget;
}

void audioFreq(float _freq)
{
    freq = _freq;

    //alSourcef(
    //    sid,      // ALuint sid
    //    AL_PITCH, // ALenum param
    //    pitch     // ALfloat value
    //);
}

void audioPlay()
{
    alSourcef(
        sid,                // ALuint sid
        AL_GAIN,            // ALenum param
        gain = DEFAULT_GAIN // ALfloat value
    );

    alSourcef(
        sid,      // ALuint sid
        AL_PITCH, // ALenum param
        pitch = freq / DEFAULT_FREQ // ALfloat value
    );

    alSourcei(
        sid,              // ALuint sid
        AL_BUFFER,        // ALenum param
        buffers[waveform] // ALint value
    );

    alSourcePlay(sid); // ALuint sid
    start = clock();
}

void audioStop()
{
    alSourceStop(sid); // ALuint sid
}

void audioUpdate()
{
    if (length > 0 && clock() - start >= length)
    {
        audioStop();
    }

    if (decay != 0 && decay < 1)
    {
        alSourcef(
            sid,                // ALuint sid
            AL_GAIN,            // ALenum param
            gain *= decay // ALfloat value
        );
    }

    if (sweep != 0)
    {
        pitch *= sweep;

        if (pitchTarget != 0)
        {
            if (sweep > 1 && pitch >= pitchTarget ||
                sweep < 1 && pitch <= pitchTarget)
            {
                audioStop();
            }
        }

        alSourcef(
            sid,      // ALuint sid
            AL_PITCH, // ALenum param
            pitch     // ALfloat value
        );
    }

#if _DEBUG
    printf("%u\n", clock());
#endif
}
