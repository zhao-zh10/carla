// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/client/detail/Client.h"

#include "carla/Version.h"
#include "carla/rpc/ActorDescription.h"
#include "carla/rpc/Client.h"
#include "carla/rpc/DebugShape.h"
#include "carla/rpc/Response.h"
#include "carla/rpc/VehicleControl.h"
#include "carla/rpc/WalkerControl.h"
#include "carla/streaming/Client.h"

#include <thread>

namespace carla {
namespace client {
namespace detail {

  template <typename T>
  static T Get(carla::rpc::Response<T> &response) {
    return response.Get();
  }

  static bool Get(carla::rpc::Response<void> &) {
    return true;
  }

  // ===========================================================================
  // -- Client::Pimpl ----------------------------------------------------------
  // ===========================================================================

  class Client::Pimpl {
  public:

    Pimpl(const std::string &host, uint16_t port, size_t worker_threads)
      : rpc_client(host, port),
        streaming_client(host) {
      streaming_client.AsyncRun(
          worker_threads > 0u ? worker_threads : std::thread::hardware_concurrency());
    }

    template <typename T, typename ... Args>
    auto CallAndWait(const std::string &function, Args && ... args) {
      auto object = rpc_client.call(function, std::forward<Args>(args) ...);
      using R = typename carla::rpc::Response<T>;
      auto response = object.template as<R>();
      if (response.HasError()) {
        throw_exception(std::runtime_error(response.GetError().What()));
      }
      return Get(response);
    }

    template <typename ... Args>
    void AsyncCall(const std::string &function, Args && ... args) {
      // Discard returned future.
      rpc_client.async_call(function, std::forward<Args>(args) ...);
    }

    rpc::Client rpc_client;

    streaming::Client streaming_client;
  };

  // ===========================================================================
  // -- Client -----------------------------------------------------------------
  // ===========================================================================

  Client::Client(
      const std::string &host,
      const uint16_t port,
      const size_t worker_threads)
    : _pimpl(std::make_unique<Pimpl>(host, port, worker_threads)) {}

  Client::~Client() = default;

  void Client::SetTimeout(time_duration timeout) {
    _pimpl->rpc_client.set_timeout(timeout.milliseconds());
  }

  std::string Client::GetClientVersion() {
    return ::carla::version();
  }

  std::string Client::GetServerVersion() {
    return _pimpl->CallAndWait<std::string>("version");
  }

  void Client::LoadEpisode(std::string map_name) {
    // Await response, we need to be sure in this one.
    _pimpl->CallAndWait<void>("load_new_episode", std::move(map_name));
  }

  rpc::EpisodeInfo Client::GetEpisodeInfo() {
    return _pimpl->CallAndWait<rpc::EpisodeInfo>("get_episode_info");
  }

  rpc::MapInfo Client::GetMapInfo() {
    return _pimpl->CallAndWait<rpc::MapInfo>("get_map_info");
  }

  std::vector<std::string> Client::GetAvailableMaps() {
    return _pimpl->CallAndWait<std::vector<std::string>>("get_available_maps");
  }

  std::vector<rpc::ActorDefinition> Client::GetActorDefinitions() {
    return _pimpl->CallAndWait<std::vector<rpc::ActorDefinition>>("get_actor_definitions");
  }

  rpc::Actor Client::GetSpectator() {
    return _pimpl->CallAndWait<carla::rpc::Actor>("get_spectator");
  }

  rpc::EpisodeSettings Client::GetEpisodeSettings() {
    return _pimpl->CallAndWait<rpc::EpisodeSettings>("get_episode_settings");
  }

  void Client::SetEpisodeSettings(const rpc::EpisodeSettings &settings) {
    _pimpl->AsyncCall("set_episode_settings", settings);
  }

  rpc::WeatherParameters Client::GetWeatherParameters() {
    return _pimpl->CallAndWait<rpc::WeatherParameters>("get_weather_parameters");
  }

  void Client::SetWeatherParameters(const rpc::WeatherParameters &weather) {
    _pimpl->AsyncCall("set_weather_parameters", weather);
  }

  std::vector<rpc::Actor> Client::GetActorsById(
      const std::vector<actor_id_type> &ids) {
    using return_t = std::vector<rpc::Actor>;
    return _pimpl->CallAndWait<return_t>("get_actors_by_id", ids);
  }

  rpc::Actor Client::SpawnActor(
      const rpc::ActorDescription &description,
      const geom::Transform &transform) {
    return _pimpl->CallAndWait<rpc::Actor>("spawn_actor", description, transform);
  }

  rpc::Actor Client::SpawnActorWithParent(
      const rpc::ActorDescription &description,
      const geom::Transform &transform,
      const rpc::Actor &parent) {
    return _pimpl->CallAndWait<rpc::Actor>("spawn_actor_with_parent",
        description,
        transform,
        parent);
  }

  bool Client::DestroyActor(const rpc::Actor &actor) {
    try {
      return _pimpl->CallAndWait<void>("destroy_actor", actor);
    } catch (const std::exception &e) {
      log_error("failed to destroy actor:", actor.id, actor.description.id);
      log_error(e.what());
      return false;
    }
  }

  void Client::SetActorLocation(const rpc::Actor &actor, const geom::Location &location) {
    _pimpl->AsyncCall("set_actor_location", actor, location);
  }

  void Client::SetActorTransform(const rpc::Actor &actor, const geom::Transform &transform) {
    _pimpl->AsyncCall("set_actor_transform", actor, transform);
  }

  void Client::SetActorSimulatePhysics(const rpc::Actor &actor, const bool enabled) {
    _pimpl->AsyncCall("set_actor_simulate_physics", actor, enabled);
  }

  void Client::SetActorAutopilot(const rpc::Actor &vehicle, const bool enabled) {
    _pimpl->AsyncCall("set_actor_autopilot", vehicle, enabled);
  }

  void Client::ApplyControlToVehicle(const rpc::Actor &vehicle, const rpc::VehicleControl &control) {
    _pimpl->AsyncCall("apply_control_to_vehicle", vehicle, control);
  }

  void Client::ApplyControlToWalker(const rpc::Actor &walker, const rpc::WalkerControl &control) {
    _pimpl->AsyncCall("apply_control_to_walker", walker, control);
  }

  void Client::SetTrafficLightState(
      const rpc::Actor &trafficLight,
      const rpc::TrafficLightState trafficLightState) {
    _pimpl->AsyncCall("set_traffic_light_state", trafficLight, trafficLightState);
  }

  void Client::SetTrafficLightGreenTime(const rpc::Actor &trafficLight, float greenTime) {
    _pimpl->AsyncCall("set_traffic_light_green_time", trafficLight, greenTime);
  }

  void Client::SetTrafficLightYellowTime(const rpc::Actor &trafficLight, float yellowTime) {
    _pimpl->AsyncCall("set_traffic_light_yellow_time", trafficLight, yellowTime);
  }

  void Client::SetTrafficLightRedTime(const rpc::Actor &trafficLight, float redTime) {
    _pimpl->AsyncCall("set_traffic_light_red_time", trafficLight, redTime);
  }

  void Client::FreezeTrafficLight(const rpc::Actor &trafficLight, bool freeze) {
    _pimpl->AsyncCall("freeze_traffic_light", trafficLight, freeze);
  }

  void Client::SetActorVelocity(const rpc::Actor &actor, const geom::Vector3D &vector) {
    _pimpl->AsyncCall("set_actor_velocity", actor, vector);
  }

  void Client::SetActorAngularVelocity(const rpc::Actor &actor, const geom::Vector3D &vector) {
    _pimpl->AsyncCall("set_actor_angular_velocity", actor, vector);
  }

  void Client::AddActorImpulse(const rpc::Actor &actor, const geom::Vector3D &vector) {
    _pimpl->AsyncCall("add_actor_impulse", actor, vector);
  }

  void Client::SubscribeToStream(
      const streaming::Token &token,
      std::function<void(Buffer)> callback) {
    _pimpl->streaming_client.Subscribe(token, std::move(callback));
  }

  void Client::UnSubscribeFromStream(const streaming::Token &token) {
    _pimpl->streaming_client.UnSubscribe(token);
  }

  void Client::DrawDebugShape(const rpc::DebugShape &shape) {
    _pimpl->AsyncCall("draw_debug_shape", shape);
  }

  std::string Client::StartRecorder(std::string name) {
    return _pimpl->CallAndWait<std::string>("start_recorder", name);
  }

  void Client::StopRecorder(void) {
    return _pimpl->AsyncCall("stop_recorder");
  }

  std::string Client::ShowRecorderFileInfo(std::string name) {
    return _pimpl->CallAndWait<std::string>("show_recorder_file_info", name);
  }

  std::string Client::ReplayFile(std::string name, double start, double duration) {
    return _pimpl->CallAndWait<std::string>("replay_file", name, start, duration);
  }

  void Client::SendTickCue() {
    _pimpl->AsyncCall("tick_cue");
  }

} // namespace detail
} // namespace client
} // namespace carla
