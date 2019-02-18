// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DevelopmentUtilitiesBL.generated.h"

/**
 *
 */
UCLASS()
class CARLAUE4_API UDevelopmentUtilitiesBL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Carla|Developers")
	static FORCEINLINE TArray<FString> GetAllMapNames()
	{
		TArray<FString> temp, Lista;
    IFileManager::Get().FindFilesRecursive(Lista, *FPaths::ProjectContentDir(), TEXT("*.umap"), true, false, false);
    for (int i = 0; i < Lista.Num(); i++) {
        Lista[i].ParseIntoArray(temp, TEXT("Content/"), true);
        Lista[i] = temp[1];
        Lista[i].ParseIntoArray(temp, TEXT("."), true);
        Lista[i] = "/Game/" + temp[0];
    }
     return Lista;
	}


};
