// Fill out your copyright notice in the Description page of Project Settings.


#include "SGPowerUp.h"
#include "TimerManager.h"

// Sets default values
ASGPowerUp::ASGPowerUp()
{
	PowerUpInterval = 0.0f;
	TotalNumOfTicks = 0;
}

// Called when the game starts or when spawned
void ASGPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

void ASGPowerUp::OnTickPowerUp()
{
	TicksProcessed++;  // Add 1 to TicksProcessed

	OnPowerUpTicked(); // Run the blueprint function.

	if (TicksProcessed >= TotalNumOfTicks)
	{
		//Spawn Powerup? 
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);

	}
}

void ASGPowerUp::ActivatePowerUp()
{
	if (PowerUpInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &ASGPowerUp::OnTickPowerUp, PowerUpInterval, true, 0.0f);
	}
	else
	{
		//OnTickPowerUp(); // <--- do we really need this if first tick interval is 0.0f?
	}
}


