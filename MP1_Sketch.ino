/*
 * Richard Halbert
 * Made for University of Washington
 * CSE 490F MP1 Spring 2021
 */

const int RGB_RED_OUTPUT_PIN = 3;
const int RGB_GREEN_OUTPUT_PIN = 5;
const int RGB_BLUE_OUTPUT_PIN = 6;

const int BUTTON_INPUT_PIN = 2;
const int LIGHT_SENSOR_INPUT_PIN = A0;
const int LOFI_POT_INPUT_PIN = A1;
const int MICROPHONE_INPUT_PIN = A2;

const int DEBOUNCE_WINDOW = 40;
const int FADE_FACTOR = 5;
const int MAX_GREEN_VAL = 40;
const int MIC_THRESHHOLD = 550;
const float MODE3_STARTING_VAL = 0.1;
const float MODE3_RISE_FACTOR = 0.05;
const float MODE3_FALL_FACTOR = 0.02;

enum RGB{
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};

int buttonPresses = 0;
int lastButtonVal = LOW;
boolean fadingUp = true;
float mode3CurrFactor = MODE3_STARTING_VAL;
boolean mode3GoingUp = true;
boolean mode3Blown = false;

/*
 * Fire Colors:
 *  Red: {255, 0, 0}
 *  Orange: {255, 127, 0}
 *  Yellow: {255, 255, 0}
 *  White: {255, 255, 255}
 */
int currRGBValues[] = {255, 0, 0};

void setup() {
  pinMode(RGB_RED_OUTPUT_PIN, OUTPUT);
  pinMode(RGB_GREEN_OUTPUT_PIN, OUTPUT);
  pinMode(RGB_BLUE_OUTPUT_PIN, OUTPUT);

  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(LIGHT_SENSOR_INPUT_PIN, INPUT);
  pinMode(LOFI_POT_INPUT_PIN, INPUT);
  pinMode(MICROPHONE_INPUT_PIN, INPUT);
}

void loop() {
  // check if button has been pressed
  int buttonCurr = digitalRead(BUTTON_INPUT_PIN);
  delay(DEBOUNCE_WINDOW);
  int buttonCurr2 = digitalRead(BUTTON_INPUT_PIN);
  
  if(buttonCurr == buttonCurr2 && lastButtonVal != buttonCurr){
    lastButtonVal = buttonCurr;
    if (buttonCurr == LOW) {
      buttonPresses++;
      // reset the RGB
      currRGBValues[RED] = 255;
      currRGBValues[GREEN] = 0;
      currRGBValues[BLUE] = 0;
      setRgbLedColor(255, 0, 0);

      // reset mode variables
      fadingUp = true;
      mode3CurrFactor = MODE3_STARTING_VAL;
      mode3GoingUp = true;
      mode3Blown = false;
    }
  }

  if (buttonPresses % 4 == 1) {
    // Mode 1: Crossfading RGB LED with brightness inversely proportional to the light level
    float lightVal = (1023.0 - analogRead(LIGHT_SENSOR_INPUT_PIN)) / 1023.0;
    // set RGB based on that
    setRgbLedColor((int)(currRGBValues[RED] * lightVal),
                   (int)(currRGBValues[GREEN] * lightVal),
                   (int)(currRGBValues[BLUE] * lightVal));
    
    // update the RGB
    if (currRGBValues[GREEN] > 255) {
      currRGBValues[GREEN] = 255;
    }

    if (currRGBValues[GREEN] < 0) {
      currRGBValues[GREEN] = 0;
    }

    if (currRGBValues[BLUE] > MAX_GREEN_VAL) {
      currRGBValues[BLUE] = MAX_GREEN_VAL;
    }

    if (currRGBValues[BLUE] < 0) {
      currRGBValues[BLUE] = 0;
    }
    
    if (fadingUp) {
      if (currRGBValues[GREEN] != 255) {
        currRGBValues[GREEN] += FADE_FACTOR;
      } else if (currRGBValues[BLUE] != MAX_GREEN_VAL) {
          currRGBValues[BLUE] += FADE_FACTOR;
      } else {
        fadingUp = false;
      }
    } else {
      if (currRGBValues[BLUE] != 0) {
        currRGBValues[BLUE] -= FADE_FACTOR;
      } else if (currRGBValues[GREEN] != 0) {
        currRGBValues[GREEN] -= FADE_FACTOR;
      } else {
        fadingUp = true;
      }
    }
  } else if (buttonPresses % 4 == 2) {
    // Mode 2: Configuring color with lo-fi input
    float lofiVal = (1023.0 - analogRead(LOFI_POT_INPUT_PIN)) / 1023.0;

    // based on the lo-fi variable resistor, move from red to yellow
    setRgbLedColor(255, 255 * lofiVal, 0);
  } else if (buttonPresses % 4 == 3) {
    // Mode 3: Creative feature
    int micVal = analogRead(MICROPHONE_INPUT_PIN);

    if (micVal > MIC_THRESHHOLD) {
      mode3Blown = true;
      mode3GoingUp = true;
    }

    if (mode3Blown) {
      if (mode3GoingUp) {
        mode3CurrFactor += MODE3_RISE_FACTOR;
      } else {
        mode3CurrFactor -= MODE3_FALL_FACTOR;
      }
    }

    if (mode3CurrFactor > 1.0) {
      mode3CurrFactor = 1.0;
      mode3GoingUp = false;
    } else if (mode3CurrFactor < MODE3_STARTING_VAL) {
      mode3CurrFactor = MODE3_STARTING_VAL;
      mode3GoingUp = true;
      mode3Blown = false;
    }

    setRgbLedColor((int)(currRGBValues[RED] * mode3CurrFactor),
                   (int)(currRGBValues[GREEN] * mode3CurrFactor),
                   (int)(currRGBValues[BLUE] * mode3CurrFactor));

    if (currRGBValues[GREEN] > 255) {
      currRGBValues[GREEN] = 255;
    }

    if (currRGBValues[GREEN] < 0) {
      currRGBValues[GREEN] = 0;
    }

    if (currRGBValues[BLUE] > 80) {
      currRGBValues[BLUE] = 80;
    }

    if (currRGBValues[BLUE] < 0) {
      currRGBValues[BLUE] = 0;
    }
    
    if (fadingUp) {
      if (currRGBValues[GREEN] != 255) {
        currRGBValues[GREEN] += FADE_FACTOR;
      } else if (currRGBValues[BLUE] != 80) {
          currRGBValues[BLUE] += FADE_FACTOR;
      } else {
        fadingUp = false;
      }
    } else {
      if (currRGBValues[BLUE] != 0) {
        currRGBValues[BLUE] -= FADE_FACTOR;
      } else if (currRGBValues[GREEN] != 0) {
        currRGBValues[GREEN] -= FADE_FACTOR;
      } else {
        fadingUp = true;
      }
    }
  } else {
    // Off
    setRgbLedColor(0, 0, 0);
  }
}

void setRgbLedColor(int red, int green, int blue){
  analogWrite(RGB_RED_OUTPUT_PIN, red);
  analogWrite(RGB_GREEN_OUTPUT_PIN, green);
  analogWrite(RGB_BLUE_OUTPUT_PIN, blue);
}
