// Copyright Epic Games, Inc. All Rights Reserved.

#include "MatchSystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include <Online/OnlineSessionNames.h>

#include "MatchSystem/Tittle/Widget/MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMatchSystemCharacter

AMatchSystemCharacter::AMatchSystemCharacter() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
}

void AMatchSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// 현재 맵 이름 확인
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // "UEDPIE_0_" 제거

	if (MapName == "ThirdPersonMap") // 예: 시작 메뉴 레벨 이름
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && MainMenuWidgetClass)
		{
			MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
			if (MainMenuWidgetInstance)
			{
				MainMenuWidgetInstance->AddToViewport();

				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}
		}
	}

}

void AMatchSystemCharacter::CreateGameSession()
{
	//Called when pressing the create room
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;

	SessionSettings->bUseLobbiesIfAvailable = true;

	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//SessionSettings->Set(SEARCH_KEYWORDS, FString("MyGameSession"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void AMatchSystemCharacter::JoinGameSession()
{

	UE_LOG(LogTemp, Warning, TEXT("JoinGameSession() called"));

	// Find game sessions
	if (!OnlineSessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSessionInterface invalid"));
		return;
	}
	
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	UE_LOG(LogTemp, Warning, TEXT("[JoinGameSession] Start finding sessions..."));

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	//SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("MyGameSession"), EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void AMatchSystemCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("[CreateSession] Result: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Created session: %s"), *SessionName.ToString())
			);

			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Select Map : %s"), *SelectedMap.ToString())
			);
		}

		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->RemoveFromParent();
			MainMenuWidgetInstance = nullptr;

			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PC)
			{
				PC->bShowMouseCursor = false;
				PC->SetInputMode(FInputModeGameOnly());
			}
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
			);
		}

		// UI 다시 띄우기
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && MainMenuWidgetClass)
		{
			MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
			if (MainMenuWidgetInstance)
			{
				MainMenuWidgetInstance->AddToViewport();

				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}
	}

}

//타입이 MatchType일 때 Join 하도록 (원래 코드인데 지금 Join이 안되서 다른 코드로 테스트 중)
void AMatchSystemCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete called: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

	if (!OnlineSessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnFindSessionsComplete] OnlineSessionInterface invalid"));
		return;
	}

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	FUniqueNetIdRepl LocalRepl = LocalPlayer->GetPreferredUniqueNetId();
	TSharedPtr<const FUniqueNetId> LocalNetId = LocalRepl.GetUniqueNetId();

	for (auto Result : SessionSearch->SearchResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling JoinSession for session: %s"), *Result.GetSessionIdStr());

		FString MatchTypeValue;
		if (Result.Session.SessionSettings.Get(FName("MatchType"), MatchTypeValue))
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session with MatchType: %s"), *MatchTypeValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session with no MatchType key"));
		}

		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		//자기 자신은 검색하지 않기
		if (Result.Session.OwningUserId.IsValid() && LocalNetId.IsValid() &&
			*Result.Session.OwningUserId == *LocalNetId)
		{
			UE_LOG(LogTemp, Warning, TEXT("Skipping own session: %s"), *Result.GetSessionIdStr());
			continue;
		}

		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
			);
		}
		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining Match Type: %s"), *MatchType)
				);
			}


			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);

		}
	}
}

//void AMatchSystemCharacter::OnFindSessionsComplete(bool bWasSuccessful)
//{
//	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete called: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));
//
//	if (!OnlineSessionInterface.IsValid())
//	{
//		UE_LOG(LogTemp, Error, TEXT("[OnFindSessionsComplete] OnlineSessionInterface invalid"));
//		return;
//	}
//
//	/*const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
//	FUniqueNetIdRepl LocalNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
//	if (!LocalNetIdRepl.IsValid())
//	{
//		UE_LOG(LogTemp, Error, TEXT("LocalNetIdRepl is invalid."));
//		return;
//	}
//
//	const FUniqueNetId* LocalNetId = LocalNetIdRepl.GetUniqueNetId();*/
//
//	for (auto& Result : SessionSearch->SearchResults)
//	{
//		
//		//if (Result.Session.OwningUserId.IsValid())
//		//{
//		//	TSharedRef<const FUniqueNetId> OwnerId = Result.Session.OwningUserId.ToSharedRef();
//
//		//	if (*OwnerId == *LocalNetId)
//		//	{
//		//		// 자기 자신의 세션은 스킵
//		//		continue;
//		//	}
//		//}
//
//		UE_LOG(LogTemp, Warning, TEXT("Calling JoinSession for session: %s"), *Result.GetSessionIdStr());
//
//		FString Id = Result.GetSessionIdStr();
//		FString User = Result.Session.OwningUserName;
//
//		if (GEngine)
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User));
//		}
//
//		// 바로 Join하도록
//		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
//
//		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
//		OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
//
//		break;
//	}
//}


void AMatchSystemCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionComplete] Called. Result: %d"), static_cast<int32>(Result));

	if (!OnlineSessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnJoinSessionComplete] OnlineSessionInterface invalid"));
		return;
	}
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionComplete] ConnectString: %s"), *Address);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect string: %s"), *Address)
			);
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[OnJoinSessionComplete] PlayerController is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[OnJoinSessionComplete] Failed to resolve connect string"));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to join session!"))
			);
		}

		// UI 다시 띄우기
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && MainMenuWidgetClass)
		{
			MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
			if (MainMenuWidgetInstance)
			{
				MainMenuWidgetInstance->AddToViewport();

				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMatchSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMatchSystemCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMatchSystemCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMatchSystemCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMatchSystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}