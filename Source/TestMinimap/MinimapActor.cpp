// Fill out your copyright notice in the Description page of Project Settings.
#include "MinimapActor.h"
#include "Kismet/GameplayStatics.h"
#include "TestMinimapCharacter.h"
#include "Landscape.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMinimapActor::AMinimapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CheckVisionStepsNum = 20;
	MinimapCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCaptureComponent"));
	MinimapCaptureComponent->SetWorldRotation(FRotator(-90.f, -90.f, 0.f));
	MinimapCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	// TextureTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));
	// TextureTarget->InitAutoFormat(256, 256);
	// MinimapCaptureComponent->TextureTarget = TextureTarget;
	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> MatClassFinder(TEXT("TextureRenderTarget2D'/Game/ThirdPersonCPP/Blueprints/MinimapRenderingTarget.MinimapRenderingTarget'"));
	if (MatClassFinder.Object)
	{
		MinimapCaptureComponent->TextureTarget = MatClassFinder.Object;
	}
}

/*UTextureRenderTarget2D* AMinimapActor::GetRenderTarget()
{
	return TextureTarget;
}*/

// Called when the game starts or when spawned
void AMinimapActor::BeginPlay()
{
	Super::BeginPlay();
	UI = CreateWidget<UMainUI, APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UMainUI::StaticClass());

	ALandscape* Landscape = Cast<ALandscape>(UGameplayStatics::GetActorOfClass(GetWorld(), ALandscape::StaticClass()));
	FVector BoxExtent;
	FVector Origin;
	Landscape->GetActorBounds(true, Origin, BoxExtent, false);
	SetActorLocation(Origin + FVector(0.f, 0.f, BoxExtent.Z + 10000), false, nullptr, ETeleportType::TeleportPhysics);
	MinimapCaptureComponent->OrthoWidth = FMath::Max(BoxExtent.X, BoxExtent.Y) * 2.f;
}

// Called every frame
void AMinimapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ProcessVision();
}

void AMinimapActor::CheckLinesIntersection(FLineSegment &L1, FLineSegment &L2)
{
	float s1_x, s1_y, s2_x, s2_y;

	s1_x = L1.End.X - L1.Start.X;     s1_y = L1.End.Y - L1.Start.Y;
	s2_x = L2.End.X - L2.Start.X;     s2_y = L2.End.Y - L2.Start.Y;

	float s, t;
	s = (-s1_y * (L1.Start.X - L2.Start.X) + s1_x * (L1.Start.Y - L2.Start.Y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (L1.Start.Y - L2.Start.Y) - s2_y * (L1.Start.X - L2.Start.X)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		L1.Color = FLinearColor::Blue;
		L2.Color = FLinearColor::Blue;
	}
}

void AMinimapActor::ProcessVision()
{
	TArray<FLineSegment> Lines0;
	TArray<FLineSegment> Lines1;
	UWorld* World = GetWorld();
	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(World, ATestMinimapCharacter::StaticClass(), Characters);
	if (Characters.Num() < 2)
		return;
	EstimateVisionArea(Cast<ATestMinimapCharacter>(Characters[0]), Lines0);
	EstimateVisionArea(Cast<ATestMinimapCharacter>(Characters[1]), Lines1);
	for (int i = 0; i < Lines0.Num(); i++)
	{
		for (int j = 0; j < Lines1.Num(); j++)
		{
			CheckLinesIntersection(Lines0[i], Lines1[j]);
		}
	}
	Lines0.Append(Lines1);
	Vision = FLineSegments(Lines0);
	UI->DrawVision(&Vision);
}

void AMinimapActor::EstimateVisionArea(ATestMinimapCharacter* Character, TArray<FLineSegment> &AreaLines)
{
	FLinearColor NormalColor = Character->GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0) ? 
		FLinearColor::Green : FLinearColor::Red;
	
	UWorld* World = GetWorld();
	FHitResult Hit;
	FVector StartPoint;
	FRotator ViewRotation;
	Character->GetActorEyesViewPoint(StartPoint, ViewRotation);
	// Search from 3 oclock
	FVector LeftPoint = Character->GetController()->GetControlRotation().Vector().RotateAngleAxis(Character->AngleOfVisionDeg, FVector(0.f, 0.f, 1.f));

	float AngleDeltaX = -180.f / static_cast<float>(CheckVisionStepsNum - 1);
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.AddIgnoredActor(Character);
	
	if (World->LineTraceSingleByChannel(Hit, StartPoint, LeftPoint * Character->VisionDistance + StartPoint, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		float Thickness = FVector2D::Distance(FVector2D(Hit.Location.X, Hit.Location.Y), FVector2D(StartPoint.X, StartPoint.Y))
			* FMath::Sin(-0.5f * PI * AngleDeltaX / 180.f) / 2.f;
		AreaLines.Add(FLineSegment(FVector2D(Hit.Location.X, Hit.Location.Y), FVector2D(Hit.Location.X + 1.f, Hit.Location.Y + 1.f),
			NormalColor, Thickness));
	};
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	// Rotate clockwize down to 9 oclock step by step
	for (int i = 0; i < CheckVisionStepsNum; i++)
	{

		int NHitsLast = INT_MAX;
		FVector EndPoint = LeftPoint.RotateAngleAxis(AngleDeltaX * i, Character->GetController()->GetControlRotation().Vector());
		FVector End = EndPoint * Character->VisionDistance + StartPoint;
		FVector2D Last = FVector2D::ZeroVector;

		float AngleDeltaY = -FMath::Asin(EndPoint.Z) * 2 * 180.f / (PI * static_cast<float>(CheckVisionStepsNum - 1));
		for (int j = 0; j < CheckVisionStepsNum; j++)
		{
			TArray<FHitResult> Hits;

			if (Multitrace(Hits, StartPoint, End, CollisionQueryParams))
			{
				float Thickness = FVector2D::Distance(FVector2D(Hits[0].Location.X, Hits[0].Location.Y), FVector2D(StartPoint.X, StartPoint.Y))
					* FMath::Sin(-0.5f * PI * AngleDeltaX / 180.f) / 2.f;

				if (Hits.Num() >= NHitsLast)
				{
					AreaLines.Add(FLineSegment(FVector2D(Hits[0].Location.X, Hits[0].Location.Y), Last, NormalColor, Thickness));
				}
				Last = FVector2D(Hits[0].Location.X, Hits[0].Location.Y);
				NHitsLast = Hits.Num();
			}
			else
			{
				if (NHitsLast == 1 && Last.Size() >= 0.8f * Character->VisionDistance)
				{
					float Thickness = Character->VisionDistance * FMath::Sin(-0.5f * PI * AngleDeltaX / 180.f) / 2.f;
					AreaLines.Add(FLineSegment(Last, FVector2D(End.X, End.Y), NormalColor, Thickness));
				}
				NHitsLast = INT_MAX;
				//break;
			}
			// Rotate vector in vertical parallel plane of forward vector 

			EndPoint = EndPoint.RotateAngleAxis(AngleDeltaY, FVector(EndPoint.Y, -EndPoint.X, 0.f));
			End = EndPoint * Character->VisionDistance + StartPoint;
		}
	}
}

bool AMinimapActor::Multitrace(TArray<FHitResult>& Hits, FVector Start, FVector End, FCollisionQueryParams CollisionQueryParams)

{
	bool Res = false;
	UWorld* World = GetWorld();
	TArray<FHitResult> HitsTmp;
	FVector Unit = (End - Start);
	Unit.Normalize();
	while (World->LineTraceMultiByChannel(HitsTmp, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		Start = HitsTmp.Last().Location + Unit;
		Hits.Append(HitsTmp);
		Res = true;
	}
	return Res;
}
