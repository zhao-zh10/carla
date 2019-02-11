// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "VehiclePhysicsControl.generated.h"

USTRUCT(BlueprintType)
struct CARLA_API FVehiclePhysicsControl
{
  GENERATED_BODY()

  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float MaxEngineRPM = 0.0f;
};