#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

class GAME_API CHelpers
{
public:
	template<typename T>
	static void GetAsset(T** OutAsset ,FString InPath)
	{
		ConstructorHelpers::FObjectFinder<T> asset(*InPath);
		verifyf(asset.Succeeded(), L"Asset Not Found");
		
		*OutAsset = asset.Object;
	}
	
	template<typename T>
	static void GetClass(TSubclassOf<T>* OutClass, FString InPath)
	{
		ConstructorHelpers::FClassFinder<T> pawnClass(*InPath);
		verifyf(pawnClass.Succeeded(), L"Class Not Found");

		*OutClass = pawnClass.Class;
	}


	template<typename T>
	static void CreateSceneComponent(AActor* InActor, T** OutComponent, FName InName, USceneComponent* InParent = nullptr)
	{
		*OutComponent = InActor->CreateDefaultSubobject<T>(InName);

		if (!!InParent)
		{
			(*OutComponent)->SetupAttachment(InParent);
			return;
		}

		InActor->SetRootComponent(*OutComponent);

	}
};

