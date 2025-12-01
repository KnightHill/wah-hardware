#include <string.h>
#include "daisy_seed.h"
#include "daisysp.h"
#include "taper.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;
Svf filter1, filter2;
Taper taper;

// https://home.cc.umanitoba.ca/~krussll/phonetics/acoustic/formants.html
// http://www.geofex.com/Article_Folders/wahpedl/voicewah.htm

#define NUM_FORMANTS 11

//                              [i] 	    [ɪ] 	    [e] 	  [ɛ] 	     [æ] 	   [ɑ] 	    [ɔ]   	[o] 	  [ʊ]   	[u]   	[ʌ] 
const float F1[NUM_FORMANTS] = {280.0f,   370.0f, 405.0f,   600.0f,  860.0f,  830.0f, 560.0f, 430.0f, 400.0f, 330.0f, 680.0f};
const float F2[NUM_FORMANTS] = {2230.0f, 2090.0f, 2080.0f, 1930.0f, 1550.0f, 1170.0f, 820.0f, 980.0f, 1100.0f, 1260.0f, 1310.0f};

constexpr int vowel1_index = 5; // [ɑ]
constexpr int vowel2_index = 0; // [i] 

constexpr float QMin = 0.5f;
constexpr float QMax = 1.0f;
constexpr float ChangeThreshold = 0.02f;

float Filter1Min, Filter1Max;
float Filter2Min, Filter2Max;
bool Filter1Dir, Filter2Dir;  // false - high to low, true, low to high

float pot1, pot2;

void BlinkLed(int num = 4, int delay = 180)
{
  for (int i = 0; i < num; i++) {
    hw.SetLed(true);
    System::Delay(delay);
    hw.SetLed(false);
    System::Delay(delay);
  }
}

void ProcessAnalogControls()
{
  const float pot1v = taper.Linear(hw.adc.GetFloat(0));
  const float pot2v = hw.adc.GetFloat(1);

  if (fabs(pot1v - pot1) > ChangeThreshold) {
    const float filter1_freq = Filter1Dir ? fmap(pot1v, Filter1Min, Filter1Max) : fmap(1.0f - pot1v, Filter1Min, Filter1Max);
    const float filter2_freq = Filter2Dir ? fmap(pot1v, Filter2Min, Filter2Max) : fmap(1.0f - pot2v, Filter2Min, Filter2Max);
    filter1.SetFreq(filter1_freq);
    filter2.SetFreq(filter2_freq);
    pot1 = pot1v;
  }

  if (fabs(pot2v - pot2) > ChangeThreshold) {
    float q = fmap(pot2v, QMin, QMax);
    filter1.SetRes(q);
    filter2.SetRes(q);
    pot2 = pot2v;
  }
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
  ProcessAnalogControls();

  for (size_t i = 0; i < size; i++) {
    float input = in[0][i];

    filter1.Process(input);
    filter2.Process(input);

    float filter1_out = filter1.Band();
    // filter2.Process(filter1_out);
    float filter2_out = filter2.Band();

    out[0][i] = out[1][i] = filter1_out + filter2_out;
  }
}

void CalcFiltersMinMax() {
  // filter direction: false - high to low, true - low to high
  Filter1Dir = F1[vowel1_index] < F2[vowel1_index];
  Filter2Dir = F1[vowel2_index] < F2[vowel2_index];

  Filter1Min = F1[vowel1_index]; // 830 <-
  Filter1Max = F2[vowel1_index]; // 1170 ->
  Filter2Min = F1[vowel2_index]; // 280
  Filter2Max = F2[vowel2_index]; // 2230
}

void Init()
{
  const int num_adc_channels = 2;

  taper.Init();
  CalcFiltersMinMax();
  float filter1_freq = fmap(.5f, Filter1Min, Filter1Max);
  float filter2_freq = fmap(.5f, Filter2Min, Filter2Max);

  hw.Init();
  hw.SetAudioBlockSize(4);

  BlinkLed();

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
  Init();

  while (true) {
  }
}
