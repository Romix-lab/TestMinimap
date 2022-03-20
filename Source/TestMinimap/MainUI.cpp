// Fill out your copyright notice in the Description page of Project Settings.
#include "MainUI.h"
#include "Kismet/GameplayStatics.h"
#include "Landscape.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Blueprint/WidgetTree.h"
#include "MinimapActor.h"
#include "Engine/TextureRenderTarget2D.h"

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();
	FVector2D Screen = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(UGameplayStatics::GetPlayerController(GetWorld(), 0)).GetLocalSize();
	UWidgetLayoutLibrary::SlotAsCanvasSlot(MinimapImage)->SetSize(FVector2D(Screen.X / 2.f, Screen.Y));
	UWidgetLayoutLibrary::SlotAsCanvasSlot(MinimapImage)->SetPosition(FVector2D(Screen.X / 2.f, 0.f));
	/*Material = UMaterial::GetDefaultMaterial(MD_UI);

	
	UMaterialExpressionTextureSample* TextureExpression = NewObject<UMaterialExpressionTextureSample>(Material);
	TextureExpression->Texture = Cast<UTexture>(Cast<AMinimapActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AMinimapActor::StaticClass()))->GetRenderTarget());
	TextureExpression->SamplerType = SAMPLERTYPE_Color;
	Material->Expressions.Add(TextureExpression);
	Material->BaseColor.Expression = TextureExpression;*/

	MinimapImage->SetBrushFromMaterial(Material);
}

bool UMainUI::Initialize()
{
	bool bStatus = Super::Initialize();
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	MinimapImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	Canvas->AddChildToCanvas(MinimapImage);
	WidgetTree->RootWidget = Canvas;
	return bStatus;
}

UMainUI::UMainUI(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterial> MatClassFinder(TEXT("Material'/Game/ThirdPersonCPP/Blueprints/MinimapRenderingTarget_Mat.MinimapRenderingTarget_Mat'"));
	if (MatClassFinder.Object)
	{
		Material = MatClassFinder.Object;
	}
}

void UMainUI::DrawVision(FLineSegments* NewVision)
{
	ALandscape* Landscape = Cast<ALandscape>(UGameplayStatics::GetActorOfClass(GetWorld(), ALandscape::StaticClass()));
	FVector BoxExtent;
	FVector Origin;

	Landscape->GetActorBounds(true, Origin, BoxExtent, false);
	LandscapeSize = FVector2D(BoxExtent.X, BoxExtent.Y);
	Vision = NewVision;
	for (int i = 0; i < Vision->Lines.Num(); i++)
	{
		RecalcCoordinates(&Vision->Lines[i].Start);
		RecalcCoordinates(&Vision->Lines[i].End);
		Vision->Lines[i].Thickness /= 15.f;
	}
	
	RemoveFromParent();
	AddToViewport();
}

int32 UMainUI::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (Vision != nullptr)
	{
		for (auto Line : Vision->Lines)
		{
			TArray<FVector2D> Points;
			Points.Add(Line.Start);
			Points.Add(Line.End);
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				Points,
				ESlateDrawEffect::None,
				Line.Color,
				false,
				Line.Thickness);
		}
		
	}
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UMainUI::RecalcCoordinates(FVector2D *World)
{
	FVector2D Screen = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(UGameplayStatics::GetPlayerController(GetWorld(), 0)).GetLocalSize();
	World->X = (0.75f + World->X / (4 * LandscapeSize.X)) * Screen.X;
	World->Y = (0.5f + World->Y / (2 * LandscapeSize.Y)) * Screen.Y;
}



