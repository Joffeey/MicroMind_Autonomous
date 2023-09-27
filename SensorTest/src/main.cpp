#include <array>
#include <algorithm>

#include <Arduino.h>

// MicroMind 20.08.2023-???

// c++17 eller nyere er nødvendig for å bruke std::optional
#if __cplusplus <= 201402L
# include <bits/c++17_warning.h>
#else

#include "sensors/miso_sensor_array.hpp"

/// @brief Global numeric constants
namespace consts
{
  constexpr unsigned long BAUD_RATE {9600};

  constexpr std::size_t IR_SENSOR_ARRAY_SELECT_BITS {3};
  constexpr std::size_t IR_SENSOR_ARRAY_SENSOR_COUNT {4};
}

/// @brief Hardware GPIO-pins
namespace pins
{
  constexpr uint32_t IR_SENSOR_ARRAY_MEASURE {A2}; //P2
  constexpr uint32_t IR_SENSOR_ARRAY_ENABLE {12}; //P12
  constexpr std::array<uint32_t, consts::IR_SENSOR_ARRAY_SELECT_BITS> IR_SENSOR_ARRAY_SELECT {13, 15, 16}; //{P13, P15, P16}

  // P14 ser ut til å ikke virke
  
  constexpr auto IR_SENSOR_ARRAY {
    sensors::MISOSensorArrayPins<consts::IR_SENSOR_ARRAY_SELECT_BITS>(
      IR_SENSOR_ARRAY_MEASURE,
      IR_SENSOR_ARRAY_ENABLE,
      IR_SENSOR_ARRAY_SELECT
    )
  };
}

/// @brief Global variables
namespace globals
{
  /// @brief IR-sensors for measuring distances to walls 
  static auto ir_sensor_array {
    sensors::MISOSensorArray<consts::IR_SENSOR_ARRAY_SENSOR_COUNT, consts::IR_SENSOR_ARRAY_SELECT_BITS>(
      pins::IR_SENSOR_ARRAY
    )
  };
}

void setup()
{
  Serial.begin(consts::BAUD_RATE);

  globals::ir_sensor_array.init();
}

void loop()
{
  // Måler med alle sensorene, en etter en:
  auto measurements = globals::ir_sensor_array.measure_all();

  // Her printer jeg bare målingene (hadde vært nice å gjort det i en one-liner, ala printf)
  Serial.print("IR-measurements: [");
  for(std::size_t i = 0; i < measurements.size(); i++)
  {
    if(i != 0)
    {
      Serial.print(", ");
    }
    Serial.print(measurements[i]*100.0);
    Serial.print("%");
  }
  Serial.println("]");
}

#endif