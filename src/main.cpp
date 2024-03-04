#include "ADS1256.h"
#include "Adafruit_MMA8451.h"
//nodeID is used for identification of data source in the server
#define NODEID 00

float clockMHZ = 7.68; // crystal frequency used on ADS12561
float vRef = 2.5; // voltage reference

float geoX,geoY,geoZ;
ADS1256 *adc;
//Adafruit_MMA8451 *mma;
typedef struct {
    int node_ID;
    int checksum;
    float data[3];
}geophoneDATA;

//typedef struct {
//    int node_ID;
//    int checksum;
//    float data[3];
//}accelerometerDATA;

typedef union{
    float data;
    int bin;
} converter;

geophoneDATA sensorDATA;
//accelerometerDATA mmaDATA;
void setup()
{
    adc = new ADS1256(clockMHZ,vRef,false);
//    mma = new Adafruit_MMA8451();
    Serial.begin(115200);
    Serial.println("Starting ADC");
//    Serial.println("Starting MMA");
//    if (! mma->begin()) {
//        Serial.println("Couldnt start");
//        while (1);
//    }

    sensorDATA.node_ID=NODEID;
    sensorDATA.checksum=0;
    for(int i=0; i<3; i++){
          sensorDATA.data[i]=0;
    }

    adc->begin(ADS1256_DRATE_1000SPS,ADS1256_GAIN_1,false);
    adc->offsetCalibration();

    Serial.println("ADC Started");
//    Serial.println("MMA8451 found!");

    adc->setChannel(0,1);
}
int calculateChecksum(geophoneDATA geoReadings);
void serializeStruct(Stream &stream, const void *geoReadings, size_t size);
void loop()
{
    adc->waitDRDY(); // wait for DRDY to go low before next register read
    sensorDATA.data[0]=adc->readCurrentChannel(); // read as voltage according to gain and vref
    Serial.println(sensorDATA.data[0]);
    adc->waitDRDY();
    adc->setChannel(2,3);
    sensorDATA.data[1]=adc->readCurrentChannel();
    Serial.println(sensorDATA.data[1]);

    adc->waitDRDY();
    adc->setChannel(4,5);
    sensorDATA.data[2]=adc->readCurrentChannel();
    Serial.println(sensorDATA.data[2]);

    sensorDATA.checksum= calculateChecksum(sensorDATA);
    serializeStruct(Serial, &sensorDATA,sizeof(sensorDATA));
}

int calculateChecksum(geophoneDATA geoReadings){
    int checksum=0;
    converter conv;
    for (int i=0;i<3; i++){
        conv.data=geoReadings.data[i];
        checksum+=conv.bin;
    }
    if (checksum==0) return -checksum;
    return checksum;
}
void serializeStruct(Stream &stream, const void *geoReadings, size_t size) {
    // sof marker 0x7E
    uint8_t sof = 0x7E;
    stream.write(sof);
    // length of the packet
    uint8_t length = size + 2;
    stream.write(length);
    const uint8_t *ptr = (const uint8_t *)geoReadings;
    for (size_t i = 0; i < size; i++) {
        stream.write(ptr[i]);
    }
};