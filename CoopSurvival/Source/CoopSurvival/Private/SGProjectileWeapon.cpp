// Fill out your copyright notice in the Description page of Project Settings.


#include "SGProjectileWeapon.h"
#include "Engine/World.h" // FActorSpawnParameters
#include "Components/SkeletalMeshComponent.h" //GetSocketLocation



void ASGProjectileWeapon::Fire()
{
	auto MyOwner = GetOwner();													// NOTE!!! You have to set it's 'Owner' var on SPAWN for this to work. 
	if (!MyOwner) { return; }													// Pointer protection.
	if (!ProjectileClass) { return; }											// Pointer Protection.

	FVector EyeLocation;
	FRotator EyeRotation;														// We don't use this, but need an Out parameter for this function.
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);					// OUT parameters

	/// Spawn Projectile
	// Setup
	FVector SpawnLocation = GunMeshComponent->GetSocketLocation(MuzzleSocketName);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;	// Override this to always spawn regardless of collision
	SpawnParams.Owner = this;

	// Spawn Projectile
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, EyeRotation, SpawnParams);
}