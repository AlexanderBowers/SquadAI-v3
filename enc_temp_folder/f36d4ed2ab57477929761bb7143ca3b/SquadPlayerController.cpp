// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerController.h"

void ASquadPlayerController::BeginPlay()
{
}

void ASquadPlayerController::Tick(float DeltaTime)
{
}

void ASquadPlayerController::SetupInputComponent()
{
}

FCommandPoint ASquadPlayerController::CreateCommandPoint(FHitResult Hitresult)
{
	return FCommandPoint();
}

FCommandPoint ASquadPlayerController::AssignLocation(FCommandPoint CommandPoint, FHitResult HitResult)
{
	return FCommandPoint();
}

FCommandPoint ASquadPlayerController::AssignType(FCommandPoint CommandPoint, FHitResult Hitresult)
{
	return FCommandPoint();
}

void ASquadPlayerController::DeployInvestigate(FCommandPoint CommandPoint)
{
}

void ASquadPlayerController::SetNewAITarget(AActor* NewTarget)
{
}

void ASquadPlayerController::MoveUpCommand()
{
}

void ASquadPlayerController::FormUpCommand()
{
}

TArray<AActor*> ASquadPlayerController::GetRooms(AActor* Building)
{
	return TArray<AActor*>();
}