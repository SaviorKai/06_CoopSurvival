// Fill out your copyright notice in the Description page of Project Settings.


#include "SGWeapon.h"

// Sets default values
ASGWeapon::ASGWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/// Create Mesh Component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));


}

// Called when the game starts or when spawned
void ASGWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASGWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

