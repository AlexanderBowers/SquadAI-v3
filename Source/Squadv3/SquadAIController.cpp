// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadAIController.h"
#include "SquadPlayerController.h"
#include "Engine/EngineTypes.h"
#include "SquadInterface.h"
#include "SquadPlayerController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

void ASquadAIController::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld()->GetFirstPlayerController())
	{
		PlayerController = GetWorld()->GetFirstPlayerController<ASquadPlayerController>();

		//Giving them their own fake command to have something to compare to the first command.
		FCommandPoint BaseCommand;
		BaseCommand.Location = PlayerController->GetPawn()->GetActorLocation();
		BaseCommand.Type = FName("Move");
		LastCommand = BaseCommand;

	}
	TheBlackboard = GetBlackboardComponent();
}

void ASquadAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PlayerController)
	{
		if (TheBlackboard)
		{
			if (TheBlackboard->GetValueAsBool(FName("bShouldFollow")))
			{
				StartFollow();

			}

			if (FVector::Distance(GetCharacter()->GetActorLocation(), PlayerController->GetPawn()->GetActorLocation()) >= 2000.0f)
			{
				//ResetPriorityCommand();

			}
		}
	}
}

void ASquadAIController::StartFollow()
{
}

void ASquadAIController::StopFollow()
{
}

void ASquadAIController::MoveToCommand(FCommandPoint CommandPoint)
{
	if (TheBlackboard)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Destination: %s"), *CommandPoint.Location.ToString());

		if (CommandPoint.Type == FName("Target"))
		{
			if (GetPawn()->Implements<USquadInterface>())
			{
				if (CommandPoint.OwnerActor != nullptr)
				{
					ISquadInterface::Execute_SetNewTarget(GetPawn(), CommandPoint.OwnerActor);
					return;
				}
			}
		}
		if (CommandPoint.Type == FName("Return"))
		{
			StartFollow();
			TheBlackboard->SetValueAsBool(FName("bShouldFollow"), true);
		}
		if (CommandPoint.Location.X == 0.00f)
		{
			return;
		}
			if (GetCharacter()->bIsCrouched)
			{
				GetCharacter()->UnCrouch();

			}
		if (this->Implements<USquadInterface>())
		{
			ISquadInterface::Execute_StopFollow(this);
		}
		MoveToLocation(CommandPoint.Location, 25);
		HandleCommand(CommandPoint);
	}
}

void ASquadAIController::HandleCommand(FCommandPoint CommandPoint)
{
	if (TheBlackboard)
	{
		//Called when AIController reached their destination
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "HandleCommand", CommandPoint);
		float DistanceThreshold = 100.0f;
		float DistanceToCommand = FVector::Distance(GetPawn()->GetActorLocation(), CommandPoint.Location);
		TheBlackboard->SetValueAsBool(FName("bIsMovingToCommand"), true);
		if (DistanceToCommand <= DistanceThreshold)
		{
			UE_LOG(LogTemp, Warning, TEXT("Distance threshold met."));
			if (CommandPoint.Type == FName("Cover")) // trying to convert this to switch statement
			{
				GetCharacter()->Crouch();

			}
			Delegate.Unbind();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 1.0f, false, 0.0f);

		}
	}
}

void ASquadAIController::StopCommandAndFollow()
{
}

void ASquadAIController::StopCommandDontFollow()
{
}

void ASquadAIController::ClearRoom(FVector RoomLocation)
{
}
