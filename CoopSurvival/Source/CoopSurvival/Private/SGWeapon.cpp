// Fill out your copyright notice in the Description page of Project Settings.


#include "SGWeapon.h"
#include "Engine/World.h" //GetWorld()
#include "DrawDebugHelpers.h" // Debug Drawing
#include "Kismet/GameplayStatics.h" // Damage
#include "Particles/ParticleSystem.h" // For Particle system spawning.
#include "Particles/ParticleSystemComponent.h" //TracerPartSystemComp
#include "Components/SkeletalMeshComponent.h" //GetSocketLocation
#include "Camera/CameraShake.h" // UCameraShake
#include "PhysicalMaterials/PhysicalMaterial.h" // UPhysicalMaterial
#include "TimerManager.h" // Timers
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps

/// Console Command for DebugDrawing on Weapons
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),									// Command we want to enter in console
	DebugWeaponDrawing,											// The Var we want to reference
	TEXT("Draws debug lines for weapons."),						// Help text.
	ECVF_Cheat);												// States that this is a cheat, and will only work with cheats enabled.


// Sets default values
ASGWeapon::ASGWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	/// Create Mesh Component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
	SetRootComponent(Cast<USceneComponent>(GunMeshComponent));

	MuzzleSocketName = "MuzzleSocket";
	TracerBeamEndName = "BeamEnd";

	BulletSpread = 2.0f;

	/// [NETWORKING]: Ensuring the tick rate of this actor is not changed by the engine.
	SetReplicates(true);
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

}


void ASGWeapon::StartFire()
{
	/// Get Time since Fired, and Fire if enough time has passed.
	float CurrentTime = GetWorld()->TimeSeconds;
	float TimeSinceFired = CurrentTime - LastFireTime;

	if (TimeSinceFired > FireRate)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASGWeapon::Fire, FireRate, true, 0.0f); // Sets the timer (var), for this class, to execute this funciton, every 1.0f second (loop)
	}
}

void ASGWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);  // Stop the timer and ensures the event doesn't go off again. 
}

void ASGWeapon::Fire()
{
	/// [NETWORKING] If this is run on client, tell it to run the server fire command for that actor on itself and OTHER clients, which will tell the server to run the ServerFunction. The ServerFunction will then run Fire();
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}
	
	/// Trace the world, from Pawn Eyes, to Crosshair location.															
	
	// Calculate Start Location & Direction Vector
	auto MyOwner = GetOwner();															// NOTE!!! You have to set it's 'Owner' var on SPAWN for this to work. 
	if (!MyOwner) { return; }															// Pointer protection.
	FVector EyeLocation;
	FRotator EyeRotation;																// We don't use this, but need an Out parameter for this function.
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);							// OUT parameters
	FVector ShotDirection = EyeRotation.Vector();
	float HalfRad = FMath::DegreesToRadians(BulletSpread);								// Change degrees to radians
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);			// Create the randomized Cone of spread.

	/// Calculate End Location
	FVector EndLocation = EyeLocation + (ShotDirection * 10000);					// No need to update this magic number, it should be far enough to hit something.
	FVector FinalHitLocation = EndLocation;						// This is set here, and updated depending on if we hit something.

	// Set Default Hit SurfaceType Var
	EPhysicalSurface HitSurfaceType = EPhysicalSurface::SurfaceType_Default;

	// Set Collision Parameters for the trace
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);			// Ignore our Owner character
	QueryParams.AddIgnoredActor(this);				// Ignore ourselves (gun mesh)
	QueryParams.bTraceComplex = true;				// Trace to Complex Collision Mesh on Target (not Simple collision mesh)
	QueryParams.bReturnPhysicalMaterial = true;		// Required for getting the surface types via surface material.

	/// Do the line trace
	FHitResult Hit;
	if ( // If Line Trace succeeds //
		GetWorld()->LineTraceSingleByChannel(
			Hit,																	// OUT Parameter
			EyeLocation,
			EndLocation,
			ECC_GameTraceChannel1,													// ECC_GameTraceChannel1 is set in UE4 Project Properties in TraceChannels // ECC_Visibility Anything blocking our visibility
			QueryParams																// Pass the Query Params
		))
	{
		/// Update FinalHitLocation (for Tracer Particle)
		FinalHitLocation = Hit.ImpactPoint;
		
		/// Process Damage
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) { return; }													// Pointer Protection

		HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());  // Used Get() here, because its a weak object pointer and we want to get the object.

		float DamageToApply = BaseDamage;
		if (HitSurfaceType == SurfaceType2) //  if FleshCritical	
		{
			DamageToApply = BaseDamage * 4.0f; // Apply Critical bonus
		}

		UGameplayStatics::ApplyPointDamage(
			HitActor,
			DamageToApply,
			EyeRotation.Vector(),
			Hit,
			MyOwner->GetInstigatorController(),
			MyOwner,
			DamageType
		);
	}

	/// Play Particle Effects
	PlayFireEffects(FinalHitLocation);
	PlayImpactEffects(HitSurfaceType, FinalHitLocation);
		
	/// [NETWORKING]: If you are the server running this code:
	if (Role == ROLE_Authority)
	{
		HitScanTrace.TraceTo = FinalHitLocation;
		HitScanTrace.SurfaceType = HitSurfaceType;
	}

	/// Set Last Fire Time for Weapon cool down
	LastFireTime = GetWorld()->TimeSeconds;

	/// DEBUG CODE ///
	if (DebugWeaponDrawing > 0)
	{
		DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::White, false, 1.0f, 0, 1.0f);
	}

}

void ASGWeapon::PlayFireEffects(FVector FinalHitLocation)
{
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
		TracerPartSystemComp->SetVectorParameter(TracerBeamEndName, FinalHitLocation); //NOTE: Name was something we get/set from the particle system in the UE4 Editor.
	}

	/// Camera Shake
	// Get Owning Actor, and Cast to APawn for GetController()
	auto MyPawn = Cast<APawn>(GetOwner());										
	
	// Get AController and Cast to APlayerController for ClientPlayCameraShake()
	if (!MyPawn) { return; }													// Pointer protection.
	auto MyPlayerController = Cast<APlayerController>(MyPawn->GetController());

	// Play Shake 
	if (!MyPlayerController) { return; }
	MyPlayerController->ClientPlayCameraShake(FireCamShakeClass);    

}

void ASGWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	/// Play Particle Effect Based on Surface Hit (NOTE! Make sure QueryParams.bReturnPhysicalMaterial = true; for the linetrace hit)
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)											// Check the HitSurfaceType and run code depending on its result
	{
	case SurfaceType1:	// FleshDefault								// SurfaceType1 is a var set in UE4 Project Editor. You can define this to something else if you want.
		SelectedEffect = FleshHitEffect;
		break;

	case SurfaceType2: // FleshCritical								// SurfaceType2 is a var set in UE4 Project Editor (Surface Types)
		SelectedEffect = FleshCritEffect;
		break;

	default:
		SelectedEffect = DefaultHitEffect;							// If none of the above, use the default.
		break;
	}

	if (SelectedEffect)												// Play Selected Effect
	{
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation(MuzzleSocketName);
		
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

// [NETWORKING]
void ASGWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASGWeapon::ServerFire_Validate()
{
	return true;
}


void ASGWeapon::OnRep_HitScanTrace()
{
	/// Play Particle Effects
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

// //[NETWORKING] Networking Replication Rules
void ASGWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASGWeapon, HitScanTrace, COND_SkipOwner);       // Example: DOREPLIFETIME(AIGuard, GuardState);
}
