#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC


//spektrum weights
#define wBASS 3
#define wMID 0.5
#define wTREBLE 1
#define peakOffset 50


//pins for led control outputs
int PRED  = 10;
int PBLUE  = 9;
int PGREEN  = 11;

double currentPeak = 0;

arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;

unsigned int counter;
 
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
   
   Sample();
   BinProcess( vReal);
   LightOutput();
   if(microseconds % 30000)
   {
    colourSwitch();
    Serial.println("1");
   }
   else{
    Serial.println("0");}
}

void Sample()
{
  /*SAMPLING*/
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overfbasss after around 70 minutes!
     
        vReal[i] = analogRead(0);
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
          LightOutput();
        }
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  currentPeak = peak;
    /*PRINT RESULTS*/
    //Serial.println(peak);     //Print out what frequency is the most dominant.
 
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
         
        //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
    }
    
    

    
    //delay(1000);  //Repeat the process every second OR:
    //while(1);       //Run code once
  }

void BinProcess(double bin[]) //processes the bins
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
        if(i > 30)
        {
          treble = treble + bin[i];
        }
        if(i > 17 && i < 35) 
        {
          mid = mid + bin[i];
        }
        /*else
        {
          bass = bass + (bin[i] / 4);
          mid = mid + (bin[i] / 4);
          treble = treble + (bin[i] / 4);
        }*/
        
    }
      if(currentPeak < 200)
        {
          bass = bass + peakOffset;
        }
        else
        {
          mid = mid + peakOffset;
        }
    

    //get the average of bass mid and treble then convert it to about a 255 range
    bass = (bass / 20)/ wBASS;
    mid = (mid / 22) / wMID;
    treble = (treble / 27)/wTREBLE ;

  bass = Cutoff(bass);
  mid = Cutoff(mid);
  treble = Cutoff(treble);
    
  Serial.print(treble); 
  Serial.print(" "); 
  Serial.print(mid); 
  Serial.print(" "); 
  Serial.print(bass); 
  Serial.println("  ");
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

void colourSwitch()
{
    
  int tempPRED  = PRED;
  int tempPBLUE  = PBLUE;
  int tempPGREEN  = PGREEN;

  PBLUE = tempPGREEN;
  PRED = tempPBLUE;
  PGREEN = tempPRED;
}

int Cutoff(int value)
{
    if(value > 255)
    { 
      return 255;
    }

    return value;
      
}
