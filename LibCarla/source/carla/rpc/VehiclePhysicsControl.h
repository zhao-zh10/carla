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
        float max_engine_rpm)
      : max_engine_rpm(max_engine_rpm) {}

    float max_engine_rpm = 0.0f;

#ifdef LIBCARLA_INCLUDED_FROM_UE4

    VehiclePhysicsControl(const FVehiclePhysicsControl &Control)
      : max_engine_rpm(Control.MaxEngineRPM){}

    operator FVehiclePhysicsControl() const {
      FVehiclePhysicsControl Control;
      Control.MaxEngineRPM = max_engine_rpm;
      return Control;
    }

#endif // LIBCARLA_INCLUDED_FROM_UE4

    bool operator!=(const VehiclePhysicsControl &rhs) const {
      return
          max_engine_rpm != rhs.max_engine_rpm;
    }

    bool operator==(const VehiclePhysicsControl &rhs) const {
      return !(*this != rhs);
    }

    MSGPACK_DEFINE_ARRAY(
        max_engine_rpm);
  };

} // namespace rpc
} // namespace carla
