#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <FastLED.h>
#include <UbidotsMicroESP8266.h>

#define NUM_LEDS 1
#define DATA_PIN D3
#define VOTE_RED D4
#define VOTE_YELLOW D2
#define VOTE_GREEN D1

#define TOKEN "YOUR_TOKEN_HERE"
#define WIFISSID "BLANK"
#define PASSWORD "BLANK"


unsigned long startTime;
unsigned long delayTime = 10000; // Interval for transmit
unsigned long delayStart;

int state = 0; // Set state for FSM to init state
int brightness = 240; // 256 = full dimming, 0 = no dimming
int vote;

char context[25] = {"Votebox=4"};

CRGB leds[NUM_LEDS];

Ubidots client(TOKEN);

void setup()
{
  pinMode(VOTE_RED, INPUT_PULLUP);
  pinMode(VOTE_YELLOW, INPUT_PULLUP);
  pinMode(VOTE_GREEN, INPUT_PULLUP);
  
  // Initialize PL9823 LED
  FastLED.addLeds<PL9823, DATA_PIN>(leds, NUM_LEDS);
  
  // Ubidots connection to wifi
  Serial.begin(115200);
  delay(10);
  client.wifiConnection(WIFISSID, PASSWORD);
  client.setDataSourceName("Customer Vote Box 1"); // Device description
  client.setDataSourceLabel("Customer Vote Box"); // Device name
  //client.setDebug(true);

  
  // Set initial time stamp
  startTime = millis();


}
 
void loop() {
  switch(state) {
    case 0:
      // Initialize
      // https://github.com/FastLED/FastLED/wiki/Controlling-leds
      leds[0] = CRGB::DarkBlue;
      leds[0].fadeLightBy(brightness);
      FastLED.show();
      
  
      state = 1;
      break;

    case 1:
      // Ready for readings
      leds[0] = CRGB::Cyan;
      leds[0].fadeLightBy(brightness);
      FastLED.show();
      if (digitalRead(VOTE_RED) == LOW){
        state = 2;
        leds[0] = CRGB::Red;
        leds[0].fadeLightBy(brightness);
        FastLED.show();
        break;
      }

      else if (digitalRead(VOTE_YELLOW) == LOW){
        state = 3;
        leds[0] = CRGB::Yellow;
        leds[0].fadeLightBy(brightness);
        FastLED.show();
        break;
      }

      else if (digitalRead(VOTE_GREEN) == LOW){
        state = 4;
        leds[0] = CRGB::Green;
        leds[0].fadeLightBy(brightness);
        FastLED.show();
        break;
      }
      else{
        // Do nothing
        break;
      }
    
    case 2: 
      // Voted for red (-1)
      vote = -1;
      if (digitalRead(VOTE_RED) == HIGH){
        state = 5;
        leds[0] = CRGB::Red;
        leds[0].fadeLightBy(brightness);
        FastLED.show();
        break;
      }
      else{
        break;
      }

    case 3:
      // Voted for yellow (0)
      vote = 0;
      if (digitalRead(VOTE_YELLOW) == HIGH){
        state = 5;
        leds[0] = CRGB::Yellow;
        leds[0].fadeLightBy(brightness);
        FastLED.show();
        break;
      }
      else{
        break;
      }


    case 4:
      // Voted for green (+1)
      vote = 1;
      if (digitalRead(VOTE_GREEN) == HIGH){
        state = 5;
        leds[0] = CRGB::Green;
        leds[0].fadeLightBy(brightness);
        FastLED.show();
        break;
      }
      else{
        break;
      }

    break;

    case 5:
      //leds[0].setRGB(128, 128, 128);
      //FastLED.show();

      if (vote == -1){
        client.add("Vote", -1, context);
      }
      else if (vote == 0){
        client.add("Vote", 0, context);
      }
      else if (vote == 1){
        client.add("Vote", 1, context);
      }
      // Vote complete, transmit
      client.sendTLATE();
      // Vote sent, clear vote
      vote = 9; // Set some arbitrary value
      state = 6; // Return to beginning of FSM
      leds[0] = CRGB::Pink;
      leds[0].fadeLightBy(brightness);
      FastLED.show();
      Serial.println("\nEntering delay");
      startTime = millis();
           
    break;

    case 6:
      // Non blocking delay
      
      
      if (millis() <= (startTime + delayTime)){ // (startTime + delayTime) >= millis()
        break; // Should be Yield(); ?
      }
      else {
        state = 1; // Delay passed, return to start
        Serial.println("Exiting delay");
        break;
      }
      
    break;

    default:
    // Default case handling
    state = 0; // Entered unexpected state, restart!
  }
}
