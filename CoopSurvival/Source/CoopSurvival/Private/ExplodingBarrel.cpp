// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplodingBarrel.h"
#include "SGHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"


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
	
	HealthComponent = CreateDefaultSubobject<USGHealthComponent>(TEXT("Health_Component"));			// Doesn't need attachment, since it's a ActorComponent (not scene component)
	
	
}

// Called when the game starts or when spawned
void AExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->OnHealthChanged.AddDynamic(this, &AExplodingBarrel::HandleOnHealthChanged);
	RadialForceComponent->ForceStrength = ExplosionForce;
}

// Called every frame
void AExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AExplodingBarrel::HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	
	

	if (Health <= 0.0f && !bHasDied)
	{
		//Explode!
		bHasDied = true;

		//Add Upwards Force
		FVector BoostIntensity = FVector::UpVector * UpwardForce;
		MeshComponent->AddImpulse(BoostIntensity, NAME_None, true);

		//RadialExplosionForce
		RadialForceComponent->FireImpulse();

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
		
		UE_LOG(LogTemp, Warning, TEXT("Exploded!"))

	}
}
