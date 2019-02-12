// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/geom/Transform.h"
#include "carla/geom/Vector3D.h"
#include "carla/rpc/ActorId.h"
#include "carla/rpc/TrafficLightState.h"
#include "carla/rpc/VehicleControl.h"
#include "carla/rpc/VehiclePhysicsControl.h"
#include "carla/rpc/WalkerControl.h"

#include <cstdint>

namespace carla {
namespace sensor {
namespace data {

namespace detail {

#pragma pack(push, 1)
  class PackedVehicleControl {
  public:

    PackedVehicleControl() = default;

    PackedVehicleControl(const rpc::VehicleControl &control)
      : throttle(control.throttle),
        steer(control.steer),
        brake(control.brake),
        hand_brake(control.hand_brake),
        reverse(control.reverse),
        manual_gear_shift(control.manual_gear_shift),
        gear(control.gear) {}

    operator rpc::VehicleControl() const {
      return {throttle, steer, brake, hand_brake, reverse, manual_gear_shift, gear};
    }

  private:

    float throttle;
    float steer;
    float brake;
    bool hand_brake;
    bool reverse;
    bool manual_gear_shift;
    int32_t gear;
  };

#pragma pack(pop)

#pragma pack(push, 1)
  class PackedVehiclePhysicsControl {
  public:

    PackedVehiclePhysicsControl() = default;

    PackedVehiclePhysicsControl(const rpc::VehiclePhysicsControl &control)
      : mass(control.mass),
        drag_coefficient(control.drag_coefficient),
        chassis_width(control.chassis_width),
        chassis_height(control.chassis_height),
        drag_area(control.drag_area),
        estimated_max_engine_speed(control.estimated_max_engine_speed),
        max_engine_rpm(control.max_engine_rpm),
        debug_drag_magnitude(control.debug_drag_magnitude) {}

    operator rpc::VehiclePhysicsControl() const {
      return {mass,
              drag_coefficient,
              chassis_width,
              chassis_height,
              drag_area,
              estimated_max_engine_speed,
              max_engine_rpm,
              debug_drag_magnitude};
    }

  private:

    float mass;
    float drag_coefficient;
    float chassis_width;
    float chassis_height;
    float drag_area;
    float estimated_max_engine_speed;
    float max_engine_rpm;
    float debug_drag_magnitude;
  };

#pragma pack(pop)

#pragma pack(push, 1)
  struct VehicleData {
    VehicleData() = default;

    PackedVehicleControl control;
    PackedVehiclePhysicsControl physics_control;
    float speed_limit;
    rpc::TrafficLightState traffic_light_state;
    bool has_traffic_light;
    rpc::actor_id_type traffic_light_id;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  class PackedWalkerControl {
  public:

    PackedWalkerControl() = default;

    PackedWalkerControl(const rpc::WalkerControl &control)
      : direction{control.direction.x, control.direction.y, control.direction.z},
        speed(control.speed),
        jump(control.jump) {}

    operator rpc::WalkerControl() const {
      return {geom::Vector3D{direction[0u], direction[1u], direction[2u]}, speed, jump};
    }

  private:

    float direction[3u];
    float speed;
    bool jump;
  };

#pragma pack(pop)

#pragma pack(push, 1)
  struct TrafficLightData {
    TrafficLightData() = default;

    rpc::TrafficLightState state;
    float green_time;
    float yellow_time;
    float red_time;
    float elapsed_time;
    bool time_is_frozen;
  };
#pragma pack(pop)
} // namespace detail

#pragma pack(push, 1)

  /// Dynamic state of an actor at a certain frame.
  struct ActorDynamicState {

    actor_id_type id;

    geom::Transform transform;

    geom::Vector3D velocity;

    geom::Vector3D angular_velocity;

    union TypeDependentState {
      detail::TrafficLightData traffic_light_data;
      detail::VehicleData vehicle_data;
      detail::PackedWalkerControl walker_control;
    } state;
  };

#pragma pack(pop)

  static_assert(
      sizeof(ActorDynamicState) == 13u * sizeof(uint32_t) + sizeof(detail::VehicleData),
      "Invalid ActorDynamicState size!");

} // namespace data
} // namespace sensor
} // namespace carla
