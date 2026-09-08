/*
 * hardware_config.cpp
 *
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

#include "hardware_config.h"

static HardwareConfig config;

///////////////////////////////////////////////////////////////////////////////
// bool ConfigureHardware(HardwareConfig *config)


HardwareConfig *ConfigureHardware(void) {

    config.dio.Init();
    config.spi_bus.Init();
    config.qc.Init(&config.spi_bus);
    config.dac.Init(&config.spi_bus);
    config.adc.Init(&config.spi_bus);
    config.buttons.Init(&config.adc);
    return &config;
}