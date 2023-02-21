#include "CFootComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/TriggerVolume.h"

UCFootComponent::UCFootComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCFootComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATriggerVolume::StaticClass(), actors);
	CheckTrue(actors.Num() < 1);

	for (AActor* actor : actors)
	{
		actor->OnActorBeginOverlap.AddDynamic(this, &UCFootComponent::OnActorBeginOverlap);
		actor->OnActorEndOverlap.AddDynamic(this, &UCFootComponent::OnActorEndOverlap);
	}
}


void UCFootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float leftDistance;
	FRotator leftRotatoin;
	Trace(LeftFootSocket, leftDistance, leftRotatoin);

	float rightDistance;
	FRotator rightRotatoin;
	Trace(RightFootSocket, rightDistance, rightRotatoin);

	float offset = FMath::Min(leftDistance, rightDistance);

	Data.PelvisDistance.Z = UKismetMathLibrary::FInterpTo(Data.PelvisDistance.Z, offset, DeltaTime, InterpSpeed);

	Data.LeftDistance.X = UKismetMathLibrary::FInterpTo(Data.LeftDistance.X, (leftDistance - offset), DeltaTime, InterpSpeed);
	Data.RightDistance.X = UKismetMathLibrary::FInterpTo(Data.RightDistance.X, -(rightDistance - offset), DeltaTime, InterpSpeed);

	Data.LeftRotation = UKismetMathLibrary::RInterpTo(Data.LeftRotation, leftRotatoin, DeltaTime, InterpSpeed);
	Data.RightRotation = UKismetMathLibrary::RInterpTo(Data.RightRotation, rightRotatoin, DeltaTime, InterpSpeed);
}

void UCFootComponent::Trace(FName InSocketName, float& OutDistance, FRotator& OutRotatoin)
{
	OutDistance = 0.f;
	OutRotatoin = FRotator::ZeroRotator;

	FVector socketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(InSocketName);
	FVector start = FVector(socketLocation.X, socketLocation.Y, OwnerCharacter->GetActorLocation().Z);

	float traceZ = start.Z - CapsuleHalfHeight - AdditionalDistance;
	FVector end = FVector(socketLocation.X, socketLocation.Y, traceZ);

	TArray<AActor*> ignores;
	ignores.Add(OwnerCharacter);

	FHitResult hitResult;
	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		start,
		end,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		ignores,
		DrawDebugType,
		hitResult,
		true,
		FLinearColor::Green,
		FLinearColor::Red
	);

	CheckFalse(hitResult.IsValidBlockingHit());

	float underGround = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	OutDistance = OffesetDistance + underGround - AdditionalDistance;

	UKismetSystemLibrary::DrawDebugArrow
	(
		GetWorld(),
		hitResult.ImpactPoint,
		hitResult.ImpactPoint + hitResult.ImpactNormal * 100.f,
		3,
		FLinearColor::Blue
	);


	FVector normal = hitResult.ImpactNormal;
	float roll = UKismetMathLibrary::DegAtan2(normal.Y, normal.Z);
	float pitch = -UKismetMathLibrary::DegAtan2(normal.X, normal.Z);

	roll = FMath::Clamp(roll, -15.f, 15.f);
	pitch = FMath::Clamp(pitch, -30.f, 30.f);

	OutRotatoin = FRotator(pitch, 0, roll);
}

void UCFootComponent::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	CheckNull(Cast<ACharacter>(OtherActor));
	bActive = true;
}

void UCFootComponent::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	CheckNull(Cast<ACharacter>(OtherActor));
	bActive = false;
}

