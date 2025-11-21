#include "daisy_seed.h"
#include "daisysp.h"
#include "taper.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

// constants
constexpr float Filter1Min = 350.0f;
constexpr float Filter1Max = 2200.0f;
constexpr float QMin = 0.45f;
constexpr float QMax = 1.0f;
constexpr float ChangeThreshold = 0.02f;

enum TaperType { LINEAR, DEAD_ZONE, LOG, INVERSE_LOG, LOG_DEAD_ZONE };

// local variables
DaisySeed hw;
Svf filter1;
Taper taper;
float filter1_freq = fmap(Filter1Min, Filter1Max, 0.5f);
float pot1, pot2;
bool filter_mode; // false - low pass, true - band pass
TaperType taper_type;

// prototypes
void ProcessAnalogControls();
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size);
void BlinkLed(int num = 3, int delay = 200);
void Init();
float Taper(float x);

float Taper(float x)
{
  switch (taper_type) {
    case DEAD_ZONE:
      return taper.DeadZone(x);
    case LOG:
      return taper.Log(x);
    case INVERSE_LOG:
      return taper.InvLog(x);
    case LOG_DEAD_ZONE:
      return taper.LogDeadZone(x);
    case LINEAR:
    default:
      return x;
  }
}

void ProcessAnalogControls()
{
  float pot1v = hw.adc.GetFloat(0);
  float pot2v = hw.adc.GetFloat(1);

  if (fabs(pot1v - pot1) > ChangeThreshold) {
    filter1_freq = fmap(Filter1Min, Filter1Max, Taper(pot1v));
    filter1.SetFreq(filter1_freq);
    pot1 = pot1v;
  }

  if (fabs(pot2v - pot2) > ChangeThreshold) {
    float q = fmap(QMin, QMax, pot2v);
    filter1.SetRes(q);
    pot2 = pot2v;
  }
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
  ProcessAnalogControls();

  for (size_t i = 0; i < size; i++) {
    float input = in[0][i];
    filter1.Process(input);
    float filter1_out = filter_mode ? filter1.Band() : filter1.Low();
    out[0][i] = out[1][i] = filter1_out;
  }
}

void BlinkLed(int num, int delay)
{
  for (int i = 0; i < num; i++) {
    hw.SetLed(true);
    System::Delay(delay);
    hw.SetLed(false);
    System::Delay(delay);
  }
}

void Init()
{
  const int num_adc_channels = 2;

  taper.Init();
  
  hw.Init();
  hw.SetAudioBlockSize(4);

  AdcChannelConfig my_adc_config[num_adc_channels];
  my_adc_config[0].InitSingle(A2);
  my_adc_config[1].InitSingle(A1);
  hw.adc.Init(my_adc_config, num_adc_channels);
  hw.adc.Start();

  const float sample_rate = hw.AudioSampleRate();

  filter1.Init(sample_rate);
  filter1.SetFreq(filter1_freq);
  filter1.SetRes(0.7f);
  filter1.SetDrive(0.75f);

  hw.StartAudio(AudioCallback);
}

int main(void)
{
  pot1 = pot2 = 0;
  filter_mode = false; // false - low pass, true - band pass
  taper_type = LINEAR;

  Init();
  BlinkLed();

  while (true) {
  }
}
