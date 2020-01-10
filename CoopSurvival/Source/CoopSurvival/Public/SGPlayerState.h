// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SGPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COOPSURVIVAL_API ASGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Setup")
	void AddScore(float ScoreDelta);
};
