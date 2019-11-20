// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPSURVIVAL_API ASGWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Fire();

protected:
	virtual void PlayFireEffects(FVector FinalHitLocation);

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



};
