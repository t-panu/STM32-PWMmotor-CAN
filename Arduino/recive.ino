#include <SPI.h>
#include "mcp2515_can.h"

#define CAN_2515
#define SPI_CS_PIN 9
#define CAN_INT_PIN 2

// Define the motor control pins
#define MOTOR_IN1_PIN 7
#define MOTOR_IN2_PIN 8
#define MOTOR_PWM_PIN 9

mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    if (CAN.begin(CAN_125KBPS) != 0) {
        Serial.println("CAN-BUS initialization failed!");
        while (1);
    }

    Serial.println("CAN-BUS initialized.");
    pinMode(CAN_INT_PIN, INPUT);
    
    // Setup motor control pins
    pinMode(MOTOR_IN1_PIN, OUTPUT);
    pinMode(MOTOR_IN2_PIN, OUTPUT);
    pinMode(MOTOR_PWM_PIN, OUTPUT);

    digitalWrite(MOTOR_IN1_PIN, LOW); // Motor off initially
    digitalWrite(MOTOR_IN2_PIN, LOW); // Motor off initially
    analogWrite(MOTOR_PWM_PIN, 0);    // Set initial speed to 0
}

void loop() {
    if (digitalRead(CAN_INT_PIN) == LOW) {
        unsigned char len = 0;
        unsigned char buf[8];

        CAN.readMsgBuf(&len, buf);
        unsigned long canId = CAN.getCanId();

        Serial.print("Received: ");
        for (int i = 0; i < len; i++) {
            Serial.print((char)buf[i]);
        }
        Serial.println();

        // Assuming the first byte is a command:
        // '1' to turn on motor and '0' to turn off motor
        // The second byte is the speed (0-255)
        if (buf[0] == '1') {
            int speed = 255; // Read the speed value

            // Ensure the speed is within valid range
            speed = constrain(speed, 0, 255);

            digitalWrite(MOTOR_IN1_PIN, HIGH); // Turn motor on
            digitalWrite(MOTOR_IN2_PIN, LOW);  // Set direction
            analogWrite(MOTOR_PWM_PIN, speed); // Set speed
            Serial.print("Motor ON, Speed: ");
            Serial.println(speed);
        } else if (buf[0] == '0') {
            digitalWrite(MOTOR_IN1_PIN, LOW);  // Turn motor off
            digitalWrite(MOTOR_IN2_PIN, LOW);  // Ensure motor is off
            analogWrite(MOTOR_PWM_PIN, 0);     // Set speed to 0
            Serial.println("Motor OFF");
        }
    }
}
