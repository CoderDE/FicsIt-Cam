#include "FicsItCamModule.h"

#include "FGGameMode.h"
#include "FICSubsystemHolder.h"
#include "mod/hooking.h"

void FFicsItCamModule::StartupModule() {
	FSubsystemInfoHolder::RegisterSubsystemHolder(UFICSubsystemHolder::StaticClass());
	/*
	SUBSCRIBE_VIRTUAL_FUNCTION_AFTER(AGameModeBase, AActor::OnConstruction, [](AActor* self, const FTransform&) {
		AGameModeBase* GameMode = Cast<AGameModeBase>(self);
		SML::Logging::error(TCHAR_TO_UTF8(*GameMode->GetName()));
		GameMode->SpectatorClass = AFICEditorCameraCharacter::StaticClass();
		GameMode->ReplaySpectatorPlayerControllerClass = AFICReplayController::StaticClass();
	});

	SUBSCRIBE_METHOD(UWorld::BeginPlay, [](CallScope<void(*)(UWorld*)>& Scope, UWorld* GameMode) {
		if (GameMode->GetAuthGameMode())
			SML::Logging::error(TCHAR_TO_UTF8(*GameMode->GetAuthGameMode()->GetName()));
        SML::Logging::error("Hey");
        if (GameMode->GetFirstPlayerController()) SML::Logging::error("Controller: ", TCHAR_TO_UTF8(*GameMode->GetFirstPlayerController()->GetName()));
        if (GameMode->GetFirstPlayerController()->GetPawn()) SML::Logging::error("Pawn: ", TCHAR_TO_UTF8(*GameMode->GetFirstPlayerController()->GetPawn()->GetName()));
        if (GameMode->GetFirstPlayerController()->GetCharacter()) SML::Logging::error("Char: ", TCHAR_TO_UTF8(*GameMode->GetFirstPlayerController()->GetCharacter()->GetName()));
        if (GameMode->GetFirstPlayerController()->GetSpectatorPawn()) SML::Logging::error("Char: ", TCHAR_TO_UTF8(*GameMode->GetFirstPlayerController()->GetSpectatorPawn()->GetName()));
		//GameMode->GetGameState()->SpectatorClass = AFICEditorCameraCharacter::StaticClass();
	});*/
}

IMPLEMENT_GAME_MODULE(FFicsItCamModule, FicsItCam);
