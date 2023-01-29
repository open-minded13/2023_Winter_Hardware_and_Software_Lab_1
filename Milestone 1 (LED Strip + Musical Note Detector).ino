// Milestone 1 (LED Strip + Musical Note Detector)
// Name: Chia-Wei Chang
// Department: Master of Science in Technology Innovation, 
//             University of Washington

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    6
#define LED_COUNT 60

// Declare our NeoPixel strip object
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// For Musical Note Detector
int in[128];
int Mic_Pin = A2; // change as per Microphone pin
float f_peaks[5]; // top 5 frequencies peaks in descending order
byte NoteV[13] = {8, 23, 40, 57, 76, 96, 116, 
                  138, 162, 187, 213, 241, 255}; //data for note detection based on frequency
char note_recognition = 'n';
char previous_note = 'p';
int note_counter = 0;
int sleep_counter = 200;

// setup() function -- runs once at startup --------------------------------
void setup() {
  
  // For LED Strip
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  // For Musical Note Detector
  Serial.begin(250000);
}

// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  
  // For Musical Note Detector
  note_recognition = Tone_det();
  Twinkle_Little_Star(note_recognition, previous_note);
  previous_note = note_recognition;

  // Piano_Key_Indicator(0, strip.Color(5, 180, 175), 100);
  // Piano_Key_Indicator(4, strip.Color(5, 180, 175), 100);
  
  // Fill along the length of the strip in various colors...
  // colorWipe(strip.Color(255,   0,   0), 50); // Red
  // colorWipe(strip.Color(  0, 255,   0), 50); // Green
  // colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  // theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  // theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  // theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  // rainbow(10);             // Flowing rainbow cycle along the whole strip
  // theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}

void Twinkle_Little_Star (char note_recognition, char previous_note) {
  
  if (previous_note == note_recognition) {
    note_counter ++;
    // Serial.print("LOG: ");
    // Serial.println(previous_note);
  }
  else {
    note_counter = 0;
  }
  
  if (note_counter >= 2) {
    switch (note_recognition) {
      case 'N':
        sleep_counter++;
        Piano_Key_Indicators(strip.Color(79, 184, 72), 1);          
        break;
      case 'C':
        Piano_Key_Indicator(25, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
      case 'D':
        Piano_Key_Indicator(27, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
      case 'E':
        Piano_Key_Indicator(29, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
      case 'F':
        Piano_Key_Indicator(30, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
      case 'G':
        Piano_Key_Indicator(32, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
      case 'A':
        Piano_Key_Indicator(34, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
      case 'B':
        Piano_Key_Indicator(36, strip.Color(5, 180, 175), 5);
        sleep_counter = 0;
        break;
    }
  }  
}

// Some functions of our own for creating animated effects -----------------
void Piano_Key_Indicator(int LED_ID, uint32_t color, int wait) {
  strip.clear();
  strip.setPixelColor(LED_ID, color);
  strip.show();
  delay(wait);
}

void Piano_Key_Indicators(uint32_t color, int wait) {
  
  if (sleep_counter >= 10 && sleep_counter < 100) {
    strip.clear();
    strip.setPixelColor(15, color);
    strip.show(); 
  }
  else if (sleep_counter >= 100 && sleep_counter <= 300) {
    for (int i = 15; i <= 37; i++) {
      strip.setPixelColor(i, color);
      strip.show();
      delay(wait); 
    } 
  }
  else if (sleep_counter > 300) {
    theaterChaseRainbow(0.1); 
  }
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { //  For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

// -----------------------------Musical Note Detector------------------------------------- //
char Tone_det () { 
  
  long unsigned int a1, b, a2;
  float a;
  float sum1 = 0, sum2 = 0;
  float sampling;
  a1 = micros();
  
  for(int i = 0; i < 128; i++) {
    a = analogRead(Mic_Pin) - 500; // rough zero shift 
    // Serial.println(a);
    if (a < 200) {
      a = 0;
    }
    // Serial.println(a);
    // utilising time between two sample for windowing & amplitude calculation
    sum1 = sum1 + a;                                       // to average value
    sum2 = sum2 + a * a;                                   // to RMS value
    a = a * (sin(i * 3.14 / 128) * sin(i * 3.14 / 128));   // Hann window
    in[i] = 10 * a;                                        // scaling for float to int conversion
    delayMicroseconds(195);                                // based on operation frequency range
  }

  b = micros();
  sum1 = sum1 / 128;                 // Average amplitude
  sum2 = sqrt(sum2 / 128);           // RMS
  sampling = 128000000 / (b - a1);   // real time sampling frequency

  // for very low or no amplitude, this code won't start
  // it takes very small aplitude of sound to initiate for value sum2-sum1 > 3, 
  // change sum2-sum1 threshold based on requirement
  if (sum2 - sum1 > 50) {  
    
    FFT (128, sampling);        
    // EasyFFT based optimised  FFT code, 
    // this code updates f_peaks array with 5 most dominent frequency in descending order
    for (int i = 0; i < 12; i++) {
      in[i] = 0;
    }  // utilising in[] array for further calculation

    // below loop will convert frequency value to note 
    int j = 0, k = 0;
    for (int i = 0; i < 5; i++) {
      if (f_peaks[i] > 1040) {
        f_peaks[i] = 0;
      }
      if (f_peaks[i] >= 65.4 && f_peaks[i] <= 130.8) {
        f_peaks[i] = 255 * ((f_peaks[i] / 65.4) - 1);
      }
      if (f_peaks[i] >= 130.8  && f_peaks[i] <= 261.6) {
        f_peaks[i] = 255 * ((f_peaks[i] / 130.8) - 1);
      }
      if (f_peaks[i] >= 261.6  && f_peaks[i] <= 523.25){
        f_peaks[i] = 255 * ((f_peaks[i] / 261.6) - 1);
      }
      if (f_peaks[i] >= 523.25 && f_peaks[i] <= 1046)  {
        f_peaks[i] = 255 * ((f_peaks[i] / 523.25) - 1);
      }
      if (f_peaks[i] >= 1046 && f_peaks[i] <= 2093)    {
        f_peaks[i] = 255 * ((f_peaks[i] / 1046) - 1);
      }
      if (f_peaks[i] > 255) {
        f_peaks[i] = 254;
      }
      
      j = 1, k = 0;
         
      while (j == 1) {
        if(f_peaks[i] < NoteV[k]) {
          f_peaks[i] = k;
          j = 0;
        }
        k++;  // a note with max peaks (harmonic) with aplitude priority is selected
        if(k > 15) { 
          j = 0;
        }
      }
      
      if (f_peaks[i] == 12) {
        f_peaks[i] = 0;
      }
      
      k = f_peaks[i];
      in[k] = in[k] + (5 - i);
    }

    k = 0; j = 0;
    for (int i = 0; i < 12; i++) {
      if(k < in[i]) {
        k = in[i];
        j = i;
      }  // Max value detection
    }
    
    // Note print
    // if you need to use note value for some application, use of note number recomendded
    // where, 0 = c; 1 = c#, 2 = D; 3 = D#; .. 11 = B;      
    // a2 = micros(); // time check
    k = j;
    if (k == 0) {Serial.println('C'); return 'C';}
    if (k == 1) {Serial.print('C'); Serial.println('#');}
    if (k == 2) {Serial.println('D'); return 'D';}
    if (k == 3) {Serial.print('D'); Serial.println('#');}
    if (k == 4) {Serial.println('E'); return 'E';}
    if (k == 5) {Serial.println('F'); return 'F';}
    if (k == 6) {Serial.print('F'); Serial.println('#');}
    if (k == 7) {Serial.println('G'); return 'G';}
    if (k == 8) {Serial.print('G'); Serial.println('#');}
    if (k == 9) {Serial.println('A'); return 'A';}
    if (k == 10) {Serial.print('A'); Serial.println('#');}
    if (k == 11) {Serial.println('B'); return 'B';}
  }
  else {
    return 'N';
  }
}

// -----------------------------FFT Function---------------------------------------------- //
// Documentation on EasyFFT: https://www.instructables.com/member/abhilash_patel/instructables/
// EasyFFT code optimised for 128 sample size to reduce mamory consumtion
float FFT(byte N, float Frequency) {

  byte data[8] = {1, 2, 4, 8, 16, 32, 64, 128};
  int a, c1, f, o, x;
  a = N;  
                                 
  for (int i = 0; i < 8; i++) {  //calculating the levels
    if (data[i] <= a) { 
      o = i;
    } 
  }
  o = 7;

  byte in_ps[data[o]] = {};     //input for sequencing
  float out_r[data[o]] = {};   //real part of transform
  float out_im[data[o]] = {};  //imaginory part of transform
           
  x = 0;  
  for (int b = 0; b < o; b++) {                   // bit reversal
    c1 = data[b];
    f = data[o] / (c1 + c1);
    for (int j = 0; j < c1; j++){ 
      x = x + 1;
      in_ps[x] = in_ps[j] + f;
    }
  }
 
  for (int i = 0; i < data[o]; i++) {           // update input array as per bit reverse order
    if (in_ps[i] < a) {
      out_r[i] = in[in_ps[i]];
    }
    if (in_ps[i] > a) {
      out_r[i] = in[in_ps[i]-a];
    }      
  }

  int i10, i11, n1;
  float e, c, s, tr, ti;

  for (int i = 0; i < o; i++) {  //fft
    i10 = data[i];                 // overall values of sine cosine  
    i11 = data[o] / data[i+1];       // loop with similar sine cosine
    e = 6.283 / data[i+1];
    e = 0 - e;
    n1 = 0;

    for (int j = 0; j < i10; j++) {
      c = cos(e*j); 
      s = sin(e*j); 
      n1 = j;
          
      for (int k = 0; k < i11; k++) {
        tr =c * out_r[i10+n1] - s * out_im[i10+n1];
        ti =s * out_r[i10+n1] + c * out_im[i10+n1];
        out_r[n1+i10] = out_r[n1] -tr;
        out_r[n1] = out_r[n1] + tr;
        out_im[n1+i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;          
        n1 = n1 + i10 + i10;
      }       
    }
  }

  //---> here onward out_r contains amplitude and our_in conntains frequency (Hz)
  for (int i = 0; i < data[o-1]; i++) {              // getting amplitude from compex number  
    out_r[i] = sqrt((out_r[i]*out_r[i]) + (out_im[i]*out_im[i])); // to  increase the speed delete sqrt
    out_im[i] = (i*Frequency) / data[o];
    /*
      Serial.print(out_im[i],2); Serial.print("Hz");
      Serial.print("\t");                            // uncomment to print freuency bin    
      Serial.println(out_r[i]); 
      */
  }

  x = 0;       // peak detection
  for (int i = 1; i < data[o-1]-1; i++) {
    if(out_r[i]>out_r[i-1] && out_r[i]>out_r[i+1]) {
      in_ps[x]=i;    //in_ps array used for storage of peak number
      x=x+1;
    }    
  }

  s = 0;
  c = 0;
  for (int i = 0; i < x; i++) {            // re arraange as per magnitude
    for (int j = c; j < x; j++) {
      if(out_r[in_ps[i]]<out_r[in_ps[j]]) {
        s = in_ps[i];
        in_ps[i] = in_ps[j];
        in_ps[j] = s;
      }
    }
  c=c+1;
  }
    
  for(int i = 0; i < 5; i++) {    // updating f_peak array (global variable)with descending order
    f_peaks[i]=(out_im[in_ps[i]-1]*out_r[in_ps[i]-1]+out_im[in_ps[i]]*out_r[in_ps[i]]+out_im[in_ps[i]+1]*out_r[in_ps[i]+1])
    /(out_r[in_ps[i]-1]+out_r[in_ps[i]]+out_r[in_ps[i]+1]);
  }
}