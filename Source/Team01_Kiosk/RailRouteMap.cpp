#include "RailRouteMap.h"
#include "Algo/Reverse.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/BorderSlot.h"
#include "Components/TextBlock.h"
#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Input/Events.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SLeafWidget.h"

namespace
{
const FVector2D MapMin(60, 320), MapMax(1510, 1240);
int32 StationIndex(const URailMapNetwork* N, const FString& Name)
{
    return N ? N->Stations.IndexOfByPredicate([&](const FRailMapStation& S) { return S.Name == Name; })
             : INDEX_NONE;
}
} // namespace

class SRailRouteMap : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SRailRouteMap) {}
        SLATE_ARGUMENT(URailRouteMap*, Owner)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        Owner = Args._Owner;
        SetClipping(EWidgetClipping::ClipToBoundsAlways);
        SetCanTick(false);
    }
    FVector2D Offset = FVector2D::ZeroVector;
    float Scale = 1;
    bool bFitPending = true;
    virtual FVector2D ComputeDesiredSize(float) const override
    {
        return FVector2D(1200, 550);
    }
    void Fit()
    {
        bFitPending = true;
        Invalidate(EInvalidateWidgetReason::Paint);
    }
    void UpdateBrush()
    {
        Artwork.SetResourceObject(Owner.IsValid() && Owner->Network ? Owner->Network->Artwork.Get()
                                                                    : nullptr);
        Artwork.ImageSize = FVector2D(1600, 1600);
        Artwork.DrawAs = ESlateBrushDrawType::Image;
        Invalidate(EInvalidateWidgetReason::Paint);
    }
    void ClampView(FVector2D Size)
    {
        const FVector2D Extent = (MapMax - MapMin) * Scale;
        for (int32 Axis = 0; Axis < 2; ++Axis)
            Offset[Axis] = Extent[Axis] < Size[Axis]
                               ? (Size[Axis] - Extent[Axis]) * .5 - MapMin[Axis] * Scale
                               : FMath::Clamp(Offset[Axis], Size[Axis] - MapMax[Axis] * Scale - 24.,
                                              24. - MapMin[Axis] * Scale);
    }
    void ApplyFit(FVector2D Size)
    {
        FVector2D Low = MapMin, High = MapMax;
        const auto R = Owner->GetRoute();
        if (R.Stations.Num() > 1)
        {
            Low = FVector2D(DBL_MAX, DBL_MAX);
            High = FVector2D(-DBL_MAX, -DBL_MAX);
            auto Include = [&](FVector2D P) {
                Low.X = FMath::Min(Low.X, P.X);
                Low.Y = FMath::Min(Low.Y, P.Y);
                High.X = FMath::Max(High.X, P.X);
                High.Y = FMath::Max(High.Y, P.Y);
            };
            for (int32 I : R.Stations)
                Include(Owner->Network->Stations[I].Position);
            for (int32 I : R.Connections)
                for (FVector2D P : Owner->Network->Connections[I].Bends)
                    Include(P);
        }
        FVector2D Extent = High - Low;
        Scale = FMath::Clamp(FMath::Min((Size.X - 290.) / FMath::Max(Extent.X, 260.),
                                        (Size.Y - 100.) / FMath::Max(Extent.Y, 120.)),
                             .15, 3.5);
        Offset = Size * .5 - (Low + High) * .5 * Scale - FVector2D(45, 0);
        // Route labels need space outside the artwork bounds; do not clamp the initial route fit.
        bFitPending = false;
    }
    void Zoom(float Factor, FVector2D Anchor, FVector2D Size)
    {
        if (bFitPending)
            ApplyFit(Size);
        const float Minimum = FMath::Min(Size.X / (MapMax.X - MapMin.X), Size.Y / (MapMax.Y - MapMin.Y)) * .8;
        const FVector2D Point = (Anchor - Offset) / Scale;
        Scale = FMath::Clamp(Scale * Factor, Minimum, 4.f);
        Offset = Anchor - Point * Scale;
        ClampView(Size);
        Invalidate(EInvalidateWidgetReason::Paint);
    }
    virtual int32 OnPaint(const FPaintArgs&, const FGeometry& G, const FSlateRect&,
                          FSlateWindowElementList& Out, int32 Layer, const FWidgetStyle&, bool) const override
    {
        if (!Owner.IsValid() || !Owner->Network)
            return Layer;
        auto Self = const_cast<SRailRouteMap*>(this);
        if (bFitPending || !G.GetLocalSize().Equals(LastSize, .1f))
        {
            Self->ApplyFit(G.GetLocalSize());
            LastSize = G.GetLocalSize();
        }
        const auto* N = Owner->Network.Get();
        const auto Route = Owner->GetRoute();
        const bool HasRoute = Route.Stations.Num() > 1;
        const auto* White = FCoreStyle::Get().GetBrush("WhiteBrush");
        auto Box = [&](FVector2D P, FVector2D Size, const FSlateBrush* B, FLinearColor C, int32 L) {
            FSlateDrawElement::MakeBox(Out, L, G.ToPaintGeometry(Size, FSlateLayoutTransform(P)), B,
                                       ESlateDrawEffect::None, C);
        };
        Box(FVector2D::ZeroVector, G.GetLocalSize(), White, FLinearColor::White, Layer);
        Box(Offset, FVector2D(1600, 1600) * Scale, &Artwork, FLinearColor(1, 1, 1, HasRoute ? .15f : 1.f),
            Layer + 1);
        auto Line = [&](const TArray<FVector2D>& Points, FLinearColor C, float Width, int32 L) {
            TArray<FVector2f> P;
            for (auto V : Points)
                P.Add(FVector2f(V * Scale + Offset));
            FSlateDrawElement::MakeLines(Out, L, G.ToPaintGeometry(), P, ESlateDrawEffect::None, C, true,
                                         Width);
        };
        for (int32 I : Route.Connections)
        {
            const auto& E = N->Connections[I];
            TArray<FVector2D> P;
            P.Add(N->Stations[E.From].Position);
            P.Append(E.Bends);
            P.Add(N->Stations[E.To].Position);
            Line(P, FLinearColor::White, 9, Layer + 2);
            Line(P, E.Colour, 5, Layer + 3);
        }
        const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 11);
        const auto Measure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
        FSlateRoundedBoxBrush Dot(FLinearColor::White, 20.f), Label(FLinearColor::White, 7.f);
        TArray<int32> Markers = Route.Stations;
        if (Markers.IsEmpty() && Owner->GetStartIndex() != INDEX_NONE)
            Markers.Add(Owner->GetStartIndex());
        TArray<FVector2D> Points;
        for (int32 K = 0; K < Markers.Num(); ++K)
        {
            const FVector2D P = N->Stations[Markers[K]].Position * Scale + Offset;
            Points.Add(P);
            const bool Endpoint = K == 0 || K == Markers.Num() - 1;
            const FLinearColor Fill = Endpoint ? FLinearColor(1, .72f, .03f) : FLinearColor(.22f, .23f, .26f);
            Box(P - FVector2D(8, 8), FVector2D(16, 16), &Dot, FLinearColor::White, Layer + 4);
            Box(P - FVector2D(5.5, 5.5), FVector2D(11, 11), &Dot, Fill, Layer + 5);
        }

        // Reserve space for endpoints and transfers before placing intermediate names.
        // Dense routes reveal more names as the user zooms, instead of pushing labels off screen.
        TArray<int32> LabelOrder;
        if (!Markers.IsEmpty())
        {
            LabelOrder.Add(0);
            LabelOrder.AddUnique(Markers.Num() - 1);
        }
        for (int32 K = 1; K < Route.Connections.Num(); ++K)
            if (N->Connections[Route.Connections[K - 1]].Line != N->Connections[Route.Connections[K]].Line)
                LabelOrder.AddUnique(K);
        for (int32 K = 0; K < Markers.Num(); ++K)
            LabelOrder.AddUnique(K);

        TArray<FSlateRect> Labels;
        const FVector2D ViewSize = G.GetLocalSize();
        for (int32 K : LabelOrder)
        {
            const FVector2D P = Points[K];
            if (P.X < 0 || P.Y < 0 || P.X > ViewSize.X || P.Y > ViewSize.Y)
                continue;
            const auto& Station = N->Stations[Markers[K]];
            const bool Endpoint = K == 0 || K == Markers.Num() - 1;
            const FVector2D TextSize = Measure->Measure(Station.Name, Font);
            const FVector2D Size = TextSize + FVector2D(14, 6);
            FVector2D At = FVector2D::ZeroVector;
            bool Placed = false;
            for (int32 Step = 0; Step <= 6 && !Placed; ++Step)
            {
                const double Shift = Step == 0 ? 0. : ((Step + 1) / 2) * (Size.Y + 3) * (Step % 2 ? -1 : 1);
                for (int32 Side = 0; Side < 2 && !Placed; ++Side)
                {
                    const FVector2D Candidate(P.X + (Side == 0 ? 13. : -Size.X - 13.), P.Y - Size.Y * .5 + Shift);
                    const FSlateRect R(Candidate.X - 2, Candidate.Y - 2, Candidate.X + Size.X + 2, Candidate.Y + Size.Y + 2);
                    if (R.Left < 6 || R.Top < 6 || R.Right > ViewSize.X - 6 || R.Bottom > ViewSize.Y - 6)
                        continue;
                    if (Labels.ContainsByPredicate([&](const FSlateRect& Other) { return FSlateRect::DoRectanglesIntersect(R, Other); }))
                        continue;
                    if (Points.ContainsByPredicate([&](FVector2D Other) { return Other.X > R.Left - 9 && Other.X < R.Right + 9 && Other.Y > R.Top - 9 && Other.Y < R.Bottom + 9; }))
                        continue;
                    At = Candidate;
                    Placed = true;
                }
            }
            if (!Placed)
            {
                if (!Endpoint)
                    continue;
                At = FVector2D(FMath::Clamp(P.X + 13., 8., ViewSize.X - Size.X - 8.),
                               FMath::Clamp(P.Y - Size.Y * .5, 8., ViewSize.Y - Size.Y - 8.));
            }
            Labels.Add(FSlateRect(At.X - 2, At.Y - 2, At.X + Size.X + 2, At.Y + Size.Y + 2));
            const FLinearColor Fill = Endpoint ? FLinearColor(1, .72f, .03f) : FLinearColor(.22f, .23f, .26f);
            if (FMath::Abs(At.Y + Size.Y * .5 - P.Y) > 3)
            {
                const FVector2D Edge(FMath::Clamp(P.X, At.X, At.X + Size.X), FMath::Clamp(P.Y, At.Y, At.Y + Size.Y));
                const FVector2D Start = P + (Edge - P).GetSafeNormal() * 8;
                TArray<FVector2f> Leader = {FVector2f(Start), FVector2f(Edge)};
                FSlateDrawElement::MakeLines(Out, Layer + 5, G.ToPaintGeometry(), Leader,
                                            ESlateDrawEffect::None, FLinearColor(.55f, .55f, .55f, .7f), true, 1.f);
            }
            Box(At, Size, &Label, Fill, Layer + 6);
            FSlateDrawElement::MakeText(Out, Layer + 7,
                G.ToPaintGeometry(TextSize, FSlateLayoutTransform(At + FVector2D(7, 3))),
                Station.Name, Font, ESlateDrawEffect::None,
                Endpoint ? FLinearColor(.1f, .1f, .1f) : FLinearColor::White);
        }
        return Layer + 8;
    }
    virtual FReply OnMouseButtonDown(const FGeometry& G, const FPointerEvent& E) override
    {
        if (E.GetEffectingButton() == EKeys::RightMouseButton)
        {
            bDragging = true;
            LastPointer = G.AbsoluteToLocal(E.GetScreenSpacePosition());
            return FReply::Handled().CaptureMouse(SharedThis(this));
        }
        if (E.GetEffectingButton() == EKeys::LeftMouseButton && Owner.IsValid() && Owner->CanSelect())
        {
            const FVector2D P = G.AbsoluteToLocal(E.GetScreenSpacePosition());
            int32 Closest = INDEX_NONE;
            double Distance = 18. * 18.;
            if (Owner->Network)
                for (int32 I = 0; I < Owner->Network->Stations.Num(); ++I)
                {
                    const double D =
                        FVector2D::DistSquared(P, Owner->Network->Stations[I].Position * Scale + Offset);
                    if (D < Distance)
                    {
                        Distance = D;
                        Closest = I;
                    }
                }
            if (Closest != INDEX_NONE)
                Owner->SelectStation(Closest);
            return FReply::Handled();
        }
        return FReply::Unhandled();
    }
    virtual FReply OnMouseMove(const FGeometry& G, const FPointerEvent& E) override
    {
        if (bDragging)
        {
            if (!E.IsMouseButtonDown(EKeys::RightMouseButton))
            {
                bDragging = false;
                return FReply::Handled().ReleaseMouseCapture();
            }
            const FVector2D P = G.AbsoluteToLocal(E.GetScreenSpacePosition());
            Offset += P - LastPointer;
            LastPointer = P;
            ClampView(G.GetLocalSize());
            Invalidate(EInvalidateWidgetReason::Paint);
            return FReply::Handled();
        }
        return FReply::Unhandled();
    }
    virtual FReply OnMouseButtonUp(const FGeometry&, const FPointerEvent& E) override
    {
        if (E.GetEffectingButton() == EKeys::RightMouseButton)
        {
            bDragging = false;
            return FReply::Handled().ReleaseMouseCapture();
        }
        return FReply::Unhandled();
    }
    virtual void OnMouseCaptureLost(const FCaptureLostEvent&) override
    {
        bDragging = false;
    }
    virtual FReply OnMouseWheel(const FGeometry& G, const FPointerEvent& E) override
    {
        Zoom(FMath::Pow(1.2f, E.GetWheelDelta()), G.AbsoluteToLocal(E.GetScreenSpacePosition()),
             G.GetLocalSize());
        return FReply::Handled();
    }
    virtual FCursorReply OnCursorQuery(const FGeometry&, const FPointerEvent&) const override
    {
        return FCursorReply::Cursor(bDragging ? EMouseCursor::GrabHandClosed : EMouseCursor::GrabHand);
    }

private:
    TWeakObjectPtr<URailRouteMap> Owner;
    FSlateBrush Artwork;
    FVector2D LastPointer;
    mutable FVector2D LastSize;
    bool bDragging = false;
};

FRailMapRoute URailRouteMap::FindRoute(const FString& Start, const FString& Destination) const
{
    FRailMapRoute Result;
    const auto* N = Network.Get();
    const int32 A = StationIndex(N, Start), B = StationIndex(N, Destination);
    if (!N || A == INDEX_NONE || B == INDEX_NONE || A == B)
        return Result;
    TArray<FName> Lines;
    Lines.Add(NAME_None);
    for (const auto& E : N->Connections)
        Lines.AddUnique(E.Line);
    const int32 Width = Lines.Num(), Count = N->Stations.Num() * Width;
    TArray<float> Cost;
    Cost.Init(FLT_MAX, Count);
    TArray<int32> Prev, Via;
    Prev.Init(INDEX_NONE, Count);
    Via.Init(INDEX_NONE, Count);
    TArray<bool> Done;
    Done.Init(false, Count);
    Cost[A * Width] = 0;
    int32 End = INDEX_NONE;
    for (int32 Iter = 0; Iter < Count; ++Iter)
    {
        int32 Current = INDEX_NONE;
        float Best = FLT_MAX;
        for (int32 I = 0; I < Count; ++I)
            if (!Done[I] && Cost[I] < Best)
            {
                Best = Cost[I];
                Current = I;
            }
        if (Current == INDEX_NONE)
            break;
        const int32 Station = Current / Width, Incoming = Current % Width;
        if (Station == B)
        {
            End = Current;
            break;
        }
        Done[Current] = true;
        for (int32 I = 0; I < N->Connections.Num(); ++I)
        {
            const auto& E = N->Connections[I];
            if (!N->Stations.IsValidIndex(E.From) || !N->Stations.IsValidIndex(E.To) || E.Minutes <= 0)
                continue;
            const int32 Next = E.From == Station ? E.To : (E.To == Station ? E.From : INDEX_NONE);
            if (Next == INDEX_NONE)
                continue;
            const int32 Line = Lines.IndexOfByKey(E.Line), State = Next * Width + Line;
            const float Candidate =
                Best + E.Minutes +
                (Incoming != 0 && Incoming != Line ? FMath::Max(0.f, N->TransferMinutes) : 0.f);
            if (Candidate < Cost[State])
            {
                Cost[State] = Candidate;
                Prev[State] = Current;
                Via[State] = I;
            }
        }
    }
    if (End == INDEX_NONE)
        return Result;
    Result.EstimatedMinutes = Cost[End];
    for (int32 S = End; S != INDEX_NONE; S = Prev[S])
    {
        Result.Stations.Add(S / Width);
        if (Via[S] != INDEX_NONE)
            Result.Connections.Add(Via[S]);
    }
    Algo::Reverse(Result.Stations);
    Algo::Reverse(Result.Connections);
    FName Previous = NAME_None;
    for (int32 I : Result.Connections)
    {
        const FName Line = N->Connections[I].Line;
        if (!Previous.IsNone() && Previous != Line)
            ++Result.Transfers;
        Previous = Line;
    }
    return Result;
}
void URailRouteMap::SetJourney(const FString& Start, const FString& Destination, bool AllowSelection)
{
    bAllowSelection = AllowSelection;
    if (StartName == Start && DestinationName == Destination)
        return;
    StartName = Start;
    DestinationName = Destination;
    Route = FindRoute(Start, Destination);
    FitRoute();
}
void URailRouteMap::FitRoute()
{
    if (Map.IsValid())
        Map->Fit();
}
void URailRouteMap::ZoomBy(float Factor)
{
    if (Map.IsValid() && Factor > 0)
        Map->Zoom(Factor, Map->GetCachedGeometry().GetLocalSize() * .5,
                  Map->GetCachedGeometry().GetLocalSize());
}
int32 URailRouteMap::GetStartIndex() const
{
    return StationIndex(Network, StartName);
}
FVector2D URailRouteMap::GetViewOffset() const
{
    return Map.IsValid() ? Map->Offset : FVector2D::ZeroVector;
}
float URailRouteMap::GetViewScale() const
{
    return Map.IsValid() ? Map->Scale : 1.f;
}
void URailRouteMap::SelectStation(int32 Index)
{
    if (bAllowSelection && Network && Network->Stations.IsValidIndex(Index))
        OnStationSelected.Broadcast(Network->Stations[Index].Name);
}
TSharedRef<SWidget> URailRouteMap::RebuildWidget()
{
    Map = SNew(SRailRouteMap).Owner(this);
    Map->UpdateBrush();
    return Map.ToSharedRef();
}
void URailRouteMap::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    if (Map.IsValid())
    {
        Map->SetClipping(EWidgetClipping::ClipToBoundsAlways);
        Map->UpdateBrush();
    }
}
void URailRouteMap::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    Map.Reset();
}

void URailRouteMap::CompactStationRow(UUserWidget* Page, FName NameBox, int32 Group, float RightEdge)
{
    if (!Page || !Page->WidgetTree)
        return;
    auto* Text = Page->WidgetTree->FindWidget(NameBox);
    auto* Slot = Text ? Cast<UCanvasPanelSlot>(Text->Slot) : nullptr;
    if (!Slot)
        return;
    auto Absolute = [](UWidget* W) {
        FVector2D P(0, 0);
        for (; W; W = W->GetParent())
            if (auto* S = Cast<UCanvasPanelSlot>(W->Slot))
                P += S->GetPosition();
        return P;
    };
    // Reserve room for three line codes without changing the station-name font size.
    constexpr float BadgeScale = .75f;
    float NextBadge = Group == 0 ? 216.f : 786.f;
    float Right = Group == 0 ? 204.f : 774.f;
    for (int32 I = 0; I < 3; ++I)
    {
        auto* Badge = Page->WidgetTree->FindWidget(FName(*FString::Printf(TEXT("Badges_%d_%d"), Group, I)));
        if (Badge && Badge->GetVisibility() != ESlateVisibility::Collapsed &&
            Badge->GetVisibility() != ESlateVisibility::Hidden)
            if (auto* B = Cast<UCanvasPanelSlot>(Badge->Slot))
            {
                const FVector2D Parent = Absolute(Badge->GetParent());
                B->SetPosition(FVector2D(NextBadge, 179) - Parent);
                Badge->SetRenderTransformPivot(FVector2D(0, .5));
                Badge->SetRenderScale(FVector2D(BadgeScale, BadgeScale));
                Right = NextBadge + B->GetSize().X * BadgeScale;
                NextBadge = Right + 6;
            }
    }
    FString LabelName = NameBox.ToString();
    LabelName.RemoveFromEnd(TEXT("_Box"));
    if (auto* Fit = Cast<UScaleBox>(Page->WidgetTree->FindWidget(FName(*(TEXT("Fit_") + LabelName)))))
    {
        Fit->SetStretch(EStretch::None);
        Fit->SetUserSpecifiedScale(1.f);
        if (auto* Child = Fit->GetChildAt(0))
            if (auto* ChildSlot = Cast<UScaleBoxSlot>(Child->Slot))
            {
                ChildSlot->SetHorizontalAlignment(HAlign_Left);
                ChildSlot->SetVerticalAlignment(VAlign_Center);
            }
    }
    if (auto* Label = Cast<UTextBlock>(Page->WidgetTree->FindWidget(FName(*LabelName))))
    {
        // Station names stay in English in every language, so keep their typeface identical too.
        Label->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 20));
        Label->SetJustification(ETextJustify::Left);
        Label->SetAutoWrapText(false);
    }
    const FVector2D Parent = Absolute(Text->GetParent());
    Slot->SetPosition(FVector2D(Right + 12, 174) - Parent);
    Slot->SetSize(FVector2D(FMath::Max(80.f, RightEdge - Right - 12), 44));

    // Captions retain the language's font, but use one size and wrap instead of shrinking.
    for (int32 I = 0; I < 2; ++I)
    {
        const FString Caption = I == 0 ? TEXT("Txt_Start_Location") : TEXT("Txt_Destination");
        auto* Box = Page->WidgetTree->FindWidget(FName(*(Caption + TEXT("_Box"))));
        const float Width = I == 0 ? 138.f : 108.f;
        if (auto* CaptionSlot = Box ? Cast<UCanvasPanelSlot>(Box->Slot) : nullptr)
        {
            CaptionSlot->SetPosition(FVector2D(I == 0 ? 6 : 606, 169) - Absolute(Box->GetParent()));
            CaptionSlot->SetSize(FVector2D(Width, 54));
        }
        if (auto* Fit = Cast<UScaleBox>(Page->WidgetTree->FindWidget(FName(*(TEXT("Fit_") + Caption)))))
        {
            Fit->SetStretch(EStretch::None);
            Fit->SetUserSpecifiedScale(1.f);
            if (auto* BorderSlot = Cast<UBorderSlot>(Fit->Slot))
            {
                BorderSlot->SetHorizontalAlignment(HAlign_Fill);
                BorderSlot->SetVerticalAlignment(VAlign_Center);
            }
            if (auto* Child = Fit->GetChildAt(0))
                if (auto* ChildSlot = Cast<UScaleBoxSlot>(Child->Slot))
                {
                    ChildSlot->SetHorizontalAlignment(HAlign_Fill);
                    ChildSlot->SetVerticalAlignment(VAlign_Center);
                }
        }
        if (auto* Label = Cast<UTextBlock>(Page->WidgetTree->FindWidget(FName(*Caption))))
        {
            auto Font = Label->GetFont();
            Font.Size = 12;
            Label->SetFont(Font);
            Label->SetJustification(ETextJustify::Center);
            Label->SetAutoWrapText(true);
            Label->SetWrapTextAt(Width);
        }
    }
}
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRailMapRouteTest, "Kiosk.RailMap.TransferAwareRouting",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FRailMapRouteTest::RunTest(const FString&)
{
    auto* W = NewObject<URailRouteMap>();
    auto* N = NewObject<URailMapNetwork>();
    W->Network = N;
    for (const TCHAR* Name : {TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("Isolated")})
    {
        FRailMapStation S;
        S.Name = Name;
        N->Stations.Add(S);
    }
    auto Add = [&](int A, int B, FName L, float T) {
        FRailMapConnection E;
        E.From = A;
        E.To = B;
        E.Line = L;
        E.Minutes = T;
        N->Connections.Add(E);
    };
    Add(0, 1, "Red", 1);
    Add(1, 2, "Red", 1);
    Add(0, 2, "Green", 2.5);
    Add(2, 3, "Green", 1);
    auto R = W->FindRoute("A", "D");
    TestEqual(TEXT("Transfer penalty favours staying on green"), R.Stations, TArray<int32>({0, 2, 3}));
    TestEqual(TEXT("No transfer"), R.Transfers, 0);
    TestEqual(TEXT("Weighted time"), R.EstimatedMinutes, 3.5f);
    R = W->FindRoute("B", "D");
    TestEqual(TEXT("Unavoidable transfer"), R.Transfers, 1);
    TestEqual(TEXT("Includes transfer time"), R.EstimatedMinutes, 6.f);
    TestTrue(TEXT("Same station has no journey"), W->FindRoute("A", "A").Stations.IsEmpty());
    TestTrue(TEXT("Unknown station"), W->FindRoute("missing", "D").Stations.IsEmpty());
    TestTrue(TEXT("Disconnected station"), W->FindRoute("A", "Isolated").Stations.IsEmpty());
    TestEqual(TEXT("Reverse route"), W->FindRoute("D", "A").Stations, TArray<int32>({3, 2, 0}));
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRailMapPanTest, "Kiosk.RailMap.DiagonalPointerMovement",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FRailMapPanTest::RunTest(const FString&)
{
    auto* W = NewObject<URailRouteMap>();
    W->Network = NewObject<URailMapNetwork>();
    auto M = SNew(SRailRouteMap).Owner(W);
    M->Scale = 2.5;
    M->Offset = FVector2D(-1000, -1500);
    M->bFitPending = false;
    const FGeometry G = FGeometry::MakeRoot(FVector2D(1200, 550), FSlateLayoutTransform(2.f));
    TSet<FKey> Held;
    Held.Add(EKeys::RightMouseButton);
    auto Event = [&](FVector2D P, FVector2D Last, FKey Effect) {
        return FPointerEvent(0, P, Last, Held, Effect, 0, FModifierKeysState());
    };
    const FVector2D Start(500, 600), End(530, 642), Before = M->Offset;
    TestTrue(TEXT("Right press is handled"),
             M->OnMouseButtonDown(G, Event(Start, Start, EKeys::RightMouseButton)).IsEventHandled());
    M->OnMouseMove(G, Event(End, Start, EKeys::Invalid));
    TestTrue(TEXT("Both axes use geometry-scaled pointer delta"),
             M->Offset.Equals(Before + FVector2D(15, 21), .001));
    M->OnMouseButtonUp(G, Event(End, End, EKeys::RightMouseButton));
    const FVector2D After = M->Offset;
    M->OnMouseMove(G, Event(End + FVector2D(50, 50), End, EKeys::Invalid));
    TestTrue(TEXT("Release stops dragging"), M->Offset.Equals(After));
    const FVector2D Anchor(400, 200), Point = (Anchor - M->Offset) / M->Scale;
    M->Zoom(1.2f, Anchor, G.GetLocalSize());
    TestTrue(TEXT("Wheel zoom keeps pointer anchor"), (Point * M->Scale + M->Offset).Equals(Anchor, .01));
    return true;
}
#endif
