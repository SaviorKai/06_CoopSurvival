// Fill out your copyright notice in the Description page of Project Settings.


#include "SGPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SGPowerUp.h"
#include "TimerManager.h"
#include "SGCharacter.h"

// Sets default values
ASGPickupActor::ASGPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(75);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetRelativeRotation(FRotator(90, 0, 0));
	DecalComp->DecalSize = FVector(64, 75, 75);

	RespawnCooldown = 5.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASGPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority) // Spawn on server only
	{
		RespawnPowerUp();
	}
}

void ASGPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{
		// Check if its a Character
		if (!Cast<ASGCharacter>(OtherActor)) { return; }

		if (PowerUpInstance)
		{
			PowerUpInstance->ActivatePowerUp(OtherActor);
			PowerUpInstance = nullptr; //Reset the instance;

			//Set timer for respawn
			GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASGPickupActor::RespawnPowerUp, RespawnCooldown);
		}
	}
}

void ASGPickupActor::RespawnPowerUp()
{
	if (!PowerUp) //Pointer Protection
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUp class not set in blueprint for %s"), *GetName());
		return;
	}
	
	// Spawn PowerUp
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance = GetWorld()->SpawnActor<ASGPowerUp>(PowerUp, GetTransform(), SpawnParams);
}

