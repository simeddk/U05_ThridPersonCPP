#include "CDissolveComponent.h"
#include "Global.h"
#include "Components/CStateComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UCDissolveComponent::UCDissolveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CHelpers::GetAsset<UMaterialInstanceConstant>(&Material, "/Game/Materials/Dissolve/M_DissolveEffect_Inst");
	CHelpers::GetAsset<UCurveFloat>(&Curve, "/Game/Enemies/Curve_Dissolve");
}


void UCDissolveComponent::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);

	FOnTimelineFloat startTimeline;
	startTimeline.BindUFunction(this, "OnStartTimeline");
	Timeline.AddInterpFloat(Curve, startTimeline);
	Timeline.SetPlayRate(PlayRate);
}


void UCDissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Timeline.IsPlaying())
		Timeline.TickTimeline(DeltaTime);
}

void UCDissolveComponent::Play()
{
	Timeline.PlayFromStart();
}

void UCDissolveComponent::Stop()
{
	Timeline.Stop();
}

void UCDissolveComponent::OnStartTimeline(float Output)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	CheckNull(character);
	
	UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(GetOwner());
	CheckNull(state);
	CheckFalse(state->IsDeadMode());

	CheckNull(character->GetMesh());
	CheckNull(DynamicMaterial);

	character->GetMesh()->SetMaterial(0, DynamicMaterial);
	character->GetMesh()->SetMaterial(1, DynamicMaterial);

	DynamicMaterial->SetScalarParameterValue("amount", Output);
}

