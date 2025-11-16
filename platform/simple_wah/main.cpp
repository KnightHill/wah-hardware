#include <string.h>
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


DaisySeed hw;

constexpr float Filter1Min = 200.0f;
constexpr float Filter1Max = 1600.0f;
constexpr float QMin = 0.5f;
constexpr float QMax = 1.2f;

inline float range(float min, float max, float value) { return min + (value * (max - min)); }

bool filter_mode; // false - low pass, true - babd pass
                  
Svf filter1;

float filter1_freq = range(Filter1Min, Filter1Max, 0.5f);
float pot1, pot2;

void ProcessAnalogControls() {
    float pot1v = hw.adc.GetFloat(0);
    float pot2v = hw.adc.GetFloat(1);

    if(fabs(pot1v - pot1) > 0.05f) {
        // 350 Hz - 2.2 kHz
        filter1_freq = range(Filter1Min, Filter1Max, pot1v);
        filter1.SetFreq(filter1_freq);
        pot1 = pot1v;
    }
 
    if(fabs(pot2v - pot2) > 0.05f) {
        float q = range(QMin, QMax, pot2v);
        filter1.SetRes(q);
        pot2 = pot2v;
    }    
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    ProcessAnalogControls();
    
    for(size_t i = 0; i < size; i++)
    {
        float input = in[0][i];   
        filter1.Process(input);
        float filter1_out = filter_mode ? filter1.Band() : filter1.Low();
        out[0][i] = out[1][i] = filter1_out; 
    }
}

void init() 
{
    const int num_adc_channels = 2;

    hw.Init();
    hw.SetAudioBlockSize(4);

    AdcChannelConfig my_adc_config[num_adc_channels];
    my_adc_config[0].InitSingle(A2);
    my_adc_config[1].InitSingle(A1);
    hw.adc.Init(my_adc_config, num_adc_channels);
    hw.adc.Start();

    float sample_rate = hw.AudioSampleRate();

    filter1.Init(sample_rate);
    filter1.SetFreq(filter1_freq);
    filter1.SetRes(0.7f);
    filter1.SetDrive(0.75f);

    hw.StartAudio(AudioCallback);
}

int main(void)
{
    pot1 = pot2 = 0;
    filter_mode = true;     // false - low pass, true - babd pass
    
    init();

    while(1) {
        hw.SetLed(pot1 > 0.5f || pot2 > 0.5f);
        System::Delay(5);
    }
}
