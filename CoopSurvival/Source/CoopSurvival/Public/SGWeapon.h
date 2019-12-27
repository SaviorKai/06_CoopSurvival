// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

// Contains information of a single hitscan weapon linetrace.
USTRUCT()
struct  FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType; ///NOTE: We have to use TEnumAsByte for it to work in this struct and for it to replicate
	
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};


UCLASS()
class COOPSURVIVAL_API ASGWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void StopFire();

protected:
	virtual void PlayFireEffects(FVector FinalHitLocation);
	
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation) //[NETWORKING]
	void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")									// NOTE: This is set in the blueprint editor
	float FireRate = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Setup")
	USkeletalMeshComponent* GunMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	float BaseDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")				// NOTE: This is set in the blueprint editor (and we made it a dropdown)
	TSubclassOf<UDamageType> DamageType;										

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FName TracerBeamEndName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")				// NOTE: This is set in the blueprint editor
	UParticleSystem* MuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")				// NOTE: This is set in the blueprint editor
	UParticleSystem* DefaultHitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")				// NOTE: This is set in the blueprint editor
	UParticleSystem* FleshHitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")				// NOTE: This is set in the blueprint editor
	UParticleSystem* FleshCritEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")				// NOTE: This is set in the blueprint editor
	UParticleSystem* TracerEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")									// NOTE: This is set in the blueprint editor (and we made it a dropdown)
	TSubclassOf<UCameraShake> FireCamShakeClass;
	
	
	/// [NETWORKING]
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;
	
	UFUNCTION()
	void OnRep_HitScanTrace();

	//[NETWORKING] Network Replication Rules Function (Not required, but Added here for code readability.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
