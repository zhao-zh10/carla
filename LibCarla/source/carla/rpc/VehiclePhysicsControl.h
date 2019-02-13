// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/MsgPack.h"

#ifdef LIBCARLA_INCLUDED_FROM_UE4
#  include "Carla/Vehicle/VehiclePhysicsControl.h"
#endif // LIBCARLA_INCLUDED_FROM_UE4

namespace carla {
namespace rpc {
  class VehiclePhysicsControl {
  public:

    VehiclePhysicsControl() = default;

    VehiclePhysicsControl(
        float max_rpm,
        float moi,
        float damping_rate_full_throttle,
        float damping_rate_zero_throttle_clutch_engaged,
        float damping_rate_zero_throttle_clutch_disengaged,

        bool use_gear_autobox,
        float gear_switch_time,
        float clutch_strength,

        float mass,
        float drag_coefficient,
        geom::Vector3D inertia_tensor_scale
        )
      : max_rpm(max_rpm),
        moi(moi),
        damping_rate_full_throttle(damping_rate_full_throttle),
        damping_rate_zero_throttle_clutch_engaged(damping_rate_zero_throttle_clutch_engaged),
        damping_rate_zero_throttle_clutch_disengaged(damping_rate_zero_throttle_clutch_disengaged),

        use_gear_autobox(use_gear_autobox),
        gear_switch_time(gear_switch_time),
        clutch_strength(clutch_strength),

        mass(mass),
        drag_coefficient(drag_coefficient),
        inertia_tensor_scale(inertia_tensor_scale) {}

    // Vehicle Engine Setup
    float max_rpm = 0.0f;
    float moi = 0.0f;
    float damping_rate_full_throttle = 0.0f;
    float damping_rate_zero_throttle_clutch_engaged = 0.0f;
    float damping_rate_zero_throttle_clutch_disengaged = 0.0f;

    // Transmission Setup
    bool use_gear_autobox = true;
    float gear_switch_time = 0.0f;
    float clutch_strength = 0.0f;

    // Vehicle Setup
    float mass = 0.0f;
    float drag_coefficient = 0.0f;
    geom::Vector3D inertia_tensor_scale = geom::Vector3D();

#ifdef LIBCARLA_INCLUDED_FROM_UE4

    VehiclePhysicsControl(const FVehiclePhysicsControl &Control)
      : max_rpm(Control.MaxRPM),
        moi(Control.MOI),
        damping_rate_full_throttle(Control.DampingRateFullThrottle),
        damping_rate_zero_throttle_clutch_engaged(Control.DampingRateZeroThrottleClutchEngaged),
        damping_rate_zero_throttle_clutch_disengaged(Control.DampingRateZeroThrottleClutchDisengaged),

        use_gear_autobox(Control.bUseGearAutoBox),
        gear_switch_time(Control.GearSwitchTime),
        clutch_strength(Control.ClutchStrength),

        mass(Control.Mass),
        drag_coefficient(Control.DragCoefficient),
        inertia_tensor_scale(Control.InertiaTensorScale) {}

    operator FVehiclePhysicsControl() const {
      FVehiclePhysicsControl Control;

      // Engine Setup
      Control.MaxRPM = max_rpm;
      Control.MOI = moi;
      Control.DampingRateFullThrottle = damping_rate_full_throttle;
      Control.DampingRateZeroThrottleClutchEngaged = damping_rate_zero_throttle_clutch_engaged;
      Control.DampingRateZeroThrottleClutchDisengaged = damping_rate_zero_throttle_clutch_disengaged;

      // Transmission Setup
      Control.bUseGearAutoBox = use_gear_autobox;
      Control.GearSwitchTime = gear_switch_time;
      Control.ClutchStrength = clutch_strength;

      // Vehicle Setup
      Control.Mass = mass;
      Control.DragCoefficient = drag_coefficient;
      Control.InertiaTensorScale = inertia_tensor_scale;

      return Control;
    }

#endif // LIBCARLA_INCLUDED_FROM_UE4

    bool operator!=(const VehiclePhysicsControl &rhs) const {
      return max_rpm != rhs.max_rpm ||
             moi != rhs.moi ||
             damping_rate_full_throttle != rhs.damping_rate_full_throttle ||
             damping_rate_zero_throttle_clutch_engaged != rhs.damping_rate_zero_throttle_clutch_engaged ||
             damping_rate_zero_throttle_clutch_disengaged != rhs.damping_rate_zero_throttle_clutch_disengaged ||

             use_gear_autobox != rhs.use_gear_autobox ||
             gear_switch_time != rhs.gear_switch_time ||
             clutch_strength != rhs.clutch_strength ||

             mass != rhs.mass ||
             drag_coefficient != rhs.drag_coefficient ||
             inertia_tensor_scale != rhs.inertia_tensor_scale;
    }

    bool operator==(const VehiclePhysicsControl &rhs) const {
      return !(*this != rhs);
    }

    MSGPACK_DEFINE_ARRAY(
       max_rpm,
       moi,
       damping_rate_full_throttle,
       damping_rate_zero_throttle_clutch_engaged,
       damping_rate_zero_throttle_clutch_disengaged,

       use_gear_autobox,
       gear_switch_time,
       clutch_strength,

       mass,
       drag_coefficient,
       inertia_tensor_scale);
  };

} // namespace rpc
} // namespace carla
