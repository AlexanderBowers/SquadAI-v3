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
			if (TheBlackboard->GetValueAsBool(FName("bShouldFollow")) == true)
			{
				StartFollow();

			}

			if (FVector::Distance(GetCharacter()->GetActorLocation(), PlayerController->GetPawn()->GetActorLocation()) >= 2000.0f)
			{
				StopCommandAndFollow();

			}
		}
	}
}

void ASquadAIController::StartFollow() //Return to the player. Each squad member has their own assigned follow position.
{
	AActor* FollowDestination = Cast<AActor>(TheBlackboard->GetValueAsObject(FName("AssignedFollow")));
	if (FollowDestination)
	{
		MoveToLocation(FollowDestination->GetActorLocation(), 25);
	}
}

void ASquadAIController::StopFollow()
{
	TheBlackboard->SetValueAsBool(FName("bShouldFollow"), false);
}

void ASquadAIController::MoveToCommand(FCommandPoint CommandPoint) //Move to new location. if it's a new target, target them instead of moving.
{
	if (TheBlackboard)
	{
		if (CommandPoint.Type == FName("Target"))
		{
			if (GetPawn()->Implements<USquadInterface>())
			{
				if (CommandPoint.OwnerActor != nullptr)
				{
					TheBlackboard->SetValueAsObject(FName("TargetActor"), CommandPoint.OwnerActor);
					return;
				}
			}
		}

		if (CommandPoint.Location.X == 0.00f) //Likely because it's an invalid location or it requires an assigned member.
		{
			return;
		}
		
		if (GetCharacter()->bIsCrouched)
		{
			GetCharacter()->UnCrouch();

		}
		StopFollow();
		MoveToLocation(CommandPoint.Location, 25);
		HandleCommand(CommandPoint);
	}
}

void ASquadAIController::HandleCommand(FCommandPoint CommandPoint) //For special cases where there is more than just moving from A->B
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

void ASquadAIController::StopCommandAndFollow() //Drop assigned position and return to the player
{
	AActor* Assignment = Cast<AActor>(TheBlackboard->GetValueAsObject(FName("AssignedPosition")));
	if (Assignment)
	{
		if (Assignment->Implements<USquadInterface>())
		{
			ISquadInterface::Execute_ResetAssignedMember(Assignment);
		}
	}
	TheBlackboard->SetValueAsBool(FName("bShouldFollow"), true);
}

void ASquadAIController::StopCommandDontFollow()//Drop assigned position. Stay where they are.
{
	AActor* Assignment = Cast<AActor>(TheBlackboard->GetValueAsObject(FName("AssignedPosition")));
	if (Assignment)
	{
		if (Assignment->Implements<USquadInterface>())
		{
			ISquadInterface::Execute_ResetAssignedMember(Assignment);
		}
	}
}

void ASquadAIController::ClearRoom(FVector RoomLocation)
{
}
