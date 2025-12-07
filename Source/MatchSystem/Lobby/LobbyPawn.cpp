#include "LobbyPawn.h"
#include "Components/SkeletalMeshComponent.h"

ALobbyPawn::ALobbyPawn()
{
    // 메시 컴포넌트 생성
    CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(RootComponent);

    // 위치 보정 (메시가 땅에 떠있지 않도록)
    CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // 충돌 비활성화 (SpectatorPawn이라 불필요)
    CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
