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