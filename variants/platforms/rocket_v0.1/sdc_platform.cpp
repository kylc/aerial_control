#include "variant/sdc_platform.hpp"

#include "hal.h"

// SDC1 configuration
static const SDCConfig SDC1_CONFIG {
  0   // Dummy
};

SDCPlatform::SDCPlatform() {
  sdcStart(&SDCD1, &SDC1_CONFIG);
}

SDCDriver& SDCPlatform::getSDCDriver() {
  return reinterpret_cast<SDCDriver&>(SDCD1);
}