#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"          // TActorIterator
#include "Engine/TargetPoint.h"   // ATargetPoint
#include "LobbyPlayerState.h"
#include "LobbyPlayerController.h"
#include "LobbyPawn.h"

ALobbyGameMode::ALobbyGameMode()
{
    bStartPlayersAsSpectators = false;

    PlayerStateClass = ALobbyPlayerState::StaticClass();
    PlayerControllerClass = ALobbyPlayerController::StaticClass();
    DefaultPawnClass = ALobbyPawn::StaticClass();

    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("LobbySlot"), LobbySlots);
}

void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

    LobbySlots.Empty();

    // TargetPoint 이름 순서대로 LobbySlots에 추가
    TArray<AActor*> FoundPoints;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundPoints);

    // 이름순 정렬 보장
    FoundPoints.Sort([](const AActor& A, const AActor& B) {
        return A.GetFName().LexicalLess(B.GetFName());
        });

    for (AActor* Point : FoundPoints)
    {
        if (Point->GetFName().ToString().StartsWith("LobbySpawn_"))
        {
            LobbySlots.Add(Point);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[LobbyGameMode] Found %d lobby spawn points."), LobbySlots.Num());
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    const int32 MaxPlayers = 4;
    if (PlayerQueue.Num() >= MaxPlayers)
    {
        NewPlayer->ClientTravel(TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap"), ETravelType::TRAVEL_Absolute);
        return;
    }

    PlayerQueue.Add(NewPlayer);
    int32 AssignedIndex = PlayerQueue.Num() - 1;

    if (LobbySlots.IsValidIndex(AssignedIndex) && LobbySlots[AssignedIndex])
    {
        AActor* Slot = LobbySlots[AssignedIndex];

        // Pawn이 없으면 생성해서 위치 지정 및 Possess
        if (APawn* ExistingPawn = NewPlayer->GetPawn())
        {
            ExistingPawn->SetActorLocation(Slot->GetActorLocation());
            ExistingPawn->SetActorRotation(Slot->GetActorRotation());
        }
        else
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = NewPlayer;

            APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(
                DefaultPawnClass,
                Slot->GetActorTransform(),
                SpawnParams
            );

            if (SpawnedPawn)
            {
                NewPlayer->Possess(SpawnedPawn);
            }
        }
    }

    // 모든 클라에게 슬롯 갱신 명령
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(It->Get()))
        {
            UE_LOG(LogTemp, Warning, TEXT("Sending slot update to: %s"), *PC->GetName());
            PC->Client_UpdatePlayerSlots();
        }
    }
}



void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    APlayerController* ExitingPC = Cast<APlayerController>(Exiting);
    if (ExitingPC)
    {
        PlayerQueue.Remove(ExitingPC);
        ReorderSlots();
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(It->Get()))
        {
            PC->Client_UpdatePlayerSlots();
        }
    }
}

void ALobbyGameMode::AssignPlayerToSlot(APlayerController* Player)
{
    int32 SlotIndex = PlayerQueue.IndexOfByKey(Player);
    if (SlotIndex != INDEX_NONE && LobbySlots.IsValidIndex(SlotIndex))
    {
        APawn* Pawn = Player->GetPawn();
        if (Pawn)
        {
            FVector Location = LobbySlots[SlotIndex]->GetActorLocation();
            FRotator Rotation = LobbySlots[SlotIndex]->GetActorRotation();
            Pawn->SetActorLocationAndRotation(Location, Rotation);
        }
    }
}

void ALobbyGameMode::ReorderSlots()
{
    for (int32 i = 0; i < PlayerQueue.Num(); ++i)
    {
        if (LobbySlots.IsValidIndex(i))
        {
            APlayerController* Player = PlayerQueue[i];
            if (Player && Player->GetPawn())
            {
                Player->GetPawn()->SetActorLocationAndRotation(
                    LobbySlots[i]->GetActorLocation(),
                    LobbySlots[i]->GetActorRotation()
                );
            }
        }
    }
}

void ALobbyGameMode::BroadcastUpdatePlayerSlots()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(It->Get()))
        {
            PC->Client_UpdatePlayerSlots();
            UE_LOG(LogTemp, Warning, TEXT("BroadcastUpdatePlayerSlots: Updated %s"), *PC->GetName());
        }
    }
}
