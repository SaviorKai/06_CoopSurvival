// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SGTrackerBot.generated.h"

class USGHealthComponent;
class UParticleSystem;
class USphereComponent;


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

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MyMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USGHealthComponent* MyHealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* AllySenseSphere;

	UFUNCTION()
	void HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	void SelfDestruct();

	void SetPowerLevel();

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ExplodeDamage = 40;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ExplodeRadius = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float MovementForce = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float FollowDistance = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float SelfDamageInterval = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bUseVelocityChange;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	USoundBase* DeathSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	USoundBase* SelfDestructNoticeSound;

	FVector NextPathPoint = FVector(0);

	bool bHasDied = false;
	bool bStartedSelfDestruct = false;
	int32 PowerLevel = 0;
	int32 MaxPowerLevel = 3;

	UMaterialInstanceDynamic* MaterialInstance; // DynamicMaterrial to Pulse on Damage

	FTimerHandle TimerHandle_SelfDamage;
	FTimerHandle TimerHandle_SetPowerLevel;

	void DamageSelf();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// Overide this function so we can use it
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};