// Fill out your copyright notice in the Description page of Project Settings.


#include "SGCharacter.h"
#include "..\Public\SGCharacter.h"

// Sets default values
ASGCharacter::ASGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	PlayerInputComponent->BindAxis("Turn", this, &ASGCharacter::AddControllerYawInput);								// AddControllerYawInput is built in function.
}

void ASGCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector() * Amount);																		// Add movement input for the character based on the value of the axis in the Actor forward direction. (Forward and back movement with +1 to -1)
}

void ASGCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector() * Amount);																		// Add movement input for the character based on the value of the axis in the Actor Right direction. (Left and Right movement with +1 to -1)
}
