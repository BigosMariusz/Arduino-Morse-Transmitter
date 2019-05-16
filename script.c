// Dolaczenie bibliotek
#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#include <Keypad.h>

#include <avr/wdt.h>

#
define SIGNALPIN 7
// Tworzenie obiektu do zarzadzania wyswietlaczem
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// Tworzenie tablicy z alfabetem Morse’a
String morseArray[26] = {
  ".-",
  "-...",
  "-.-.",
  "-..",
  ".",
  "..-.",
  "--.",
  "....",
  "..",
  ".---",
  "-.-",
  ".-..",
  "--",
  "-.",
  "---",
  ".--.",
  " ",
  ".-.",
  "...",
  "-",
  "..-",
  " ",
  ".--",
  "-..-",
  "-.--",
  "--.."
};
// Towrzenie globalnych zmiennych pomocniczych
char lastKey = 'x';
int counter = 0;
int cursorPosition = -1;
int positionOfChar = -1;
unsigned long timeValue;
char wordToConvert[16];
// Ustawianie klawiatury
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {
    'A',
    'B',
    'C'
  },
  {
    'D',
    'E',
    'F'
  },
  {
    'G',
    'H',
    'I'
  },
  {
    'J',
    'K',
    'L'
  }
};
char realKeys[8][3] = {
  {
    'A',
    'B',
    'C'
  },
  {
    'D',
    'E',
    'F'
  },
  {
    'G',
    'H',
    'I'
  },
  {
    'J',
    'K',
    'L'
  },
  {
    'M',
    'N',
    'O'
  },
  {
    'P',
    'R',
    'S'
  },
  {
    'T',
    'U',
    'W'
  },
  {
    'X',
    'Y',
    'Z'
  }
};
byte rowPins[ROWS] = {
  3,
  2,
  1,
  0
};
byte colPins[COLS] = {
  6,
  5,
  4
};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// Funkcje obslugujace konkretne zdarzenia klawiatury
void printBackspace() {
  if (cursorPosition >= 0) {
    lcd.setCursor(cursorPosition, 1);
    lcd.print(' ');
    lcd.setCursor(cursorPosition, 1);
    lastKey = 'x';
    counter = 0;
    cursorPosition--;
    wordToConvert[positionOfChar] = 'x';
    positionOfChar--;
  }
}
void resetArduino() {
  wdt_enable(WDTO_15MS);
}
void printSpace() {
  lcd.print(' ');
  lastKey = ' ';
  counter = 0;
  positionOfChar++;
  wordToConvert[positionOfChar] = ' ';
  cursorPosition++;
}
void printSelectedCharacter(char pressedKey) {
  lcd.print(changeKeyToReal(pressedKey, lastKey));
  lastKey = pressedKey;
  timeValue = millis();
}
char changeKeyToReal(char pressedKey, char lastKey) {
  if (pressedKey == lastKey && millis() - timeValue < 2000) {
    counter++;
    lcd.setCursor(cursorPosition, 1);
  } else {
    counter = 0;
    cursorPosition++;
    positionOfChar++;
  }
  if (counter > 2) {
    counter = counter % 3;
  }
  int asciiValueMinus65 = pressedKey - 65;
  char outputKey = realKeys[asciiValueMinus65][counter];
  wordToConvert[positionOfChar] = outputKey;
  return outputKey;
}
int howLongIsArray(char tab[]) {
  int localCounter = 0;
  for (int i = 0; tab[i] != '\0'; i++) {
    localCounter = i;
  }
  localCounter++;
  return localCounter;
}
void sendSignal() {
  String encodedString = convertStringToMorse();
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Wysylanie");
  lcd.setCursor(4, 1);
  lcd.print("sygnalu!");
  for (int i = 0; i < encodedString.length(); i++) {
    if (encodedString[i] == '.') {
      digitalWrite(SIGNALPIN, HIGH);
      delay(200);
      digitalWrite(SIGNALPIN, LOW);
      delay(300);
    } else if (encodedString[i] == '-') {
      digitalWrite(SIGNALPIN, HIGH);
      delay(600);
      digitalWrite(SIGNALPIN, LOW);
      delay(300);
    } else if (encodedString[i] == ' ') {
      delay(1400);
    }
  }
  resetArduino();
}
// Funkcja konwertujaca ciag znaków na kod Morse’a
String convertStringToMorse() {
  String stringToEncoding = "";
  for (int i = 0; i < howLongIsArray(wordToConvert); i++) {
    if (wordToConvert[i] != 'x') {
      stringToEncoding += morseArray[wordToConvert[i] - 65];
      stringToEncoding += ' ';
    }
  }
  return stringToEncoding;
}
void setup() {
  pinMode(SIGNALPIN, OUTPUT);
  MCUSR = 0;
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Wpisz tekst:");
  lcd.setCursor(0, 1);
}
void loop() {
  char pressedKey = customKeypad.getKey();
  if (pressedKey) {
    // Kontroler zarzadzajacy zdarzeniami klawiatury
    switch (pressedKey - 65) {
    case 8:
      printBackspace();
      break;
    case 9:
      resetArduino();
      break;
    case 10:
      printSpace();
      break;
    case 11:
      sendSignal();
      break;
    default:
      printSelectedCharacter(pressedKey);
    }
  }
}