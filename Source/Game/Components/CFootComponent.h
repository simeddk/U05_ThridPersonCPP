#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CFootComponent.generated.h"

USTRUCT(BlueprintType)
struct FFootData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector LeftDistance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector RightDistance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector PelvisDistance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FRotator LeftRotation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FRotator RightRotation;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UCFootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCFootComponent();

	FORCEINLINE const FFootData& GetData() { return Data; }
	FORCEINLINE bool IsIK() { return bActive; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void Trace(FName InSocketName, float& OutDistance, FRotator& OutRotatoin);

	UFUNCTION()
		void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		FName LeftFootSocket = "Foot_L";

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		FName RightFootSocket = "Foot_R";

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float AdditionalDistance = 55.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForOneFrame;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float OffesetDistance = 5.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float InterpSpeed = 20.f;

private:
	class ACharacter* OwnerCharacter;
	float CapsuleHalfHeight;

	FFootData Data;

	bool bActive;
};
