// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SquadPlayerController.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCommandPoint
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OwnerActor;

};

UCLASS()
class SQUADV3_API ASquadPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//TArray<ASquadAIController*> SquadMembers;

public:

	//Command Point functions
	UFUNCTION()
	FCommandPoint CreateCommandPoint(FHitResult Hitresult);
	UFUNCTION()
	FCommandPoint AssignLocation(FCommandPoint CommandPoint, FHitResult HitResult);
	UFUNCTION()
	FCommandPoint AssignType(FCommandPoint CommandPoint, FHitResult Hitresult);
	
	//Squad Management
	UFUNCTION()
	void DeployInvestigate(FCommandPoint CommandPoint);
	UFUNCTION()
	void AssignPosition(FCommandPoint CommandPoint, ASquadAIController* SquadMember);
	UFUNCTION(BlueprintCallable)
	ASquadAIController* GetAvailableMember(FCommandPoint CommandPoint);
	UFUNCTION()
	void SetNewAITarget(AActor* NewTarget);
	UFUNCTION(BlueprintCallable)
	void MoveUpCommand();
	UFUNCTION(BlueprintCallable)
	void FormUpCommand();

	//Room Clearing
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetRooms(AActor* Building);
	//UFUNCTION()
	//void CheckRoomValues(ARoom* Room);

	
private:
	UPROPERTY(EditAnywhere)
	float MaxRange = 3000;

	FVector CameraLocation;
	FRotator CameraRotation;
	APawn* ControlledPawn;






	
};
