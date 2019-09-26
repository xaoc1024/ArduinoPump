//#include <EEPROMex.h>
//#include <EEPROMVar.h>

// include the library code:
#include <LiquidCrystal.h>
//#include <Wire.h>
#include <EEPROMex.h>

const byte CALIBRATION_MENU_POS = 0;

const int dirPin = 12; 
const int stepPin = 13;  
const int enPin = 11;
const byte menuItemsCount = 3;
const String items[menuItemsCount] = {"Calibration", "Regime", "Back"};
const float expectedCallibratedMilliliters = 1.0;
const float defaultCalibrationCoeficient = 100.0; // тут має бути таке число, щоб якщо його помножити на milliliters - результуючий тон відповідмав бажаним ml/h

int defaultSpeed = 100;

enum Key {
  Select,
  Up,
  Down,
  Left,
  Right,
  None
};

enum State {
  Default,
  Menu,
  Calibration,
  Regime
};

float actualCallibratedMilliliters = expectedCallibratedMilliliters;
float calibrationCoeficient = defaultCalibrationCoeficient; // це число треба буде підібрати 
int milliliters = 20; 

byte currentMenuPosition = 0;
State state = Default;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite (dirPin, HIGH);
  digitalWrite (enPin, HIGH);
  showDefaultState();
}

void loop() {
  processKeyInput();
}

void processKeyInput() {
  switch (key()) {
   case Select:
    handleSelect();
    break;

   case Left:
    handleLeft();
    break;

   case Right:
    handleRight();
    
    break;

   case Up:
    handleUp();
    break;   

   case Down:
    handleDown();
    break;  
  }
}

void handleSelect() {
  switch (state) {
    case Default:
      showMenu();
      break;
    
    case Menu:
      processMenuItem();
      break;

    case Calibration:
      applyCalibration();
      showDefaultState();
      break;
  }
}

void handleLeft() {
    switch (state) {
    case Menu:
      scrollMenuLeft();
      break;
      case Default:
      showDefaultState();
        break;
    }
}

void handleRight() {
    switch (state) {
      case Menu:
      scrollMenuRight();
        break;
      case Default:
      tone(stepPin, 9600);
      //delay(1000);
      
      //noTone(stepPin);
        break;
        
    }
}

void handleUp() {
  switch (state) {
  case Default:
    increaseSpeed();
    break;

  case Calibration:
    increaseCalibrationMillimenters();
    break;
  } 
}

void handleDown() {
  switch (state) {
  case Default:
    decreaseSpeed();
    break;

  case Calibration:
    decreaseCalibrationMillimenters();
    break;
  }
}

//////////////////////////////////////////////
// робота із кнопкою Select
void showMenu() {
  state = Menu;

  stopEngine();
  lcd.clear();
  lcd.setCursor(0, 0);
  currentMenuPosition = 0;
  lcd.print(getCurrentMenuItem());
}

void processMenuItem() {
  if (currentMenuPosition == CALIBRATION_MENU_POS) {
    startCalibration();
  } else {
    // зараз інші опції меню не підтримуються. кнопка select буде вести назад.
    showDefaultState();
  }
}

void stopEngine() {
  noTone(stepPin);
}

void startCalibration() {
  state = Calibration;

  //bool flag = false; 
//{
  
  
  //for (int i = 0; i < rotationCountFor20ml; i++) {
    //digitalWrite (stepPin, flag ? HIGH : LOW);
    //flag = !flag;
    //delayMicroseconds(10000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibrating...");
    lcd.setCursor(0,1);
    tone(stepPin, 4800, 10000);
    delay(11000);
    //noTone (stepPin);
    // пробуємо вивести відсотки програсу
    //int percent =  (int)((i / (float)rotationCountFor20ml) * 100.0);
    //lcd.print(percent);
  //}

  //lcd.setCursor(0,1);
  //lcd.print("100%% completed");
  //delay(1000);

  printCalibration();
}

void increaseCalibrationMillimenters() {
  actualCallibratedMilliliters += 1;
  printCalibration();
}

void decreaseCalibrationMillimenters() {
  // тут перевірка на білье 1 а не більше 0, 
  // тому що якщо буде нуль і ми на цей коефіцієнт десь захочемо поділити,
  // програма впаде, бо ділини на 0 не можна.
  if (actualCallibratedMilliliters > 1) {
    actualCallibratedMilliliters -= 1;
  }
  printCalibration();
}

void printCalibration() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter ml:");
  lcd.print(actualCallibratedMilliliters);
}

void increaseSpeed() {
  milliliters += 1;
  showDefaultState();
}

void decreaseSpeed() {
  if (milliliters > 1) {
      milliliters -= 1;
  }

  showDefaultState();
}

void applyCalibration() {
  calibrationCoeficient = defaultCalibrationCoeficient / (actualCallibratedMilliliters / expectedCallibratedMilliliters);
}

void showDefaultState() {
  state = Default;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ml/h:");
  lcd.setCursor(0,1);
  lcd.print(milliliters);
  tone(stepPin, round(milliliters * calibrationCoeficient));
}

//////////////////////////////////////////////
// робота з меню

void scrollMenuLeft() {
  if (currentMenuPosition == 0) {
    currentMenuPosition = menuItemsCount - 1;
  } else {
    currentMenuPosition -= 1;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(getCurrentMenuItem());
}

void scrollMenuRight() {
  if (currentMenuPosition == menuItemsCount - 1) {
    currentMenuPosition = 0;
  } else {
    currentMenuPosition += 1;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(getCurrentMenuItem());
}

String getCurrentMenuItem() {
  return items[currentMenuPosition];
}

/////////////////////////////////////
// Робота з кнопками

Key key(){//1-719, 2 - 477, 3 - 131, 4 -305, 5 - 0
  delay(100);
  int val = analogRead(0);
  delay(100);
  if (val < 100) return Right;
  else if (val < 200) return Up;
  else if (val < 400) return Down;
  else if (val < 600) return Left;
  else if (val < 800) return Select;
  else if (val <= 1023) return None;
}
