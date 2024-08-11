// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CHPlayerController.generated.h"


enum class ECHGameState : uint8;
class UCHDACharacterConfig;
class UInputMappingContext;

UCLASS()
class FPSTEST_API ACHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void ReplaceMappingContext(ECHGameState NewState, UCHDACharacterConfig* CharacterDA) const;
};
