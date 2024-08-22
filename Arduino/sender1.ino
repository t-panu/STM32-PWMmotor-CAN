#include <SPI.h>
#include "mcp2515_can.h"

#define CAN_2515
#define SPI_CS_PIN 9

mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    if (CAN.begin(CAN_125KBPS) != 0) {
        Serial.println("CAN-BUS initialization failed!");
        while (1);
    }

    Serial.println("CAN-BUS initialized.");
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        int len = command.length();
        byte data[8] = {0};
        
        for (int i = 0; i < len && i < 8; i++) {
            data[i] = command[i];
        }

        // Send data: id = 0x00, standard frame, data length = len, data buffer = data
        CAN.sendMsgBuf(0x00, 0, len, data);
        Serial.println("Sent: " + command);
    }

    delay(100); // Delay to avoid flooding the bus
}
