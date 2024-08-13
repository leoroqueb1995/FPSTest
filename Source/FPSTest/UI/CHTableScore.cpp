// Fill out your copyright notice in the Description page of Project Settings.


#include "CHTableScore.h"

#include "CHPlayerScoreWidget.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Misc/CHUtils.h"
#include "Kismet/GameplayStatics.h"

void UCHTableScore::NativeConstruct()
{
	Super::NativeConstruct();

	SB_TableScore->ClearChildren();
	CHECK_POINTER(GetWorld())

	FTimerDelegate UpdateDelegate;
	UpdateDelegate.BindUObject(this, &ThisClass::UpdateTableScore);
	GetWorld()->GetTimerManager().SetTimer(UpdateTableScoreHandle, UpdateDelegate, 0.5f, true);
}

void UCHTableScore::UpdateTableScore()
{
	SB_TableScore->ClearChildren();
	CHECK_POINTER(PlayerScoreClass)
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetOwningPlayer(), ACHCharacterBase::StaticClass(), OutActors);

	CHECK_EMPTY_ARRAY(OutActors)
	Algo::Sort(OutActors, [](AActor* A, AActor* B)
	{
		ACHCharacterBase* CharacterA = Cast<ACHCharacterBase>(A);
		ACHCharacterBase* CharacterB = Cast<ACHCharacterBase>(B);
		
		CHECK_POINTER(CharacterA, false)
		CHECK_POINTER(CharacterB, false)

		return CharacterA->GetPoints() > CharacterB->GetPoints();
	});

	for(AActor* Actor : OutActors)
	{
		ACHCharacterBase* Player = Cast<ACHCharacterBase>(Actor);

		if(!Player)
		{
			continue;
		}

		UCHPlayerScoreWidget* PlayerScore = CreateWidget<UCHPlayerScoreWidget>(GetOwningPlayer(), PlayerScoreClass);
		PlayerScore->SetPlayerReference(Player);
		PlayerScore->UpdatePlayerScore();
		if(PlayerScore)
		{
			SB_TableScore->AddChildToStackBox(PlayerScore);
		}
	}
}
