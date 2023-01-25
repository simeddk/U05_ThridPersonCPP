#include "GameGameModeBase.h"
#include "Global.h"

AGameGameModeBase::AGameGameModeBase()
{
	CHelpers::GetClass<APawn>(&DefaultPawnClass, "Blueprint'/Game/Player/BP_CPlayer.BP_CPlayer_C'");

	//Todo : 애님인스턴스, 이동기능
}