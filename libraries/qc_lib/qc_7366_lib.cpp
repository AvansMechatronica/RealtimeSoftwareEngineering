///////////////////////////////////////////////////////////////////////////////
//
// qc_7366_lib.cpp
//
// Author:	 	Roel Smeets
// Edit date: 	14-10-2022
//				25-06-2025
// Revision: 	V2.0
// Modified: 	07-09-2026
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

void qc7366::Init(spi_device *spi_bus)
{
	if (spi_bus == nullptr)
	{
		return;
	}

	this->spi_bus = spi_bus;

	uint8_t channel		 = 0;
	uint8_t defaultMode  = 0;
	mode_register_t modeRegister = QC_MODE_REGISTER_0;
	
	// mode depends on quadrature pulse definitions of the encoder used!!
	defaultMode = MODE_QC_1 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	//spi_bus->Init();
	//spi_bus->DeselectDevice();

	QCSPISettings._bitOrder = SPI_MSBFIRST;
	QCSPISettings._dataMode = SPI_MODE0;
	QCSPISettings._clock 	= SPI_QC_SPEED;

	//spi_bus->BeginTransaction(QCSPISettings);
	//spi_bus->EndTransaction();

	for (channel = 0; channel <= QC_MAX_CHANNEL; channel++)
	{
		WriteModeRegister(channel, modeRegister, defaultMode);
		DisableCounter(channel);
		ClearCountRegister(channel);
	}

}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc7366::GetSPIDeviceNumber(uint8_t qcChannel)

uint8_t qc7366::GetSPIDeviceNumber(uint8_t qcChannel)
{

	
	if (qcChannel <= QC_MAX_CHANNEL)
	{
		if (qcChannel == 0)
		{
			return SPI_DEVICE_QC0;
		}
		else if (qcChannel == 1)
		{
			return SPI_DEVICE_QC1;
		}
	}
	return SPI_DEVICE_QC0;
}


///////////////////////////////////////////////////////////////////////////////
// void qc7366::ClearStatusRegister(uint8_t channel)

void qc7366::ClearStatusRegister(uint8_t channel)
{
	SendCommand(channel, CLR_STR);
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc7366::ReadStatusRegister(uint8_t channel)

uint8_t qc7366::ReadStatusRegister(uint8_t channel)
{
	uint8_t statusValue = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{

		uint8_t spiDevice = GetSPIDeviceNumber(channel);
		spi_bus->BeginTransaction(QCSPISettings, spiDevice);
	
	
		spi_bus->WriteByte(READ_STR);
		spi_bus->ReadByte(&statusValue);


		spi_bus->EndTransaction();
	}
	
	return statusValue;
}


///////////////////////////////////////////////////////////////////////////////
// bool qc7366::IsIndexSet(uint8_t channel)

bool qc7366::IsIndexSet(uint8_t channel)
{
	bool indexSet = false;
	uint8_t statusValue = 0;
	
	statusValue = ReadStatusRegister(channel);
	
	if ((statusValue & IDX_BIT) != 0)
	{
		indexSet = true;
	}
	
	return indexSet;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::WriteModeRegister(uint8_t channel, mode_register_t modeRegister, 
//							 uint8_t valueMDR)

void qc7366::WriteModeRegister(uint8_t channel, mode_register_t modeRegister, uint8_t valueMDR)
{
	uint8_t writeMDRCommand = 0;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		writeMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? WRITE_MDR0 : WRITE_MDR1;
		
		spi_bus->BeginTransaction(QCSPISettings, GetSPIDeviceNumber(channel));


		spi_bus->WriteByte(writeMDRCommand);
		spi_bus->WriteByte(valueMDR);

	
		spi_bus->EndTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc7366::ReadModeRegister(uint8_t channel, mode_register_t modeRegister)

uint8_t qc7366::ReadModeRegister(uint8_t channel, mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	uint8_t mdrValue = 0xff;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? READ_MDR0 : READ_MDR1;
	
		spi_bus->BeginTransaction(QCSPISettings, GetSPIDeviceNumber(channel));

		spi_bus->WriteByte(readMDRCommand);
		spi_bus->ReadByte(&mdrValue);

		spi_bus->EndTransaction();
	}
	
	return mdrValue;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::ClearModeRegister(uint8_t channel, mode_register_t modeRegister)

void qc7366::ClearModeRegister(uint8_t channel, mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? CLR_MDR0 : CLR_MDR1;
		SendCommand(channel, readMDRCommand);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::ClearCountRegister(uint8_t channel)

void qc7366::ClearCountRegister(uint8_t channel)
{
	SendCommand(channel, CLR_CNTR);
}

///////////////////////////////////////////////////////////////////////////////
// int32_t qc7366::ReadCountRegister(uint8_t channel)

int32_t  qc7366::ReadCountRegister(uint8_t channel)
{
	int32_t count = 0;
	uint8_t ix	  = 0;
	uint8_t val	  = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{

		spi_bus->BeginTransaction(QCSPISettings, GetSPIDeviceNumber(channel));

		spi_bus->WriteByte(READ_CNTR);
		for (ix = 0; ix < 4; ix++)
		{
			spi_bus->ReadByte(&val);
			count = (count << 8) | val;
		}

		//spi_bus->DeselectDevice();
		spi_bus->EndTransaction();
	}
	
	return count;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::TransferDataRegisterToCountRegister(uint8_t channel)

void qc7366::TransferDataRegisterToCountRegister(uint8_t channel)
{
	SendCommand(channel, LOAD_CNTR);
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::WriteDataRegister(uint8_t channel, int32_t dtrValue)

void qc7366::WriteDataRegister(uint8_t channel, int32_t dtrValue)
{
	uint8_t ix = 0;
	uint8_t spiData = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_bus->BeginTransaction(QCSPISettings, GetSPIDeviceNumber(channel));

		spi_bus->WriteByte(WRITE_DTR);
		for (ix = 0; ix < 4; ix++) // Most Significant byte first!
		{
			spiData = (uint8_t)(dtrValue >> 8*(3 - ix));	// shift right 24, 16, 8, 0
			spi_bus->WriteByte(spiData);
		}		

		//spi_bus->DeselectDevice();
		spi_bus->EndTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// int32_t qc7366::ReadOutputRegister(uint8_t channel)

int32_t qc7366::ReadOutputRegister(uint8_t channel)
{
	int32_t count = 0;
	uint8_t ix = 0;
	uint8_t val = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_bus->BeginTransaction(QCSPISettings, GetSPIDeviceNumber(channel));

		spi_bus->WriteByte(READ_OTR);
		for (ix = 0; ix < 4; ix++)
		{
			spi_bus->ReadByte(&val);
			count = (count << 8) | val;
		}

		//spi_bus->DeselectDevice();
		spi_bus->EndTransaction();
	}
	
	return count;
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::DisableCounter(uint8_t channel)

void qc7366::DisableCounter(uint8_t channel)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = qc7366::ReadModeRegister(channel, QC_MODE_REGISTER_1);
	mdrValue |= CNT_DISABLE;
	qc7366::WriteModeRegister(channel, QC_MODE_REGISTER_1, mdrValue);
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::EnableCounter(uint8_t channel)

void qc7366::EnableCounter(uint8_t channel)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = qc7366::ReadModeRegister(channel, QC_MODE_REGISTER_1);
	mdrValue &= ~CNT_DISABLE;
	qc7366::WriteModeRegister(channel, QC_MODE_REGISTER_1, mdrValue);
}

///////////////////////////////////////////////////////////////////////////////
// void qc7366::SendCommand(uint8_t channel, uint8_t commandByte)

void qc7366::SendCommand(uint8_t channel, uint8_t commandByte)
{
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_bus->BeginTransaction(QCSPISettings, channel);

		spi_bus->WriteByte(commandByte);

		spi_bus->EndTransaction();
	}
}
