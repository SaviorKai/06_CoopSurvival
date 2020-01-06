// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SGTrackerBot.generated.h"

class USGHealthComponent;


UCLASS()
class COOPSURVIVAL_API ASGTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASGTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MyMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USGHealthComponent* MyHealthComponent;
	
	UFUNCTION()
	void HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UMaterialInstanceDynamic* MaterialInstance; // DynamicMaterrial to Pulse on Damage
	
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float MovementForce = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float FollowDistance = 100;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bUseVelocityChange;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
