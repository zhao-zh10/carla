<h1>How to add a new sensor</h1>

> _This document is a work in progress, more detail is required._

This tutorial explains the basics for adding a new sensor to CARLA. It provides
the steps to implement the sensor from UE4 to the Python API.

#### 1. Sensor Actor

Create a sensor actor deriving from `ASensor` (or one of its derived classes),
this class requires to implement the following methods:

  - `static FActorDefinition GetSensorDefinition();` Returns an actor definition
    that will be given to the user in the client-side to configure the details
    of the sensor.
  - `void Set(const FActorDescription &ActorDescription) override;` Configure
    the sensor with the attributes that the user requested.
  - `void Tick(float DeltaTime) override;` This function is called on every game
    update and here the sensor is expected to produce the data and send it (more
    on this below).
  - Also requires the UE4 macros, `UCLASS()` and `GENERATED_BODY()`.

This class should be added to
`Unreal/CarlaUE4/Plugins/Carla/Source/Carla/Sensor` folder.

The class declaration should look similar to this

```cpp
#pragma once

#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"

#include "MySensor.generated.h"

UCLASS()
class CARLA_API AMySensor : public ASensor
{
  GENERATED_BODY()

public:

  static FActorDefinition GetSensorDefinition();

  void Set(const FActorDescription &ActorDescription) override;

  void Tick(float DeltaSeconds) override;
};
```

and the .cpp something like

```cpp
#include "Carla.h"
#include "Carla/Sensor/MySensor.h"

#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"

#include "StaticMeshResources.h"

FActorDefinition AMySensor::GetSensorDefinition()
{
  auto Def = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
      TEXT("other"),
      TEXT("my_sensor"));
  // Add custom attributes to Def here if necessary.
  return Def;
}

AMySensor::AMySensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true; // False if Tick is not necessary.

  auto MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
  MeshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
  MeshComp->bHiddenInGame = true;
  MeshComp->CastShadow = false;
  MeshComp->PostPhysicsComponentTick.bCanEverTick = false;
  RootComponent = MeshComp;
}

AMySensor::Set(const FActorDescription &ActorDescription)
{
  Super::Set(ActorDescription);
  // Retrive custom attributes here if necessary.
}

AMySensor::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  auto CustomData = ... // Do the measurement.

  GetDataStream().Send_GameThread(*this, CustomData);
}
```

`ASensor` contains a `FDataStream` for sending the data through the streaming
server. Sensor actors should write their measurements to this stream using one
of the two methods in the stream, `Send_Async` or `Send_GameThread` depending in
which thread they're producing the data. The class `FDataStream` is quite well
documented, take a look for more details.

The stream methods won't compile however until the sensor and its serializer are
registered, this takes us to the next class required.

#### 2. Sensor Serializer

This class is in charge of serializing and deserializing the data. The only
requirement of this class is providing two static methods with the following
signatures:

  - `static Buffer Serialize(const YourASensor &sensor, ...data here...);` This
    function should convert the data provided by the sensor actor into a
    `Buffer` object. The signature is quite open.
  - `static SharedPtr<SensorData> Deserialize(RawData data);` This function has
    to convert a `RawData` object (which is a wrapper around a buffer with some
    extra meta-information) into a `SensorData` derived class.

Serialization happens in the simulator-side, right after writing the data to the
stream. Deserialization happens in the client-side, right after delivering the
data to the user in the callback function provided to the `listen` method of the
sensors.

This class should be added to `LibCarla/source/carla/sensor/s11n` folder, and
its corresponding namespace.

There are already some `SensorData` classes implemented, like images and arrays,
otherwise you must add a new class fitting your needs. In this case the class
must also be exposed to Python. I will add more info on this in the tutorial.

#### 3. Register your classes

Your classes must be registered within the `SensorRegistry` as an
`std::pair<Sensor *, Serializer>`. Also follow the steps in the header as the
includes must be added in the right places.

```cpp
// LibCarla/source/carla/sensor/SensorRegistry.h

using SensorRegistry = CompositeSerializer<
  std::pair<ASceneCaptureCamera *, s11n::ImageSerializer>,
  std::pair<ADepthCamera *, s11n::ImageSerializer>,
  std::pair<ASemanticSegmentationCamera *, s11n::ImageSerializer>,
  std::pair<ARayCastLidar *, s11n::LidarSerializer>
>;
```

And that's it, the sensor registry now do its compile-time magic to dispatch the
right data to the right serializer.
