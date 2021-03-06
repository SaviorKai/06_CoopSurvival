// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SGCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASGWeapon;
class USGHealthComponent;

UCLASS()
class COOPSURVIVAL_API ASGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASGCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Override the UE4 GetPawnViewLocation() function, which is called by the GetActorEyesViewPoint() on Weapon.
	virtual FVector GetPawnViewLocation() const override;

	/// Moved to Public, because we want to access these in our behavoir tree.
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void StartFireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void StopFireWeapon();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// Movement Functions.
	void MoveForward(float Amount);
	void MoveRight(float Amount);
	
	/// Action Functions
	void BeginCrouch();
	void EndCrouch();
	void BeginZoom();
	void EndZoom();

	//  Movement Properties
	//TODO: BUG: Build own Movement componenent and replicate speed properly for clients, since Speedboost is BUSTED on clients. Fix the silly Blueprint code suggested by lecure.

	/// Weapon Properties
	UPROPERTY(Replicated) //[NETWORKING]
	ASGWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Setup")
	FName WeaponAttachSocketName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<ASGWeapon> StarterWeaponClass; // Set in Blueprint
	
	/// Health Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup")
	USGHealthComponent* MyHealthComponent;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Setup")
	bool bHasDied;																// Used by the animation blueprint graph, and also to ensure we don't play effects twice. 

	UFUNCTION() // REQUIRED FOR EVENT DELEGATES
	void HandleOnHealthChanged(USGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/// Camera Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Setup")								
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup")
	USpringArmComponent* SpringArm;

	/// Aim Down Sight FOV Properties.
	bool bWantsToZoom;
	
	float DefaultFOV; // Set in constructor

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ZoomedFOV = 65.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ClampMin = 1.0f, ClampMax = 100.0f))   /// meta = (ClampMin = 1.0f, ClampMax = 100.0f)), useful for clamping values in editor for designers.
	float ZoomInterpSpeed = 20.0f;

	//[NETWORKING] Network Replication Rules Function (Not required, but Added here for code readability.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
};




////// RULES FOR SETTING UP KEYBINDINGS, AND BINDING TO FUNCTIONS //////
/*
This guide shows you how to setup Key bindings on your CHARACTER Actor, and bind them to functions in C++. 

1. Create key binding functions:
*In Header File:
	void MoveForward(float Amount);
	void MoveRight(float Amount);

*In CPP File: 
	void ASGCharacter::MoveForward(float Amount)
	{
		AddMovementInput(GetActorForwardVector() * Amount);		// Add movement input for the character based on the value of the axis in the Actor forward direction. (Forward and back movement with +1 to -1)
	}

	void ASGCharacter::MoveRight(float Amount)
	{
		AddMovementInput(GetActorRightVector() * Amount);		// Add movement input for the character based on the value of the axis in the Actor Right direction. (Left and Right movement with +1 to -1)
	}

2. Setup the Mappings and BIND them to the correct functions
*In the CPP File, on the SetupPlayerInputComponent() function, Add:

	PlayerInputComponent->BindAxis("Name Here", this, &ASGCharacter::FunctionNameHere);

- Note: Don't use "()" when referencing a function name.
- Note: The Name here, is the exact name you will need to use in UE4's project settings.

*Example code:

void ASGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/// Setup the movement bindings and the functions to call (remember, don't use () when referencing '&' a function)
	PlayerInputComponent->BindAxis("Move Forward", this, &ASGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right", this, &ASGCharacter::MoveRight);
}

3. In the UE4 Editor, 
- Go to Project settings
- Add the Mapping Type (Axis or Action, depending on what you used)
- Give it the exact same name as in the C++ code
- Then, bind it to keys:
- If Axis Mappings: Remember that they have positive (+1) and negative(-1) values. 
- For Keyboard: Bind two keys for Forward, and two keys for Right. Example:  W(+1), S(-1), D(+1), A(-1) 
- For Gamepad: Just use the stick Y-Axis for forward (as it has + and -), and X-Axis for right.



4. Extra:
You can add Look bindings pretty easily as well, by using built in functions. All you have to do is bind the Axis in SetupPlayerInputComponent():

	PlayerInputComponent->BindAxis("Look Up", this, &ASGCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Right", this, &ASGCharacter::AddControllerYawInput);

And next, set the up in the UE4 Editor, properties window.

	Same names as used in C++, use Mouse X / Mouse Y, or the controller stick X / Y.
*/



