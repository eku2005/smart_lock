#include <SPI.h> #include <MFRC522.h> #include <Adafruit_Fingerprint.h> #include <SoftwareSerial.h> #include <Servo.h>

#define RST_PIN 9 #define SS_PIN 10 #define SERVO_PIN 5 #define BUTTON_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN); SoftwareSerial fingerSerial(2, 3); Adafruit_Fingerprint finger(&fingerSerial); Servo lockServo;

String authorizedUID = "YOURUIDHERE";

void setup() { Serial.begin(9600); SPI.begin(); mfrc522.PCD_Init();

finger.begin(57600); if (!finger.verifyPassword()) { while (1); }

pinMode(BUTTON_PIN, INPUT_PULLUP); lockServo.attach(SERVO_PIN); lock(); Serial.println("System Ready. Press button for fingerprint or scan RFID."); }

void loop() { if (digitalRead(BUTTON_PIN) == LOW) { Serial.println("Fingerprint mode selected"); delay(300); int id = getFingerprintID(); if (id > 0) { unlock(); delay(5000); lock(); } else { Serial.println("Access Denied"); } } else { if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

String uid = "";
for (byte i = 0; i < mfrc522.uid.size; i++) {
  uid += String(mfrc522.uid.uidByte[i], HEX);
}
uid.toUpperCase();

Serial.print("Detected UID: ");
Serial.println(uid);

if (uid == authorizedUID) {
  unlock();
  delay(5000);
  lock();
} else {
  Serial.println("Access Denied");
}

mfrc522.PICC_HaltA();
mfrc522.PCD_StopCrypto1();

} }

void unlock() { lockServo.write(180); Serial.println("Unlocked!"); }

void lock() { lockServo.write(0); Serial.println("Locked!"); }

int getFingerprintID() { finger.getImage(); if (finger.image2Tz() != FINGERPRINT_OK) return -1; if (finger.fingerSearch() != FINGERPRINT_OK) return -1; return finger.fingerID; }
