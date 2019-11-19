// Fill out your copyright notice in the Description page of Project Settings.


#include "SGWeapon.h"
#include "Engine/World.h" //GetWorld()
#include "DrawDebugHelpers.h" // Debug Drawing
#include "Kismet/GameplayStatics.h" // Damage
#include "Particles/ParticleSystem.h" // For Particle system spawning.
#include "Particles/ParticleSystemComponent.h" //TracerPartSystemComp
#include "Components/SkeletalMeshComponent.h" //GetSocketLocation

// Sets default values
ASGWeapon::ASGWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create Mesh Component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
	SetRootComponent(Cast<USceneComponent>(GunMeshComponent));

	MuzzleSocketName = "MuzzleSocket";

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
	auto MyOwner = GetOwner();													// NOTE!!! You have to set it's 'Owner' var on SPAWN for this to work. 
	if (!MyOwner) { return; }													// Pointer protection.
	FVector EyeLocation;
	FRotator EyeRotation;														// We don't use this, but need an Out parameter for this function.
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);					// OUT parameters
	
	// Calculate End Location
	FVector EndLocation = EyeLocation + (EyeRotation.Vector() * 10000);			// No need to update this magic number, it should be far enough to hit something.
	FVector FinalHitLocation = EndLocation;										// This is set here, and updated depending on if we hit something.

	// Set Collision Parameters for the trace
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);		// Ignore our Owner character
	QueryParams.AddIgnoredActor(this);			// Ignore ourselves (gun mesh)
	QueryParams.bTraceComplex = true;			// Trace to Complex Collision Mesh on Target (not Simple collision mesh)

	/// Do the line trace
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
		// Update FinalHitLocation (for Tracer Particle)
		FinalHitLocation = Hit.ImpactPoint;
		
		// Process Damage
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) { return; }													// Pointer Protection

		UGameplayStatics::ApplyPointDamage(
			HitActor,
			BaseDamage,
			EyeRotation.Vector(),
			Hit,
			MyOwner->GetInstigatorController(),
			this,
			DamageType
		);

		// Particle Effect: Hit
		if (HitEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}
	}

	/// Particle Effect: Muzzle
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMeshComponent, MuzzleSocketName);
	}

	/// Particle Effect: TracerSmoke
	if (TracerEffect)
	{
		// Get the socket location from the mesh
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation(MuzzleSocketName);
		
		// Spawn the Particle system start point at the muzzle (and set it to a var we can call)
		auto TracerPartSystemComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (!TracerPartSystemComp) { return; }
		
		// Set parameters specific to this Particle System Type (Beam)
		TracerPartSystemComp->SetVectorParameter("BeamEnd", FinalHitLocation); //NOTE: Name was something we get/set from the particle system in the UE4 Editor.
	}
	/// DEBUG CODE ///
	//DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::White, false, 1.0f, 0, 1.0f);

}

// Called every frame
void ASGWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

