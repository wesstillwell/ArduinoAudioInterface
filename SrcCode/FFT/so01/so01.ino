#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

#define PRED 11
#define PBLUE 10
#define PGREEN 9

//spektrum weights
#define wBASS 3
#define wMID 1
#define wTREBLE 1

arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

unsigned int bass = 0;
unsigned int mid = 0;
unsigned int treble= 0;
 
void setup() {
    Serial.begin(115200);
    pinMode(PRED, OUTPUT); 
    pinMode(PBLUE, OUTPUT); 
    pinMode(PGREEN, OUTPUT); 
    
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}
 
void loop() {
   LightOutput();
    /*SAMPLING*/
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overfbasss after around 70 minutes!
     
        vReal[i] = analogRead(0);
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
    //Serial.println(peak);     //Print out what frequency is the most dominant.
 
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
         
        //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
    }
    
    BinProcess( vReal, peak);

    
    //delay(1000);  //Repeat the process every second OR:
    //while(1);       //Run code once
}

void BinProcess(double bin[], double peak) //processes the bins
{
  
  ///3 colours = 3 frequency ranges
  ///basspass kills everything abovee 840Hz
  ///fg = 846Hz, ~840
  ///ex = 280Hz per category, yet bass should have more visual feedback
  ///bass : 0 - 350Hz
  ///mid : 351Hz - 680Hz
  ///treble: 681Hz - 800Hz

  //for test purposes im just gonna divide the band by 3, so 128/2 / 3

  //counters
  bass = 0;
  mid = 0;
  treble= 0;

  double divisor;
  divisor = SAMPLES;
  for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
         
        if(i < 21)
        {
          bass = bass + bin[i];
        }
        else if(i > 43)
        {
          treble = treble + bin[i];
        }
        else 
        {
          mid = mid + bin[i];
        }
        
    }

    //get the average of bass mid and treble then convert it to about a 255 range
    bass = (bass / 20)/ wBASS;
    mid = (mid / 22) / wMID;
    treble = (treble / 22)/wTREBLE ;

  bass = Cutoff(bass);
  mid = Cutoff(mid);
  treble = Cutoff(treble);
    
  Serial.print(treble); 
  Serial.print("h "); 
  Serial.print(mid); 
  Serial.print("m "); 
  Serial.print(bass); 
  Serial.println("l ");
  ///basss will be blue
  ///mids will be green
  ///trebles will be red
  

  
}

void LightOutput()
{
    analogWrite(PRED, treble);
    analogWrite(PGREEN, mid);
    analogWrite(PBLUE, bass);
}

int Cutoff(int value)
{
    if(value > 255)
    { 
      return 255;
    }

    return value;
      
}
