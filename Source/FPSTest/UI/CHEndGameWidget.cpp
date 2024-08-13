// Fill out your copyright notice in the Description page of Project Settings.


#include "CHEndGameWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "FPSTest/Misc/CHUtils.h"
#include "Kismet/KismetSystemLibrary.h"

void UCHEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CHECK_POINTER(BT_ExitGame)
	BT_ExitGame->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UCHEndGameWidget::SetWinnerName(FText Name)
{
	CHECK_POINTER(TB_WinnerName)
	TB_WinnerName->SetText(Name);
}

void UCHEndGameWidget::OnExitButtonClicked()
{
	CHECK_POINTER(GetOwningPlayer())
	UKismetSystemLibrary::QuitGame(GetOwningPlayer(), GetOwningPlayer(),EQuitPreference::Quit, false);
}
