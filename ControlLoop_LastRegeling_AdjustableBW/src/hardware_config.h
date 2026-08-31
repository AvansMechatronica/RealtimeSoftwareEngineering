#ifndef HARDWARE_CONFIG_H_
#define HARDWARE_CONFIG_H_

#include "dio_lib.h"
#include "spi_lib.h"
#include "qc_7366_lib.h"
#include "dac_4922_lib.h"
#include "adc_3208_lib.h"


typedef struct {
    dio dio_device;
    spi spi_bus;
    qc7366 qc;
    dac4922 dac;
    adc3208 adc;
} HardwareConfig;


bool configureHardware(HardwareConfig *config);

#endif  // HARDWARE_CONFIG_H_   