/*
 * QuadratureCounters.c
 *
 * Created: 23-11-2023 11:54:44
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <asf.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// FreeRTOS includes

#include "CommandConsole.h"
#include "vPrintString.h"
#include "TaskSleep.h"

///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

#include "QC7366Lib.h"

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "QuadratureCounters.h"

///////////////////////////////////////////////////////////////////////////////
// void QCEncodersSetup(void)

void QCEncodersSetup(void)
{
	uint8_t qcChannel     = 0;
	uint8_t	qcDefaultMode = 0;
	mode_register_t qcModeRegister = QC_MODE_REGISTER_0;
	
	qcDefaultMode = MODE_QC_4 | MODE_FREERUNNING | INDEX_DISABLE | INDEX_ASYNC | FILTERCLOCK_DIV_2;

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_WriteModeRegister(qcChannel, qcModeRegister, qcDefaultMode);
		qc_EnableCounter(qcChannel);
		qc_ClearCountRegister(qcChannel);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void QCEncodersShowCount(const char *idString)

void QCEncodersShowCount(const char *idString)
{
	int32_t qcCountRegister = 0;
	uint8_t qcChannel = 0;

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qcCountRegister = qc_ReadCountRegister(qcChannel);
		vPrintString("%s channel %d: CNT = %8d\n", idString, qcChannel, qcCountRegister);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void QCEncodersClearCount(void)

void QCEncodersClearCount(void)
{
	uint8_t qcChannel = 0;

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_ClearCountRegister(qcChannel);
	}
}
