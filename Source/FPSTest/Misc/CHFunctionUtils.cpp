// Fill out your copyright notice in the Description page of Project Settings.


#include "CHFunctionUtils.h"

#include "CHUtils.h"
#include "FPSTest/PlayerController/CHPlayerController.h"
#include "Kismet/GameplayStatics.h"

ACHPlayerController* UCHFunctionUtils::GetCHPlayerController(const UObject* WorldContext)
{
	ACHPlayerController* PC = Cast<ACHPlayerController>(UGameplayStatics::GetPlayerController(WorldContext,0));
	CHECK_POINTER(PC, nullptr)
	
	return PC;
}
