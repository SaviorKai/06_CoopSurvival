// Fill out your copyright notice in the Description page of Project Settings.


#include "SGPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

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
}

// Called when the game starts or when spawned
void ASGPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASGPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// TODO: Grant Player Powerup if available.
}

