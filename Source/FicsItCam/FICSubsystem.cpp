﻿#include "FICSubsystem.h"


#include "FICCommand.h"
#include "FICSubsystemHolder.h"
#include "mod/ModSubsystems.h"

AFICSubsystem::AFICSubsystem() {
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void AFICSubsystem::BeginPlay() {
	Super::BeginPlay();

	AChatCommandSubsystem::Get(this)->RegisterCommand(AFICCommand::StaticClass());
}

void AFICSubsystem::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
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
	return GetSubsystemHolder<UFICSubsystemHolder>(WorldContext)->Subsystem;
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
		EditorContext->SetAnimation(ActiveAnimation);
		EditorContext->ShowEditor();
	}
}

UFICEditorContext* AFICSubsystem::GetEditor() const {
	return EditorContext;
}

void AFICSubsystem::CreateCamera() {
	if (!Camera) Camera = GetWorld()->SpawnActor<AFICCameraCharacter>();
}
