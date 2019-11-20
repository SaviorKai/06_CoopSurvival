// Fill out your copyright notice in the Description page of Project Settings.


#include "SGHealthComponent.h"
#include "GameFramework/Actor.h" // GetOwner()

// Sets default values for this component's properties
USGHealthComponent::USGHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	DefaultHealth = 100;
}


// Called when the game starts
void USGHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* MyOwner = GetOwner();
	if (!MyOwner) { return; }
	
	MyOwner->OnTakeAnyDamage.AddDynamic(this, &USGHealthComponent::HandleTakeAnyDamage);
	
	Health = DefaultHealth;

	
}

void USGHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
}


