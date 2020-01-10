// Fill out your copyright notice in the Description page of Project Settings.


#include "SGGameStateBase.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps


void ASGGameStateBase::OnRep_WaveState(EWaveState OldState)
{
	WavesStateChanged(WaveState, OldState);
}


// //[NETWORKING] Networking Replication Rules
void ASGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGGameStateBase, WaveState);
}

void ASGGameStateBase::SetWaveState(EWaveState NewState)
{
	/*	
	*	This ensures that the state is only changed on the server. 
	*	Once the state is changed, it will replicate to other clients, and trigger the "OnRep" funciton.
	*/
	if (Role == ROLE_Authority)
	{
		EWaveState OldState = WaveState;

		WaveState = NewState;

		// Manually run this on the server
		OnRep_WaveState(OldState);
	}
}