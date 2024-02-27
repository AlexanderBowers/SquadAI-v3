// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerController.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Components.h"
#include "Engine/Engine.h"
#include "CommandComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SquadInterface.h"
#include "SquadAIController.h"
#include <Kismet/GameplayStatics.h>
#include "UObject/Class.h"
#include "GameFramework/Character.h"

void ASquadPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (ASquadPlayerController::GetPawn())
	{
		ControlledPawn = GetPawn();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASquadAIController::StaticClass(), SquadMembers); // Get all Squad Member controllers.

	}
}

void ASquadPlayerController::Tick(float DeltaTime)
{
}

FCommandPoint ASquadPlayerController::CreateCommandPoint(FHitResult HitResult)
{
	//if we get a collision, create a FCommandPoint. 
	//If the collided actor has a Command Component, get its type and add to the CommandList.

	FCommandPoint CommandPoint;
	CommandPoint = AssignLocation(CommandPoint, HitResult);
	CommandPoint = AssignType(CommandPoint, HitResult);
	return CommandPoint;
}

FCommandPoint ASquadPlayerController::AssignLocation(FCommandPoint CommandPoint, FHitResult HitResult)
{
	//Starting location of a CommandPoint to be given to an AI. This may change depending on its type.
	CommandPoint.Location = HitResult.Location;
	return CommandPoint;
}

FCommandPoint ASquadPlayerController::AssignType(FCommandPoint CommandPoint, FHitResult HitResult)
{
	//CommandPoints can have a variety of types. 
  //Move: self explanatory. This is the default fallback.
  //Target: All squad members should set this to their target actor.
  //Detonate: One AI temporarily has this set as a priority. They will place a bomb then return. Selecting this again will blow up the object.
  //Cover: crouch at location; this is set in SquadAIController::HandleCommand
  //Investigate: CommandPoint.Location gets set to a specific component.
  //FirePoint: One AI gets set this as a priority to move to or recalled from. See SquadPlayerController::GetAvailableMembers() for assignment.
  //Return: used in SquadPlayerController::FormUpCommand. Otherwise used as a backup if there was no actor found from the HitResult.

	//Most of these seem redundant. Realistically we should only need Move, Target, Assign, Investigate

	AActor* Actor = HitResult.GetActor();
	if (Actor)
	{
		UActorComponent* Component = Actor->FindComponentByClass<UCommandComponent>();
		if (Component)
		{
			FString TagType = Component->ComponentTags[0].ToString(); //Always look for the first tag on an actor to determine the type of CommandPoint.
			if (TagType.Len() > 0)
			{
				CommandPoint.Type = FName(TagType);
				CommandPoint.OwnerActor = Actor;
				DrawDebugSphere(GetWorld(), HitResult.Location, 20, 8, FColor::Green, false, 2, 0, 1.f);

				if (CommandPoint.Type == FName("Detonate"))
				{
					if (Actor->Implements<USquadInterface>())
					{
						UStaticMeshComponent* BombPoint = Cast<UStaticMeshComponent>(Actor->GetDefaultSubobjectByName(TEXT("BombLocation")));
						if (BombPoint)
						{
							ISquadInterface::Execute_CheckAssignedMember(Actor, CommandPoint);
							CommandPoint.Location.X = 0.00f; //prevents squad members who didn't receive the assignment from moving to it.
						}
					}
				}
				if (CommandPoint.Type == FName("Target"))
				{
					//SetNewAITarget(Actor);
					CommandPoint.Location.X = 0.00f;
				}
				if (CommandPoint.Type == FName("Investigate")) //Grab a static mesh called EndLocation on the actor. That will be the new location to move to.
				{
					UStaticMeshComponent* EndLocation = Cast<UStaticMeshComponent>(Actor->GetDefaultSubobjectByName(TEXT("EndLocation")));
					if (EndLocation)
					{
						FVector RightLocation = EndLocation->GetComponentLocation();
						CommandPoint.Location = RightLocation;
						return CommandPoint;
					}
				}
				if (CommandPoint.Type == FName("FirePoint"))
				{
					if (Actor->Implements<USquadInterface>())
					{	//We're checking to see if this actor has an assigned member. If not, assign one through SquadPlayerController::GetAvailableMember. If it already does, recall them.
						ISquadInterface::Execute_CheckAssignedMember(Actor, CommandPoint);
						CommandPoint.Location.X = 0.00f; //This is to restrict other AI other than the one specified in GetAvailableMember from moving to this location.
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Found Component but not tag!"))
					CommandPoint.Type = FName("Move");
			}
		}
		else //If there is no component, default the type to move.
		{
			CommandPoint.Type = FName("Move");
			DrawDebugSphere(GetWorld(), HitResult.Location, 20, 8, FColor::Red, false, 2, 0, 1.f);
		}
		return CommandPoint;

	}
	//If there is no actor hit, return to the player
	UE_LOG(LogTemp, Warning, TEXT("No actor found. Returning to player."))
		CommandPoint.Location = this->GetPawn()->GetActorLocation();
	CommandPoint.Type = FName("Return");
	return CommandPoint;

}

void ASquadPlayerController::DeployInvestigate(FCommandPoint CommandPoint)
{
}

void ASquadPlayerController::AssignPosition(FCommandPoint CommandPoint, ASquadAIController* SquadMember)
{
}

ASquadAIController* ASquadPlayerController::GetAvailableMember(FCommandPoint CommandPoint)
{
	ASquadAIController* AvailableMember = nullptr;

	if (CommandPoint.OwnerActor->Implements<USquadInterface>())
	{
		for (AActor* AI : SquadMembers)
		{
			ASquadAIController* Commando = Cast<ASquadAIController>(AI);
		
			if (Commando->GetBlackboardComponent()->GetValueAsObject(FName("AssignedPosition")) == nullptr)
			{
				Commando->GetBlackboardComponent()->SetValueAsObject(FName("AssignedPosition"), CommandPoint.OwnerActor);
				ISquadInterface::Execute_SetAssignedMember(CommandPoint.OwnerActor, Commando);
				AvailableMember = Commando;
				return AvailableMember;
			}
		
		}
	}
	if (AvailableMember == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Available Member found in GetAvailableMember. returning nullptr"));
	}
	return nullptr;
}

void ASquadPlayerController::SetNewAITarget(AActor* NewTarget)
{
}

void ASquadPlayerController::MoveUpCommand()
{
	//Use a line trace to find a location for AI to move to.

	if (ControlledPawn)
	{
		GetPlayerViewPoint(CameraLocation, CameraRotation);
		FVector End = CameraLocation + CameraRotation.Vector() * MaxRange;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(ControlledPawn);

		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECC_Visibility, CollisionParams);
		if (bHit)
		{
			//CreateCommandPointy checks to see if the hitresult actor has a Command Component and its type.
			FCommandPoint CommandPoint = CreateCommandPoint(HitResult);
			for (AActor* AI : SquadMembers)
			{
				ASquadAIController* Commando = Cast<ASquadAIController>(AI);
				if (Commando)
				{
					UBlackboardComponent* Blackboard = Commando->GetBlackboardComponent();
					if (Blackboard->GetValueAsObject(FName("AssignedPosition")) == nullptr)
					{
						Commando->StopFollow();
						Commando->MoveToCommand(CommandPoint);
					}
				}
			}
		}
	}
}

void ASquadPlayerController::FormUpCommand()
{
	for (AActor* AI : SquadMembers)
	{
		ASquadAIController* Commando = Cast<ASquadAIController>(AI);
		if (Commando)
		{
			UBlackboardComponent* Blackboard = Commando->GetBlackboardComponent();
			if (Blackboard->GetValueAsObject(FName("AssignedPosition")) == nullptr)
			{
				Blackboard->SetValueAsBool(FName("bShouldFollow"), true);
			}
		}
	}
}

TArray<AActor*> ASquadPlayerController::GetRooms(AActor* Building)
{
	return TArray<AActor*>();
}