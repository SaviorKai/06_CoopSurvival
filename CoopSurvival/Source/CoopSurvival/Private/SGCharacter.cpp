// Fill out your copyright notice in the Description page of Project Settings.


#include "SGCharacter.h"
#include "Camera/CameraComponent.h" // UCameraComponent
#include "GameFramework/SpringArmComponent.h" // USpringArmComponent
#include "GameFramework/PawnMovementComponent.h" // GetMovementComponent()

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

	/// Create Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArm);



}

// Called when the game starts or when spawned
void ASGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ASGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump); // Note: Jump is already a function on ACharacter (so no need to use our class, or define this)

}

void ASGCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector() * Amount);																		// Add movement input for the character based on the value of the axis in the Actor forward direction. (Forward and back movement with +1 to -1)
}

void ASGCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector() * Amount);																		// Add movement input for the character based on the value of the axis in the Actor Right direction. (Left and Right movement with +1 to -1)
}

void ASGCharacter::BeginCrouch()
{
	Crouch();					//Built in UE4 Function. This makes the capsule tiny in size, which also moves the camera down, since the main point is lower.
}

void ASGCharacter::EndCrouch()
{
	UnCrouch();					//Built in UE4 Function - this resets the capsule size.
}
