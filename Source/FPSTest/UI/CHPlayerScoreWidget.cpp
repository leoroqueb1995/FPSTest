// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPlayerScoreWidget.h"

#include "Components/TextBlock.h"

void UCHPlayerScoreWidget::UpdatePlayerScore()
{
	CHECK_POINTER(PlayerReference)
	CHECK_POINTER(TB_Score)
	CHECK_POINTER(TB_PlayerName)

	const FText Kills = FText::FromString(FString::FromInt(PlayerReference->GetPoints()));
	TB_Score->SetText(Kills);

	TB_PlayerName->SetText(PlayerReference->GetPlayerName());
}
