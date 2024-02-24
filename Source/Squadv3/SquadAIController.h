// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SquadPlayerController.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SquadAIController.generated.h"

/**
 * 
 */
UCLASS()
class SQUADV3_API ASquadAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	//Command references
	FCommandPoint LastCommand;
	TArray<FCommandPoint> LocalCommandList; //In case I need to store multiple commands
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* AssignedPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* AssignedFollow;

	//Commands
	UFUNCTION(BlueprintCallable)
	void StartFollow();
	UFUNCTION(BlueprintCallable)
	void StopFollow();
	UFUNCTION(BlueprintCallable)
	void MoveToCommand(FCommandPoint CommandPoint);
	UFUNCTION()
	void HandleCommand();
	UFUNCTION(BlueprintCallable)
	void StopCommandAndFollow();
	UFUNCTION(BlueprintCallable)
	void StopCommandDontFollow();
	UFUNCTION()
	void ClearRoom(FVector RoomLocation);

	UPROPERTY(VisibleAnywhere)
	UBlackboardComponent* TheBlackboard;
	UPROPERTY()
	ASquadPlayerController* PlayerController;
	UPROPERTY()
	FTimerHandle TimerHandle;


	
};
