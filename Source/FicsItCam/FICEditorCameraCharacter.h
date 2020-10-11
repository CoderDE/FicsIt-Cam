#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpectatorPawn.h"


#include "FICEditorCameraCharacter.generated.h"

class UFICEditorContext;
UCLASS()
class AFICEditorCameraCharacter : public ASpectatorPawn {
	GENERATED_BODY()

private:
	UPROPERTY()
	UCameraComponent* Camera;

	bool bIsSprinting = false;
	bool bIsJumping = false;
	float FlyMultiplier = 10000;

public:
	float MaxFlySpeed = 1000;
	
	UPROPERTY()
	UFICEditorContext* EditorContext = nullptr;

	AFICEditorCameraCharacter();
	
	// Begin AActor
	virtual void Tick( float DeltaSeconds ) override;
	virtual void BeginPlay() override;
	// End AActor

	// Begin ACharacter
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	// End ACharacter
	
	UFUNCTION()
    void FICMoveForward(float Value);
	UFUNCTION()
    void FICMoveRight(float Value);
	UFUNCTION()
	void RotatePitch(float Value);
	UFUNCTION()
	void RotateYaw(float Value);
	UFUNCTION()
	void RotateRoll(float Value);

	UFUNCTION()
	void FlyUp(float Value);

	UFUNCTION()
	void SprintPressed();
	UFUNCTION()
	void SprintReleased();

	UFUNCTION()
    void JumpPressed();
	UFUNCTION()
    void JumpReleased();

	UFUNCTION()
	void NextKeyframe();
	UFUNCTION()
	void PrevKeyframe();

	UFUNCTION()
	void NextFrame();
	UFUNCTION()
	void PrevFrame();

	UFUNCTION()
	void ChangedKeyframe();

	UFUNCTION()
	void Zoom(float Value);

	// Updates Camera Values based on the current value cache in the referenced editor context
	void UpdateValues();
};
