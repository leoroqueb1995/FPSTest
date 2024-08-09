// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define CHECK_POINTER(Pointer, ...) \
	if(!IsValid(Pointer)) \
	{ \
		UE_LOG(LogTemp, Error, TEXT("%s: Pointer {%s} is not valid. Aborting."), *FString(__FUNCTION__), *FString(#Pointer)); \
		return __VA_ARGS__; \
	}

#define CHECK_POINTER_CATEGORY(Pointer, LogCategory, ...) \
	if (!IsValid(Pointer)) \
	{ \
		UE_LOG(LogCategory, Error, TEXT("%s: The pointer {%s} was not valid. Aborting."), *FString(__FUNCTION__), *FString(#Pointer)); \
		return __VA_ARGS__; \
	}

// In case pointer is not valid will return ReturnValue and Log specified text
#define CHECK_POINTER_WITH_TEXT_RET(Pointer, ReturnValue, Format, ...) \
	if (!IsValid(Pointer)) \
	{ \
		UE_LOG(LogTemp, Error, TEXT("%s: Pointer {%s} invalid. Info text: ") Format, *FString(__FUNCTION__), *FString(#Pointer), ##__VA_ARGS__); \
		return ReturnValue; \
	}

// In case pointer is not valid will Log specified text and return void
#define CHECK_POINTER_WITH_TEXT_VOID(Pointer, Format, ...) \
	if (!IsValid(Pointer)) \
	{ \
		UE_LOG(LogTemp, Error, TEXT("%s: Pointer {%s} invalid. Info text: ") Format, *FString(__FUNCTION__), *FString(#Pointer), ##__VA_ARGS__); \
		return; \
	}

// Will Check if given array is empty
#define CHECK_EMPTY_ARRAY(Array, ...) \
	if (Array.Num() == 0) \
	{ \
		UE_LOG(LogTemp, Warning, TEXT("%s: Array {%s} is empty. Aborting..."), *FString(__FUNCTION__), *FString(#Array)); \
		return __VA_ARGS__; \
	}

#define CHECK_VALID_TAG(Tag, ...) \
	if (!Tag.IsValid()) \
	{ \
		UE_LOG(LogTemp, Warning, TEXT("%s: Tag {%s} is not valid. Aborting..."), *FString(__FUNCTION__), *FString(#Tag)); \
		return __VA_ARGS__; \
	}
