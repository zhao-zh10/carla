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

  // Vehicle Setup
  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float Mass = 0.0f;

  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float DragCoefficient = 0.0f;

  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float ChassisWidth = 0.0f;

  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float ChassisHeight = 0.0f;

  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float DragArea = 0.0f;

  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float EstimatedMaxEngineSpeed = 0.0f;
  
  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float MaxEngineRPM = 0.0f;
  
  UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  float DebugDragMagnitude = 0.0f;

  // Vehicle Setup - Inertia
  // UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  // float InertiaTensorScale = 0.0f;

  // UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  // float MinNormalizedTireLoad = 0.0f;

  // UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  // float MinNormalizedTireLoadFiltered = 0.0f;

  // UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  // float MaxNormalizedTireLoad = 0.0f;

  // UPROPERTY(Category = "Vehicle Physics Control", EditAnywhere, BlueprintReadWrite)
  // float MaxNormalizedTireLoadFiltered = 0.0f;
};