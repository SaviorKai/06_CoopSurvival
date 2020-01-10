// Fill out your copyright notice in the Description page of Project Settings.


#include "SGPlayerState.h"

void ASGPlayerState::AddScore(float ScoreDelta)
{
	// Score is a built in var as part of the base C++ class of PlayerState. Note, this is also replicated.
	Score += ScoreDelta;
}