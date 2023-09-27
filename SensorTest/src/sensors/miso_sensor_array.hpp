#pragma once

#include <optional>
#include <array>
#include <misc/msb.hpp>

namespace sensors
{
  /// @brief Contains the pins used for any MISO sensor array
  template<std::size_t SENSOR_SELECT_BITS>
  class MISOSensorArrayPins
  {
    public:
      /// @brief Analog-in GPIO-pin. Measures voltage from common-output sensors
      const uint32_t measure;
      /// @brief Digital-out GPIO-pin. Enables/disables all sensors
      const uint32_t enable;
      /// @brief Little-endian digital-out GPIO-pins. Selects sensor
      const std::array<uint32_t, SENSOR_SELECT_BITS> select;
        
    public:
      /// @brief Contains the pins used for any MISO sensor array
      /// @param measure Analog-in GPIO-pin
      /// @param enable Digital-out GPIO-pin
      /// @param select Little-endian digital-out GPIO-pins
      constexpr MISOSensorArrayPins(
        uint32_t measure,
        uint32_t enable,
        std::array<uint32_t, SENSOR_SELECT_BITS> select
      ) noexcept;
  };

  //constexpr std::size_t TEST = std::bit_width(3U);

  /// @brief A helper class for using an array of common-output multiplexed sensors.
  /// @tparam SENSOR_COUNT The number of sensors.
  /// @tparam SENSOR_SELECT_BITS The number of bits used to represent the index with GPIO when selecting sensors.
  template<std::size_t SENSOR_COUNT, std::size_t SENSOR_SELECT_BITS>
  class MISOSensorArray
  {
    public:
      // static constexpr ----------------------------------------------------------------------------------------------

      /// @brief Minimum amount of bits needed to represent the sensor indices for this sensor array.
      static constexpr std::size_t SENSOR_SELECT_BITS_MIN {misc::msb(SENSOR_COUNT - 1)};

    private:
      // const ---------------------------------------------------------------------------------------------------------

      const MISOSensorArrayPins<SENSOR_SELECT_BITS> pins;

      // mut -----------------------------------------------------------------------------------------------------------

      std::array<std::optional<float>, SENSOR_COUNT> measurements {{}};
      std::optional<std::size_t> active_sensor_index {};

      bool is_init = false;

    public:
      // static constexpr ----------------------------------------------------------------------------------------------

      constexpr MISOSensorArray(MISOSensorArrayPins<SENSOR_SELECT_BITS> pins) noexcept;

      /// @brief Checks if the given index is available for this sensor array.
      /// @param sensor_index 
      /// @return true if available.
      static constexpr bool is_sensor_available(std::size_t sensor_index) noexcept;

      // constexpr -----------------------------------------------------------------------------------------------------
      
      /// @brief Only one sensor may be active at a time. This checks if the sensor with the given index is the one
      /// currently active.
      /// @param sensor_index 
      /// @return true if active.
      constexpr bool is_sensor_active(std::size_t sensor_index) const noexcept;

      /// @brief This checks if any sensor if active at the time.
      /// @return true if any are active.
      constexpr bool is_any_sensor_active(void) const noexcept;
      
      /// @brief Get the logged measurement tied to a given sensor index.
      /// @param sensor_index 
      /// @return Some measurement if index is available, or none if index is unavailable.
      constexpr std::optional<float> get_measurement(std::size_t sensor_index) const noexcept;

      /// @brief Get all logged measurments for each sensor.
      /// @return All measurements as array-slice.
      constexpr const std::array<std::optional<float>, SENSOR_COUNT>& get_all_measurements(void) const noexcept;

      // const ---------------------------------------------------------------------------------------------------------
      
      /// @brief Get the index of the currently active sensor, if any.
      /// @return Some index if active, otherwise none.
      std::optional<std::size_t> get_active_index(void) const noexcept;

      // mut -----------------------------------------------------------------------------------------------------------
      
      /// @brief Sets up sensor array. To be called once at startup.
      void init(void) noexcept;
      
      /// @brief Change the currently active sensor and select it by index.
      /// @param sensor_index 
      /// @return Some-some index if previously selected sensor if any, otherwise some-none, or none if given index is
      /// unavailable
      std::optional<std::optional<std::size_t>> set_active(std::size_t sensor_index) noexcept;

      /// @brief Deselects the currently selected sensor, if any.
      /// @return Some index if previously selected sensor if any, otherwise none.
      std::optional<std::size_t> set_inactive(void) noexcept;

      /// @brief Measures the currently selected sensor, if any. Both stores it, and returns a copy.
      /// @return Measurement, if any sensor was selected, normalized to 0.0-1.0.
      std::optional<float> measure_currently_active_sensor(void) noexcept;

      /// @brief Selects a sensor with a given index, and then measures it. Both stores it, and returns a copy.
      /// @param sensor_index 
      /// @return Measurement, if index was available, normalized to 0.0-1.0.
      std::optional<float> measure(std::size_t sensor_index) noexcept;

      /// @brief Measures all sensors in array, one by one. Stores all measurments and returns a copy.
      /// @return All measurements, normalized to 0.0-1.0.
      std::array<float, SENSOR_COUNT> measure_all(void) noexcept;

      /// @brief Clears array of measurements and returns them.
      /// @return All measurements, normalized to 0.0-1.0.
      std::array<std::optional<float>, SENSOR_COUNT> eject_all_measurements(void) noexcept;

    private:
      // const ---------------------------------------------------------------------------------------------------------

      /// @brief Sets up digital output pins. Only needs to be called once, at startup.
      void init_pins(void) const noexcept;

      /// @brief Writes to "select"-pins using a given index. Will overflow if out of bounds.
      /// @param sensor_index 
      void write_select(std::size_t sensor_index) const noexcept;

      /// @brief Reads from "measure"-pin while sending pulse using "enable"-pin.
      /// @return Measured value, normalized to 0.0-1.0.
      float read_measurement(void) const noexcept;
    
    // assertions ------------------------------------------------------------------------------------------------------

    static_assert(
      SENSOR_SELECT_BITS >= SENSOR_SELECT_BITS_MIN,
      "\"select\"-bits are not enough to represent all indices for this sensor array."
    );
  };
}

#include "miso_sensor_array_impl.hpp"