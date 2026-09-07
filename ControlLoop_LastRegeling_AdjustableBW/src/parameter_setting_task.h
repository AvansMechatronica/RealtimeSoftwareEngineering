/*
 * parameter_setting_task.h
 *
 * Created: 23-11-2023 13:20:37
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 


#ifndef PARAMETERSETTINGTASK_H_
#define PARAMETERSETTINGTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// #define's

// Bandwidth limits

#define WBLFACTOR_MIN	0.100
#define WBLFACTOR_MAX	0.400

#define WBLTHRESHOLD	0.001

//////////////////////////////////////////////////////////////////////////////
// function prototypes

void ParameterSettingTask(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* PARAMETERSETTINGTASK_H_ */