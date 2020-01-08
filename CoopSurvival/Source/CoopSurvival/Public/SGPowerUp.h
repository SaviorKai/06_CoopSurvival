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

	int32 TicksProcessed;

	UFUNCTION()
	void OnTickPowerUp();

public:	

	void ActivatePowerUp();

	UFUNCTION(BlueprintCallable, Category = "PowerUps")
	void OnActivated();
	
	UFUNCTION(BlueprintCallable, Category = "PowerUps")
	void OnPowerUpTicked();

	UFUNCTION(BlueprintCallable, Category = "PowerUps")
	void OnExpired();
};
