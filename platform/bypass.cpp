#include <string.h>
#include "daisy_seed.h"

using namespace daisy;

DaisySeed hw;

static void Callback(AudioHandle::InterleavingInputBuffer  in,
                     AudioHandle::InterleavingOutputBuffer out,
                     size_t                                size)
{
    memcpy(out, in, size * sizeof(float));
}

int main(void)
{
    bool led_state = false;
    
    hw.Init();
    hw.SetAudioBlockSize(4);
    hw.StartAudio(Callback);
    while(1) {
            // Set the onboard LED
        hw.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 500ms
        System::Delay(500);
    }
}
