#include "CEnemy.h"
#include "Global.h"
#include "Actions/CActionData.h"
#include "Actions/CThrow.h"
#include "Components/CStatusComponent.h"
#include "Components/CMontagesComponent.h"
#include "Components/CActionComponent.h"
#include "Components/CDissolveComponent.h"
#include "Widgets/CUserWidget_Name.h"
#include "Widgets/CUserWidget_Health.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

ACEnemy::ACEnemy()
{
	//Create SceneComponent
	CHelpers::CreateSceneComponent(this, &NameWidget, "NameWidget", GetMesh());
	CHelpers::CreateSceneComponent(this, &HealthWidget, "HealthWidget", GetMesh());

	//Create ActorComponent
	CHelpers::CreateActorComponent(this, &Action, "Action");
	CHelpers::CreateActorComponent(this, &Montages, "Montages");
	CHelpers::CreateActorComponent(this, &Status, "Status");
	CHelpers::CreateActorComponent(this, &State, "State");
	CHelpers::CreateActorComponent(this, &Dissolve, "Dissolve");

	//Component Settings
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	USkeletalMesh* meshAsset;
	CHelpers::GetAsset<USkeletalMesh>(&meshAsset, "SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
	GetMesh()->SetSkeletalMesh(meshAsset);

	TSubclassOf<UAnimInstance> animInstanceClass;
	CHelpers::GetClass<UAnimInstance>(&animInstanceClass, "AnimBlueprint'/Game/Enemies/ABP_CEnemy.ABP_CEnemy_C'");
	GetMesh()->SetAnimInstanceClass(animInstanceClass);

	TSubclassOf<UCUserWidget_Name> nameWidgetClass;
	CHelpers::GetClass<UCUserWidget_Name>(&nameWidgetClass, "WidgetBlueprint'/Game/Widgets/WB_Name.WB_Name_C'");
	NameWidget->SetWidgetClass(nameWidgetClass);
	NameWidget->SetRelativeLocation(FVector(0, 0, 240));
	NameWidget->SetDrawSize(FVector2D(240, 30));
	NameWidget->SetWidgetSpace(EWidgetSpace::Screen);

	TSubclassOf<UCUserWidget_Health> healthWidgetClass;
	CHelpers::GetClass<UCUserWidget_Health>(&healthWidgetClass, "WidgetBlueprint'/Game/Widgets/WB_Health.WB_Health_C'");
	HealthWidget->SetWidgetClass(healthWidgetClass);
	HealthWidget->SetRelativeLocation(FVector(0, 0, 190));
	HealthWidget->SetDrawSize(FVector2D(120, 20));
	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);

	//Movment Settings
	GetCharacterMovement()->MaxWalkSpeed = Status->GetRunSpeed();
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);
}


void ACEnemy::BeginPlay()
{
	//Create Dynmaic Material
	UMaterialInstanceConstant* bodyMaterial;
	UMaterialInstanceConstant* logoMaterial;

	CHelpers::GetAssetDynamic(&bodyMaterial, "MaterialInstanceConstant'/Game/Character/Materials/M_UE4Man_Body_Inst.M_UE4Man_Body_Inst'");
	CHelpers::GetAssetDynamic(&logoMaterial, "MaterialInstanceConstant'/Game/Character/Materials/M_UE4Man_ChestLogo_Inst.M_UE4Man_ChestLogo_Inst'");

	BodyMaterial = UMaterialInstanceDynamic::Create(bodyMaterial, nullptr);
	LogoMaterial = UMaterialInstanceDynamic::Create(logoMaterial, nullptr);

	GetMesh()->SetMaterial(0, BodyMaterial);
	GetMesh()->SetMaterial(1, LogoMaterial);

	//StateType Changed Event
	State->OnStateTypeChanged.AddDynamic(this, &ACEnemy::OnStateTypeChanged);

	Super::BeginPlay();

	//Widget Property Settings
	NameWidget->InitWidget();

	UCUserWidget_Name* nameWidgetObject = Cast<UCUserWidget_Name>(NameWidget->GetUserWidgetObject());
	if (!!nameWidgetObject)
	{
		nameWidgetObject->SetPawnName(GetName());
		nameWidgetObject->SetControllerName(GetController()->GetName());
		nameWidgetObject->SetVisibility(visibleType);
	}

	HealthWidget->InitWidget();
	UCUserWidget_Health* healthWidgetObject = Cast<UCUserWidget_Health>(HealthWidget->GetUserWidgetObject());
	if (!!healthWidgetObject)
		healthWidgetObject->Update(Status->GetHealth(), Status->GetMaxHealth());

	//Action->SetUnaremdMode();

}

void ACEnemy::ChangeColor(FLinearColor InColor)
{
	if (State->IsHittedMode())
	{
		LogoMaterial->SetVectorParameterValue("LogoLightColor", InColor);
		LogoMaterial->SetScalarParameterValue("IsHitted", 1);

		return;
	}

	BodyMaterial->SetVectorParameterValue("BodyColor", InColor);
	LogoMaterial->SetVectorParameterValue("BodyColor", InColor);

}


void ACEnemy::RestoreLogoColor()
{
	CheckTrue(State->IsDeadMode());

	FLinearColor color = Action->GetCurrent()->GetEquipmentColor();

	LogoMaterial->SetVectorParameterValue("LogoLightColor", color);
	LogoMaterial->SetScalarParameterValue("IsHitted", 0);
}

void ACEnemy::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
	switch (InNewType)
	{
		case EStateType::Hitted:	Hitted();	break;
		case EStateType::Dead:		Dead();		break;
	}
}

float ACEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageValue = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	Causer = DamageCauser;
	Attacker = Cast<ACharacter>(EventInstigator->GetPawn());

	Action->AbortByDamaged();

	Status->DecreaseHealth(DamageValue);

	if (Status->GetHealth() <= 0.f)
	{
		State->SetDeadMode();
		return DamageValue;
	}

	State->SetHittedMode();

	return DamageValue;
}

void ACEnemy::Hitted()
{
	//Decrease Health Widget
	UCUserWidget_Health* healthWidgetObject = Cast<UCUserWidget_Health>(HealthWidget->GetUserWidgetObject());
	if (!!healthWidgetObject)
		healthWidgetObject->Update(Status->GetHealth(), Status->GetMaxHealth());

	//Play Hit Montage
	Montages->PlayHitted();
	Status->SetMove();

	//Lauch HitBack
	FVector start = GetActorLocation();
	FVector target = Attacker->GetActorLocation();
	FVector direction = start - target;
	direction.Normalize();
	
	LaunchCharacter(direction * LaunchValue * DamageValue, true, false);

	//Change LogoColor
	ChangeColor(FLinearColor::Red * 35.f);
	UKismetSystemLibrary::K2_SetTimer(this, "RestoreLogoColor", 1.f, false);

}

void ACEnemy::Dead()
{
	CheckFalse(State->IsDeadMode());

	//Widget Visible false
	NameWidget->SetVisibility(false);
	HealthWidget->SetVisibility(false);

	//All Weapon Collision Disable
	Action->Dead();

	//Ragdoll
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->GlobalAnimRateScale = 0.f;
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//Play Dissolve
	Dissolve->Play();

	FVector start = GetActorLocation();
	FVector target = Causer->GetActorLocation();
	FVector direction = start - target;
	direction.Normalize();

	if (Causer->IsA<ACThrow>())
		DeadLaunchValue *= 0.075f;

	GetMesh()->AddForce(direction * DamageValue * DeadLaunchValue);

	//End_Dead
	UKismetSystemLibrary::K2_SetTimer(this, "End_Dead", 5.f, false);
}

void ACEnemy::End_Dead()
{
	//Stop Dissolve
	Dissolve->Stop();

	Action->End_Dead();

	Destroy();
}
