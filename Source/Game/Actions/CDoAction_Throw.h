#pragma once

#include "CoreMinimal.h"
#include "Actions/CDoAction.h"
#include "CDoAction_Throw.generated.h"

UCLASS()
class GAME_API ACDoAction_Throw : public ACDoAction
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	
	virtual void DoAction() override;
	virtual void Begin_DoAction() override;
	virtual void End_DoAction() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnAim() override;
	virtual void OffAim() override;

private:
	UPROPERTY()
		class UCAim* Aim;
};
