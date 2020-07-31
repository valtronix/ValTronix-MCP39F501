/*!
 * @file xxx.h
 *
 * This is a library for the XXX
 * sensor
 *
 * Designed specifically to work with the XXX Breakout
 * ----> http://www.valtronix.com/xxx
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

#ifndef XXX_H
#define XXX_H

#include "Arduino.h"

/* Uncomment to enable printing out nice debug messages. */
//#define XXX_DEBUG

#define DEBUG_PRINTER                                                          \
  Serial /**< Define where debug output will be printed.                       \
          */

/* Setup debug printing macros. */
#ifdef XXX_DEBUG
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

/*!
 * @brief Main XXX class
 */
class XXX {
public:
  XXX();
  /*!
   * @brief Starts XXX
   * @param pin Pin to use
   * @return Returns true if successful
   */
  boolean begin(uint8_t pin);

private:
  uint8_t _pin;

};

#endif //  XXX_H
