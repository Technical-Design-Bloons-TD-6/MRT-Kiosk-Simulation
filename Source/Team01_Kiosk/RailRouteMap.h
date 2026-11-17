#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Engine/DataAsset.h"
#include "RailRouteMap.generated.h"

USTRUCT(BlueprintType)
struct FRailMapStation
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Codes;
    // Coordinates in the source artwork's 1600 x 1600 design space.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Position = FVector2D::ZeroVector;
};

USTRUCT(BlueprintType)
struct FRailMapConnection
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 From = INDEX_NONE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 To = INDEX_NONE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Line;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Minutes = 2.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor Colour = FLinearColor::White;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector2D> Bends;
};

/** Editable network snapshot; no files or external services are needed in a packaged game. */
UCLASS(BlueprintType)
class TEAM01_KIOSK_API URailMapNetwork : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FRailMapStation> Stations;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FRailMapConnection> Connections;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float TransferMinutes = 4.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> Artwork;
};

USTRUCT(BlueprintType)
struct FRailMapRoute
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    TArray<int32> Stations;
    UPROPERTY(BlueprintReadOnly)
    TArray<int32> Connections;
    UPROPERTY(BlueprintReadOnly)
    float EstimatedMinutes = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 Transfers = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRailStationSelected, const FString&, StationName);
class SRailRouteMap;
class UUserWidget;

/** One captured pointer controls both pan axes; route rendering and hit testing share one transform. */
UCLASS(meta = (DisplayName = "Rail Route Map"))
class TEAM01_KIOSK_API URailRouteMap : public UWidget
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
    TObjectPtr<URailMapNetwork> Network;
    UPROPERTY(BlueprintAssignable, Category = "Map")
    FRailStationSelected OnStationSelected;
    UFUNCTION(BlueprintCallable, Category = "Map")
    void SetJourney(const FString& Start, const FString& Destination, bool AllowSelection);
    UFUNCTION(BlueprintCallable, Category = "Map")
    void FitRoute();
    UFUNCTION(BlueprintCallable, Category = "Map")
    static void CompactStationRow(UUserWidget* Page, FName NameBox, int32 Group, float RightEdge);
    UFUNCTION(BlueprintCallable, Category = "Map")
    void ZoomBy(float Factor);
    UFUNCTION(BlueprintPure, Category = "Map")
    FRailMapRoute FindRoute(const FString& Start, const FString& Destination) const;
    UFUNCTION(BlueprintPure, Category = "Map")
    FRailMapRoute GetRoute() const
    {
        return Route;
    }
    UFUNCTION(BlueprintPure, Category = "Map")
    FVector2D GetViewOffset() const;
    UFUNCTION(BlueprintPure, Category = "Map")
    float GetViewScale() const;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    virtual void SynchronizeProperties() override;
    void SelectStation(int32 Index);
    bool CanSelect() const
    {
        return bAllowSelection;
    }
    int32 GetStartIndex() const;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    FString StartName, DestinationName;
    FRailMapRoute Route;
    bool bAllowSelection = true;
    TSharedPtr<SRailRouteMap> Map;
};
