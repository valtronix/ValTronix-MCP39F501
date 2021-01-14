#include <ValTronix_MCP39F501.h>

/*
 * VV  VV    AA    LL      TTTTTT  RRRR     OOOO   NN  NN  IIIIII  XX  XX
 * VV  VV  AA  AA  LL        TT    RR  RR  OO  OO  NNN NN    II     X  X
 *  V  V   AAAAAA  LL        TT    RRRRR   OO  OO  NN NNN    II      XX
 *  VVVV   AA  AA  LL        TT    RR  RR  OO  OO  NN  NN    II     X  X
 *   VV    AA  AA  LLLLLL    TT    RR  RR   OOOO   NN  NN  IIIIII  XX  XX
 *   
 * Sensor Test using ValTronix MCP39F501 Library
 * (c)2020 ValTronix
 */

ValTronix_MCP39F501 nrjsensor(&Serial1);

void setup() {
  Serial.begin(9600);
  Serial.println("Looking for MCP39F501 sensor...");

  // Energy module
  /* The default baud rate is 4.8 kbps. The UART operates
   * in 8-bit mode, plus one start bit and one stop bit, for a
   * total of 10 bits per byte.
   */
  if (nrjsensor.begin())
  {
    Serial.print("Sensor hardware version: ");
    Serial.println(nrjsensor.systemVersion);
  }
  else
  {
    Serial.println("Sensor not found!");
  }

}

void loop() {
}
