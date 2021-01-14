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
	delay(10);
	bool ok = readVersion();
	DEBUG_PRINT("Initialisation MCP39F501...");
	DEBUG_PRINTLN(systemVersion);
	return ok;	
}

bool ValTronix_MCP39F501::readVersion()
{
	beginFrame();
	read16(0x0002);
	systemVersion = "0000-00-00";
	if (sendFrame(2) == MCP39F501_OK)
	{
		uint8_t i = bufin[2]>>4;
		if (i > 9) systemVersion[2] = '1';
		systemVersion[3] = '0' + (i%10);
		systemVersion[0] = '2';
		i = bufin[2] & 0x0f;
		if (i > 9) systemVersion[5] = '1';
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

void ValTronix_MCP39F501::setDioRegister(uint8_t dio0, uint8_t dio1, uint8_t dio2, uint8_t dio3)
{
	dio0 &= 0x07;
	dio1 &= 0x07;
	dio2 &= 0x07;
	dio3 &= 0x07;
	uint16_t reg = dio3<<9 | dio2<<6 | dio1<<3 | dio0;
	beginFrame();
	write16(0x0046, reg);
	sendFrame(0);
}

uint16_t ValTronix_MCP39F501::getEnableEvents()
{
	return getRegister16(0x008a);
}

void ValTronix_MCP39F501::setEnableEvents(uint16_t events)
{
	setRegister16(0x008a, events);
}

void ValTronix_MCP39F501::testEvents(uint16_t events)
{
	setRegister16(0x0090, events);
}

void ValTronix_MCP39F501::clearEvents(uint16_t events)
{
	setRegister16(0x0092, events);
}

uint16_t ValTronix_MCP39F501::readEvents()
{
	return getRegister16(0x001c);
}

uint16_t ValTronix_MCP39F501::getCriticalMask()
{
	return getRegister16(0x008c);
}

void ValTronix_MCP39F501::setCriticalMask(uint16_t events)
{
	setRegister16(0x008c, events);
}

uint16_t ValTronix_MCP39F501::getStandardMask()
{
	return getRegister16(0x008e);
}

void ValTronix_MCP39F501::setStandardMask(uint16_t events)
{
	setRegister16(0x008e, events);
}


bool ValTronix_MCP39F501::refresh()
{
	beginFrame();
	readBytes(0x0004, 28);
	uint8_t rc = sendFrame(28);
	DEBUG_PRINT("sendFrame rc=");
	DEBUG_PRINTLN(rc);
	if (rc == MCP39F501_OK)
	{
		currentRms = ((uint32_t)bufin[5])<<24 | ((uint32_t)bufin[4])<<16 | ((uint32_t)bufin[3])<<8 | bufin[2];
		//currentRms = (uint32_t)bufin[5];
		//currentRms = currentRms<<8 | (uint32_t)bufin[4];
		//currentRms = currentRms<<8 | (uint32_t)bufin[3];
		//currentRms = currentRms<<8 | (uint32_t)bufin[2];
		DEBUG_PRINT("Current RMS: ");
		DEBUG_PRINTLN(currentRms);
		voltageRms = ((uint16_t)bufin[7]<<8) | (uint16_t)bufin[6];
		DEBUG_PRINT("Voltage RMS:");
		DEBUG_PRINTLN(voltageRms);
		activePower = bufin[11]<<24 | bufin[10]<<16 | bufin[9]<<8 | bufin[8];
		DEBUG_PRINT("Active power:");
		DEBUG_PRINTLN(activePower);
		reactivePower = bufin[15]<<24 | bufin[14]<<16 | bufin[13]<<8 | bufin[12];
		DEBUG_PRINT("Reactive power:");
		DEBUG_PRINTLN(reactivePower);
		apparentPower = bufin[19]<<24 | bufin[18]<<16 | bufin[17]<<8 | bufin[16];
		DEBUG_PRINT("Apparent power:");
		DEBUG_PRINTLN(apparentPower);
		powerFactor = ((int16_t)bufin[21])<<8 | (int16_t)bufin[20];
		DEBUG_PRINT("Power factor:");
		DEBUG_PRINTLN(powerFactor);
		lineFrequency = ((uint16_t)bufin[23]<<8) | (uint16_t)bufin[22];
		DEBUG_PRINT("Line frequency:");
		DEBUG_PRINTLN(lineFrequency);
		thermistorVoltage = ((uint16_t)bufin[25]<<8) | (uint16_t)bufin[24];
		DEBUG_PRINT("Thermistor voltage:");
		DEBUG_PRINTLN(thermistorVoltage);
		eventFlag = bufin[27]<<8 | bufin[26];
		DEBUG_PRINT("Event flag:");
		DEBUG_PRINTLN(eventFlag, BIN);
		systemStatus = bufin[29]<<8 | bufin[28];
		DEBUG_PRINT("System status:");
		DEBUG_PRINTLN(systemStatus, BIN);
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



uint16_t ValTronix_MCP39F501::getRegister16(uint16_t addr)
{
	beginFrame();
	read16(addr);
	if (sendFrame(2) == MCP39F501_OK)
	{
		return bufin[2]<<8 | bufin[3];
	}
	else
	{
		return 0xffff;
	}
}

void ValTronix_MCP39F501::setRegister16(uint16_t addr, uint16_t data)
{
	beginFrame();
	write16(addr, data);
	sendFrame(0);
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
	DEBUG_PRINT("Envoi du buffer:");
	for (int j=0; j<bufout[1]; j++)
	{
		DEBUG_PRINT(" 0x");
		DEBUG_PRINT(bufout[j], HEX);
	}
	_serial->flush();
	DEBUG_PRINTLN(" OK");

	DEBUG_PRINT("Reception :");
	uint8_t rc = MCP39F501_OK;
	uint8_t idx = 0;
    bool waiting = true;
	while (waiting)
	{
		uint8_t i = 20;
		DEBUG_PRINT('<');
		while(waiting && !_serial->available())
		{
			DEBUG_PRINT('*');
			if ((expectedLength == 0) && (idx == 1))
			{
//				rc = MCP39F501_OK;
				waiting = false;
			}
			else if (idx >= (expectedLength + 2))
			{
				if (bufin[1] != idx)
				{
					DEBUG_PRINT(" Bad size");
					rc = MCP39F501_INSIZE;
				}
				waiting = false;
			}
			else
			{
				DEBUG_PRINT('.');
				delay(5);
				i--;
				if (i == 0)
				{
					waiting = false;
					if (rc == MCP39F501_OK)
					{
						rc = MCP39F501_TIMEOUT;
						DEBUG_PRINT(" Timeout");
					}
				}
			}
		}
		DEBUG_PRINTLN('>');
		DEBUG_PRINT("<<");
		while (_serial->available() > 0)
		{
			DEBUG_PRINT('*');
			uint8_t b = _serial->read();
			if (rc == MCP39F501_OK)
			{
				if (idx < sizeof(bufin))
				{
					bufin[idx++] = b;
					DEBUG_PRINT(" 0x");
					DEBUG_PRINT(b, HEX);
					if (i == 1)
					{
						switch (b)
						{
							case 0x51: // CSFAIL
								DEBUG_PRINT(" CSFAIL");
								rc = MCP39F501_CSFAIL;
								break;
							case 0x06: // ACK
								DEBUG_PRINT(" ACK");
								break;
							default:   // Unexpected
								DEBUG_PRINT(" ?!?");
								rc = MCP39F501_INBAD;
								break;
						}
					}
				}
				else
				{
					DEBUG_PRINT(" Overflow");
					rc = MCP39F501_INOVFLOW;
				}
			}
		}
		DEBUG_PRINT(">>");
	}
	
	if (rc == MCP39F501_OK)
	{
		if ((idx > 1) && (crc(bufin, bufin[1] - 1) != bufin[idx - 1]))
		{
			DEBUG_PRINT(" Bad Checksum");
			rc = MCP39F501_BADCSUM;
		}
		else
		{
			DEBUG_PRINT(" Ok");
		}
	}
	DEBUG_PRINTLN();
	bufout[1] = 0;
	DEBUG_PRINT("Retour de sendFrame: ");
	DEBUG_PRINTLN(rc);
	return rc;
}

uint8_t ValTronix_MCP39F501::crc(uint8_t *buf, int length)
{
	uint8_t crc = 0;
	for (int i=0; i<length; i++)
	{
		crc += buf[i];
	}
	DEBUG_PRINT(" [0x");
	DEBUG_PRINT(crc, HEX);
	DEBUG_PRINT("]");
	return crc;
}

void ValTronix_MCP39F501::read16(uint16_t addr)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x52);			// Register Read, 16 bits
}

void ValTronix_MCP39F501::write16(uint16_t addr, uint16_t data)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x57);			// Register Write, 16 bits
	write(highByte(data));
	write(lowByte(data));
}

void ValTronix_MCP39F501::read32(uint16_t addr)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x44);			// Register Read, 32 bits
}

void ValTronix_MCP39F501::write32(uint16_t addr, uint32_t data)
{
	write(0x41);			// Set Address Pointer
	write(highByte(addr));
	write(lowByte(addr));
	write(0x57);			// Register Write, 32 bits
	write((data>>24) & 0xff);
	write((data>>16) & 0xff);
	write((data>>8) & 0xff);
	write(data & 0xff);
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
