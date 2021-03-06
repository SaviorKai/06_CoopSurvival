// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGHealthComponent.generated.h"

/// On Health Changed Event (Delegate)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USGHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPSURVIVAL_API USGHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USGHealthComponent();

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category=Setup)
	void Heal(float HealAmount);

	//HelperFunction
	float GetHealth() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	uint8 TeamNum;											// We are using uint8 here instead of int32 because we are trying to limit what we replicate. uint8 uses less, and only has 255 values.

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Setup)
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bHasDied;

	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category = "Setup")
	float Health;

	UFUNCTION()
	void OnRep_Health(float OldHealth);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float DefaultHealth;

	UFUNCTION()	// ALWAYS USE WITH DELEGATES
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	//[NETWORKING] Network Replication Rules Function (Not required, but Added here for code readability.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
};
