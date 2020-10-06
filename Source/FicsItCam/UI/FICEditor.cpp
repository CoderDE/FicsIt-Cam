﻿#include "FICEditor.h"

#include "Engine/World.h"
#include "FICEditorContext.h"
#include "FICTimeline.h"
#include "FICDetails.h"
#include "FICSubsystem.h"
#include "WidgetBlueprintLibrary.h"

FSlateColorBrush SFICEditor::Background = FSlateColorBrush(FColor::FromHex("030303"));

void SFICEditor::Construct(const FArguments& InArgs) {
	Context = InArgs._Context.Get();
	GameWidget = InArgs._GameWidget.Get();

	TSharedPtr<SHorizontalBox> GameViewportContainer = StaticCastSharedPtr<SHorizontalBox>(GameWidget->GetParentWidget());

	Children.Add(SNew(SGridPanel)
		.FillColumn(1, 1)
		.FillRow(1, 1)
		+SGridPanel::Slot(0,0).ColumnSpan(2)[
			SNew(SOverlay)
			+SOverlay::Slot()[
				SNew(SImage)
				.Image(&Background)
			]
			+SOverlay::Slot()[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot().AutoWidth()[
					SNew(SButton)
					.Text(FText::FromString("Exit"))
					.OnClicked_Lambda([this]() {
						AFICSubsystem::GetFICSubsystem(Context->GetWorld())->SetActiveAnimation(nullptr);
						return FReply::Handled();
					})
				]
				+SHorizontalBox::Slot().FillWidth(1)[
					SNew(SSpacer)
				]
			]
		]
		+SGridPanel::Slot(1,1)[
			GameViewportContainer.ToSharedRef()
		]
		+SGridPanel::Slot(0, 1)[
			SNew(SBox)
			.WidthOverride(400)
			.Content()[
				SNew(SFICDetails)
				.Context(Context)
			]
		]
		+SGridPanel::Slot(0, 2).ColumnSpan(2)[
			SNew(SFICTimelinePanel)
			.Context(Context)
		]
	);
}

SFICEditor::SFICEditor() : Children(this) {}

void SFICEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	SPanel::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bIsLeft || bIsRight) {
		KeyPressTime += InDeltaTime;
		while (KeyPressTime > 0.5) {
			Context->SetCurrentFrame(Context->GetCurrentFrame() + (bIsLeft ? -1 : 1));
			KeyPressTime -= 0.2;
		}
	}
}

void SFICEditor::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const {
	ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(Children[0], FVector2D(0, 0), AllottedGeometry.GetLocalSize()));
}

FVector2D SFICEditor::ComputeDesiredSize(float) const {
	return Children[0]->GetDesiredSize();
}

FChildren* SFICEditor::GetChildren() {
	return &Children;
}

FReply SFICEditor::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) {
	if (InKeyEvent.GetKey() == EKeys::RightAlt) {
		if (GameWidget->HasUserFocus(InKeyEvent.GetUserIndex())) {
			FSlateApplication::Get().SetUserFocus(InKeyEvent.GetUserIndex(), SharedThis(this));
			APlayerController* Controller = Context->GetWorld()->GetFirstPlayerController();
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(Controller);
		} else {
			FSlateApplication::Get().SetUserFocusToGameViewport(InKeyEvent.GetUserIndex());
			APlayerController* Controller = Context->GetWorld()->GetFirstPlayerController();
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(Controller);
		}
		return FReply::Handled();
	} else if (!GameWidget->HasAnyUserFocusOrFocusedDescendants()) {
		if (InKeyEvent.GetKey() == EKeys::I) {
			int64 Time = Context->GetCurrentFrame();
			if (Context->PosX.GetKeyframe(Time) && Context->PosY.GetKeyframe(Time) && Context->PosZ.GetKeyframe(Time) && Context->RotPitch.GetKeyframe(Time) && Context->RotYaw.GetKeyframe(Time) && Context->RotRoll.GetKeyframe(Time) && Context->FOV.GetKeyframe(Time) &&
                !Context->PosX.HasChanged(Time) && !Context->PosY.HasChanged(Time) && !Context->PosZ.HasChanged(Time) && !Context->RotPitch.HasChanged(Time) && !Context->RotYaw.HasChanged(Time) && !Context->RotRoll.HasChanged(Time) && !Context->FOV.HasChanged(Time)) {
				Context->All.RemoveKeyframe(Time);
                } else {
                	Context->All.SetKeyframe(Time);
                }
			return FReply::Handled();
		} else if (InKeyEvent.GetKey() == EKeys::Left) {
			Context->SetCurrentFrame(Context->GetCurrentFrame()-1);
			bIsLeft = true;
			KeyPressTime = 0;
			return FReply::Handled();
		} else if (InKeyEvent.GetKey() == EKeys::Right) {
			Context->SetCurrentFrame(Context->GetCurrentFrame()+1);
			bIsRight= true;
			KeyPressTime = 0;
			return FReply::Handled();
		} else if (InKeyEvent.GetKey() == EKeys::N) {
			int64 Time;
			TSharedPtr<FFICKeyframeRef> KF;
			if (Context->All.GetAttribute()->GetPrevKeyframe(Context->GetCurrentFrame(), Time, KF)) Context->SetCurrentFrame(Time);
			return FReply::Handled();
		} else if (InKeyEvent.GetKey() == EKeys::M) {
			int64 Time;
			TSharedPtr<FFICKeyframeRef> KF;
			if (Context->All.GetAttribute()->GetNextKeyframe(Context->GetCurrentFrame(), Time, KF)) Context->SetCurrentFrame(Time);
			return FReply::Handled();
		}
	}
	return SPanel::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SFICEditor::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) {
	if (InKeyEvent.GetKey() == EKeys::Left && bIsLeft) {
		bIsLeft = false;
		return FReply::Handled();
	} else if (InKeyEvent.GetKey() == EKeys::Right && bIsRight) {
		bIsRight = false;
		return FReply::Handled();
	}
	return SPanel::OnKeyUp(MyGeometry, InKeyEvent);
}

FReply SFICEditor::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
	if (MouseEvent.GetModifierKeys().IsControlDown()) {
		float Delta = MouseEvent.GetWheelDelta();
		int64 Range = Context->GetAnimation()->AnimationEnd - Context->GetAnimation()->AnimationStart;
		while (Range > 300) {
			Range /= 10;
			Delta *= 10;
		}
		Context->SetCurrentFrame(Context->GetCurrentFrame() + Delta);
		return FReply::Handled();
	} else if (MouseEvent.GetModifierKeys().IsShiftDown()) {
		float Delta = MouseEvent.GetWheelDelta();
		Context->SetFlySpeed(Context->GetFlySpeed() + Delta);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

bool SFICEditor::SupportsKeyboardFocus() const {
	return true;
}

void SFICEditor::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) {
	SPanel::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
	if (!PreviousFocusPath.ContainsWidget(GameWidget.ToSharedRef()) && NewWidgetPath.ContainsWidget(GameWidget.ToSharedRef())) {
		APlayerController* Controller = Context->GetWorld()->GetFirstPlayerController();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(Controller);
	}
}

#pragma optimize("", on)
