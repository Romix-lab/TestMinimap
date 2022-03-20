// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"
/**
 * 
 */
class UMaterial;

USTRUCT(BlueprintType)
struct FLineSegment
{
	GENERATED_BODY()

		FLineSegment()
	{}

	FLineSegment(FVector2D P1, FVector2D P2, FLinearColor LineColor, float LineThickness) :
		Start(P1), End(P2), Color(LineColor), Thickness(LineThickness) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line properties")
		FVector2D Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line properties")
		FVector2D End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line properties")
		FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line properties")
		float Thickness;
};


USTRUCT(BlueprintType)
struct FLineSegments
{
	GENERATED_BODY()

		FLineSegments()
	{}
	FLineSegments(TArray<FLineSegment> LineSegments) :
		Lines(LineSegments) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line properties")
		TArray<FLineSegment> Lines;
};


UCLASS()
class TESTMINIMAP_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
	void RecalcCoordinates(FVector2D* Coords);
	FVector2D LandscapeSize;
	FLineSegments* Vision = {nullptr};
	UMaterial* Material;
protected:

	class UCanvasPanel *Canvas;
	// UPROPERTY(meta = (BindWidget))
	class UImage* MinimapImage;

	void NativeConstruct() override;
public:
	virtual bool Initialize() override;
	UMainUI(const FObjectInitializer& ObjectInitializer);
	void DrawVision(FLineSegments *Vision);
	// virtual void NativePaint(FPaintContext& InContext) const override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
