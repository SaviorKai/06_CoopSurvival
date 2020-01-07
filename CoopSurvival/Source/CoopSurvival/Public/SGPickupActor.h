// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;

UCLASS()
class COOPSURVIVAL_API ASGPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;


public:	

	/// Overide this function so we can use it
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
