/*!
 *  @file ValTronix_MCP39F501.cpp
 *
 *  @mainpage MCP39F501, a 24-Bit Single Phase Power Monitoring sensor.
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for MCP39F501 sensors.
 *
 *  ValTronix invests time and resources providing this open source code,
 *  please support ValTronix andopen-source hardware by donating on any
 *  provided platform!
 *
 *  @section author Author
 *
 *  Written by ValTronix.
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include "ValTronix_MCP39F501.h"

/*!
 *  @brief  Constructor using default HardwareSerial
 */
ValTronix_MCP39F501::ValTronix_MCP39F501() {
	_serial = &Serial;
	systemVersion = "0000-00-00";
	powerFactor = 0x7fff;
}

/*!
 *  @brief  Constructor when using HardwareSerial
 *  @param ser Pointer to a HardwareSerial object
 */
ValTronix_MCP39F501::ValTronix_MCP39F501(HardwareSerial *ser) {
	_serial = ser;
}

bool ValTronix_MCP39F501::begin() {
  /* The default baud rate is 4.8 kbps. The UART operates
   * in 8-bit mode, plus one start bit and one stop bit, for a
   * total of 10 bits per byte.
   */
	_serial->begin(4800, SERIAL_8N1);
	return readVersion();	
}

bool ValTronix_MCP39F501::readVersion()
{
	beginFrame();
	read16(0x0002);
	systemVersion = "0000-00-00";
	if (sendFrame(2) == MCP39F501_OK)
	{
		uint8_t i = bufin[2]>>4;
		if (i > 10) systemVersion[2] = '1';
		systemVersion[3] = '0' + (i%10);
		systemVersion[0] = '2';
		i = bufin[2] & 0x0f;
		if (i > 10) systemVersion[5] = '1';
		systemVersion[6] = '0' + (i%10);
		i = bufin[3]>>4;
		systemVersion[8] = '0' + (bufin[3]>>4);
		systemVersion[9] = '0' + (bufin[3]&0x0f);
		return true;
	}
	else
	{
		return false;
	}
}


bool ValTronix_MCP39F501::refresh()
{
	beginFrame();
	readBytes(0x0004, 28);
	if (sendFrame(28) == MCP39F501_OK)
	{
		currentRms = bufin[0]<<24 + bufin[1]<<16 + bufin[2]<<8 + bufin[3];
		voltageRms = bufin[4]<<8 + bufin[5];
		activePower = bufin[6]<<24 + bufin[7]<<16 + bufin[8]<<8 + bufin[9];
		reactivePower = bufin[10]<<24 + bufin[11]<<16 + bufin[12]<<8 + bufin[13];
		apparentPower = bufin[14]<<24 + bufin[15]<<16 + bufin[16]<<8 + bufin[17];
		powerFactor = bufin[18]<<8 + bufin[19];
		lineFrequency = bufin[20]<<8 + bufin[21];
		thermistorVoltage = bufin[22]<<8 + bufin[23];
		eventFlag = bufin[24]<<8 + bufin[25];
		systemStatus = bufin[27];
		return true;
	}
	else
	{
		return false;
	}
}

bool ValTronix_MCP39F501::digitalRead(uint8_t port)
{
	port = port%4;
	return ((systemStatus>>3) & (1<<port)) != 0;
}

void ValTronix_MCP39F501::beginFrame() {
	bufout[0] = MCP39F501_HEADER;
	bufout[1] = 3;
}

void ValTronix_MCP39F501::write(uint8_t data) {
	bufout[bufout[1] - 1] = data;
	bufout[1]++;
}


uint8_t ValTronix_MCP39F501::sendFrame(uint8_t expectedLength) {
	if (bufout[1] > 2)
	{
		uint8_t len = bufout[1] - 1;
		bufout[len] = crc(bufout, len);
		_serial->write(bufout, bufout[1]);
	}
	DEBUG_PRINTER.print("Envoi du buffer:");
	for (int j=0; j<bufout[1]; j++)
	{
		DEBUG_PRINTER.print(" 0x");
		DEBUG_PRINTER.print(bufout[j], HEX);
	}
	DEBUG_PRINTER.print("... ");
	_serial->flush();
	uint8_t i = 20;
	while(!_serial->available())
	{
		delay(5);
		i--;
		if (i==0)
		{
			DEBUG_PRINTER.println(" Timeout");
			return MCP39F501_TIMEOUT;
		}
	}
	DEBUG_PRINTER.println();
	DEBUG_PRINTER.print("Reception :");
	i = 0;
	while (_serial->available() > 0)
	{
		if (i == sizeof(bufin))
		{
			DEBUG_PRINTER.println("Overflow");
			return MCP39F501_INOVFLOW;
		}
		bufin[i] = _serial->read();
		DEBUG_PRINTER.print(" 0x");
		DEBUG_PRINTER.print(bufout[i], HEX);
		i++;
	}
	if (bufin[0] == 0x51)
	{ // CSFAIL
		DEBUG_PRINTER.println(" CSFAIL");
		return MCP39F501_CSFAIL;
	}
	else if (bufin[0] != 0x06)
	{  // Unexpected
		DEBUG_PRINTER.println(" ?!?");
		return MCP39F501_INBAD;
	}
	// ACK
	DEBUG_PRINTER.println(" ACK");
	if ((expectedLength == 0) && (i == 1)) return MCP39F501_OK;
	if ((i < 2) || (bufin[1] != i)) return MCP39F501_INSIZE;
	if (i < (expectedLength + 2)) return MCP39F501_TOOSHORT;
	if (i > (expectedLength + 2)) return MCP39F501_TOOLONG;
	if (crc(bufin, bufin[1] - 1) != bufin[i - 1]) return MCP39F501_BADCSUM;
	bufout[1] = 0;
	return MCP39F501_OK;
}

uint8_t ValTronix_MCP39F501::crc(uint8_t *buf, int length)
{
	uint8_t crc = 0;
	for (int i=0; i<length; i++)
	{
		crc += buf[i];
	}
	return crc;
}

void ValTronix_MCP39F501::read16(uint16_t addr)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x52);			// Register Read, 16 bits
}

void ValTronix_MCP39F501::read32(uint16_t addr)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x44);			// Register Read, 32 bits
}

void ValTronix_MCP39F501::readBytes(uint16_t addr, uint8_t length)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x4e);			// Register Read, N bytes
	write(length);
}

void ValTronix_MCP39F501::save()
{
	write(0x53);
	write(0x00);		// Device Address
}

void ValTronix_MCP39F501::readPage(uint8_t addr)
{
	write(0x42);
	write(addr);
}

void ValTronix_MCP39F501::writePage(uint8_t addr, uint8_t* page)
{
	write(0x50);
	write(addr);
	for (int i=0; i<16; i++)
	{
		write(page[i]);
	}
}
	
void ValTronix_MCP39F501::bulkErase()
{
	write(0x4f);
}


/*
float convertPowerFactor(int16_t raw)
{
	powerFactor = raw / 32767.0;
}
*/

  /* Event Enable Register (0x008a)
   *  OC (Over Current): bit 15
   *  OT (Over Temperature): bit 9
   *  UT (Under Temperature): bit 8
   */
   //b1000001100000000
//  Serial1.write(0x83);
//  Serial1.write(0x00);
  /* PGA Current: Gain x8 (Vin +-62.5mV) b011
   * PGA Voltage: Gain 
   */

  /* Configure Digital I/O (0x0046)
   *  DIO0: output for critical events (b100)
   *  DIO1: output for standard events (b101)
   *  DIO2: temperature sensor analog input (b011)
   *  DIO3: digital input (b010)
   */
   //b0000010011101100
