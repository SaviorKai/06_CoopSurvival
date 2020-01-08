// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGPowerUp.generated.h"

UCLASS()
class COOPSURVIVAL_API ASGPowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGPowerUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Time between powerup ticks
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float PowerUpInterval;

	// Total times we apply the power up effect.
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int32 TotalNumOfTicks;

	FTimerHandle TimerHandle_PowerUpTick;

	int32 TicksProcessed = 0;

	UFUNCTION()
	void OnTickPowerUp();

	UPROPERTY(ReplicatedUsing = OnRep_PowerUpActive)    //NOTE: Don't add Categories to Replicated Using Functions.
	bool bIsPowerUpActive;

	UFUNCTION()
	void OnRep_PowerUpActive();

	//[NETWORKING] Network Replication Rules Function (Not required, but Added here for code readability.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	

	void ActivatePowerUp(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")		// REMEMBER: We don't define BlueprintImplementableEvent in C++! It's defined in BluePrints.
	void OnActivated(AActor* OtherActor);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")		// REMEMBER: We don't define BlueprintImplementableEvent in C++! It's defined in BluePrints.
	void OnPowerUpTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")		// REMEMBER: We don't define BlueprintImplementableEvent in C++! It's defined in BluePrints.
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")		// REMEMBER: We don't define BlueprintImplementableEvent in C++! It's defined in BluePrints.
	void OnPowerUpStateChanged(bool bNewIsActive);
};
