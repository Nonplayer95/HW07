#include "MyGameMode.h"
#include "MyPawn.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AMyPawn::StaticClass();
}

