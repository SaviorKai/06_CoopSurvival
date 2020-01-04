// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SGTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ActorComponent.h" //SetCanEverAffectNavigation()
#include "NavigationSystem.h" //UNavigationSystemV1
#include "NavigationPath.h" //UNavigationPath
#include "Kismet/GameplayStatics.h" //UGameplayStatics
#include "DrawDebugHelpers.h" // DrawDebugSphere
#include "GameFramework/Character.h"


// Sets default values
ASGTrackerBot::ASGTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MyMeshComp->SetCanEverAffectNavigation(false);
	SetRootComponent(MyMeshComp);
	

}

// Called when the game starts or when spawned
void ASGTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	GetNextPathPoint();
	
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

			UE_LOG(LogTemp, Warning, TEXT("PathPoint Drawn"));
		}

		return NavPath->PathPoints[1];
	}
	//Failed to find point
	return GetActorLocation();
}

// Called every frame
void ASGTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

