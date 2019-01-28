#include <iostream>
#include <thread>
#include <conio.h>

#include "audio.h"

#define LOOP_STEP (10000000)

enum
{
    KEY_MODE_WAVEFORM,
    KEY_MODE_WAVEFREQ
};

static int key_mode(KEY_MODE_WAVEFORM);

void loop();

class Timer
{
public:
    template <class callable, class... arguments>
    Timer(int after, bool async, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        if (async)
        {
            std::thread([after, task]()
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }).detach();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }
};

int main()
{
    if (!audioInit())
    {
        std::cerr << "function audioInit failed!" << std::endl;
        return 0;
    }

    //audioLength(1000);
    audioDecay(.925f);

    /*audioPitchTarget(.5f);
    audioSweep(.99f);*/

    /*audioPitchTarget(2);
    audioSweep(1.1f);*/

    /*audioPitchTarget(.25f);
    audioSweep(.95f);*/

    //audioFreq(440 * 2);
    audioFreq(440 / 2);

    /*std::thread th(loop);
    th.join();*/

    Timer timer(1000, true, &loop);

    int space_key_count(0);
    while (1)
    {
        int key = _getch();

#if _DEBUG
        std::cout << "key: " << key << std::endl;
#endif

        switch (key)
        {
        case 9:  // tab key
            key_mode = key_mode == KEY_MODE_WAVEFORM ? KEY_MODE_WAVEFREQ : KEY_MODE_WAVEFORM;
            break;
        case 27: // esc key
            goto exit;
            break;
        case 32: // space key
            if (space_key_count++ % 2 == 0)
                audioPlay();
            else
                audioStop();
            break;
        }

        if(key_mode == KEY_MODE_WAVEFORM)
        {
            if (key >= '1' && key <= '5')
            {
                audioWaveForm(key - '1');
                audioStop();
                audioPlay();

                space_key_count = 1;
            }
        }
        else
        {
            if (key >= '0' && key <= '9')
            {
                int k = key - '0';
                //audioWaveForm(AUDIO_WAVEFORM_PULSE_50);
                audioFreq(440 * powf(2, (1 + k / 12.f)));
                audioStop();
                audioPlay();

                space_key_count = 1;
            }
        }
    }

exit:
    return 0;
}

static int loop_count(0);
void loop()
{
    while (1)
    {
        if (++loop_count % LOOP_STEP == 0)
            audioUpdate();
    }
}
