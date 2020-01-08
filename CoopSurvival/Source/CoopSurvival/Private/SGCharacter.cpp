// Fill out your copyright notice in the Description page of Project Settings.

#include "SGCharacter.h"
#include "Camera/CameraComponent.h" // UCameraComponent
#include "GameFramework/SpringArmComponent.h" // USpringArmComponent
#include "GameFramework/PawnMovementComponent.h" // GetMovementComponent()
#include "Engine/World.h" // FActorSpawnParameters, GetWorld()
#include "SGWeapon.h" // ASGWeapon
#include "Components/CapsuleComponent.h" // Capsule Component
#include "SGHealthComponent.h" // USGHealthComponent
#include "Components/InputComponent.h" // UInputComponent
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASGCharacter::ASGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create Spring Arm for Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;														//Uses the pawn control rotation instead.
	SpringArm->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;							// Turns on Crouching.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);		// Turn Blocking off for the capsule. We want the Mesh to handle this, not capsule.

	/// Create Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArm);

	/// Create Health Component
	MyHealthComponent = CreateDefaultSubobject<USGHealthComponent>(TEXT("HealthComponent"));			// Doesn't need attachment, since it's a ActorComponent (not scene component)

	/// Setup Weapon Socket
	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASGCharacter::BeginPlay()
{
	Super::BeginPlay();

	/// Aim Down Sights FOV
	DefaultFOV = CameraComponent->FieldOfView;			// Gets the current Field of view, and stores it as the default. This is important if the player wants to change it or if you want to change it.
	
	/// OnHealthChanged Delegate
	MyHealthComponent->OnHealthChanged.AddDynamic(this, &ASGCharacter::HandleOnHealthChanged);
	
	/// Spawn Weapon
	if (Role == ROLE_Authority)  // [NETWORKING] : Ensures this is only run on server.
	{
		if (!StarterWeaponClass) { return; }				//Pointer protection
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;  // IMPORTANT
		SpawnParams.Owner = this;
		FVector SpawnLocation = FVector::ZeroVector;
		FRotator SpawnRotation = FRotator::ZeroRotator;

		CurrentWeapon = GetWorld()->SpawnActor<ASGWeapon>(StarterWeaponClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (!CurrentWeapon) { return; }						// Pointer Protection
		CurrentWeapon->AttachToComponent(Cast<USceneComponent>(GetMesh()), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttachSocketName);
	}

}


// Called every frame
void ASGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/// Aim Down Sights FOV
	float CurrentFOV = CameraComponent->FieldOfView;
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV; // This sets the var based on the bool statement.
	float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomInterpSpeed); // FOV this frame   // Interpolation! 
	
	CameraComponent->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/// Setup the movement bindings and the functions to call (remember, don't use () when referencing '&' a function)
	PlayerInputComponent->BindAxis("Move Forward", this, &ASGCharacter::MoveForward);										
	PlayerInputComponent->BindAxis("Move Right", this, &ASGCharacter::MoveRight);

	/// Setup Look bindings
	PlayerInputComponent->BindAxis("Look Up", this, &ASGCharacter::AddControllerPitchInput);								// AddControllerPitchInput is built in function.
	PlayerInputComponent->BindAxis("Turn", this, &ASGCharacter::AddControllerYawInput);										// AddControllerYawInput is built in function.

	/// Bind Actions
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASGCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASGCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);	// Note: Jump is already a function on ACharacter (so no need to use our class, or define this)

	PlayerInputComponent->BindAction("AimDownSight", IE_Pressed, this, &ASGCharacter::BeginZoom);
	PlayerInputComponent->BindAction("AimDownSight", IE_Released, this, &ASGCharacter::EndZoom);

	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &ASGCharacter::StartFireWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Released, this, &ASGCharacter::StopFireWeapon);
}


void ASGCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector() * Amount);						// Add movement input for the character based on the value of the axis in the Actor forward direction. (Forward and back movement with +1 to -1)
}

void ASGCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector() * Amount);						// Add movement input for the character based on the value of the axis in the Actor Right direction. (Left and Right movement with +1 to -1)
}

void ASGCharacter::BeginCrouch()
{
	Crouch();																// Built in UE4 Function. This makes the capsule tiny in size, which also moves the camera down, since the main point is lower.
}

void ASGCharacter::EndCrouch()
{
	UnCrouch();																// Built in UE4 Function - this resets the capsule size.
}

void ASGCharacter::BeginZoom()
{
	/// Aim Down Sights
	bWantsToZoom = true;
}

void ASGCharacter::EndZoom()
{
	/// Return to normal FOV
	bWantsToZoom = false;
}

void ASGCharacter::StartFireWeapon()
{
	/// Fire your weapon
	if (!CurrentWeapon) { return; }											// Pointer protection
	CurrentWeapon->StartFire();
}

void ASGCharacter::StopFireWeapon()
{
	/// Fire your weapon
	if (!CurrentWeapon) { return; }											// Pointer protection
	CurrentWeapon->StopFire();
}

FVector ASGCharacter::GetPawnViewLocation() const
{
	if (!CameraComponent) { return Super::GetPawnViewLocation(); }			// Pointer Protection (Return the original function code from the super (inherited) class).
	
	return CameraComponent->GetComponentLocation();
}

void ASGCharacter::HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bHasDied) // Note: No need to set the ROLE_Authority check here, since this is already happening on the server only due to the health component hook that's only set to run on server.
	{
		// Die!
		bHasDied = true;
		
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
		DetachFromControllerPendingDestroy();											// Remove Controller
		SetLifeSpan(10.0f);																// Destroy this actor in seconds.

	}
}

// //[NETWORKING] Networking Replication Rules
void ASGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGCharacter, CurrentWeapon);			
	DOREPLIFETIME(ASGCharacter, bHasDied);
}