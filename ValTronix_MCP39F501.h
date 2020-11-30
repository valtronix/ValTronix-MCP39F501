/*!
 * @file ValTronix_MCP39F501.h
 *
 * This is a library for the MCP39F501
 * sensor
 *
 * Designed specifically to work with the VTM0001-EnergyMonitor Breakout
 * ----> https://valtronix.com/index.php/realisations/vt-modules/23-energy-monitor
 *
 * These ...
 *
 * ValTronix invests time and resources providing this open source code,
 * please support ValTronix and open-source hardware by donating on
 * provided platforms!
 *
 * Written by ValTronix.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef VALTRONIX_MCP39F501_H
#define VALTRONIX_MCP39F501_H

#include "Arduino.h"

/* Uncomment to enable printing out nice debug messages. */
//#define VALTRONIX_MCP39F501_DEBUG

#define DEBUG_PRINTER                                                          \
  Serial /**< Define where debug output will be printed.                       \
          */

/* Setup debug printing macros. */
#ifdef VALTRONIX_MCP39F501_DEBUG
#define DEBUG_PRINT(...)                                                       \
  { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...)                                                     \
  { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...)                                                       \
  {} /**< Debug Print Placeholder if Debug is disabled */
#define DEBUG_PRINTLN(...)                                                     \
  {} /**< Debug Print Line Placeholder if Debug is disabled */
#endif

#define MCP39F501_HEADER 0xa5
#define MCP39F501_OK 0x00
#define MCP39F501_TIMEOUT 0xff
#define MCP39F501_CSFAIL 0x01
#define MCP39F501_BADCSUM 0x02
#define MCP39F501_INOVFLOW 0x03
#define MCP39F501_OUTOVFLOW 0x04
#define MCP39F501_TOOSHORT 0x05
#define MCP39F501_TOOLONG 0x06
#define MCP39F501_INBAD 0x07
#define MCP39F501_INSIZE 0x08

/*!
 * @brief Main ValTronix_MCP39F501 class
 */
class ValTronix_MCP39F501 {
public:
  /* Constructors */
  ValTronix_MCP39F501();
  ValTronix_MCP39F501(HardwareSerial *ser);  // Constructor when using HardwareSerial
  
  /*!
   * @brief Starts ValTronix_MCP39F501
   * @return Returns true if successful
   */
  bool begin();
  bool readVersion();
  bool refresh();
  bool digitalRead(uint8_t port);
  char* systemVersion;
  uint32_t currentRms;
  uint16_t voltageRms;
  uint32_t activePower;
  uint32_t reactivePower;
  uint32_t apparentPower;
  int16_t powerFactor;
  uint16_t lineFrequency;
  uint16_t thermistorVoltage;

private:
  void beginFrame();
  uint8_t sendFrame(uint8_t expectedLength);
  void write(uint8_t data);
  void read16(uint16_t addr);
  void read32(uint16_t addr);
  void readBytes(uint16_t addr, uint8_t length);
  uint8_t crc(uint8_t *buf, int length);
  void save();
  void readPage(uint8_t addr);
  void writePage(uint8_t addr, uint8_t* page);
  void bulkErase();
  HardwareSerial *_serial;
  uint8_t bufin[64];
  uint8_t bufout[64];
  uint16_t eventFlag;
  uint8_t systemStatus;
};

#endif //  VALTRONIX_MCP39F501_H
