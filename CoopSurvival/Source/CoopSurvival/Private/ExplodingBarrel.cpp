// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplodingBarrel.h"
#include "SGHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps

// Sets default values
AExplodingBarrel::AExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh_Component"));
	SetRootComponent(MeshComponent);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce_Component"));
	RadialForceComponent->SetupAttachment(MeshComponent); 
	RadialForceComponent->SetAutoActivate(false);
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->ForceStrength = ExplosionForce;
	
	MyHealthComponent = CreateDefaultSubobject<USGHealthComponent>(TEXT("Health_Component"));			// Doesn't need attachment, since it's a ActorComponent (not scene component)
	
	SetReplicates(true);
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	MyHealthComponent->OnHealthChanged.AddDynamic(this, &AExplodingBarrel::HandleOnHealthChanged);
	RadialForceComponent->ForceStrength = ExplosionForce;
}

// Called every frame
void AExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// NOTE: Remember this function only runs on server due to how health component hook has been implemented.
void AExplodingBarrel::HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bHasDied)
	{
		//Explode!
		bHasDied = true;

		PlayExplodeEffects();
	}
}

void AExplodingBarrel::PlayExplodeEffects()
{
	// TODO: Add Radial Damage on explode

	/// Add forces if Server (Note: These are only done server side, since movement is replicated.
	if (ROLE_Authority)
	{
		//Add Upwards Force
		FVector BoostIntensity = FVector::UpVector * UpwardForce;
		MeshComponent->AddImpulse(BoostIntensity, NAME_None, true);

		//RadialExplosionForce
		RadialForceComponent->FireImpulse();

	}

	//Change Material
	if (ExplodedMaterial)
	{
		MeshComponent->SetMaterial(0, ExplodedMaterial);
	}

	//Spawn Explosion FX
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetTransform());
	}
}

// [NETWORKING]: When the replicated var gets changed, run this function
void AExplodingBarrel::OnRep_HasDied()
{
	PlayExplodeEffects();
}

// //[NETWORKING] Networking Replication Rules
void AExplodingBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplodingBarrel, bHasDied);
}