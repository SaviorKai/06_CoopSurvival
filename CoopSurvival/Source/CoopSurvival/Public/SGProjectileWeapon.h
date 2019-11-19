// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGWeapon.h"
#include "SGProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPSURVIVAL_API ASGProjectileWeapon : public ASGWeapon
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	TSubclassOf<AActor> ProjectileClass;

};
