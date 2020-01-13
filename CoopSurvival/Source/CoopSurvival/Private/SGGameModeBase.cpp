// Fill out your copyright notice in the Description page of Project Settings.


#include "SGGameModeBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SGHealthComponent.h"
#include "SGTrackerBot.h"
#include "SGGameStateBase.h"
#include "SGPlayerState.h"


ASGGameModeBase::ASGGameModeBase()
{
	TimeBetweenWaves = 2.0f;

	// Set default classes
	GameStateClass = ASGGameStateBase::StaticClass();
	PlayerStateClass = ASGPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 5.0f;
}

void ASGGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckAnyPlayerAlive();
}


void ASGGameModeBase::StartWave()
{	
	WaveCount++;

	NumberOfBotsToSpawn = 2 * WaveCount;
	
	float BotSpawnRate = 1.0f;  //TODO: Update to editable var

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGGameModeBase::SpawnBotTimerElapsed, BotSpawnRate, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void ASGGameModeBase::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	GetWorldTimerManager().SetTimer(TimerHandle_CheckWaveTimer, this, &ASGGameModeBase::CheckWaveState, 3.0f, true, 0.0f);
	
	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGGameModeBase::PrepareForNextWave()
{
	FTimerHandle TimerHandle_NextWavePrepTime;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWavePrepTime, this, &ASGGameModeBase::StartWave, TimeBetweenWaves,false);

	SetWaveState(EWaveState::WaitingToStart);

	//Respawn Players
	RespawnDeadPlayers();
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

// Check if any enemies are alive
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
	
	SetWaveState(EWaveState::WaveComplete);

	PrepareForNextWave();
}

void ASGGameModeBase::CheckAnyPlayerAlive()
{
	FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();

	for (It; It; ++It)
	{
		APlayerController* TempPlayerController = It->Get();
		if (!TempPlayerController) { continue; }

		APawn* TempPlayerPawn = TempPlayerController->GetPawn();
		if (!TempPlayerPawn) { continue; }

		USGHealthComponent* HealthComp = TempPlayerPawn->FindComponentByClass<USGHealthComponent>();
		if (!HealthComp) { continue; }

		if (HealthComp->GetHealth() > 0.0f)
		{
			return;	
		}
	}

	// If it gets to this point, there are no players alive
	GameOver(); // TODO: This isn't working correctly in Dedicated server mode (triggers before players join). Fix.


	
	
}

void ASGGameModeBase::GameOver()
{
	EndWave();

	SetWaveState(EWaveState::GameOver);

	// TODO: Finish up the match, present Game Over to players.
}

void ASGGameModeBase::SetWaveState(EWaveState NewState)
{
	ASGGameStateBase* GameState = GetGameState<ASGGameStateBase>(); // Has a cast type built in

	if (!GameState) { return; }

	// Call the SetWaveState() on the Game State to modify it's value.
	GameState->SetWaveState(NewState);
}

void ASGGameModeBase::RespawnDeadPlayers()
{
	FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();

	for (It; It; ++It)
	{
		// Check if it's a player controller
		APlayerController* TempPlayerController = It->Get();
		if (!TempPlayerController) { continue; }

		// Check the pawn for this player exists (Is it dead?)
		APawn* TempPlayerPawn = TempPlayerController->GetPawn();
		if (TempPlayerPawn == nullptr) // its dead
		{
			// Respawn the player pawn
			RestartPlayer(TempPlayerController);
		}
	}
}
