/*
 * ts_printf.h
 *
 * Created: 1-3-2016 20:39:37
 *  Author: Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 


#ifndef V_PRINTF_H_
#define V_PRINTF_H_

#ifdef __cplusplus
extern "C" {
#endif

void StartTsPrintfTask(void *pvParameters);
void ts_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif


#endif /* V_PRINTF_H_ */