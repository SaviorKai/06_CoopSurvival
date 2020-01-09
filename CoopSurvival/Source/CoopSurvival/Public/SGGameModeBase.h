// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class COOPSURVIVAL_API ASGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	ASGGameModeBase();


protected:
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float TimeBetweenWaves;

	// # of bots to spawn in a wave.
	int32 NumberOfBotsToSpawn;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION(BlueprintImplementableEvent, Category="Setup")
	void SpawnNewBot();

	//Start Spawning Bots
	void StartWave();
	FTimerHandle TimerHandle_BotSpawner;

	//Stop Spawning Bots
	void EndWave();

	//Set timer for next start wave.
	void PrepareForNextWave();

public:

	virtual void StartPlay() override;

};
