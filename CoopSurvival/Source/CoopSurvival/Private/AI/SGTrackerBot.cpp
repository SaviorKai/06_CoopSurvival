// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SGTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ActorComponent.h" //SetCanEverAffectNavigation()

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
	
}

// Called every frame
void ASGTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

