// Fill out your copyright notice in the Description page of Project Settings.


#include "SGGameModeBase.h"
#include "TimerManager.h"

ASGGameModeBase::ASGGameModeBase()
{
	TimeBetweenWaves = 2.0f;
}


void ASGGameModeBase::StartWave()
{
	WaveCount++;

	NumberOfBotsToSpawn = 2 * WaveCount;
	
	float BotSpawnRate = 1.0f;  //TODO: Update to editable var

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGGameModeBase::SpawnBotTimerElapsed, BotSpawnRate, true, 0.0f);
}

void ASGGameModeBase::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	PrepareForNextWave();
}

void ASGGameModeBase::PrepareForNextWave()
{
	FTimerHandle TimerHandle_NextWavePrepTime;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWavePrepTime, this, &ASGGameModeBase::StartWave, TimeBetweenWaves,false);
}

void ASGGameModeBase::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}


void ASGGameModeBase::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NumberOfBotsToSpawn--;

	if (NumberOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}