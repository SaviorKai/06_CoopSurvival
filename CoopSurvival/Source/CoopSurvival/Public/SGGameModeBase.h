// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGGameModeBase.generated.h"

enum class EWaveState : uint8;  // This is how you forwad declare Enums.

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilledSignature, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

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

	UFUNCTION(BlueprintImplementableEvent, Category="Setup")  //Actual Spawning is handled in BP, because we use EQS
	void SpawnNewBot();

	//Start Spawning Bots
	void StartWave();
	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_CheckWaveTimer;

	//Stop Spawning Bots
	void EndWave();

	//Set timer for next start wave.
	void PrepareForNextWave();

	//Check if enemies are still alive before we can start next wave.
	void CheckWaveState();

	//Check if players are still alive.
	void CheckAnyPlayerAlive();

	//Game Over 
	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RespawnDeadPlayers();

public:

	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Events") // Makes this event blueprint assignable when, so that we can register for it
	FOnActorKilledSignature OnActorKilled;
};

