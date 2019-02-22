// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Sensor/WorldObserver.h"

#include "Carla/Traffic/TrafficLightBase.h"
#include "Carla/Walker/WalkerController.h"

#include "CoreGlobals.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/sensor/SensorRegistry.h>
#include <carla/sensor/data/ActorDynamicState.h>
#include <compiler/enable-ue4-macros.h>

static auto FWorldObserver_GetActorState(const FActorView &View, const FActorRegistry &Registry)
{
  using AType = FActorView::ActorType;

  carla::sensor::data::ActorDynamicState::TypeDependentState state;

  if (AType::Vehicle == View.GetActorType())
  {
    auto Vehicle = Cast<ACarlaWheeledVehicle>(View.GetActor());
    if (Vehicle != nullptr)
    {
      state.vehicle_data.control = carla::rpc::VehicleControl{Vehicle->GetVehicleControl()};
      auto Controller = Cast<AWheeledVehicleAIController>(Vehicle->GetController());
      if (Controller != nullptr)
      {
        using TLS = carla::rpc::TrafficLightState;
        state.vehicle_data.traffic_light_state = static_cast<TLS>(Controller->GetTrafficLightState());
        state.vehicle_data.speed_limit = Controller->GetSpeedLimit();
        auto TrafficLight = Controller->GetTrafficLight();
        if (TrafficLight != nullptr)
        {
          state.vehicle_data.has_traffic_light = true;
          auto TrafficLightView = Registry.Find(TrafficLight);
          state.vehicle_data.traffic_light_id = TrafficLightView.GetActorId();
        }
        else
        {
          state.vehicle_data.has_traffic_light = false;
        }
      }
    }
  }

  else if (AType::Walker == View.GetActorType())
  {
    auto Walker = Cast<APawn>(View.GetActor());
    auto Controller = Walker != nullptr ? Cast<AWalkerController>(Walker->GetController()) : nullptr;
    if (Controller != nullptr)
    {
      state.walker_control = carla::rpc::WalkerControl{Controller->GetWalkerControl()};
    }
  }
  else if (AType::TrafficLight == View.GetActorType())
  {
    auto TrafficLight = Cast<ATrafficLightBase>(View.GetActor());
    if (TrafficLight != nullptr)
    {
      using TLS = carla::rpc::TrafficLightState;
      state.traffic_light_data.state = static_cast<TLS>(TrafficLight->GetTrafficLightState());
      state.traffic_light_data.green_time = TrafficLight->GetGreenTime();
      state.traffic_light_data.yellow_time = TrafficLight->GetYellowTime();
      state.traffic_light_data.red_time = TrafficLight->GetRedTime();
      state.traffic_light_data.elapsed_time = TrafficLight->GetElapsedTime();
      state.traffic_light_data.time_is_frozen = TrafficLight->GetTimeIsFrozen();
    }
  }

  return state;
}

static carla::geom::Vector3D FWorldObserver_GetAngularVelocity(const AActor &Actor)
{
  const auto RootComponent = Cast<UPrimitiveComponent>(Actor.GetRootComponent());
  const FVector AngularVelocity =
      RootComponent != nullptr ?
          RootComponent->GetPhysicsAngularVelocityInDegrees() :
          FVector{0.0f, 0.0f, 0.0f};
  return {AngularVelocity.X, AngularVelocity.Y, AngularVelocity.Z};
}

static carla::geom::Vector3D FWorldObserver_GetAcceleration(
    const FActorView &View,
    const FVector &Velocity,
    const float DeltaSeconds)
{
  FVector &PreviousVelocity = View.GetActorInfo()->Velocity;
  const FVector Acceleration = (Velocity - PreviousVelocity) / DeltaSeconds;
  PreviousVelocity = Velocity;
  return {Acceleration.X, Acceleration.Y, Acceleration.Z};
}

static carla::Buffer FWorldObserver_Serialize(
    carla::Buffer buffer,
    const UCarlaEpisode &Episode,
    float DeltaSeconds)
{
  using Serializer = carla::sensor::s11n::EpisodeStateSerializer;
  using ActorDynamicState = carla::sensor::data::ActorDynamicState;

  const auto &Registry = Episode.GetActorRegistry();

  // Set up buffer for writing.
  buffer.reset(sizeof(Serializer::Header) + sizeof(ActorDynamicState) * Registry.Num());
  auto begin = buffer.begin();
  auto write_data = [&begin](const auto &data)
  {
    std::memcpy(begin, &data, sizeof(data));
    begin += sizeof(data);
  };

  // Write header.
  Serializer::Header header;
  header.episode_id = Episode.GetId();
  header.game_timestamp = Episode.GetElapsedGameTime();
  header.platform_timestamp = FPlatformTime::Seconds();
  header.delta_seconds = DeltaSeconds;
  write_data(header);

  // Write every actor.
  for (auto &&View : Registry)
  {
    check(View.IsValid());
    constexpr float TO_METERS = 1e-2;
    const auto Velocity = TO_METERS * View.GetActor()->GetVelocity();

    ActorDynamicState info = {
      View.GetActorId(),
      View.GetActor()->GetActorTransform(),
      carla::geom::Vector3D{Velocity.X, Velocity.Y, Velocity.Z},
      FWorldObserver_GetAngularVelocity(*View.GetActor()),
      FWorldObserver_GetAcceleration(View, Velocity, DeltaSeconds),
      FWorldObserver_GetActorState(View, Registry)
    };
    write_data(info);
  }

  check(begin == buffer.end());
  return buffer;
}

void FWorldObserver::BroadcastTick(const UCarlaEpisode &Episode, float DeltaSeconds)
{
  auto AsyncStream = Stream.MakeAsyncDataStream(*this);

  auto buffer = FWorldObserver_Serialize(
      AsyncStream.PopBufferFromPool(),
      Episode,
      DeltaSeconds);

  AsyncStream.Send(*this, std::move(buffer));
}
