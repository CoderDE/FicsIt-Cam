#include "FICSubsystem.h"


#include "FICCommand.h"
#include "FICSubsystemHolder.h"
#include "Engine/DemoNetDriver.h"
#include "mod/ModSubsystems.h"

AFICSubsystem::AFICSubsystem() {
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void AFICSubsystem::BeginPlay() {
	Super::BeginPlay();
}

void AFICSubsystem::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (bSetup) {
		bSetup = false;
		AChatCommandSubsystem* CCS = AChatCommandSubsystem::Get(this);
		if (CCS) CCS->RegisterCommand(AFICCommand::StaticClass());

		UDemoNetDriver* Demo = GetWorld()->DemoNetDriver;
		if (Demo) {
			AFICAnimation* Animation = GetWorld()->SpawnActor<AFICAnimation>();
			SetActiveAnimation(Animation);
		}
	}
	
	if (bShouldShowEditor) {
		bShouldShowEditor = false;
		EditorContext->ShowEditor();
	}
}

void AFICSubsystem::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	
	if (Camera) Camera->Destroy();
	Camera = nullptr;
}

bool AFICSubsystem::ShouldSave_Implementation() const {
	return true;
}

AFICSubsystem* AFICSubsystem::GetFICSubsystem(UObject* WorldContext) {
	UFICSubsystemHolder* SubSysHolder = GetSubsystemHolder<UFICSubsystemHolder>(WorldContext);
	if (SubSysHolder) return SubSysHolder->Subsystem;
	else {
		TArray<AActor*> SubSystems;
		UGameplayStatics::GetAllActorsOfClass(WorldContext, AFICSubsystem::StaticClass(), SubSystems);
		if (SubSystems.Num() > 0) return Cast<AFICSubsystem>(SubSystems[0]);
		return WorldContext->GetWorld()->SpawnActor<AFICSubsystem>();
	}
}

void AFICSubsystem::PlayAnimation(AFICAnimation* Path) {
	CreateCamera();
	if (!Camera || !Path) return;
	Camera->StartAnimation(Path);
}

void AFICSubsystem::StopAnimation() {
	if (!Camera) return;
	Camera->StopAnimation();
}

void AFICSubsystem::AddVisibleAnimation(AFICAnimation* Path) {
	if (Path) VisibleAnimations.Add(Path);
}

void AFICSubsystem::SetActiveAnimation(AFICAnimation* inActiveAnimation) {
	if (EditorContext) {
		EditorContext->HideEditor();
		EditorContext = nullptr;
	}
	ActiveAnimation = inActiveAnimation;
	if (ActiveAnimation) {
		EditorContext = NewObject<UFICEditorContext>(this);
		EditorContext->SetAnimation(ActiveAnimation, Cast<AFICEditorCameraCharacter>(GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator()));
		bShouldShowEditor = true;
	}
}

UFICEditorContext* AFICSubsystem::GetEditor() const {
	return EditorContext;
}

void AFICSubsystem::CreateCamera() {
	if (!Camera) Camera = GetWorld()->SpawnActor<AFICCameraCharacter>();
}

bool AFICSubsystem::IsRecording() {
	return bIsRecording;
}

void AFICSubsystem::StopRecording() {
	if (bIsRecording) GetWorld()->GetGameInstance()->StopRecordingReplay();
	bIsRecording = false;
}

void AFICSubsystem::StartRecording(const FString& Name) {
	StopRecording();
	GetWorld()->GetGameInstance()->StartRecordingReplay(Name, Name);
	bIsRecording = true;
}

void AFICSubsystem::PlayRecording(const FString& Name) {
	GetWorld()->GetAuthGameMode()->SpectatorClass = AFICEditorCameraCharacter::StaticClass();
	GetWorld()->GetGameInstance()->PlayReplay(Name);
}
