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
        float mass,
        float drag_coefficient,
        float chassis_width,
        float chassis_height,
        float drag_area,
        float estimated_max_engine_speed,
        float max_engine_rpm,
        float debug_drag_magnitude)
      : mass(mass),
        drag_coefficient(drag_coefficient),
        chassis_width(chassis_width),
        chassis_height(chassis_height),
        drag_area(drag_area),
        estimated_max_engine_speed(estimated_max_engine_speed),
        max_engine_rpm(max_engine_rpm),
        debug_drag_magnitude(debug_drag_magnitude) {}

    float mass = 0.0f;
    float drag_coefficient = 0.0f;
    float chassis_width = 0.0f;
    float chassis_height = 0.0f;
    float drag_area = 0.0f;
    float estimated_max_engine_speed = 0.0f;
    float max_engine_rpm = 0.0f;
    float debug_drag_magnitude = 0.0f;

#ifdef LIBCARLA_INCLUDED_FROM_UE4

    VehiclePhysicsControl(const FVehiclePhysicsControl &Control)
      : mass(Control.Mass),
        drag_coefficient(Control.DragCoefficient),
        chassis_width(Control.ChassisWidth),
        chassis_height(Control.ChassisHeight),
        drag_area(Control.DragArea),
        estimated_max_engine_speed(Control.EstimatedMaxEngineSpeed),
        max_engine_rpm(Control.MaxEngineRPM),
        debug_drag_magnitude(Control.DebugDragMagnitude) {}

    operator FVehiclePhysicsControl() const {
      FVehiclePhysicsControl Control;
      Control.Mass = mass;
      Control.DragCoefficient = drag_coefficient;
      Control.ChassisWidth = chassis_width;
      Control.ChassisHeight = chassis_height;
      Control.DragArea = drag_area;
      Control.EstimatedMaxEngineSpeed = estimated_max_engine_speed;
      Control.MaxEngineRPM = max_engine_rpm;
      Control.DebugDragMagnitude = debug_drag_magnitude;
      return Control;
    }

#endif // LIBCARLA_INCLUDED_FROM_UE4

    bool operator!=(const VehiclePhysicsControl &rhs) const {
      return max_engine_rpm != rhs.max_engine_rpm ||
             mass != rhs.mass ||
             drag_coefficient != rhs.drag_coefficient ||
             chassis_width != rhs.chassis_width ||
             chassis_height != rhs.chassis_height ||
             drag_area != rhs.drag_area ||
             estimated_max_engine_speed != rhs.estimated_max_engine_speed ||
             max_engine_rpm != rhs.max_engine_rpm ||
             debug_drag_magnitude != rhs.debug_drag_magnitude;
    }

    bool operator==(const VehiclePhysicsControl &rhs) const {
      return !(*this != rhs);
    }

    MSGPACK_DEFINE_ARRAY(
        mass,
        drag_coefficient,
        chassis_width,
        chassis_height,
        drag_area,
        estimated_max_engine_speed,
        max_engine_rpm,
        debug_drag_magnitude);
  };

} // namespace rpc
} // namespace carla
