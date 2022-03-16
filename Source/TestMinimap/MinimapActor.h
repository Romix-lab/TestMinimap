// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Delegates/DelegateCombinations.h"
#include "MinimapActor.generated.h"

class ATestMinimapCharacter;

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
	FLineSegments(TArray<FLineSegment> LineSegments):
		Lines(LineSegments){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line properties")
	TArray<FLineSegment> Lines;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalculatedVision, FLineSegments, Segments);

UCLASS()
class TESTMINIMAP_API AMinimapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinimapActor();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Vision)
	int CheckVisionStepsNum;

	UPROPERTY(BlueprintAssignable)
	FOnCalculatedVision OnCalculatedVision;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void ProcessVision();
	void EstimateVisionArea(ATestMinimapCharacter* Character, TArray<FLineSegment> &AreaLines);
	bool Multitrace(TArray<FHitResult>& Hits, FVector Start, FVector End,  FCollisionQueryParams CollisionQueryParams);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void CheckLinesIntersection(FLineSegment &L1, FLineSegment &L2);

};
