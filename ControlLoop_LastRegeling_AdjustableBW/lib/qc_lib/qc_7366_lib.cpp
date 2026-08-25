///////////////////////////////////////////////////////////////////////////////
//
// QC7366Lib.cpp
//
// Author:	 	Roel Smeets
// Edit date: 	14-10-2022
//				25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// application includes


#include "qc_7366_lib.h"


///////////////////////////////////////////////////////////////////////////////
// void qc7366::Init(void)

void qc7366::init(spi *spi)
{
	this->spi_bus = spi;

	uint8_t channel		 = 0;
	uint8_t defaultMode  = 0;
	mode_register_t modeRegister = QC_MODE_REGISTER_0;
	
	// mode depends on quadrature pulse definitions of the encoder used!!
	defaultMode = MODE_QC_1 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	spi_bus->init();
	spi_bus->deselectDevice();

	QCSPISettings._bitOrder = SPI_MSBFIRST;
	QCSPISettings._dataMode = SPI_MODE0;
	QCSPISettings._clock 	= SPI_QC_SPEED;

	spi_bus->beginTransaction(QCSPISettings);
	spi_bus->endTransaction();

	for (channel = 0; channel <= QC_MAX_CHANNEL; channel++)
	{
		writeModeRegister(channel, modeRegister, defaultMode);
		disableCounter(channel);
		clearCountRegister(channel);
	}

}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::SelectSPIDevice(uint8_t qcChannel)

void qc7366::selectSPIDevice(uint8_t qcChannel)
{
	uint8_t spiDevice = 0;
	
	if (qcChannel <= QC_MAX_CHANNEL)
	{
		if (qcChannel == 0)
		{
			spiDevice = SPI_DEVICE_QC0;
		}
		else if (qcChannel == 1)
		{
			spiDevice = SPI_DEVICE_QC1;
		}
		spi_bus->selectDevice(spiDevice);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void qc7366::ClearStatusRegister(uint8_t channel)

void qc7366::clearStatusRegister(uint8_t channel)
{
	sendCommand(channel, CLR_STR);
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc7366::ReadStatusRegister(uint8_t channel)

uint8_t qc7366::readStatusRegister(uint8_t channel)
{
	uint8_t statusValue = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{

		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);
	
		spi_bus->writeByte(READ_STR);
		spi_bus->readByte(&statusValue);

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
	
	return statusValue;
}


///////////////////////////////////////////////////////////////////////////////
// bool qc7366::IsIndexSet(uint8_t channel)

bool qc7366::isIndexSet(uint8_t channel)
{
	bool indexSet = false;
	uint8_t statusValue = 0;
	
	statusValue = readStatusRegister(channel);
	
	if ((statusValue & IDX_BIT) != 0)
	{
		indexSet = true;
	}
	
	return indexSet;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::WriteModeRegister(uint8_t channel, mode_register_t modeRegister, 
//							 uint8_t valueMDR)

void qc7366::writeModeRegister(uint8_t channel, mode_register_t modeRegister, uint8_t valueMDR)
{
	uint8_t writeMDRCommand = 0;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		writeMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? WRITE_MDR0 : WRITE_MDR1;
		
		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);

		spi_bus->writeByte(writeMDRCommand);
		spi_bus->writeByte(valueMDR);

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc7366::ReadModeRegister(uint8_t channel, mode_register_t modeRegister)

uint8_t qc7366::readModeRegister(uint8_t channel, mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	uint8_t mdrValue = 0xff;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? READ_MDR0 : READ_MDR1;
	
		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);

		spi_bus->writeByte(readMDRCommand);
		spi_bus->readByte(&mdrValue);

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
	
	return mdrValue;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::ClearModeRegister(uint8_t channel, mode_register_t modeRegister)

void qc7366::clearModeRegister(uint8_t channel, mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? CLR_MDR0 : CLR_MDR1;
		sendCommand(channel, readMDRCommand);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::ClearCountRegister(uint8_t channel)

void qc7366::clearCountRegister(uint8_t channel)
{
	sendCommand(channel, CLR_CNTR);
}

///////////////////////////////////////////////////////////////////////////////
// int32_t qc7366::ReadCountRegister(uint8_t channel)

int32_t  qc7366::readCountRegister(uint8_t channel)
{
	int32_t count = 0;
	uint8_t ix	  = 0;
	uint8_t val	  = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{

		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);

		spi_bus->writeByte(READ_CNTR);
		for (ix = 0; ix < 4; ix++)
		{
			spi_bus->readByte(&val);
			count = (count << 8) | val;
		}

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
	
	return count;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::TransferDataRegisterToCountRegister(uint8_t channel)

void qc7366::transferDataRegisterToCountRegister(uint8_t channel)
{
	sendCommand(channel, LOAD_CNTR);
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::WriteDataRegister(uint8_t channel, int32_t dtrValue)

void qc7366::writeDataRegister(uint8_t channel, int32_t dtrValue)
{
	uint8_t ix = 0;
	uint8_t spiData = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);

		spi_bus->writeByte(WRITE_DTR);
		for (ix = 0; ix < 4; ix++) // Most Significant byte first!
		{
			spiData = (uint8_t)(dtrValue >> 8*(3 - ix));	// shift right 24, 16, 8, 0
			spi_bus->writeByte(spiData);
		}		

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// int32_t qc7366::ReadOutputRegister(uint8_t channel)

int32_t qc7366::readOutputRegister(uint8_t channel)
{
	int32_t count = 0;
	uint8_t ix = 0;
	uint8_t val = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);

		spi_bus->writeByte(READ_OTR);
		for (ix = 0; ix < 4; ix++)
		{
			spi_bus->readByte(&val);
			count = (count << 8) | val;
		}

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
	
	return count;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::DisableCounter(uint8_t channel)

void qc7366::disableCounter(uint8_t channel)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = qc7366::readModeRegister(channel, QC_MODE_REGISTER_1);
	mdrValue |= CNT_DISABLE;
	qc7366::writeModeRegister(channel, QC_MODE_REGISTER_1, mdrValue);
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::DisableCounter(uint8_t channel)

void qc7366::enableCounter(uint8_t channel)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = qc7366::readModeRegister(channel, QC_MODE_REGISTER_1);
	mdrValue &= ~CNT_DISABLE;
	qc7366::writeModeRegister(channel, QC_MODE_REGISTER_1, mdrValue);
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::SendCommand(uint8_t channel)

void qc7366::sendCommand(uint8_t channel, uint8_t commandByte)
{
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_bus->beginTransaction(QCSPISettings);
		selectSPIDevice(channel);

		spi_bus->writeByte(commandByte);

		spi_bus->deselectDevice();
		spi_bus->endTransaction();
	}
}
