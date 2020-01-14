// Fill out your copyright notice in the Description page of Project Settings.


#include "SGHealthComponent.h"
#include "GameFramework/Actor.h" // GetOwner()
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps
#include "SGGameModeBase.h"
#include "Engine/World.h"

// Sets default values for this component's properties
USGHealthComponent::USGHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	DefaultHealth = 100;

	bHasDied = false;

	TeamNum = 255;

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

void USGHealthComponent::OnRep_Health(float OldHealth)  // NOTE: We can use the abbreviation 'Old' directly infront of the value, to get the value it was before it changed and was replicated.
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

/// NOTE: This is only setup on the server (in BeginPlay()), which means this won't run on any client.
void USGHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bHasDied) { return; }
	if (Damage <= 0.0f) { return; }
	
	// Check Friendly Fire
	if (DamagedActor != DamageCauser)	// If I'm not damaging myself (Trackerbot) Skip friendly fire check and execute the damage to myself.
	{
		if (IsFriendly(DamagedActor, DamageCauser)) { return; }
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (Health <= 0.0f)
	{
		bHasDied = true;

		// Get the game mode via GetAuthGameMode(), which will only return a valid pointer when run as server.
		ASGGameModeBase* MyGameMode = Cast<ASGGameModeBase>(GetWorld()->GetAuthGameMode());
		if (MyGameMode)
		{
			MyGameMode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}


bool USGHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (!ActorA || !ActorB) { return true; }													// Pointer protection, assume 'Friendly'
	
	USGHealthComponent* HealthComp_A = ActorA->FindComponentByClass<USGHealthComponent>();
	USGHealthComponent* HealthComp_B = ActorB->FindComponentByClass<USGHealthComponent>();

	if (!HealthComp_A || !HealthComp_B) { return true; }										// Pointer protection, assume 'Friendly'
	
	return (HealthComp_A->TeamNum == HealthComp_B->TeamNum);									// Check if A's team num is = to B's  team num, and return bool. True = Friendly.
}


// //[NETWORKING] Networking Replication Rules
void USGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USGHealthComponent, Health);       // Example: DOREPLIFETIME(AIGuard, GuardState);
}

void USGHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0 || Health <= 0.0f) { return; }

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Warning, TEXT("Healed + %f. Current HP = %f"), HealAmount, Health);

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);

}

float USGHealthComponent::GetHealth() const
{
	return Health;
}
