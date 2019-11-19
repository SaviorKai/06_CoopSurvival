// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class COOPSURVIVAL_API ASGWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Setup")
	USkeletalMeshComponent* GunMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
