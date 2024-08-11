// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "FPSTest/DataAssets/CHDACharacterConfig.h"
#include "FPSTest/Misc/CHUtils.h"

void ACHPlayerController::ReplaceMappingContext(ECHGameState NewState, UCHDACharacterConfig* CharacterDA) const
{
	CHECK_POINTER(CharacterDA)

	UInputMappingContext** MappingContext = CharacterDA->MappingContexts.Find(NewState);
	CHECK_POINTER(*MappingContext);
	
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(*MappingContext, 0);
	}
}
