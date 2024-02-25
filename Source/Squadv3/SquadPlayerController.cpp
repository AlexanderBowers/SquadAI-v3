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
	Super::SetupInputComponent();

	InputComponent->BindAction("MoveUpCommand", IE_Pressed, this, &ASquadPlayerController::MoveUpCommand);
	InputComponent->BindAction("FormUpCommand", IE_Pressed, this, &ASquadPlayerController::FormUpCommand);
	//InputComponent->BindAction("FireProjectile", IE_Pressed, this, &ASquadPlayerController::FireProjectile);
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

void ASquadPlayerController::AssignPosition(FCommandPoint CommandPoint, ASquadAIController* SquadMember)
{
}

ASquadAIController* ASquadPlayerController::GetAvailableMember(FCommandPoint CommandPoint)
{
	return nullptr;
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