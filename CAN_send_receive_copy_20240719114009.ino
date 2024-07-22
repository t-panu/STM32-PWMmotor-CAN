/*
    This is an example of sending and receiving on its own, connect two channels to each other using jumpers.
*/

#include <SPI.h>
#define CAN_2515
// #define CAN_2518FD





// Set SPI CS Pin according to your hardware
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
// *****************************************
// For Arduino MCP2515 Hat:
// SPI_CS Pin: D9
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10




#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

// TEST TEST MCP2518FD CAN2.0 data transfer
#define MAX_DATA_SIZE 8
// To TEST MCP2518FD CANFD data transfer, uncomment below lines
// #undef  MAX_DATA_SIZE
// #define MAX_DATA_SIZE 64

const int SPI_CS_PIN_SEND = 9;
const int SPI_CS_PIN_RECEIVE = 10;

mcp2518fd CAN_SEND(SPI_CS_PIN_SEND);
mcp2518fd CAN_RECEIVE(SPI_CS_PIN_RECEIVE);

#else
#ifdef CAN_2515
#include "mcp2515_can.h"

#define MAX_DATA_SIZE 8
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;

mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#else
#error "Please define CAN_2518FD or CAN_2515 marco"
#endif
#endif//CAN_2518FD


unsigned char len = 0;
unsigned char buf[8];

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR) {}

    while(!Serial); // wait for Serial
#ifdef CAN_2518FD
    if (CAN_SEND.begin(CAN_500K_1M) != 0 || CAN_RECEIVE.begin(CAN_500K_1M) != 0) {
#else
    #if MAX_DATA_SIZE > 8
    /*
     * To compatible with MCP2515 API,
     * default mode is CAN_CLASSIC_MODE
     * Now set to CANFD mode.
     */
    CAN.setMode(CAN_NORMAL_MODE);
    #endif

    if (CAN.begin(CAN_125KBPS) != 0) {
#endif
      SERIAL_PORT_MONITOR.println("CAN-BUS initiliased error!");
      while(1);
    }
    
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

uint32_t id;
uint8_t  type; // bit0: ext, bit1: rtr

byte cdata[MAX_DATA_SIZE] = {0};
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void loop() {
    
#ifdef CAN_2518FD
// send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    stmp[7] = stmp[7] + 1;
    if (stmp[7] == 100) {
        stmp[7] = 0;
        stmp[6] = stmp[6] + 1;

        if (stmp[6] == 100) {
            stmp[6] = 0;
            stmp[5] = stmp[5] + 1;
        }
    }
    CAN_SEND.sendMsgBuf(0x00, 0, 8, stmp);
#else
    type = random(4);
    if (type & 0x1) {
        // total 29 bits
        // Arduino AVR only generate up to 16 bits random number
        id = random(0x1U << 14);
        id |= (uint32_t)random(0x1U << 15) << 14;
    } else {
        id = random(0x1U << 11);
    }
    if (type & 0x2) {
        len = 0;
        // remote frame could also carry data
        // but don't do it.
        // len = random(0, MAX_DATA_SIZE + 1);
    } else {
        len = random(0, MAX_DATA_SIZE + 1);
    }

    int i;
    for (i = 0; i < len; i++) {
        cdata[i] = random(0x100);
    }
    #if MAX_DATA_SIZE > 8
    // pad CANFD extra bytes with 0
    for (i = len; i < MAX_DATA_SIZE; i++) {
        cdata[i] = 0;
    }
    #endif
    CAN.sendMsgBuf(id, bool(type & 0x1),
                       bool(type & 0x2),
                     #if MAX_DATA_SIZE > 8
                       CANFD::len2dlc(len),
                     #else
                       len,
                     #endif
                       cdata);
#endif
    delay(1000);                       // send data per 100ms
    SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");

    // ---------------------
    
#ifdef CAN_2518FD        
    if (CAN_MSGAVAIL == CAN_RECEIVE.checkReceive()) {
    // read data,  len: data length, buf: data buf
      SERIAL_PORT_MONITOR.println("checkReceive");
      CAN_RECEIVE.readMsgBuf(&len, buf);

       // print the data
      for (int i = 0; i < len; i++) {
        SERIAL_PORT_MONITOR.print(buf[i]); SERIAL_PORT_MONITOR.print(" ");
      }
#else 
    unsigned long t = millis();
    if (CAN_MSGAVAIL != CAN.checkReceive()) {
        return;
    } else {
    
    char prbuf[32 + MAX_DATA_SIZE * 3];
    int i, n;

    unsigned long t = millis();
    // read data, len: data length, buf: data buf
    CAN.readMsgBuf(&len, cdata);
  
    id = CAN.getCanId();
    type = (CAN.isExtendedFrame() << 0) |
          (CAN.isRemoteRequest() << 1);
    /*
    * MCP2515(or this driver) could not handle properly
    * the data carried by remote frame
    */
    n = sprintf(prbuf, "%04lu.%03d ", t / 1000, int(t % 1000));
    /* Displayed type:
    *
    * 0x00: standard data frame
    * 0x02: extended data frame
    * 0x30: standard remote frame
    * 0x32: extended remote frame
    */
    static const byte type2[] = {0x00, 0x02, 0x30, 0x32};
    n += sprintf(prbuf + n, "RX: [%08lX](%02X) ", (unsigned long)id, type2[type]);
    // n += sprintf(prbuf, "RX: [%08lX](%02X) ", id, type);

    for (int i = 0; i < len; i++) {
        n += sprintf(prbuf + n, "%02X ", buf[i]);
    }
    SERIAL_PORT_MONITOR.println(prbuf);
    CAN.sendMsgBuf(id, 0, len, cdata);
#endif
   
    SERIAL_PORT_MONITOR.println();
    }
    SERIAL_PORT_MONITOR.println("---LOOP END---");
}

// END FILE
