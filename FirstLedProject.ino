#include <FastLED.h>
#define LED_PIN 10
#include <time.h>

#include <stdlib.h>

#define seconds()(millis() / (double) 1000)
CRGB * leds;
CRGB defaultLedColor;


//variables to send via application 
float stairOnTime;
int detectionDistance; 
int numberOfStaircases; //nr trepte
float delayBetweenStaircase;//viteza leduri margine
int ledPerStairCase; //numarul de leduri pe treapta
int numOfLeds;

//Helpers
int ledOnStatus; //0-off, 1-up->down, 2-down->up
float lastTime;
float currentTime;
int lineNumber;
float ledOnTime; //the moment when leds turned on
int firstLedIndexForLateralLine;

struct {
  int trigPin;
  int echoPin;
  float duration;
  float distance;
  bool hasEncounterPerson = false;
  double detectionTime;
}
sensors[2];

void initSensors();

void initColor();

void initLedStrip();

void getDistances();

void printDistances();

void turnOnLeds(int indexStart, int indexEnd, CRGB color);

void turnOffLeds(int indexStart, int indexEnd);

void changeColorRandomly();

bool detectMovementOnSensor(int index);

void getData();

void startMovementFromTopToBottom();

void endMovementFromTopToBottom();

void startMovementFromBottomToTop();

void endMovementFromBottomToTop();

void sendDataToUnity();

void setup() {
  Serial.begin(4800);
  delay(20000);
  numOfLeds = 20;
  leds = new CRGB[numOfLeds];
  initLedStrip();
  getData();
  if(!Serial.available())
    sendDataToUnity();
  initLedStrip();
  initSensors();
  
}



void loop() {
  getDistances();
  if (ledOnStatus && (detectMovementOnSensor(0) || detectMovementOnSensor(1))) {
    ledOnTime = seconds();

    if(detectMovementOnSensor(0))
        ledOnStatus = 1;
    else
        ledOnStatus = 2;
    
  }
  
  //start aprindere leduri sus->jos
  startMovementFromTopToBottom();

  
  endMovementFromTopToBottom();

  startMovementFromBottomToTop();

  endMovementFromBottomToTop();

}



void turnOnLine(int line) {
  int i;
  for (int i = (line * ledPerStairCase); i < (line + 1) * ledPerStairCase; i++)
    leds[i] = defaultLedColor;
    delay(2000);
  FastLED.show();
}

void turnOffLine(int line) {
  int i;
  for (int i = (line * ledPerStairCase); i < (line + 1) * ledPerStairCase; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();
}

void initLedStrip() {
  FastLED.addLeds < WS2812, LED_PIN, GRB > (leds, numOfLeds);

  for (int i = 0; i < numOfLeds; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

void getDistances() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(sensors[i].trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(sensors[i].trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensors[i].trigPin, LOW);
    sensors[i].duration = pulseIn(sensors[i].echoPin, HIGH);
    sensors[i].distance = (sensors[i].duration / 2) * 0.0343;
  }
}

void printDistances() {
  for (int i = 0; i < 2; i++) {
    Serial.print("Distance from the sensor ");
    Serial.print(i);
    Serial.print(" = ");
    if (sensors[i].distance >= 400 || sensors[i].distance <= 2) {
      Serial.println("Out of range");
    } else {
      Serial.print(sensors[i].distance);
      Serial.println(" cm");

    }
  }
  Serial.println("");
  //delay(500);
}

void sendDataToUnity()
{
  Serial.println(numOfLeds);
  Serial.println(ledPerStairCase);
  Serial.println(numberOfStaircases);
  Serial.println(stairOnTime);
  Serial.println(detectionDistance);
  Serial.println(delayBetweenStaircase);
}

void initSensors() {
  sensors[0].trigPin = 2;
  sensors[0].echoPin = 3;

  sensors[1].trigPin = 4;
  sensors[1].echoPin = 5;

  for (int i = 0; i < 2; i++) {
    pinMode(sensors[i].trigPin, OUTPUT);
    pinMode(sensors[i].echoPin, INPUT);
  }
}

bool detectMovementOnSensor(int index) {
  if (sensors[index].distance <= detectionDistance && sensors[index].distance >= 2)
    return true;
  return false;
}

void getData()
{

  if (Serial.available() > 0)
  {
     for(int i=0;i<numOfLeds;i++)
      leds[i] = CRGB(255,255,255);
     FastLED.show();
     Serial.flush();
  }
  else
  {
    stairOnTime = 5;
    detectionDistance = 20; 
    numberOfStaircases = 6; //nr trepte
    delayBetweenStaircase = 0.375;   //viteza leduri margine
    ledPerStairCase = 15; //numarul de leduri pe treapta
    numOfLeds = 130;
  }
  //helpers
  ledOnStatus = 0; //0-off, 1-up->down, 2-down->up
  lastTime = 0;
  lineNumber = 0;
  ledOnTime = 0;
  firstLedIndexForLateralLine = (ledPerStairCase) * numberOfStaircases + 5;
  
  defaultLedColor = CRGB(0, 0, 255);
  numOfLeds = 130;
  leds = new CRGB[numOfLeds];
}

void startMovementFromTopToBottom()
{
  if (detectMovementOnSensor(0) && !ledOnStatus) {
    ledOnTime = seconds();
    lastTime = 0;
    lineNumber = 0;
    int lineNumberReverse = numberOfStaircases - 1;
    bool reverse = false;
    for (int i = numOfLeds - 1, j = firstLedIndexForLateralLine; i >= firstLedIndexForLateralLine; i--) {
      getDistances();
      if (detectMovementOnSensor(0) || detectMovementOnSensor(1))
        ledOnTime = seconds();  
      if(detectMovementOnSensor(1))
          reverse = true;
      if(reverse && j <= i)
        leds[j++] = CRGB(255, 0, 0);
        
      leds[i] = CRGB(255, 0, 0);
      currentTime = seconds();
      if (currentTime - lastTime >= delayBetweenStaircase && lineNumber < numberOfStaircases) {
        if(reverse && lineNumberReverse >= 0){
          turnOnLine(lineNumberReverse);
          lineNumberReverse--;
        }
        turnOnLine(lineNumber);
        lineNumber++;
        lastTime = seconds();
      }
      FastLED.show();
      delay(50);
      
    }
    if(reverse)
      ledOnStatus = 2;
    else
      ledOnStatus = 1;
  }
}

void endMovementFromTopToBottom()
{

  currentTime = seconds();
  if (currentTime - ledOnTime >= stairOnTime && ledOnStatus == 1) //5 timp scara activa
  {
    //ar trebui sa oprim scara
    lineNumber = 0;
    lastTime = 0;
    bool quit = false;
    bool changed = false;
    for (int i = numOfLeds - 1; i >= firstLedIndexForLateralLine; i--) {
      leds[i] = CRGB(0, 0, 0);
      currentTime = seconds();
      if (currentTime - lastTime >= delayBetweenStaircase && lineNumber < numberOfStaircases) {
        turnOffLine(lineNumber);
        lineNumber++;
        lastTime = seconds();
      }
      FastLED.show();
      getDistances();
      if (detectMovementOnSensor(0)) {
        int lastLine = lineNumber;
        int lastLateralLedIndex = i;
        bool comingFromDown = false;
        int downLine = numberOfStaircases - 1;
        ledOnTime = seconds();
        lastTime = 0;
        lineNumber = 0;
        for (int i = numOfLeds - 1, j = lastLateralLedIndex, k = firstLedIndexForLateralLine; i >= firstLedIndexForLateralLine; i--) {
           getDistances();
          if (detectMovementOnSensor(0) || detectMovementOnSensor(1)){
            ledOnTime = seconds();
            if(detectMovementOnSensor(0))
              ledOnStatus = 1;
            else
              ledOnStatus = 2;
              changed = true;
          }
          if(detectMovementOnSensor(1) && !comingFromDown)
              comingFromDown = true;
              
          leds[i] = CRGB(255, 0, 0);
          if(comingFromDown && k < i)
          {
              leds[k] = CRGB(255, 0, 0);
              k++;
          }
          
          if((j >= firstLedIndexForLateralLine && comingFromDown && j > k) || (!comingFromDown && j >= firstLedIndexForLateralLine))
            leds[j--] = CRGB(0, 0, 0);
          currentTime = seconds();
          if (currentTime - lastTime >= delayBetweenStaircase && lineNumber < numberOfStaircases) {
            if(lastLine < numberOfStaircases && !comingFromDown)
              {
                turnOffLine(lastLine);
                lastLine++;
              }
             if(comingFromDown && downLine >= 0)
             {
                turnOnLine(downLine);
                downLine--;
             }
            turnOnLine(lineNumber);
            lineNumber++;
            lastTime = seconds();
          }
          FastLED.show();
         
          delay(50);
        }
        quit = true;
        if(!changed)
          ledOnStatus = 1;
      }

      if (detectMovementOnSensor(1)) {
        ledOnTime = seconds();
        lastTime = 0;
        int lastLine = lineNumber;
        lineNumber = numberOfStaircases - 1;
        int lastLateralIndex = i;
        int reverseLateralIndex = numOfLeds - 1;
        int reverseLine = 0;
        bool reverse = false;
        for (int i = firstLedIndexForLateralLine, j = lastLateralIndex; i < numOfLeds; i++) {
    
          getDistances();
          if(detectMovementOnSensor(0) && reverseLateralIndex > i)
          {
            reverse = true;
            changed = true;
            ledOnStatus = 1;
          }

          if(reverse && reverseLateralIndex > i)
          {
            leds[reverseLateralIndex] = CRGB(255,0,0);
            reverseLateralIndex--;
          }
          
          if (detectMovementOnSensor(0) || detectMovementOnSensor(1))
            ledOnTime = seconds();
          
          leds[i] = CRGB(255, 0, 0);
          if(j > i)
          {
              leds[j] = 0;
              j--;
          }
          currentTime = seconds();
          if (abs(currentTime - lastTime) >= delayBetweenStaircase && lineNumber >= 0) {
            if(lastLine < lineNumber){
              turnOffLine(lastLine);
              lastLine++;
            }            
            if(reverse && reverseLine < lineNumber)
            {
              turnOnLine(reverseLine);
              reverseLine++;
            }
            turnOnLine(lineNumber);
            lineNumber--;
            lastTime = seconds();
          }
          FastLED.show();  
          delay(50);
        }
        if(!changed)
          ledOnStatus = 2;
        quit = true;
      }
      if (quit)
        break;
      delay(50);
    }
    if (!quit)
      ledOnStatus = 0;
  }
}

void startMovementFromBottomToTop(){
  
  if (detectMovementOnSensor(1) && !ledOnStatus) {
    ledOnTime = seconds();
    lastTime = 0;
    lineNumber = numberOfStaircases - 1;
    int lineNumberReverse = 0;
    bool reverse = false;
    for (int i = firstLedIndexForLateralLine, j = numOfLeds - 1; i < numOfLeds; i++) {
      getDistances();
      if (detectMovementOnSensor(0) || detectMovementOnSensor(1))
        ledOnTime = seconds();  
      if(detectMovementOnSensor(0))
          reverse = true;
      if(reverse && j > i)
        leds[j--] = CRGB(255, 0, 0);
        
      leds[i] = CRGB(255, 0, 0);
      currentTime = seconds();
      if (currentTime - lastTime >= delayBetweenStaircase && lineNumber >= 0) {
        if(reverse && lineNumberReverse < numberOfStaircases){
          turnOnLine(lineNumberReverse);
          lineNumberReverse++;
        }
        turnOnLine(lineNumber);
        lineNumber--;
        lastTime = seconds();
      }
      FastLED.show();
      delay(50);
      
    }
    if(reverse)
      ledOnStatus = 1;
    else
      ledOnStatus = 2;
  }
}


void endMovementFromBottomToTop(){
  currentTime = seconds();
  if (currentTime - ledOnTime >= stairOnTime && ledOnStatus == 2)
  {
    //ar trebui sa oprim scara
    lineNumber = numberOfStaircases - 1;
    lastTime = 0;
    bool quit = false;
    bool changed = false;
    for (int i = firstLedIndexForLateralLine; i < numOfLeds; i++) {
      leds[i] = CRGB(0, 0, 0);
      currentTime = seconds();
      if (currentTime - lastTime >= delayBetweenStaircase && lineNumber >= 0) {
        turnOffLine(lineNumber);
        lineNumber--;
        lastTime = seconds();
      }
      FastLED.show();
      getDistances();
      if (detectMovementOnSensor(1)) {
        int lastLine = lineNumber;
        int lastLateralLedIndex = i;
        bool comingFromTop = false;
        int topLine = 0;
        ledOnTime = seconds();
        lastTime = 0;
        lineNumber = numberOfStaircases - 1;
        for (int i = firstLedIndexForLateralLine, j = lastLateralLedIndex, k = numOfLeds - 1; i < numOfLeds; i++) {
           getDistances();
          if (detectMovementOnSensor(0) || detectMovementOnSensor(1)){
            ledOnTime = seconds();
            if(detectMovementOnSensor(0))
              ledOnStatus = 1;
            else
              ledOnStatus = 2;
              changed = true;
          }
          if(detectMovementOnSensor(0) && !comingFromTop)
              comingFromTop = true;
              
          leds[i] = CRGB(255, 0, 0);
          if(comingFromTop && k > i)
          {
              leds[k] = CRGB(255, 0, 0);
              k--;
          }
          
          if((j < numOfLeds && comingFromTop && j < k) || (!comingFromTop && j < numOfLeds))
            leds[j++] = CRGB(0, 0, 0);
          currentTime = seconds();
          if (currentTime - lastTime >= delayBetweenStaircase && lineNumber >= 0) {
            if(lastLine >= 0 && !comingFromTop)
              {
                turnOffLine(lastLine);
                lastLine--;
              }
             if(comingFromTop && topLine < numberOfStaircases)
             {
                turnOnLine(topLine);
                topLine++;
             }
            turnOnLine(lineNumber);
            lineNumber--;
            lastTime = seconds();
          }
          FastLED.show();
         
          delay(50);
        }
        quit = true;
        if(!changed)
          ledOnStatus = 2;
      }

      if (detectMovementOnSensor(0)) {
        int lastLine = lineNumber;
        int lastLateralLedIndex = i;
        bool comingFromDown = false;
        int downLine = numberOfStaircases - 1;
        ledOnTime = seconds();
        lastTime = 0;
        lineNumber = 0;
        for (int i = numOfLeds - 1, j = lastLateralLedIndex, k = firstLedIndexForLateralLine; i >= firstLedIndexForLateralLine; i--) {
           getDistances();
          if (detectMovementOnSensor(0) || detectMovementOnSensor(1)){
            ledOnTime = seconds();
            if(detectMovementOnSensor(0))
              ledOnStatus = 1;
            else
              ledOnStatus = 2;
              changed = true;
          }

          if(detectMovementOnSensor(1) && !comingFromDown)
              comingFromDown = true;
              
          leds[i] = CRGB(255, 0, 0);
          if(comingFromDown && k < i)
          {
              leds[k] = CRGB(255, 0, 0);
              k++;
          }
          
          if(j < i)
             leds[j++] = CRGB(0, 0, 0);
          currentTime = seconds();

          if (currentTime - lastTime >= delayBetweenStaircase && lineNumber < numberOfStaircases) {
            if(lastLine > lineNumber && !comingFromDown)
              {
                turnOffLine(lastLine);
                lastLine--;
              }
             if(comingFromDown && downLine > lineNumber)
             {
                turnOnLine(downLine);
                downLine--;
             }
            turnOnLine(lineNumber);
            lineNumber++;
            lastTime = seconds();
          }
          FastLED.show();
         
          delay(50);
        }
        quit = true;
        if(!changed)
          ledOnStatus = 2;
      }


      if (quit)
        break;
      delay(50);
    }
    if (!quit)
      ledOnStatus = 0;
  }
}
