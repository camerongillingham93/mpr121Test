#include "daisy_seed.h"
#include "daisysp.h"
#include "mpr121.h"

using namespace daisy;
using namespace daisysp;

// Define the frequencies for twelve keys chromatically
double scale[] = {
    261.63, // C
    277.18, // C#
    293.66, // D
    311.13, // D#
    329.63, // E
    349.23, // F
    369.99, // F#
    392.00, // G
    415.30, // G#
    440.00, // A
    466.16, // A#
    493.88  // B
};

DaisySeed hw;
Mpr121I2C mpr121;
Oscillator osc;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}
}

int main(void)
{
	hw.Init();
    hw.StartLog();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

    //How many samples we'll output per second
    float samplerate = hw.AudioSampleRate();

	// Initialize MPR121 with I2C configuration
    Mpr121I2C::Config mprConfig;
    mprConfig.transport_config.periph = I2CHandle::Config::Peripheral::I2C_1;
    mprConfig.transport_config.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    mprConfig.transport_config.scl = Pin(PORTB,6);  // Replace with your SCL pin
    mprConfig.transport_config.sda = Pin(PORTB,7);  // Replace with your SDA pin

	// Set additional MPR121 configuration if needed
    mprConfig.touch_threshold = 12;
    mprConfig.release_threshold = 6;

    //Set up oscillator
    osc.Init(samplerate);
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetAmp(1.f);
    osc.SetFreq(440);

	if(mpr121.Init(mprConfig) != Mpr121I2C::OK)
    {
        // Handle initialization error
        while(1)
        {
            // Error handling code (blinking LED, etc.)
        }
    }
	// Setup to print 
	hw.StartLog();

	while(1) 
	{
		// Read touch status
        uint16_t touched = mpr121.Touched();

        // Process the touch status as needed
        for(int i = 0; i < 12; i++)
        {
            if(touched & (1 << i))
            {
                // Channel i is touched, print the channel number
                hw.PrintLine("Channel %d touched", i);
            }
        }

        switch (touched)
        {
        case 0:
            osc.SetFreq(scale[0]);
            break;
        
        case 1:
            osc.SetFreq(scale[1]);
            break;
        
        default:
            break;
        }
        // Add necessary delay or other code here...
    }
}
