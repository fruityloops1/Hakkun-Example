#pragma once

#include "hk/ro/RoModule.h"
#include "hk/ro/RoUtil.h"

inline const hk::ro::RoModule* getRpModule() { return hk::ro::getModuleByIndex(0); }