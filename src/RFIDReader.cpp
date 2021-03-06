#include "RFIDReader.h"

// Clear the serial buffer
void RFIDReader::flush() {
    while (Serial.available()) {
        Serial.read();
        log("Flushed byte from serial buffer");
    }
}

// Returns the RFID card number of what was read.
// No checking is done for the validity of the card or the data. It will return whatever it read from the serial buffer.
// Returns negative values for logical errors and for reads that are too close together.
long RFIDReader::readCard() {
    // Ignore reads that are too close together in time
    if ((millis() - this->lastReadTime) / 1000 < Core::rfidIgnoreTime) return -1;

#ifdef RF125PS_READER
    char tagData[8];                // Stores raw tag bytes
    unsigned char byteCounter = 0;  // Count how many bytes have been read from serial
    unsigned char tagDataIndex = 0;
    while (Serial.available()) {
        // Skip unused bytes and flush and extra bytes
        if (byteCounter < 3 || byteCounter > 10) {
            byteCounter++;
            Serial.read();
            continue;
        }

        // Load useful bytes into tagData
        tagData[tagDataIndex] = Serial.read();

        // Increment
        tagDataIndex++;
        byteCounter++;
    }

    // Convert raw bytes to hex number represented in ASCII string
    String tagStr = String(tagData);

    // Convert hex number to decimal
    return strtol(tagStr.c_str(), nullptr, 16);
#endif
#ifdef OLD_READER
    char tagBytes[6];

    if (Serial.readBytes(tagBytes, 5) == 5) {
        uint32_t cardId = 0;
        //tagBytes[6] = 0;

        for (int i = 0; i < 4; i++) {
            cardId = cardId << 8 | tagBytes[i];
        }
        this->flush();
        return cardId;
    }
    return -1;
#endif
}
