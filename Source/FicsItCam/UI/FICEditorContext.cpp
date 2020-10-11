#include "FICEditorContext.h"

#include "SceneViewport.h"
#include "WidgetBlueprintLibrary.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void PrintWidget(const TSharedPtr<SWidget>& Widget, FString Prefix) {
	if (!Widget) return;
	SML::Logging::error(TCHAR_TO_UTF8(*(Prefix + Widget->GetType().ToString())), " ", Widget.Get());
	for (int i = 0; i < Widget->GetChildren()->Num(); ++i) {
		PrintWidget(Widget->GetChildren()->GetChildAt(i), Prefix + " ");
	}
}

#pragma optimize("", off)
void UFICEditorContext::ShowEditor() {
	HideEditor();

	FVector Location = FVector(PosX.GetValue(), PosY.GetValue(), PosZ.GetValue());
	FRotator Rotation = FRotator(RotPitch.GetValue(), RotYaw.GetValue(), RotRoll.GetValue());
	if (!CameraCharacter) {
		OriginalCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
		CameraCharacter = GetWorld()->SpawnActor<AFICEditorCameraCharacter>(Location, Rotation);
		GetWorld()->GetFirstPlayerController()->Possess(CameraCharacter);
	} else CameraCharacter->SetActorLocationAndRotation(Location, Rotation);
	CameraCharacter->EditorContext = this;
	
	GameViewport = GetWorld()->GetGameViewport()->GetGameViewportWidget();
	TSharedPtr<SWindow> Window = GetWorld()->GetGameViewport()->GetWindow();
	GameOverlay = StaticCastSharedRef<SOverlay>(Window->GetChildren()->GetChildAt(0));
	GameViewportContainer = StaticCastSharedRef<SHorizontalBox>(GameOverlay->GetChildren()->GetChildAt(0));

	GameOverlay->RemoveSlot(GameViewportContainer.ToSharedRef());

	EditorWidget = SNew(SFICEditor)
        .Context(this)
        .GameWidget(GameViewport)
		.GameWidgetHolder(GameViewportContainer);
	
	GameOverlay->AddSlot()[
		EditorWidget.ToSharedRef()
	];
}

void UFICEditorContext::HideEditor() {
	if (CameraCharacter && OriginalCharacter) {
		APlayerController* Controller = GetWorld()->GetFirstPlayerController();
		if (Controller) Controller->Possess(OriginalCharacter);
	}
	if (EditorWidget) {
		GameOverlay->RemoveSlot(EditorWidget.ToSharedRef());
		GameOverlay->AddSlot()[
			GameViewportContainer.ToSharedRef()
		];
		EditorWidget = nullptr;
		APlayerController* Controller = GetWorld()->GetFirstPlayerController();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(Controller);
	}
}

UFICEditorContext::UFICEditorContext() :
	PosX(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->PosX : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	PosY(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->PosY : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	PosZ(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->PosZ : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	RotPitch(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->RotPitch : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	RotYaw(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->RotYaw : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	RotRoll(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->RotRoll : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	FOV(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->FOV : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	Replay(TAttribute<FFICFloatAttribute*>::Create([this](){ return Animation ? &Animation->Replay : nullptr; }), FFICAttributeValueChanged::CreateUObject(this, &UFICEditorContext::UpdateCharacterValues)),
	All({{"X", &PosX }, {"Y", &PosY}, {"Z", &PosZ}, {"Pitch", &RotPitch}, {"Yaw", &RotYaw}, {"Roll", &RotRoll}, {"FOV", &FOV}, {"Replay", &Replay}}) {}

void UFICEditorContext::SetAnimation(AFICAnimation* Anim, AFICEditorCameraCharacter* Camera) {
	if (Camera) {
		if (CameraCharacter) CameraCharacter->Destroy();
		CameraCharacter = Camera;
	}
	Animation = Anim;
	SetCurrentFrame(Animation->AnimationStart);
}

AFICAnimation* UFICEditorContext::GetAnimation() const {
	return Animation;
}

void UFICEditorContext::SetCurrentFrame(int64 inFrame) {
	CurrentFrame = inFrame;

	PosX.SetFrame(CurrentFrame);
	PosY.SetFrame(CurrentFrame);
	PosZ.SetFrame(CurrentFrame);
	RotPitch.SetFrame(CurrentFrame);
	RotYaw.SetFrame(CurrentFrame);
	RotRoll.SetFrame(CurrentFrame);
	FOV.SetFrame(CurrentFrame);
	Replay.SetFrame(CurrentFrame);
	
	UpdateCharacterValues();
}

int64 UFICEditorContext::GetCurrentFrame() const {
	return CurrentFrame;
}

void UFICEditorContext::SetFlySpeed(float Speed) {
	CameraCharacter->MaxFlySpeed = FMath::Clamp(Speed, 0.0f, 10000.0f);
}

float UFICEditorContext::GetFlySpeed() {
	return CameraCharacter->MaxFlySpeed;
}

void UFICEditorContext::UpdateCharacterValues() {
	if (CameraCharacter) {
		CameraCharacter->UpdateValues();
		GetWorld()->DemoNetDriver->GotoTimeInSeconds(Replay.GetValue());
	}
}

#pragma optimize("", on)
