#include <string.h>
#include "daisy_seed.h"

using namespace daisy;
using namespace daisy::seed;

DaisySeed hw;

float pot1, pot2;

void ProcessAnalogControls() {
    pot1 = hw.adc.GetFloat(0);
    //int pot2 = hw.adc.Get(1);
}

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    //hw.ProcessDigitalControls();
    ProcessAnalogControls();

    // Audio Loop
    for(size_t i = 0; i < size; i += 2)
    {
        out[i]     = pot1 * in[i];
        out[i + 1] = pot1 * in[i+1];
    }
}

void init() 
{
    hw.Init();

    // Create an ADC Channel Config object
    AdcChannelConfig adc_config;

    // Set up the ADC config with a connection to pin A2
    adc_config.InitSingle(A2);
    //adc_config.InitSingle(A1);
    hw.adc.Init(&adc_config, 1);
    hw.adc.Start();
/*
    // Create an array of two AdcChannelConfig objects
    const int num_adc_channels = 2;
    AdcChannelConfig my_adc_config[num_adc_channels];
    my_adc_config[0].InitSingle(A2);
    my_adc_config[1].InitSingle(A1);
    hw.adc.Init(my_adc_config, num_adc_channels);
*/
    hw.SetAudioBlockSize(4);
    hw.StartAudio(AudioCallback);
}

int main(void)
{
    pot1 = pot2 = 0;
    
    init();

    while(1) {
        hw.SetLed(pot1 > 0.5f);
        System::Delay(5);
    }
}
