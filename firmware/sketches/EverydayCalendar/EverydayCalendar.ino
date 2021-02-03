#include <EverydayCalendar_lights.h>
#include <EverydayCalendar_touch.h>

typedef struct {
   int8_t    x;
   int8_t    y;
} Point;

EverydayCalendar_touch cal_touch;
EverydayCalendar_lights cal_lights;
int16_t brightness = 128;

bool isNightTimeSelection = false;
bool isNightTime = false;
unsigned long nightStart = -1;
unsigned long nightDuration = -1;

void setup() {
  Serial.begin(9600);
  Serial.println("Sketch setup started");

  // Initialize LED functionality
  cal_lights.configure();
  cal_lights.setBrightness(200);
  cal_lights.begin();

  // Perform startup animation
  honeyDrip();

  // Fade out
  for(int b = 200; b >= 0; b--){
    cal_lights.setBrightness(b);
    delay(4);
  }

  // Initialize touch functionality
  cal_touch.configure();
  cal_touch.begin();
  cal_lights.loadLedStatesFromMemory();
  delay(1500);

  // Fade in
  for(int b = 0; b <= brightness; b++){
    cal_lights.setBrightness(b);
    delay(4);
  }

  Serial.println("Sketch setup complete");
}

//   ##        #######   #######  ########
//   ##       ##     ## ##     ## ##     ##
//   ##       ##     ## ##     ## ##     ##
//   ##       ##     ## ##     ## ########
//   ##       ##     ## ##     ## ##
//   ##       ##     ## ##     ## ##
//   ########  #######   #######  ##


void loop() {
  static Point previouslyHeldButton = {0xFF, 0xFF}; // 0xFF and 0xFF if no button is held
  static uint16_t touchCount = 1;
  static const uint8_t debounceCount = 3;
  static const uint16_t clearCalendarCount = 1300; // ~40 seconds.  This is in units of touch sampling interval ~= 30ms.
  static const uint16_t nightTimerCount = 150; // ~5 seconds.  This is in units of touch sampling interval ~= 30ms.
  Point buttonPressed = {0xFF, 0xFF};
  bool touch = cal_touch.scanForTouch();

  // Run the selection loop
  if (isNightTimeSelection) {
    nightTimerSelectionLoop(touch);
  }
  // Run the night time loop
  else if (isNightTime) {
    nightTimerLoop(touch);
  }
  // Run the regular routine
  else
  {
    // Handle a button press
    if(touch)
    {

      // Brightness Buttons
      if(cal_touch.y == 31){
        if(cal_touch.x == 4){
          brightness -= 3;
        }else if(cal_touch.x == 6){
          brightness += 2;
        }
        brightness = constrain(brightness, 0, 200);
        Serial.print("Brightness: ");
        Serial.println(brightness);
        cal_lights.setBrightness((uint8_t)brightness);
      }
      // If all buttons aren't touched, reset debounce touch counter
      if(previouslyHeldButton.x == -1){
        touchCount = 0;
      }

      // If this button is been held, or it's just starting to be pressed and is the only button being touched
      if(((previouslyHeldButton.x == cal_touch.x) && (previouslyHeldButton.y == cal_touch.y))
      || (debounceCount == 0))
      {
        // The button has been held for a certain number of consecutive checks
        // This is called debouncing
        if (touchCount == debounceCount){
          // Button is activated
          cal_lights.toggleLED((uint8_t)cal_touch.x, (uint8_t)cal_touch.y);
          cal_lights.saveLedStatesToMemory();
          Serial.print("x: ");
          Serial.print(cal_touch.x);
          Serial.print("\ty: ");
          Serial.println(cal_touch.y);
        }

        // Check if the special "Reset" January 1 button is being held
        if((cal_touch.x == 0) && (cal_touch.y == 0) && (touchCount == clearCalendarCount)){
          Serial.println("Resetting all LED states");
          clearAnimation();
        }

        // Check if the special "Night Timer" December 31 button is being held
        if((cal_touch.x == 11) && (cal_touch.y == 30) && (touchCount == nightTimerCount)){
          Serial.println("Starting Night Timer Selection");

          // Undo the date change that would have happened
          cal_lights.toggleLED((uint8_t)cal_touch.x, (uint8_t)cal_touch.y);
          cal_lights.saveLedStatesToMemory();

          startNightTimer();
        }

        if(touchCount < 65535){
          touchCount++;
          //Serial.println(touchCount);
        }
      }
    }
  }

  previouslyHeldButton.x = cal_touch.x;
  previouslyHeldButton.y = cal_touch.y;
}

//   ##    ## ####  ######   ##     ## ########
//   ###   ##  ##  ##    ##  ##     ##    ##
//   ####  ##  ##  ##        ##     ##    ##
//   ## ## ##  ##  ##   #### #########    ##
//   ##  ####  ##  ##    ##  ##     ##    ##
//   ##   ###  ##  ##    ##  ##     ##    ##
//   ##    ## ####  ######   ##     ##    ##


void startNightTimer(){

  // Blitz all the lights off when entering control mode
  static const uint8_t monthMaxDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for(int month = 11; month >= 0; month--){
    for(int day = monthMaxDay[month]-1; day >=0; day--){
       cal_lights.setLED(month, day, false);
    }
  }

  // Engage night timer
  isNightTimeSelection = true;
}

// This loop runs when the night timer selector is active
void nightTimerSelectionLoop(bool touch) {
  static const uint16_t flashRate = 30;
  static uint16_t flashTick = flashRate;

  static uint16_t selectedTime = -1;


  // Alternate the flash
  if (flashTick == 0) {
    // Dec 31 flashes in this mode
    cal_lights.toggleLED(11, 30);
    flashTick = flashRate;
  } else {
    flashTick--;
  }

  // Deal with time selection
  if (touch) {
    // If we've selected a time, Dec 31 starts the timer
    if (selectedTime != -1 && cal_touch.x == 11 && cal_touch.y == 30) {
      Serial.println("Starting Night Timer");
      nightStart = millis();
      nightDuration = (unsigned long)selectedTime * 1000 * 60 * 60;

      // Move to next stage (night time)
      isNightTimeSelection = false;
      isNightTime = true;

      // Clear this to avoid issues on the next run
      selectedTime = -1;

      // Throw on the startup effect (is pretty and debounces)
      sleepEffect();
    }

    // Otherwise we capture the number from the Jan column
    else if (cal_touch.x == 0) {
      Serial.print("Selected Duration: ");
      Serial.println(cal_touch.y + 1);

      cal_lights.setLED(cal_touch.x, cal_touch.y, true);

      // Disable the last selected one (and debounce)
      if (selectedTime != -1 && (selectedTime - 1) != cal_touch.y) {
        cal_lights.setLED(cal_touch.x, selectedTime - 1, false);
      }

      selectedTime = cal_touch.y + 1;
    }
  }
}

// This loop runs when night timer is active
void nightTimerLoop(bool touch) {
  // Any button press wakes from sleep
  if (touch) {
    Serial.println("Touch Detected: Begin Daytime");
    isNightTime = false;
  }

  // The timer also wakes from sleep
  if (millis() - nightStart > nightDuration) {
    Serial.println("Timer expired: Begin Daytime");
    Serial.print("Duration: ");
    Serial.print((millis() - nightStart) / 1000);
    Serial.println("s");
    
    isNightTime = false;
  }

  // Cleanup
  if (!isNightTime) {
    cal_lights.loadLedStatesFromMemory();
    nightDuration = -1;
    nightStart = -1;

    // Fade back on
    for(int b = 0; b <= brightness; b++){
      cal_lights.setBrightness(b);
      delay(20);
    }
  }
}

//   ##     ## ######## #### ##       #### ######## #### ########  ######
//   ##     ##    ##     ##  ##        ##     ##     ##  ##       ##    ##
//   ##     ##    ##     ##  ##        ##     ##     ##  ##       ##
//   ##     ##    ##     ##  ##        ##     ##     ##  ######    ######
//   ##     ##    ##     ##  ##        ##     ##     ##  ##             ##
//   ##     ##    ##     ##  ##        ##     ##     ##  ##       ##    ##
//    #######     ##    #### ######## ####    ##    #### ########  ######


void sleepEffect(){
  // These two numbers combined effect the speed of the effect
  uint16_t interval_ms = 15;
  double increment = 0.2;

  uint8_t lastLit[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Iterate out on a radius. 32 is the diagonal
  for(double r = 0; r < 32 + increment; r = r + increment) {
    double r2 = r * r;
    // Iterate over the columns, lighting them from the bottom up if needed
    for (int month = 11; month >= 0; month--) {
      for(int day = lastLit[month] - 1; day >= 0; day--){
        // Calculate the cells distance from the bottom right corner
        double x = 11 - month;
        double y = 30 - day;
        double distance2 = x * x + y * y;

        // Check if within lighting range
        if (distance2 <= r2) {
          cal_lights.setLED(month, day, true);
          lastLit[month] = day;
        } else {
          // No more can be lit
          break;
        }
      }
    }
    delay(interval_ms);
  }

  // Fade out
  for(int b = brightness; b >= 0; b--){
    cal_lights.setBrightness(b);
    delay(20);
  }
}

void honeyDrip(){
  uint16_t interval_ms = 25;
  static const uint8_t monthDayOffset[12] = {0, 3, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0};
  // Turn on all LEDs one by one in the span of a few second
  for(int day = 0; day < 31; day++){
    for(int month = 0; month < 12; month++){
      int8_t adjustedDay = day - monthDayOffset[month];
      if(adjustedDay >= 0 ){
        cal_lights.setLED(month, adjustedDay, true);
      }
    }
    delay(interval_ms);
    interval_ms = interval_ms + 2;
  }
}

void clearAnimation(){
  uint16_t interval_ms = 25;
  static const uint8_t monthMaxDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for(int month = 11; month >= 0; month--){
    for(int day = monthMaxDay[month]-1; day >=0; day--){
       cal_lights.setLED(month, day, false);
       delay(interval_ms);
    }
  }
  cal_lights.saveLedStatesToMemory();
}
