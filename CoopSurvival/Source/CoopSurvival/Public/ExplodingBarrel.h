// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplodingBarrel.generated.h"


class USGHealthComponent;
class UStaticMeshComponent;
class URadialForceComponent;
class UMaterialInterface;
class UParticleSystem;


UCLASS()
class COOPSURVIVAL_API AExplodingBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplodingBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Setup")
	USGHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Setup")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Setup")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(ReplicatedUsing = OnRep_HasDied)
	bool bHasDied = false;

	UFUNCTION()
	void OnRep_HasDied();

	UFUNCTION()
	void HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void PlayExplodeEffects();

	//[NETWORKING] Network Replication Rules Function (Not required, but Added here for code readability.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float UpwardForce = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ExplosionForce = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterialInterface* ExplodedMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* ExplosionEffect;
};
