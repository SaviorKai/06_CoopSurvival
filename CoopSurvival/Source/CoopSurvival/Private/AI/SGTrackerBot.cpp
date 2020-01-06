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

	bUseVelocityChange = false;
	

}

// Called when the game starts or when spawned
void ASGTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
	
}

// Called every frame
void ASGTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

FVector ASGTrackerBot::GetNextPathPoint()
{
	FVector MyLocation = GetActorLocation();
	ACharacter* TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // TODO: Fix for multiplayer

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(
		GetWorld(),
		MyLocation,
		TargetActor
	);

	if (NavPath->PathPoints.Num() > 1) 
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


void ASGTrackerBot::HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on Hitpoints on 0
	
	UE_LOG(LogTemp, Warning, TEXT("%s 's Health is %f"), *GetName(), Health);
	
	// TODO: Pulse Material on hit
}