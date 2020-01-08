// Fill out your copyright notice in the Description page of Project Settings.


#include "SGPowerUp.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps

// Sets default values
ASGPowerUp::ASGPowerUp()
{
	PowerUpInterval = 0.0f;
	TotalNumOfTicks = 0;

	SetReplicates(true);

	bIsPowerUpActive = false;
}

// Called when the game starts or when spawned
void ASGPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

void ASGPowerUp::OnTickPowerUp()
{
	TicksProcessed++;  // Add 1 to TicksProcessed

	OnPowerUpTicked(); // Run the blueprint function for FX to be able to be attached.

	if (TicksProcessed >= TotalNumOfTicks)
	{
		OnExpired();

		bIsPowerUpActive = false;	// This is replicated_using, and thus it will run the replicated function on clients when required.
		OnRep_PowerUpActive();		// Note that we manually call the function here, since it will not be REPLICATED TO SERVER, only to clients, since we're running from server.

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);
	}
}

void ASGPowerUp::ActivatePowerUp(AActor* OtherActor)
{
	OnActivated(OtherActor);

	bIsPowerUpActive = true;	// This is replicated_using, and thus it will run the replicated function on clients when required.
	OnRep_PowerUpActive();		// Note that we manually call the function here, since it will not be REPLICATED TO SERVER, only to clients, since we're running from server.

	if (PowerUpInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &ASGPowerUp::OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}


void ASGPowerUp::OnRep_PowerUpActive()
{
	OnPowerUpStateChanged(bIsPowerUpActive);
}


// //[NETWORKING] Networking Replication Rules
void ASGPowerUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGPowerUp, bIsPowerUpActive);

}