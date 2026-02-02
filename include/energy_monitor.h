#ifndef EnergyMonitor_EnergyMonitor_H_
#define EnergyMonitor_EnergyMonitor_H_

#include <stdint.h>

//
// Energy Monitor
//

//
// Constants
//
const int bitmask_electric_usage = 1;
const int bitmask_electric_cost = 2;
const int bitmask_gas_usage = 4;
const int bitmask_gas_cost = 8;
//
// Enumerations
//
typedef enum
{
    electric_usage,
    electric_cost,
    gas_usage,
    gas_cost,
} unit_type;

//
// Data structures 
//
typedef struct
{
    uint64_t timestamp;
    double electric_usage;
    double electric_cost;
    double gas_usage;
    double gas_cost;
    uint8_t status;
    
} usage_snapshot;

#endif
