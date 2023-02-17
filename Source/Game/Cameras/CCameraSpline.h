#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCameraSpline.generated.h"

UCLASS()
class GAME_API ACCameraSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	ACCameraSpline();

	FORCEINLINE class USplineComponent* GetSpline() { return Spline; }
	FORCEINLINE class UCurveFloat* GetCurve() { return Curve; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USplineComponent* Spline;

	UPROPERTY(EditDefaultsOnly)
		class UCurveFloat* Curve;
};
