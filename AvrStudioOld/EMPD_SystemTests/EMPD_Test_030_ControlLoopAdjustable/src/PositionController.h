/*
 * PositionController.h
 *
 * Created: 18-9-2023 10:52:38
 *  Author: rasmsmee
 */ 


#ifndef POSITIONCONTROLLER_H_
#define POSITIONCONTROLLER_H_

///////////////////////////////////////////////////////////////////////////////
// #defines

#define N_ADC_CHANNELS	2

///////////////////////////////////////////////////////////////////////////////
// typedefs

typedef struct
{
	uint8_t	 buttonCode;
	uint32_t sequenceNumber;
	uint16_t adcData[N_ADC_CHANNELS];
} CONTROL_STRUCT_T;


///////////////////////////////////////////////////////////////////////////////
// function prototypes

void ctrl_InitializeConstants(void);
void ctrl_ExecuteController(void);

#endif /* POSITIONCONTROLLER_H_ */