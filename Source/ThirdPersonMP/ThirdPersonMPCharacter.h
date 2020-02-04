// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonMPCharacter.generated.h"

UCLASS(config=Game)
class AThirdPersonMPCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AThirdPersonMPCharacter();

	/** Property replication **/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** The player's maximum health. This is highest that their health can be, and the value that their health starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category="Health")
	float MaxHealth;

	/** The player's current health. When reduced to 0,they are considered dead.**/
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly,Category="Gameplay|Projectile")
	TSubclassOf<class AThirdPersonMPProjectile> ProjectileClass;

	/** Delay between shots in seconds. Used to control fire rate for out test projectile, but also  to prevent an overflow of server functions
	 * from binding SpawnProjectile directly to input
	 **/
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float FireRate;

	/** If true, we are in the process of firing projectiles **/
	bool bIsFiringWeapon;

	/** Function for beginning weapong fire. */
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StartFire();

	/** Function for ending weapon fire. Once this is called, the player can use StartFire again. */
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StopFire();

	/** Server function for spawning projectiles */
	UFUNCTION(Server,WithValidation, Reliable)
	void HandleFire();

	/** A timer handle use for providing the fire rate delay in-between spawns */
	FTimerHandle FiringTimer;

	/** RepNotify for changes made to current health**/
	UFUNCTION()
	void OnRep_CurrentHealth();

	/** Response to health being updated. Called on the server immediately after modification, and  on clients in response to a RepNotify **/
	void OnHealthUpdate();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


// Se você seguiu esta seção até agora, o seguinte deve ser o fluxo para aplicar danos a um ator:
// Um ator ou função externa chama CauseDamagenosso personagem, que por sua vez chama sua TakeDamagefunção.
// TakeDamagechamadas SetCurrentHealthpara alterar o valor de integridade atual do jogador no servidor.
// SetCurrentHealthchama OnHealthUpdateo servidor, causando a execução de qualquer funcionalidade que ocorra em resposta a alterações na saúde do jogador.
// CurrentHealth replica nas cópias do Personagem de todos os clientes conectados.
// Quando cada cliente recebe um novo CurrentHealthvalor do servidor, eles chamam OnRep_CurrentHealth.
// OnRep_CurrentHealthchamadas OnHealthUpdate, garantindo que cada cliente responda da mesma maneira ao novo CurrentHealthvalor.

	/** Getter for Max Health. */
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** Getterfor Current Health. */
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** Setter for Current Health. Clamps the value between 0 and Max Health and calls OnHealthUpdate. Should only be called on the server*/
	UFUNCTION(BlueprintCallable, Category= "Health")
	void SetCurrentHealth(float healthValue);

	/** Event for taking damage. Overridden from APawn. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent,AController* EventInstigator,AActor* DamageCause) override;
};

