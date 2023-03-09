// Milestone 2.5
// Name: Chia-Wei Chang
// Department: Master of Science in Technology Innovation,
//             University of Washington

#include <PianEase.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
// #include <esp_sleep.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// Required for 16 MHz Adafruit Trinket.
// #ifdef __AVR__
// #include <avr/power.h> 
// #endif

#define MIC_PIN A0
#define LED_PIN D2
#define PIR_PIN D6
#define LED_COUNT 38
#define MAX_IDLE_TIME_COUNT 600

// Declare the LED Strip Object
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// For Musical Note Detector
int in[128];
// Top 5 frequencies peaks in descending order.
float f_peaks[5];                                                            
// Data for note detection based on frequency.
byte NoteV[13] = {8, 23, 40, 57, 76, 96, 116, 138, 162, 187, 213, 241, 255}; 

// For Main Function
char note_recognition = 'N';
char previous_note_recognition = 'N';
int same_note_counter = 0;
uint32_t default_color = strip.Color(5, 180, 175);
uint32_t current_note_color = strip.Color(255, 0, 0);
uint32_t double_press_color = strip.Color(255, 0, 100);
uint32_t next_different_note_color = strip.Color(182, 71, 2);

// For Sleep Mode Function
int sleep_counter = -1;
bool esp8266_is_sleeping = false;
bool led_strip_is_sleeping = false;
bool motion_detected = false;

// For ESP8266 Wifi Module
const char *ssid = "WEICLAIRE";
const char *password = "WEI888888";
AsyncWebServer server(80);

/* ---------------------------- Song Library  -------------------------------------------- */
int progress_indicator = 0;
// Define the sheet music and duration arrays
char piano_sheet[] = "C4C4G4G4A4A4G4F4F4E4E4D4D4C4G4G4F4F4E4E4D4G4G4F4F4E4E4D4C4C4G4G4A4A4G4F4F4E4E4D4D4C4";
int note_durations[] = {400, 400, 400, 400, 400, 400, 800,  // Twinkle, twinkle, little star,
                        400, 400, 400, 400, 400, 400, 800,  // How I wonder what you are.
                        400, 400, 400, 400, 400, 400, 800,  // Up above the world so high,
                        400, 400, 400, 400, 400, 400, 800,  // Like a diamond in the sky.
                        400, 400, 400, 400, 400, 400, 800,  // Twinkle, twinkle, little star,
                        400, 400, 400, 400, 400, 400, 800}; // How I wonder what you are.
// Twinkle Little Star: "C4C4G4G4A4A4G4F4F4E4E4D4D4C4G4G4F4F4E4E4D4G4G4F4F4E4E4D4C4C4G4G4A4A4G4F4F4E4E4D4D4C4"
// Peppa Pig Theme Song: "G4E4C4D4G3G3B3D4F4E4C4G4E4C4D4G3G3B3D4F4E4C4G4E4C4D4G3G3B3D4F4E4C4E4E4G4G4E4C4D4G3G3B3D4F4E4C4E4E4D4G4E4C4D4G3G3B3D4F4E4C4G4G4C5"

/* ---------------------------- Setup Function  ------------------------------------------ */
// Run once at startup.
void setup() {

  Serial.begin(9600);

  // Setup the ESP8266 Wifi Module.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("ESP8266 is trying to connect to: ");
    Serial.println(ssid);
  }
  Serial.print("ESP8266 has successfully connected to: ");
  Serial.println(ssid);
  Serial.print("The IP Address of ESP8266 is: ");
  Serial.println(WiFi.localIP());
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
 
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it): 
  // #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  //   clock_prescale_set(clock_div_1);
  // #endif
  // END of Trinket-specific code.
  // Initialize NeoPixel strip object (REQUIRED).
  
  // Setup the LED Strip.
  pinMode(LED_PIN, OUTPUT);
  strip.begin();           
  strip.show();            
  
  // Set the LED Brightness to about 50 (max = 255).
  strip.setBrightness(100); 
  for (int i = 0; i <= 38; i++) {
    strip.setPixelColor(i, strip.Color(5, 180, 175));
    strip.show();
    led_strip_is_sleeping = false;
  }

  // Setup HC-SR501 PIR Motion Sensors.
  // Set the pin mode to input 0x00.
  pinMode(PIR_PIN, INPUT); 
}

// ---------------------------- Loop Function  ------------------------------------------- //
// Run repeatedly as long as board is on.
void loop() {
  note_recognition = Tone_det();
  Main_Function(note_recognition, previous_note_recognition);
  previous_note_recognition = note_recognition;  
  // The IR human presence sensor debugger.
  // if ((digitalRead(PIR_PIN))) {
  //   Serial.println("O");
  // }
  // else {
  //   Serial.println("X");
  // }
}

// ---------------------------- Main Function  ------------------------------------------- //
// Detect notes played and display associated lights.
void Main_Function(char note_recognition, char previous_note) {
  
  // Wake up the LED strip when the user presses the piano key.
  if (led_strip_is_sleeping == true && note_recognition != 'N') {
    sleep_counter = -1;
    progress_indicator = 0;
    Sleep_Mode();
  }

  // Start teaching users to play the piano.
  if (led_strip_is_sleeping == false && progress_indicator == 0) {
    
    char musical_note = piano_sheet[progress_indicator];
    char pitch = piano_sheet[progress_indicator+1];
    int LED_ID = Note_to_LED_ID_Map(musical_note, pitch);
    Piano_Key_Indicator(LED_ID, current_note_color, 5);
    
    if (progress_indicator+3 < strlen(piano_sheet)) {
      musical_note = piano_sheet[progress_indicator+2];
      pitch = piano_sheet[progress_indicator+3];
      LED_ID = Note_to_LED_ID_Map(musical_note, pitch);
      Piano_Key_Indicator(LED_ID, next_different_note_color, 5);
    }
  }

  // Double check that the correct note is detected.
  if (previous_note_recognition == note_recognition) {
    same_note_counter++;
  }
  else {
    same_note_counter = 0;
  }

  if (same_note_counter >= 1) {
    
    // Go to the Sleep_Mode function to check whether to make the LED strip and ESP8266 go to sleep.
    if (note_recognition == 'N') {
      Sleep_Mode();
      return;
    }
    // When the user presses the correct key and the song has not finished.
    else if (note_recognition == piano_sheet[progress_indicator] && progress_indicator < strlen(piano_sheet)) {
      
      // If the LED strip is sleeping, wake it up.
      if (led_strip_is_sleeping) {
        sleep_counter = -1;
        Sleep_Mode();
      } 
      else {
        sleep_counter = 0;
      }

      // When the next note to be pressed is the same as the current note.
      if (piano_sheet[progress_indicator] == piano_sheet[progress_indicator+2]) {
        
        // The pressing duration of the note, according to the musical sheet.
        delay(note_durations[progress_indicator/2]);
        
        // Make the progress indicator point to the next in the sheet. 
        progress_indicator = progress_indicator+2;

        // Change the color of the note's light to let the user know that they should press the same key again.
        char musical_note = piano_sheet[progress_indicator];
        char pitch = piano_sheet[progress_indicator+1];
        int LED_ID = Note_to_LED_ID_Map(musical_note, pitch);
        Piano_Key_Indicator(LED_ID, double_press_color, 5);
        
        // Light up the different note to be played next.
        if (progress_indicator+3 < strlen(piano_sheet)) {
          musical_note = piano_sheet[progress_indicator+2];
          pitch = piano_sheet[progress_indicator+3];
          LED_ID = Note_to_LED_ID_Map(musical_note, pitch); 
          Piano_Key_Indicator(LED_ID, next_different_note_color, 5);
        }
      }
      // When the next note to be pressed is not same as the current note.
      else {

        // The pressing duration of the note, according to the musical sheet.
        delay(note_durations[progress_indicator/2]);

        // Change the color of the note's light to the default color 
        // to indicate to users they press the correct key.
        char musical_note = piano_sheet[progress_indicator];
        char pitch = piano_sheet[progress_indicator+1];
        int LED_ID = Note_to_LED_ID_Map(musical_note, pitch);        
        Piano_Key_Indicator(LED_ID, default_color, 5);
        
        // Make the progress indicator point to the next in the sheet. 
        progress_indicator = progress_indicator + 2;

        // Light up the different note to be played now.
        musical_note = piano_sheet[progress_indicator];
        pitch = piano_sheet[progress_indicator+1];
        LED_ID = Note_to_LED_ID_Map(musical_note, pitch);        
        Piano_Key_Indicator(LED_ID, current_note_color, 5);

        // Light up the different note to be played next.
        if (progress_indicator+2 < strlen(piano_sheet)) {
          if (piano_sheet[progress_indicator] == piano_sheet[progress_indicator+2]) {
            musical_note = piano_sheet[progress_indicator+4];
            pitch = piano_sheet[progress_indicator+5];
            LED_ID = Note_to_LED_ID_Map(musical_note, pitch);                
            Piano_Key_Indicator(LED_ID, next_different_note_color, 5);
          }
          else {
            musical_note = piano_sheet[progress_indicator+2];
            pitch = piano_sheet[progress_indicator+3];
            LED_ID = Note_to_LED_ID_Map(musical_note, pitch);    
            Piano_Key_Indicator(LED_ID, next_different_note_color, 5);
          }
        }
      }
    }
    // When the user presses any key to wake up the LED strip.
    else {
      // If the LED strip is sleeping, wake it up.
      if (led_strip_is_sleeping) {
        sleep_counter = -1;
        Sleep_Mode();
      }
      else {
        sleep_counter = 0;
      }
    }
  }
  
  // After the user finishes the song, the LED strip starts the rainbow show. 
  if (progress_indicator == strlen(piano_sheet)) {
    TheaterChaseRainbow(50);
    delay(1000);
    progress_indicator = 0;
    sleep_counter = -1;
  }
}

// ---------------------------- Sleep Mode Function -------------------------------------- //
void Sleep_Mode() {
  
  if (sleep_counter == MAX_IDLE_TIME_COUNT) {
    
    // Trun off the LED strip.
    if (led_strip_is_sleeping == false) {
      strip.clear();
      for (int i = 0; i <= 38; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
        strip.show();
      }
      led_strip_is_sleeping = true;
      Serial.print("Log: The LED strip is sleeping.\n");
    }
    
    // Check whether to make ESP8266 go to sleep.
    if (digitalRead(PIR_PIN) && esp8266_is_sleeping) {
      WiFi.forceSleepWake();
      while (WiFi.status() != WL_CONNECTED) {
        delay(2000);
        Serial.print("ESP8266 is trying to reconnect to: ");
        Serial.println(ssid);
      }
      Serial.print("Log: The ESP8266 has woken up (WIFI has reconnected).\n");
      esp8266_is_sleeping = false;
      sleep_counter = -1;
    } 
    else {
      // No motion detected, ESP8266 go to sleep.
      if (esp8266_is_sleeping == false) {
        motion_detected = false;
        esp8266_is_sleeping = true;
        Serial.print("Log: The ESP8266 is sleeping.\n");
        // Put ESP8266 into light sleep mode.
        WiFi.forceSleepBegin();
      }
    }
  }
  
  // Trun on the LED strip.
  if (sleep_counter == -1) {  
    for (int i = 0; i <= 38; i++) {
      strip.setPixelColor(i, strip.Color(5, 180, 175));
      strip.show();
      if (led_strip_is_sleeping == true) {
        Serial.print("Log: The LED strip has woken up.\n");
      }
      led_strip_is_sleeping = false;
    }
  }

  // The device is idle. At this time, the idle time is 
  // automatically calculated by the sleep counter. 
  if (sleep_counter < 300) {
    sleep_counter++;
  }
}

// ---------------------------- Note to LED Map  ----------------------------------------- //
// Map each musical note to its associated LED indicator.
int Note_to_LED_ID_Map(char musical_note, char pitch) {
  
  switch (musical_note) {
  case 'C':
    if (pitch == '4') {
      return 25;
    }
    else if (pitch == '5') {
      return 37;
    }
  case 'D':
    if (pitch == '4') {
      return 27;
    }
  case 'E':
    if (pitch == '4') {
      return 29;
    }
  case 'F':
    if (pitch == '4') {
      return 30;
    }
  case 'G':
    if (pitch == '3') {
      return 21;
    }
    else if (pitch == '4') {
      return 32;
    }
  case 'A':
    if (pitch == '3') {
      return 23;
    }
    else if (pitch == '4') {
      return 34;
    }
  case 'B':
    if (pitch == '3') {
      return 24;
    }
    else if (pitch == '4') {
      return 36;
    }
  }
  return -1;
}

// ---------------------------- LED Strip: Piano Key Indicator --------------------------- //
void Piano_Key_Indicator(int LED_ID, uint32_t color, int wait) {
  
  // strip.clear();
  strip.setPixelColor(LED_ID, color);
  strip.show();
  delay(wait);
}

// ---------------------------- LED Strip: Rainbow Chasing Show -------------------------- //
// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void TheaterChaseRainbow(int wait) {
  
  // First pixel starts at red (hue 0)
  int firstPixelHue = 0; 
  
  // Repeat 30 times...
  for (int a = 0; a < 30; a++) { 
    
    //  'b' counts from 0 to 2...
    for (int b = 0; b < 3; b++) {                
      
      // Set all pixels in RAM to 0 (off)
      strip.clear(); 
      
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        
        // hue -> RGB
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); 
        
        // Set pixel 'c' to value 'color'
        strip.setPixelColor(c, color);                       
      }
      
      // Update strip with new contents
      strip.show();                
      
      // Pause for a moment
      delay(wait);           
      
      // One cycle of color wheel over 90 frames      
      firstPixelHue += 65536 / 90;
    }
  }
}

// ---------------------------- Musical Note Detector ------------------------------------ //
char Tone_det()
{

  long unsigned int a1, b, a2;
  float a;
  float sum1 = 0, sum2 = 0;
  float sampling;
  a1 = micros();

  for (int i = 0; i < 128; i++)
  {
    a = analogRead(MIC_PIN) - 500; // rough zero shift
    if (a < 130)
    {
      a = 0;
    }
    // utilising time between two sample for windowing & amplitude calculation
    sum1 = sum1 + a;                                     // to average value
    sum2 = sum2 + a * a;                                 // to RMS value
    a = a * (sin(i * 3.14 / 128) * sin(i * 3.14 / 128)); // Hann windowㄑㄑ
    in[i] = 10 * a;                                      // scaling for float to int conversion
    delayMicroseconds(195);                              // based on operation frequency range
  }

  b = micros();
  sum1 = sum1 / 128;               // Average amplitude
  sum2 = sqrt(sum2 / 128);         // RMS
  sampling = 128000000 / (b - a1); // real time sampling frequency

  // for very low or no amplitude, this code won't start
  // it takes very small aplitude of sound to initiate for value sum2-sum1 > 3,
  // change sum2-sum1 threshold based on requirement
  if (sum2 - sum1 > 50)
  {

    FFT(128, sampling);
    // EasyFFT based optimised  FFT code,
    // this code upㄉㄉdates f_peaks array with 5 most dominent frequency in descending order
    for (int i = 0; i < 12; i++)
    {
      in[i] = 0;
    } // utilising in[] array for further calculation

    // below loop will convert frequency value to note
    int j = 0, k = 0;
    for (int i = 0; i < 5; i++)
    {
      if (f_peaks[i] > 1040)
      {
        f_peaks[i] = 0;
      }
      if (f_peaks[i] >= 65.4 && f_peaks[i] <= 130.8)
      {
        f_peaks[i] = 255 * ((f_peaks[i] / 65.4) - 1);
      }
      if (f_peaks[i] >= 130.8 && f_peaks[i] <= 261.6)
      {
        f_peaks[i] = 255 * ((f_peaks[i] / 130.8) - 1);
      }
      if (f_peaks[i] >= 261.6 && f_peaks[i] <= 523.25)
      {
        f_peaks[i] = 255 * ((f_peaks[i] / 261.6) - 1);
      }
      if (f_peaks[i] >= 523.25 && f_peaks[i] <= 1046)
      {
        f_peaks[i] = 255 * ((f_peaks[i] / 523.25) - 1);
      }
      if (f_peaks[i] >= 1046 && f_peaks[i] <= 2093)
      {
        f_peaks[i] = 255 * ((f_peaks[i] / 1046) - 1);
      }
      if (f_peaks[i] > 255)
      {
        f_peaks[i] = 254;
      }

      j = 1, k = 0;

      while (j == 1)
      {
        if (f_peaks[i] < NoteV[k])
        {
          f_peaks[i] = k;
          j = 0;
        }
        k++; // a note with max peaks (harmonic) with aplitude priority is selected
        if (k > 15)
        {
          j = 0;
        }
      }

      if (f_peaks[i] == 12)
      {
        f_peaks[i] = 0;
      }

      k = f_peaks[i];
      in[k] = in[k] + (5 - i);
    }

    k = 0;
    j = 0;
    for (int i = 0; i < 12; i++)
    {
      if (k < in[i])
      {
        k = in[i];
        j = i;
      } // Max value detection
    }

    // Note print
    // if you need to use note value for some application, use of note number recomendded
    // where, 0 = c; 1 = c#, 2 = D; 3 = D#; .. 11 = B;
    // a2 = micros(); // time check
    k = j;
    if (k == 0)
    {
      Serial.println('C');
      // WebSerial.println("C");
      // delay(75);
      return 'C';
    }
    if (k == 1)
    {
      Serial.print('C');
      Serial.println('#');
      // WebSerial.println("C#");
      // delay(75);
    }
    if (k == 2)
    {
      Serial.println('D');
      // WebSerial.println("D");
      // delay(75);
      return 'D';
    }
    if (k == 3)
    {
      Serial.print('D');
      Serial.println('#');
      // WebSerial.println("D#");
      // delay(75);
    }
    if (k == 4)
    {
      Serial.println('E');
      // WebSerial.println("E");
      // delay(75);
      return 'E';
    }
    if (k == 5)
    {
      Serial.println('F');
      // WebSerial.println("F");
      // delay(75);
      return 'F';
    }
    if (k == 6)
    {
      Serial.print('F');
      Serial.println('#');
      // WebSerial.println("F#");
      // delay(75);
    }
    if (k == 7)
    {
      Serial.println('G');
      // WebSerial.println("G");
      // delay(75);
      return 'G';
    }
    if (k == 8)
    {
      Serial.print('G');
      Serial.println('#');
      // WebSerial.println("G#");
      // delay(75);
    }
    if (k == 9)
    {
      Serial.println('A');
      // WebSerial.println("A");
      // delay(75);
      return 'A';
    }
    if (k == 10)
    {
      Serial.print('A');
      Serial.println('#');
      // WebSerial.println("A#");
      // delay(75);
    }
    if (k == 11)
    {
      Serial.println('B');
      // WebSerial.println("B");
      // delay(75);
      return 'B';
    }
  }
  else
  {
    return 'N';
  }

  return 'N';
}

// ---------------------------- FFT Function for Musical Note Detector ------------------- //
// Documentation on EasyFFT: https://www.instructables.com/member/abhilash_patel/instructables/
// EasyFFT code optimised for 128 sample size to reduce mamory consumtion
float FFT(byte N, float Frequency)
{

  byte data[8] = {1, 2, 4, 8, 16, 32, 64, 128};
  int a, c1, f, o, x;
  a = N;

  for (int i = 0; i < 8; i++)
  { // calculating the levels
    if (data[i] <= a)
    {
      o = i;
    }
  }
  o = 7;

  byte in_ps[data[o]] = {};   // input for sequencing
  float out_r[data[o]] = {};  // real part of transform
  float out_im[data[o]] = {}; // imaginory part of transform

  x = 0;
  for (int b = 0; b < o; b++)
  { // bit reversal
    c1 = data[b];
    f = data[o] / (c1 + c1);
    for (int j = 0; j < c1; j++)
    {
      x = x + 1;
      in_ps[x] = in_ps[j] + f;
    }
  }

  for (int i = 0; i < data[o]; i++)
  { // update input array as per bit reverse order
    if (in_ps[i] < a)
    {
      out_r[i] = in[in_ps[i]];
    }
    if (in_ps[i] > a)
    {
      out_r[i] = in[in_ps[i] - a];
    }
  }

  int i10, i11, n1;
  float e, c, s, tr, ti;

  for (int i = 0; i < o; i++)
  {                              // fft
    i10 = data[i];               // overall values of sine cosine
    i11 = data[o] / data[i + 1]; // loop with similar sine cosine
    e = 6.283 / data[i + 1];
    e = 0 - e;
    n1 = 0;

    for (int j = 0; j < i10; j++)
    {
      c = cos(e * j);
      s = sin(e * j);
      n1 = j;

      for (int k = 0; k < i11; k++)
      {
        tr = c * out_r[i10 + n1] - s * out_im[i10 + n1];
        ti = s * out_r[i10 + n1] + c * out_im[i10 + n1];
        out_r[n1 + i10] = out_r[n1] - tr;
        out_r[n1] = out_r[n1] + tr;
        out_im[n1 + i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;
        n1 = n1 + i10 + i10;
      }
    }
  }

  //---> here onward out_r contains amplitude and our_in conntains frequency (Hz)
  for (int i = 0; i < data[o - 1]; i++)
  {                                                                   // getting amplitude from compex number
    out_r[i] = sqrt((out_r[i] * out_r[i]) + (out_im[i] * out_im[i])); // to  increase the speed delete sqrt
    out_im[i] = (i * Frequency) / data[o];
    /*
      Serial.print(out_im[i],2); Serial.print("Hz");
      Serial.print("\t");                            // uncomment to print freuency bin
      Serial.println(out_r[i]);
      */
  }

  x = 0; // peak detection
  for (int i = 1; i < data[o - 1] - 1; i++)
  {
    if (out_r[i] > out_r[i - 1] && out_r[i] > out_r[i + 1])
    {
      in_ps[x] = i; // in_ps array used for storage of peak number
      x = x + 1;
    }
  }

  s = 0;
  c = 0;
  for (int i = 0; i < x; i++)
  { // re arraange as per magnitude
    for (int j = c; j < x; j++)
    {
      if (out_r[in_ps[i]] < out_r[in_ps[j]])
      {
        s = in_ps[i];
        in_ps[i] = in_ps[j];
        in_ps[j] = s;
      }
    }
    c = c + 1;
  }

  for (int i = 0; i < 5; i++)
  { // updating f_peak array (global variable)with descending order
    f_peaks[i] = (out_im[in_ps[i] - 1] * out_r[in_ps[i] - 1] + out_im[in_ps[i]] * out_r[in_ps[i]] + out_im[in_ps[i] + 1] * out_r[in_ps[i] + 1]) / (out_r[in_ps[i] - 1] + out_r[in_ps[i]] + out_r[in_ps[i] + 1]);
  }

  return -1;
}

void recvMsg(uint8_t *data, size_t len)
{
  WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }
  WebSerial.println(d);
}