// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaitingToComplete,
	WaveComplete,
	GameOver
};


UCLASS()
class COOPSURVIVAL_API ASGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category="Setup")  // Remember that we don't define BlueprintImplementableEvents in C++ (defined in BP)
	void WavesStateChanged(EWaveState NewState, EWaveState OldState);

	//[NETWORKING] Network Replication Rules Function (Not required, but Added here for code readability.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category="Setup")
	EWaveState WaveState;

public: 

	void SetWaveState(EWaveState NewState);


};
