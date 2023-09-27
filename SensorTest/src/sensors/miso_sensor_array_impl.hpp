#pragma once

#include "miso_sensor_array.hpp"

#include <cassert>
#include <algorithm>

namespace sensors
{
  // MISOSensorArrayPins impl ==========================================================================================

  // public ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // static constexpr --------------------------------------------------------------------------------------------------

  template<std::size_t SENSOR_SELECT_BITS>
  constexpr MISOSensorArrayPins<SENSOR_SELECT_BITS>::MISOSensorArrayPins(
    uint32_t measure,
    uint32_t enable,
    std::array<uint32_t, SENSOR_SELECT_BITS> select
  ) noexcept:
    measure(measure),
    enable(enable),
    select(select)
  {

  }

  // MISOSensorArray impl ==============================================================================================

  // hidden ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // global constexpr --------------------------------------------------------------------------------------------------
  
  constexpr uint32_t SENSING_TIME_ON_MILLIS = 10; // minimum 10ms
  constexpr uint32_t SENSING_TIME_OFF_MILLIS = 0; // can probably be zero

  constexpr bool DEBUG_IO = false;

  // public ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // static constexpr --------------------------------------------------------------------------------------------------

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  constexpr MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::MISOSensorArray(MISOSensorArrayPins<SENSOR_SELECT_BITS> pins) noexcept:
      pins(pins)
  {

  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  constexpr bool MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::is_sensor_available(std::size_t sensor_index) noexcept
  {
    return sensor_index < SENSOR_COUNT;
  }

  // constexpr ---------------------------------------------------------------------------------------------------------

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  constexpr bool MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::is_sensor_active(std::size_t sensor_index) const noexcept
  {
    return this->get_active_index() == sensor_index;
  }
  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  constexpr bool MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::is_any_sensor_active() const noexcept
  {
    return this->get_active_index().has_value();
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  constexpr std::optional<float> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::get_measurement(std::size_t sensor_index) const noexcept
  {
    if(this->is_sensor_available(sensor_index))
    {
      return {this->get_all_measurements()[sensor_index]};
    }
    return {};
  }
  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  constexpr const std::array<std::optional<float>, SENSOR_COUNT>& MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::get_all_measurements() const noexcept
  {
    return this->measurements;
  }

  // const -------------------------------------------------------------------------------------------------------------

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::optional<std::size_t> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::get_active_index(void) const noexcept
  {
    // Assert that index is valid if selected
    assert(!this->active_sensor_index.has_value() || this->is_sensor_available(*this->active_sensor_index));

    return this->active_sensor_index;
  }

  // mut ---------------------------------------------------------------------------------------------------------------
  
  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  void MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::init(void) noexcept
  {
    this->init_pins();
    this->is_init = true;
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::optional<std::optional<std::size_t>> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::set_active(std::size_t sensor_index) noexcept
  {
    if(this->is_sensor_available(sensor_index))
    {
      std::optional<std::size_t> swap = {sensor_index};
      if(swap != this->active_sensor_index)
      {
        this->write_select(sensor_index);
      }
      this->active_sensor_index.swap(swap);
      return {swap};
    }
    return {};
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::optional<std::size_t> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::set_inactive(void) noexcept
  {
    std::optional<std::size_t> swap = {};
    this->active_sensor_index.swap(swap);
    return swap;
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::optional<float> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::measure_currently_active_sensor(void) noexcept
  {
    auto active_sensor_index = this->get_active_index();
    if(active_sensor_index.has_value())
    {
      return {this->measurements[*active_sensor_index] = this->read_measurement()};
    }
    return {};
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::optional<float> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::measure(std::size_t sensor_index) noexcept
  {
    this->set_active(sensor_index);
    return this->measure_currently_active_sensor();
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::array<float, SENSOR_COUNT> MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::measure_all(void) noexcept
  {
    std::array<float, SENSOR_COUNT> measurements = {0.0};
    for(std::size_t i = 0; i < SENSOR_COUNT; i++)
    {
      auto measurement = this->measure(i);

      // unwrap value
      assert(measurement.has_value());
      measurements[i] = *measurement;
    }
    return measurements;
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  std::array<std::optional<float>, SENSOR_COUNT>MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>
    ::eject_all_measurements() noexcept
  {
    std::array<std::optional<float>, SENSOR_COUNT> swap = {{}};
    std::swap(swap, this->measurements);
    return swap;
  }

  // private ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // const -------------------------------------------------------------------------------------------------------------

  #include <Arduino.h>

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  void MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::init_pins(void) const noexcept
  {
    pinMode(this->pins.enable, OUTPUT);
    for(auto pin : this->pins.select)
    {
      pinMode(pin, OUTPUT);
    }
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  void MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::write_select(std::size_t sensor_index) const noexcept
  {
    assert(this->is_init && this->is_sensor_available(sensor_index));

    if(DEBUG_IO) Serial.println(String("Select = ") + sensor_index);

    // Write number bits across "select"-pins, in order of least to most significant
    for(auto pin : this->pins.select)
    {
      digitalWrite(pin, sensor_index % 2);
      sensor_index >>= 1;
    }
  }

  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  float MISOSensorArray<SENSOR_COUNT, SENSOR_SELECT_BITS>::read_measurement(void) const noexcept
  {
    assert(this->is_init);

    // Her har jeg antatt at ADC_RESOLUTION er konstant oppløsning i bits for ADC
    constexpr uint32_t ADC_MAX_VALUE = (1 << ADC_RESOLUTION) - 1;
    constexpr float ADC_MAX_VALUE_INV = 1.0f/(float)ADC_MAX_VALUE;

    if(!this->is_any_sensor_active())
    {
      if(DEBUG_IO) Serial.println("Tried to read, but no sensor was selected");
      return 0.0;
    }

    // Enable sensing
    digitalWrite(this->pins.enable, true);

    // Sustain the "enable"-signal for a given time so that the reading can be stabilized
    delay(SENSING_TIME_ON_MILLIS);

    // Read measurement
    float measurement = (float)analogRead(this->pins.measure)*ADC_MAX_VALUE_INV;

    // Disable sensing
    digitalWrite(this->pins.enable, false);
    
    // Wait after sensing?
    delay(SENSING_TIME_OFF_MILLIS);

    if(DEBUG_IO) Serial.println(String("Measurement = ") + measurement);
    return measurement;
  }
}