#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

#define PRED 11
#define PBLUE 10
#define PGREEN 9
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];
 
void setup() {
    Serial.begin(115200);
 
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}
 
void loop() {
   
    /*SAMPLING*/
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!
     
        vReal[i] = analogRead(A0);
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
 
    /*PRINT RESULTS*/
    Serial.println(" ");
    Serial.println(peak);     //Print out what frequency is the most dominant.
    Serial.println(" ");
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
         
        //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
    }
    
    BinProcess( vReal);

    
    //delay(1000);  //Repeat the process every second OR:
    //while(1);       //Run code once
}

void BinProcess(double bin[]) //processes the bins
{
  ///3 colours = 3 frequency ranges
  ///lowpass kills everything abovee 840Hz
  ///fg = 846Hz, ~840
  ///ex = 280Hz per category, yet bass should have more visual feedback
  ///low : 0 - 350Hz
  ///mid : 351Hz - 680Hz
  ///high: 681Hz - 800Hz

  //counters
  int low;
  int mid;
  int high;

  double divisor;
  divisor = SAMPLES;

  for(int i=0; i<(SAMPLES/2); i++)
  {
    if(bin[i] < 351)
    {
      low++;
    }
    else if( bin[i] > 350 && bin[i] < 681)
    {
      mid++;
    }
    else if(bin[i] > 680)
    {
      high++;
    }
      
  }
  low = low  / divisor;
  mid = mid / divisor;
  high = high / divisor;
  Serial.println(high); 
  ///lows will be blue
  ///mids will be green
  ///highs will be red

  LightOutput(low, mid, high);
}

void LightOutput(int low, int mid, int high)
{
    analogWrite(PRED, high);
    analogWrite(PGREEN, mid);
    analogWrite(PBLUE, low);
}
