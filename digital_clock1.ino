#include <LiquidCrystal.h>

// --- 1. PIN SETUP ---
// Matches your exact Tinkercad wiring
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Define button pins
const int btnMode = 10;
const int btnUp = 9;
const int btnDown = 8;

// --- 2. TIME VARIABLES ---
int hours = 12;     
int minutes = 0;
int seconds = 0;
bool is24Hour = false; // Start in 12-hour mode
bool isPM = false;     

// --- 3. MENU STATE ---
// 0 = Normal Clock
// 1 = Set Hours
// 2 = Set Minutes
// 3 = Set Seconds
// 4 = Toggle 12/24 Hour Format
int menuMode = 0; 

// Timer variables for non-blocking timekeeping
unsigned long previousMillis = 0;
const long interval = 1000; // 1000 milliseconds = 1 second

// Button debounce variable
unsigned long lastButtonPress = 0;

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  
  // Set up buttons. INPUT_PULLUP means we connect buttons to GND.
  pinMode(btnMode, INPUT_PULLUP);
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  
  lcd.print("Clock Starting...");
  delay(1000);
  lcd.clear();
}

void loop() {
  handleButtons();
  updateTime();
  drawDisplay();
}

// --- BUTTON HANDLING ---
void handleButtons() {
  // Read the raw states of the buttons (LOW means pressed because of INPUT_PULLUP)
  bool modePressed = (digitalRead(btnMode) == LOW);
  bool upPressed = (digitalRead(btnUp) == LOW);
  bool downPressed = (digitalRead(btnDown) == LOW);

  // --- DEBUGGING PRINTS ---
  // If you open the Serial Monitor in Tinkercad, you will see these messages
  // when you press a button. If you don't see them, your wiring is wrong!
  if (modePressed) Serial.println("MODE button is pressed!");
  if (upPressed) Serial.println("UP button is pressed!");
  if (downPressed) Serial.println("DOWN button is pressed!");

  // Debounce: Only check buttons every 250ms to prevent double-presses
  if (millis() - lastButtonPress < 250) return;

  // MODE BUTTON
  if (modePressed) {
    menuMode++;
    if (menuMode > 4) menuMode = 0; // Wrap back to normal clock
    lastButtonPress = millis();
  }

  // UP BUTTON
  if (upPressed) {
    if (menuMode == 1) { // Set Hours
      hours++;
      if (hours > 23) hours = 0;
    } 
    else if (menuMode == 2) { // Set Minutes
      minutes++;
      if (minutes > 59) minutes = 0;
    } 
    else if (menuMode == 3) { // Set Seconds
      seconds++;
      if (seconds > 59) seconds = 0;
    } 
    else if (menuMode == 4) { // Toggle 12/24 Hour
      is24Hour = !is24Hour; 
    }
    lastButtonPress = millis();
  }

  // DOWN BUTTON
  if (downPressed) {
    if (menuMode == 1) { // Set Hours
      hours--;
      if (hours < 0) hours = 23;
    } 
    else if (menuMode == 2) { // Set Minutes
      minutes--;
      if (minutes < 0) minutes = 59;
    } 
    else if (menuMode == 3) { // Set Seconds
      seconds--;
      if (seconds < 0) seconds = 59;
    } 
    else if (menuMode == 4) { // Toggle 12/24 Hour
      is24Hour = !is24Hour;
    }
    lastButtonPress = millis();
  }
}

// --- TIME KEEPING ---
void updateTime() {
  // Only tick the clock forward if we are in Normal Mode (0)
  if (menuMode == 0) {
    unsigned long currentMillis = millis();
    
    // Check if 1 second has passed
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      seconds++;
      
      if (seconds > 59) {
        seconds = 0;
        minutes++;
        if (minutes > 59) {
          minutes = 0;
          hours++;
          if (hours > 23) {
            hours = 0;
          }
        }
      }
    }
  } else {
    // If we are setting the time, keep resetting the timer 
    // so it doesn't tick while you are trying to change it
    previousMillis = millis();
  }
}

// --- DISPLAY UPDATE ---
void drawDisplay() {
  lcd.setCursor(0, 0); // Top Left corner

  // 1. Determine what hour to display (12h vs 24h)
  int displayHour = hours;
  if (!is24Hour) {
    if (hours == 0) {
      displayHour = 12;
      isPM = false;
    } else if (hours > 12) {
      displayHour = hours - 12;
      isPM = true;
    } else if (hours == 12) {
      displayHour = 12;
      isPM = true;
    } else {
      isPM = false;
    }
  }

  // 2. Print Hours
  if (displayHour < 10) lcd.print("0");
  lcd.print(displayHour);
  lcd.print(":");

  // 3. Print Minutes
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");

  // 4. Print Seconds
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(" ");

  // 5. Print AM/PM (only if in 12-hour mode)
  if (!is24Hour) {
    if (isPM) lcd.print("PM");
    else lcd.print("AM");
  } else {
    lcd.print("  "); // Blank space for 24h mode
  }

  // 6. Print Menu Status on the bottom row
  lcd.setCursor(0, 1);
  if (menuMode == 0) {
    lcd.print("Clock Running   ");
  } else if (menuMode == 1) {
    lcd.print("Set Hours       ");
  } else if (menuMode == 2) {
    lcd.print("Set Minutes     ");
  } else if (menuMode == 3) {
    lcd.print("Set Seconds     ");
  } else if (menuMode == 4) {
    lcd.print("Format: ");
    if (is24Hour) lcd.print("24-Hour ");
    else lcd.print("12-Hour ");
  }
}