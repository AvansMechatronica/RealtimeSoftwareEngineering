/*
 * vprintf.h
 *
 * Created: 1-3-2016 20:39:37
 *  Author: Gerard Harkema
 */ 


#ifndef V_PRINTF_H_
#define V_PRINTF_H_

#ifdef __cplusplus
extern "C" {
#endif

void Start_vPrintTask(void *pvParameters);
void vPrint(const char *format, ...);

#ifdef __cplusplus
}
#endif


#endif /* V_PRINTF_H_ */