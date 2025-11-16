#include <string.h>
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


DaisySeed hw;

constexpr float Filter1Min = 200.0f;
constexpr float Filter1Max = 1600.0f;
constexpr float Filter2Min = 450.0f;
constexpr float Filter2Max = 3500.0f;
constexpr float QMin = 0.5f;
constexpr float QMax = 1.2f;

inline float range(float min, float max, float value) { return min + (value * (max - min)); }

bool filter_mode; // false - low pass, true - babd pass
bool wah_mode;    // false - single filter, true - two (formant) filters
                  
Svf filter1, filter2;

float filter1_freq = range(Filter1Min, Filter1Max, 0.5f);
float filter2_freq = range(Filter2Min, Filter2Max, 0.5f);
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
        filter2.Process(input);

        // TODO: Also try filter2.Process(filter1_out);
        float filter1_out = filter_mode ? filter1.Band() : filter1.Low();
        float filter2_out = filter2.Band();

        if(wah_mode)
          out[0][i] = out[1][i] = filter1_out + filter2_out;
        else 
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

    filter2.Init(sample_rate);
    filter2.SetFreq(filter2_freq);
    filter2.SetRes(0.7f);
    filter2.SetDrive(0.75f);

    hw.StartAudio(AudioCallback);
}

int main(void)
{
    pot1 = pot2 = 0;
    filter_mode = true;     // false - low pass, true - babd pass
    wah_mode = false;       // false - single filter, true - two (formant) filters
    
    init();

    while(1) {
        hw.SetLed(pot1 > 0.5f || pot2 > 0.5f);
        System::Delay(5);
    }
}
