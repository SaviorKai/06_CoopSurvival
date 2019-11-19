// Fill out your copyright notice in the Description page of Project Settings.


#include "SGWeapon.h"
#include "Engine/World.h" //GetWorld()
#include "DrawDebugHelpers.h" // Debug Drawing

// Sets default values
ASGWeapon::ASGWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create Mesh Component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
	SetRootComponent(Cast<USceneComponent>(GunMeshComponent));


}

// Called when the game starts or when spawned
void ASGWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASGWeapon::Fire()
{
	/// Trace the world, from Pawn Eyes, to Crosshair location.															
	
	// Calculate Start Location
	auto MyOwner = GetOwner();
	if (!MyOwner) { return; }													// Pointer protection.
	FVector EyeLocation;
	FRotator EyeRotation;														// We don't use this, but need an Out parameter for this function.
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);					// OUT parameters
	
	// Calculate End Location
	FVector EndLocation = EyeLocation + (EyeRotation.Vector() * 10000);			// No need to update this magic number, it should be far enough to hit something.

	// Set Collision Parameters for the trace
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);		// Ignore our Owner character
	QueryParams.AddIgnoredActor(this);			// Ignore ourselves (gun mesh)
	QueryParams.bTraceComplex = true;			// Trace to Complex Collision Mesh on Target (not Simple collision mesh)

	// Do the line trace
	FHitResult Hit;
	if ( // If Line Trace succeeds //
		GetWorld()->LineTraceSingleByChannel(
			Hit,																	// OUT Parameter
			EyeLocation,
			EndLocation,
			ECC_Visibility,															// Anything blocking our visibility
			QueryParams																// Pass the Query Params
		))
	{
		// Do code.
	}

	/// DEBUG CODE ///
	DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::White, false, 1.0f, 0, 1.0f);

	
}

// Called every frame
void ASGWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

