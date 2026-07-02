#include <Wire.h>
#include <AFS01Sensor.h>

AFS01Sensor flowSensor;

void setup() {
    Serial.begin(115200);
    flowSensor.begin();
    
    Serial.println("Initializing AFS01 Flow Sensor...");
    delay(1000); // Stability delay
    
    uint32_t sensorID = 0;
    if (flowSensor.readSensorID(sensorID)) {
        Serial.print("Authenticated Sensor ID: 0x");
        Serial.println(sensorID, HEX);
    } else {
        Serial.println("Error: Failed to confirm Sensor ID check.");
    }
}

void loop() {
    int16_t currentFlow = 0;
    
    if (flowSensor.readFlow(currentFlow)) {
        Serial.print("Flow Rate: ");
        Serial.print(currentFlow);
        Serial.println(" sccm"); // Standard Conditions[cite: 1]
    } else {
        Serial.println("Error: Communication disruption or bad checksum.");
    }
    
    // Default datasheet internal sampling cycle updates every 500ms[cite: 1]
    delay(500); 
}