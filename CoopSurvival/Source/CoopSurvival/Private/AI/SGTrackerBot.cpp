// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SGTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ActorComponent.h" //SetCanEverAffectNavigation()
#include "NavigationSystem.h" //UNavigationSystemV1
#include "NavigationPath.h" //UNavigationPath
#include "Kismet/GameplayStatics.h" //UGameplayStatics
#include "DrawDebugHelpers.h" // DrawDebugSphere
#include "GameFramework/Character.h"
#include "SGHealthComponent.h" //USGHealthComponent
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"
#include "SGCharacter.h"
#include "TimerManager.h"

// Sets default values
ASGTrackerBot::ASGTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MyMeshComp->SetCanEverAffectNavigation(false);
	MyMeshComp->SetSimulatePhysics(true);
	SetRootComponent(MyMeshComp);

	MyHealthComponent = CreateDefaultSubobject<USGHealthComponent>(TEXT("MyHealthComp"));
	MyHealthComponent->OnHealthChanged.AddDynamic(this, &ASGTrackerBot::HandleOnHealthChanged);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(200);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);				// Important for performance enhancement
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);					// Important for performance enhancement
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);			// Important for performance enhancement
	SphereComponent->SetupAttachment(RootComponent);

	AllySenseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AllySenseSphere"));
	AllySenseSphere->SetSphereRadius(300);
	AllySenseSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);				// Important for performance enhancement
	AllySenseSphere->SetCollisionResponseToAllChannels(ECR_Ignore);					// Important for performance enhancement
	AllySenseSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);			// Important for performance enhancement
	AllySenseSphere->SetupAttachment(RootComponent);

	bUseVelocityChange = false;

	//SetReplicateMovement(true);
	//SetReplicates(true);
}

// Called when the game starts or when spawned
void ASGTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Start Power Level Timer
	GetWorldTimerManager().SetTimer(TimerHandle_SetPowerLevel, this, &ASGTrackerBot::SetPowerLevel, 1.0f, true, 0.0f);

	/// [NETWORKING]: Run Gettig next path point on server only.
	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
	}
}


// Called every frame
void ASGTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	/// [NETWORKING]: Run Movement Code on Server Only
	if (Role == ROLE_Authority && bHasDied == false)
	{
		///Movement
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();               // FVector::Size() allows you to take the number and convert it into magnitude.
	
		if (DistanceToTarget > FollowDistance)								// .Equals() checks a vector with tolerance.
		{
			///Keep moving to NextPathPoint
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			MyMeshComp->AddForce(ForceDirection * MovementForce, NAME_None, bUseVelocityChange);
		}
		else
		{
			NextPathPoint = GetNextPathPoint();
		}
	}

	/// TODO: Move the Rolling sound from Blueprints to C++? 
}

FVector ASGTrackerBot::GetNextPathPoint()
{
	FVector MyLocation = GetActorLocation();
	ACharacter* TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // TODO: Fix for multiplayer

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(
		GetWorld(),
		MyLocation,
		TargetActor
	);

	if (NavPath && NavPath->PathPoints.Num() > 1) 
	{
		/// DEBUGDRAWING  /// TODO: Rework this method, its CPU intensive. Turn on Debug Drawing below to see what I mean here.
		/*
		for (auto i : NavPath->PathPoints)
		{
			DrawDebugSphere(
				GetWorld(),
				i,
				24,
				32,
				FColor(255, 0, 0),
				false,
				200.0f
			); 
		}   
		*/

		return NavPath->PathPoints[1];
	}
	//Failed to find point
	return GetActorLocation();
}

/// !! NOTE: The delegate is setup on the HEALTH COMPONENT for CLIENTS AND SERVER to run. We added an extra broadcast for clients. 
void ASGTrackerBot::HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	/// SelfDestruct on Hitpoints = 0
	if (Health <= 0.0f && bHasDied == false)
	{
		SelfDestruct();
	}
	
	/// Pulse Material on hit
	if (MaterialInstance == nullptr)
	{
		// This creates a reference to the material
		MaterialInstance = MyMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MyMeshComp->GetMaterial(0));
	}

	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds); //This is the paramater we created in the material editor.
	}
	
}

void ASGTrackerBot::SelfDestruct()
{
	bHasDied = true;
	
	// Spawn FX Emitter
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	/// Death
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	MyMeshComp->SetVisibility(false);
	MyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		// Apply AOE Damage
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			ExplodeDamage * PowerLevel,
			GetActorLocation(),
			ExplodeRadius,
			nullptr,
			IgnoredActors,   // NOTE: You can also use this, if you don't want to create a filter: TArray<AActor*>()
			this,
			GetInstigatorController(),
			true);

		// Destroy Self
		//Destroy();
		SetLifeSpan(2.0f);
	}
}



void ASGTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// Check if the overlapping actor is a character
	if (!bStartedSelfDestruct && !bHasDied)
	{
		// Check if it's a character (player)
		if (Cast<ASGCharacter>(OtherActor)) 
		{
			if (Role == ROLE_Authority)
			{
				//Start Self destruct Sequence Damage
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASGTrackerBot::DamageSelf, 0.5f, true, 0.0f);
			}
		
			bStartedSelfDestruct = true;

			if (SelfDestructNoticeSound)
			{
				UGameplayStatics::SpawnSoundAttached(SelfDestructNoticeSound, MyMeshComp, NAME_None);
			}
		}
	}
}

void ASGTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, SelfDamageInterval, GetInstigatorController(), this, nullptr);
}

void ASGTrackerBot::SetPowerLevel()
{
	PowerLevel = 0;

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	
	for (auto i : OverlappingActors)
	{
		if (Cast<ASGTrackerBot>(i))
		{
			PowerLevel = FMath::Clamp(PowerLevel + 1, 0, MaxPowerLevel);
		}
	}

	/// PowerLevel Material Changes
	if (MaterialInstance == nullptr)
	{
		// This creates a reference to the material
		MaterialInstance = MyMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MyMeshComp->GetMaterial(0));
	}

	if (MaterialInstance)
	{
		float PowerLevelAlpha = float(PowerLevel) / float(MaxPowerLevel); // NOTE: You have to turn both values into floats before doing the math, or else it will return 0, 1.

		MaterialInstance->SetScalarParameterValue("PowerrLevelAlpha", PowerLevelAlpha); //This is the paramater we created in the material editor.
	}

}