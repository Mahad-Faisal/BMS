#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN 5
MCP_CAN can(CAN_CS_PIN);

void setup() {
    Serial.begin(115200);
    delay(2000);

    SPI.begin();
    
    // Try multiple times
    for (int i = 0; i < 5; i++) {
        int result = can.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
        Serial.printf("Attempt %d result: %d\n", i+1, result);
        if (result == CAN_OK) {
            can.setMode(MCP_LOOPBACK);
            Serial.println("MCP2515 OK");
            
            byte data[8] = {1,2,3,4,5,6,7,8};
            can.sendMsgBuf(0x100, 0, 8, data);
            delay(100);
            
            if (can.checkReceive() == CAN_MSGAVAIL) {
                Serial.println("Loopback SUCCESS");
            } else {
                Serial.println("Loopback FAILED");
            }
            return;
        }
        delay(500);
    }
    Serial.println("All attempts failed");
}

void loop() {}