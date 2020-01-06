// Fill out your copyright notice in the Description page of Project Settings.


#include "SGHealthComponent.h"
#include "GameFramework/Actor.h" // GetOwner()
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps

// Sets default values for this component's properties
USGHealthComponent::USGHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	DefaultHealth = 100;

	SetIsReplicated(true);  // Components use this instead of SetReplicates(true);
}


// Called when the game starts
void USGHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// [NETWORKING]: Only setup hook if we are the server running this code.
	if (GetOwnerRole() == ROLE_Authority) // This is used for components, to get the role.
	{
		AActor* MyOwner = GetOwner();
		if (!MyOwner) { return; }
	
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USGHealthComponent::HandleTakeAnyDamage);
	}

	Health = DefaultHealth;
}

/// NOTE: This is only setup on the server (in BeginPlay()), which means this won't run on any client.
void USGHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}


// //[NETWORKING] Networking Replication Rules
void USGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USGHealthComponent, Health);       // Example: DOREPLIFETIME(AIGuard, GuardState);
}