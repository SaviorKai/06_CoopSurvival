// Fill out your copyright notice in the Description page of Project Settings.


#include "SGGameModeBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SGHealthComponent.h"
#include "SGTrackerBot.h"


ASGGameModeBase::ASGGameModeBase()
{
	TimeBetweenWaves = 2.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 5.0f;
}

void ASGGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UE_LOG(LogTemp,Warning,TEXT("GameModeBase C++ : This is just to show that you can override a tick function and make it tick on a different interval, like 5 seconds."))
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

	GetWorldTimerManager().SetTimer(TimerHandle_CheckWaveTimer, this, &ASGGameModeBase::CheckWaveState, 3.0f, true, 0.0f);
	
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
	SpawnNewBot(); //Actual Spawning is handled in BP, because we use EQS

	NumberOfBotsToSpawn--;

	if (NumberOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

void ASGGameModeBase::CheckWaveState()
{
	FConstPawnIterator Enemies = GetWorld()->GetPawnIterator(); //TODO: Review this as it may be deprecated in the future.

	for (Enemies; Enemies; ++Enemies)
	{
		APawn* TestPawn = Enemies->Get();
		
		//Valid checks
		if (!TestPawn) { continue; }
		if (!Cast<ASGTrackerBot>(TestPawn)) { continue; }

		USGHealthComponent* HealthComp = TestPawn->FindComponentByClass<USGHealthComponent>();
		if (!HealthComp) { continue; }
		if (HealthComp->GetHealth() > 0.0f)
		{
			return;
		}
	}
	
	GetWorldTimerManager().ClearTimer(TimerHandle_CheckWaveTimer);
	PrepareForNextWave();
}