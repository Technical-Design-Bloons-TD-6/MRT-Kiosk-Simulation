# Team01 Kiosk Prototype (Unreal)

Public transit ticket kiosk prototype for Technical Design Methods, Part III. Read this before you touch the project.

## 1. Versions (everyone must match)

| Tool | Version |
|---|---|
| Unreal Engine | 5.8.4 |
| Visual Studio | Not required to edit or run the Blueprint kiosk |
| Git | Git for Windows (latest), includes Git LFS |
| Project file | `Team01_Kiosk.uproject` |

Do not open the project in a different engine version. Opening it in a newer one upgrades the files, and you cannot go back.

The kiosk is implemented in editable Blueprint graphs and UMG widgets, including the route map, camera, physical interactions, payment animations and collection. The project does not require a custom C++ runtime module. Start with `Content/UI/Kiosk/Blueprint/` for the shared behavior and `Content/UI/Kiosk/Interaction/Pages/` for screen layouts.

## 2. First-time setup

1. Install the versions above. Run `git lfs install` once in a terminal.
2. In GitHub Desktop: File, Clone repository, URL tab, then enter `https://github.com/Technical-Design-Bloons-TD-6/MRT-Kiosk-Simulation`.
3. Pick an **empty folder with a short path**, for example `C:\Dev\MRT-Kiosk-Simulation`. Long paths cause Unreal build errors.
4. Double-click `Team01_Kiosk.uproject`. Unreal may prepare shaders on first open. No project DLL rebuild is needed.
5. Check it worked: Help, About Unreal Editor should say 5.8.4. The current editor startup map is `Content/Maps/L_Island`.

## 3. Basics

- **Pull** when you start working. **Push** before you stop.
- Commit small, with a clear message, like `Day pass: calendar screen`.
- Close the editor before you commit, so files are not changing underneath you.
- If a push fails or something looks wrong, tell the group chat before you try to fix it.

## 4. Binary files (important)

Blueprints, widgets and levels (`.uasset`, `.umap`) are binary. Git can't merge them. If two people edit the same file, the last push wins and the other person's work is lost.

So: **never edit a file someone else is working on.** How we split the work and handle shared files is agreed in the group chat.

Also:
- Don't rename, move or delete shared assets without telling the group. It breaks links.
- Keep fares, station names and text in a CSV under `Content/Data`, and import it as a Data Table. CSV is plain text, so Git can merge it.
- Don't commit builds, packaged output or large source files (`.blend`, `.psd`, screen recordings). Keep those on the shared drive.

## 5. Folders and names

```
Content/
  UI/          Shared widgets, Screens
  Environment/ Kiosk, Station, Props, Materials, Lighting
  Data/        CSV files and Data Tables
  Maps/        L_Main and sub-levels
```

| Prefix | Use |
|---|---|
| `WBP_` | Widget Blueprint (screens) |
| `BP_` | Blueprint |
| `M_` / `MI_` | Material / Material Instance |
| `SM_` | Static Mesh |
| `T_` | Texture |
| `L_` | Level |
| `DT_` | Data Table |

## 6. Submission checklist

- The submitted source folder holds only `Config`, `Content`, `Plugins` (if present), `Source` and the `.uproject`. Delete all other folders first.
- Test the cleaned folder on a machine that has not opened the project before.
- Naming format: `ID_AssessmentTitle.zip`, containing one folder `ID_AssessmentTitle`, with files named `ID_Filename.extension`.
- Video: 1920x1080, MP4, 5 minutes at most.
- Build is optional. If included: windowed, 1920x1080, Windows EXE, in a `Build` folder.

## 7. Kiosk editing guide

### Start here: the screens used in play

In the Content Browser, open **Content > UI > Kiosk > Interaction > Pages**. The current screens are the `WBP_Runtime_...` Widget Blueprints in this folder. Open a page's **Designer** to change its layout and appearance.

`Content/UI/Kiosk/Screens/` also contains imported screens and review versions with similar names. For changes to the current kiosk, start with `Interaction/Pages/`. Editing an older `WBP_...` screen does not update its separate `WBP_Runtime_...` copy.

All page names below have the prefix `WBP_Runtime_`:

| Screen name | What to change here |
|---|---|
| `LanguageSelection`, `TicketingOptions` | Language selection and the main service choices. |
| `TransitCardBalance` | Card-reader illustration, centered card-placement instruction, balance label and amount. |
| `RechargeCheckout` | Top-up amount choices, card-reader reminder and payment choices. |
| `Calendar` | Day-pass date selection, instructions, activation and expiry labels, and confirmation layout. |
| `DayPassCheckout` | Day-pass details, dates, quantity, total fare and payment choices. |
| `StartLocationSearch`, `StartLocationSelected`, `DestinationSearch`, `SearchSuggestions` | Station search and selection layouts. |
| `JourneyMap`, `DestinationMap`, `JourneySelected` | Map-screen headers, start/destination rows and confirmation layout. See the map logic notes below for zoom and clickable stations. |
| `SingleUseCheckout` | Start/destination details, one-way/round-trip choices, route summary, quantity, fare and payment choices. |
| `CardPayment`, `QRPayment` | Payment instructions, accepted-payment logos, fare and cancel button layout. |
| `CashPayment`, `CashPaymentProgress`, `CashPaymentComplete` | Cash instructions, accepted notes, inserted cash and remaining amount. These are separate pages; check each state. |
| `PaymentSuccessful`, `PaymentFailed` | Result messages, receipt, retry and return-home controls. |
| `RequestAssistance` | Assistance screen layout. |

The payment illustrations use editable **Designer > Animations** tracks: `PaymentExample_CardTapThenInsert` plays tap followed by insertion in one shared illustration area, `PaymentExample_InsertCash` feeds a vertically oriented note short edge first into the slot on the three cash pages, and the card/cash examples loop while their pages are active. QR payment now displays a scannable code and uses the physical phone sequence described below. The page function `UpdatePaymentExample` and the main widget's `RefreshPaymentExamples` start/stop playback, so returning to a page starts its example again. Keep `Canvas_CashNoteClip` in place when editing the cash illustration; it hides the note as it enters the slot. The static `Img_image_31` overlay is hidden because its artwork contains a second note. Payment instructions, summary columns and cancel controls share a centered layout.

The shared `PaymentProcessingView` in `WBP_KioskInteractive` displays a centered message and spinner for all three payment methods, inside a rounded white panel with a native material shadow on the grey page background. It covers the payment content after cash Pay, card contact, or Pay on the QR phone, then closes when the success or failure result is ready. Initial QR scanning retains its scan instructions. `BP_KioskScreenLogic.RefreshPaymentProcessingView` controls visibility and the four translations; a native UMG vertical layout centers the message and spinner together. The processing overlay and its panel are anchored to the available display area below the header, so wider displays keep the group centered without resizing the header or page text. `BeginCardProcessing` retains its 2.4-second processing period before `CompleteCardProcessing` runs the approval flow, and leaving the card page cancels the timer.

Cash payment accepts notes before Pay is pressed. `CashCents` continues increasing and Remaining is clamped to zero. Pay stays disabled until enough money is inserted, then turns green; `ConfirmCashPay` starts a 1.4-second processing animation before `CompleteCashSettlement` attempts payment. There is no automatic payment timer after the final note. Leaving cash payment clears the processing timer through `RefreshCashSettlement`. The Change summary row is hidden; the existing hardware change/refund accounting remains in place.

The first payment attempt in each play session fails. `BP_KioskSession.AuthorizePaymentAttempt` shares this state across all kiosks and payment methods, so returning home or switching kiosks cannot bypass it. The failure screen directs the player to the physical green help button. `BP_StaffNPC.CompleteKioskAssistance`, called when the staff conversation finishes, resolves the issue and restores the kiosk view. Retrying afterward succeeds. Failed cash is refunded and failed top-ups do not increase the card balance.

QR payment displays a valid demo QR code on the kiosk. `WBP_KioskQRPhone` supplies the physical phone's camera preview, payment form, animated Pay tap and result popup. A Scene Capture camera attached to the phone renders the kiosk while scanning. `DeviceQR` starts the timed stages; the phone result appears 1.6 seconds before the kiosk result. Cancelling clears every pending stage. The QR payload is a local demo URI and does not initiate a real payment.

### Where the other kiosk parts live

Paths in this table are relative to the project root. In Unreal, `Content/` is shown as `/Game/` in asset references.

| Location | What it controls |
|---|---|
| `Content/UI/Kiosk/Interaction/WBP_KioskInteractive` | Main kiosk widget: screen switching, transaction state, translations, runtime text updates, button actions and map interaction. Open **Graph** for behavior; the page widgets supply the screen layouts. |
| `Content/UI/Kiosk/Interaction/BP_KioskTerminal` | Kiosk actor and the connection between the player, kiosk screen and simulated hardware. |
| `Content/UI/Kiosk/Interaction/WBP_KioskInteractPrompt` | Prompt shown when interacting with the kiosk. |
| `Content/UI/Kiosk/Interaction/WBP_KioskHardware` | Simulated reader, cash-slot and output controls. |
| `Content/UI/Kiosk/Interaction/BP_KioskInventory` and `WBP_PlayerInventory` | Legacy record storage for collected documents. The inventory HUD, equip keys and held-item menu are no longer used; physical actions are automatic. |
| `Content/UI/Kiosk/Interaction/BP_KioskDocument` | Ticket/receipt document actor. |
| `Content/UI/Kiosk/Interaction/BP_TicketValidator` | Ticket-validation actor and behavior. |
| `Content/UI/Kiosk/Interaction/Props/` | Materials for cards, notes, tickets, receipts and device housings. |
| `Content/UI/Kiosk/StationAssets/WBP_StationBadge` | Shared MRT/LRT pictogram and station line-code badge. Changes affect its instances across the kiosk. Placement and instance size are set by the containing page or runtime map. |
| `Content/UI/Kiosk/Textures/` | Imported UI images. `FigmaDetails/` contains the recovered Figma detail textures; `Ticketing/` contains ticketing illustrations. Select an Image widget and inspect its **Brush > Image** to find the texture it actually uses. |
| `ArtSource/Kiosk/FigmaDetails/` | Source artwork for the imported Figma detail textures. Changing a source image requires reimporting its Unreal texture. |
| `Content/UI/Kiosk/Fonts/` | Font assets, including the Tamil fonts. Font selection can also be applied by the main widget's translation functions. |
| `Content/Data/Kiosk/` | Demo fares/balance, station source data and translation source text; see below. |
| `Content/Maps/L_Island` | Current editor startup level. Select the kiosk actor in the level to inspect its instance settings. `L_Main` also exists; check which level you are editing. |

### Changing text size, spacing and alignment

Many runtime text widgets use this hierarchy in the Designer:

```text
Txt_..._Box (Border, positioned in a Canvas Panel)
  Fit_Txt_... (Scale Box)
    Txt_... (Text Block)
```

- **Position and available space:** select `Txt_..._Box` and edit its Canvas Slot position, size and anchors. Give translated text enough height to wrap without clipping.
- **Text size:** check both the Text Block's font size and the Scale Box's stretch mode / user-specified scale. A small container can shrink text when `Scale To Fit` is used.
- **Alignment:** check Text Block justification, the parent slots' horizontal/vertical alignment and Border padding. Centering text inside a narrow or misplaced box will not center it within the visible panel.
- **Runtime differences:** `WBP_KioskInteractive` applies translations and updates dynamic values during play. A Designer-only font or text change may be overwritten by those functions.
- **Shared controls:** cancel buttons, payment choices and assistance controls appear on multiple pages. Update and verify each relevant page, rather than assuming one page changes them all.

For example, the balance screen uses `Txt_Place_your_card_on_the_reader`, `Txt_Total_card_balance` and `Txt_0_00`. The instruction is centered above the balance row; the balance label is left-aligned and the amount is right-aligned.

### Languages and data

The kiosk supports **English, Chinese, Malay and Tamil**. In `WBP_KioskInteractive`, search the function list for `Translate_`. Functions follow names such as `Translate_English_DestinationMap` and `Translate_Tamil_JourneyMap`; these apply runtime text and font settings. Also inspect `Refresh` and the relevant `Update_...` functions when a label changes during play.

| File / asset in `Content/Data/Kiosk/` | Purpose and editing notes |
|---|---|
| `KioskDemoData.csv` / `DT_KioskDemoData` | Demo values such as `SingleFare`, `DayPassFare` and `InitialBalance`. The `Cents` column stores cents internally. Runtime values are rounded to the nearest whole dollar; use multiples of 100 (for example `200` for `$2`) to set an exact fare. After editing the CSV, reimport the Data Table and verify the transaction in play. |
| `ST_KioskDemoRow` | Row structure for the demo Data Table (`Value` and `Cents`). Changing the structure can affect Blueprint readers. |
| `KioskStations.csv` | Station names, station codes and lines. Keep station IDs consistent with the runtime map targets and station-selection logic. |
| `KioskTranslations.csv` | Source text with English, Chinese, Malay and Tamil columns. Keep this consistent with the main widget's runtime translation functions. |

CSV files are source data, not automatically live UI updates. In particular, editing the station or translation CSV alone does not rebuild the existing Blueprint map targets or translation graphs.

### Map routing, zoom and station selection

New sessions start at **Punggol Coast**, with no destination preselected. `InitializeSession` in `WBP_KioskInteractive` sets the origin. Station names use one fixed font and size immediately after their MRT/LRT badges. `CompactStationRow` compacts the code badges to leave room for interchange names without shrinking individual labels. Start/destination captions keep their language font at a consistent size and wrap when needed.

`RouteMap`, inside the main widget's `InteractiveRailMap` panel, displays the existing Figma artwork. Choosing a destination highlights the recommended route in line colours and fades the rest of the network. Start/end markers are yellow. Changing either endpoint fits the route into the viewport. Endpoint and transfer labels take priority; additional station names appear as space allows when zooming. Repeated UI refreshes retain the user's manual view.

Right-button dragging captures one pointer and applies its local movement to both axes together. Wheel zoom stays centred on the pointer. `MapZoomIn`, `MapZoomOut` and `MapFit` call this same control, so rendering and station hit testing always share the same transform. The old nested scroll boxes and fixed zoom layouts remain in the Blueprint for reference but are hidden and are no longer used for map interaction.

| Location | Purpose |
|---|---|
| `Content/UI/Kiosk/Blueprint/WBP_KioskRouteMap` | Blueprint OnPaint, pointer handling, route fitting, label placement and station hit testing. |
| `Content/UI/Kiosk/Blueprint/BP_KioskRailNetwork` | Blueprint route search, including transfer penalties. |
| `Content/Data/Kiosk/DA_KioskRailNetwork_BP` | Editable, packaged network: 180 stations, positions, connections, line colours, bend points and artwork. |
| `Content/Data/Kiosk/KioskMapStations.csv` | Source station coordinates in the original artwork's 1600 x 1600 design space. |
| `Content/Data/Kiosk/KioskMapConnections.csv` | Source connections, colours, bend points and estimated traversal costs. |
| `RefreshRouteMap`, `MapApplyStation`, `MapConfirm` | Passing journey state to the map, accepting station clicks and confirming the trip. |

Routes minimise estimated travel cost across the project's existing station snapshot, with two minutes per connection and a four-minute line-change penalty. Incoming line is part of the search state, so interchange choices are compared correctly. These are prototype estimates, not live service times. The network excludes future stations that are present only in the background artwork. Edit `DA_KioskRailNetwork_BP` to change runtime data; the CSV files document its source and do not automatically reimport it.

Clicking either the Start Location or Destination caption, field or search icon on DestinationMap and JourneyMap opens the existing on-screen keyboard and station options. `OpenStartKeyboard` and `OpenDestinationKeyboard` cover those map-only fields; `RefreshMapSearchField` hides them on the search pages so `StationQuery` and the keyboard remain usable. `OpenMapSearchResults` preserves typed text when showing filtered station results. Both complete station fields, including their search icons, open search on every map screen. The journey details screens hide the inline input but keep their search fields and map stations clickable. Selecting another station updates the journey and fits the new route; panning and zooming remain available.

Blueprint routing, map interaction and station-filter verification results are retained in `Saved/BlueprintConversion/`. Also verify the map in PIE with real station selections and all four languages.

### Blue kiosks K1-K5

In `L_Island`, K1-K5 use the supplied turquoise housing and one attached `BP_KioskTerminal` each. Their housing centres are at X = -400, -200, 0, 200 and 400 cm: 200 cm between centres and approximately 87.04 cm between housings. All five match the reference model at scale 1: 112.96 cm wide and 134.35 cm high, with their bases at Z = 63.95 cm, matching its mounting height. Move the parent K1-K5 actor to move its screen and devices together. The housing material is the project asset `Content/Materials/M_BaseColor_Turquoise`.

Edit the terminal Blueprint's `Model_...` components for the visible hardware. The hidden `...3D` components are interaction targets and must stay aligned with the model. The lower reader accepts the transit card; the upper reader accepts card/phone interactions; the payment keypad accepts the card payment interaction. The green help button opens assistance, and the cash slot receives automatically animated notes. Tickets, receipts and change fall into the centre of the bottom opening and stack against its collision floor. The three labels above the opening are hidden. `InteractiveScreen` replaces the model's striped screen placeholder.

`BP_KioskSession` shares transit balance and collected-document records across all five terminals and directs input to the kiosk the player approaches or aims at. Input stays with the current kiosk while its screen or an automatic action is active. Pending transactions and dispensed items stay at their own kiosk. Screen and payment calculations remain in Blueprints.

`KioskPresentationBP` on `BP_KioskTerminal` controls presentation at runtime. All 22 screens fade in on navigation. It also cycles the language/assistance prompts, brings in the ticketing options, blinks the empty station-search cursor, fades updated station results, demonstrates a transit-card scan, animates the success-screen ticket and flap, and gently animates the assistance icon. The collection illustration follows the supplied November 18 recording: ticket drop and a door that slides upward. QR payment uses the November 19 camera, phone payment and result sequence. Other Designer payment animations remain in place. Both start and destination keyboards use the same navigation behavior.

The EZ-Link card stands on the lower reader's white shelf, with its face visible against the scanner, and stays there during amount selection and bank-card, QR or cash payment. The hand withdraws empty after placement. After the top-up settles, it reaches back to retrieve the card. Changing payment method or requesting assistance keeps the card in place; cancelling the transaction or leaving retrieves it before unlocking the camera.

The collection camera settles first, then the green dispenser light remains visible for one second before individual physical items drop into the middle of the bottom tray with a short stagger. `BP_KioskPresentation.AnimateOutputs` uses rigid-body simulation against the collection tray and other items, waits for the pile to settle, then opens the door. The glass collection door slides straight upward over 0.4 seconds, preserving its rotation. Collection starts after the opening completes; the door stays open until the pickup animation finishes, then slides closed. A refresh or partial collection does not restart the drop. Adjust timings and `FlapSlideDistance` on `KioskPresentationBP`.

`KioskActionsBP` on `BP_KioskTerminal` runs the automatic demonstrations. Click the screen to move into a fixed reading view, then choose the purchase and payment method. Press E to return to movement. Right-drag remains available for panning the map. The player never selects or equips a card, phone or note. Entering transit-card balance places the EZ-Link card on the lower reader. Bank-card payment taps the upper reader, QR payment shows a code on the upper card reader and presents a camera phone, and cash payment inserts the required $50/$10/$5 notes in sequence. Card processing and QR scanning start at physical contact; QR payment commits only after the phone confirmation sequence, and cash is credited after the full note feeds into the slot. Cash then waits for the on-screen Pay button. Backing out before contact cancels the action without charging. A procedural Manny right arm reaches, grips and withdraws with each item.

After dispensing, the hand collects tickets, receipts and change automatically and a short message appears, such as "x1 ticket collected". The green physical help button opens the calling-for-assistance screen; it can be clicked with the cursor or targeted with F. Blue on-screen assistance buttons and their click targets are hidden. The calling screen's Back button remains available. There is no inventory HUD or equipment selection.

Physical card and ticket props reuse the project's Figma artwork, including the vector EZ-Link design and the ticket/day-pass exports. The extra $10 and $50 images are imported under `Content/UI/Kiosk/Textures/Payment`; their original SPECIMEN labels remain. Download sources are recorded in `ArtSource/Kiosk/PaymentReferences/Sources.txt`. The action graphs are in `BP_KioskActions`; prop/feedback widgets are `WBP_KioskProp` and `WBP_KioskCollectionFeedback`, under `Content/UI/Kiosk/Blueprint/`.

The four white bars on the upper reader have separate material slots in `SM_KioskReaderFourLights`, preserving the original geometry. During card processing they turn green one at a time, reaching all four before the existing payment timer completes. Completion/cancellation resets them to white. Accepted card/phone use flashes the matching indicator green; accepted cash keeps its indicator green during payment. The collection surround stays green while output is pending and returns white when empty. `M_KioskDeviceLight` provides per-terminal dynamic colour, so using one kiosk does not change another kiosk's lights.

The shared screen inherits the engine UserWidget class; `BP_KioskScreenLogic` holds its editable shared behavior. Day-pass dates toggle independently and remain selected while changing months. A new purchase starts with today selected; an empty selection cannot proceed. One pass covers exactly the selected days, including nonconsecutive dates, at $5.00 per selected day, multiplied by ticket quantity. Checkout shows the earliest activation day and final expiry at midnight after the last selected day. The extra selected-date list is hidden at the user’s request; the pass still stores all selected dates. `BP_KioskDocument` inherits the engine Actor class and snapshots `ValidPassDates`; validation checks membership rather than accepting the entire span. Starting another purchase cannot change an issued pass.

The calendar places activation/expiry above the description, with smaller labels and larger dates. Its panel sizes and typography use updated Figma calendar frame `1094:2270`; only the start date has a blue outline and the end date is solid blue; intervening dates are unmarked. Confirm becomes green when a date is selected. The top-up calculator uses a 900 x 740 panel, 260 x 92 number buttons and rounded corners. `F_KioskMultilingual` combines Roboto, Noto Sans CJK SC and the existing Tamil faces across screen, translation and inventory text. The Chinese source font and SIL Open Font License are in `ArtSource/Fonts/NotoSansCJK`.
### Kiosk screen and camera updates (18 November)

`BP_KioskScreenLogic` keeps the calculator and station results consistent with the kiosk theme. Both search fields open the keyboard and show matching stations immediately as the player types. A Filter button toggles the colour-coded line choices; choosing a line narrows the results, and Clear filter restores search across all lines while keeping the query. Interchanges appear on each applicable line. Map/search header names sit directly beside the train logo and the visible line badges, without reserving space for absent interchange codes. The map Confirm Trip control uses the Figma tick icon, green rounded rectangle and shadow, and opens checkout. Payment Successful includes a visible Print Receipt button bound to the existing once-only receipt action. The four cycling language prompts use a consistent, full-opacity 38-point font.

`BP_KioskScreenLogic.RefreshReferenceTheme` applies dark-blue payment buttons and dark-blue/white selected recharge amounts after language and state refreshes. It uses the original imported MRT and LRT pictograms, rounded code capsules, and the supplied checkout frame dimensions and typography. `WBP_KioskTripSlider` implements the lavender track, sliding blue selection and straight/curved arrow icons, with pointer dragging and keyboard selection. Payment hit targets follow their visible buttons. Configured fares and balances are rounded to the nearest whole dollar before calculation; payments, change and receipt labels display whole dollars with two decimal places (for example `$2.00`). Clicking an EZ-Link header logo returns to language selection when no payment or uncollected output is active. The physical green help button remains the assistance entry point; cancel, receipt, and map confirmation groups are retained even where the exported Figma widget name contains `Assistance`.

`WBP_KioskCrosshair` is a small white cross with a dark outline at the centre of the viewport. `BP_KioskSession.UpdateCrosshair` shows it during free movement and hides it while the kiosk camera is locked. `AimHitsScreen` checks the visible screen rectangle and rejects nearer obstacles; left-click enters the kiosk and E leaves. `WBP_KioskInteractionHint` shows a boxed E prompt while the kiosk view is locked and “Click to interact” when nearby outside it. The hint hides during staff assistance. `OnPreviewKeyDown` leaves keys unhandled so it does not consume the editor shortcut. Escape stops Play/Simulate in the editor; E remains the kiosk exit key. Both old E-key prompt components and their prompt widget are hidden.

`BP_KioskSession` positions the player and camera to frame the screen, upper payment readers and the complete physical green help button with a margin. `BuildScreenViewForAspect` uses the actual viewport ratio and shifts the camera horizontally to fit the hardware while keeping the screen close. The camera explicitly maintains horizontal FOV so the framing calculation stays correct across viewport aspect ratios. During a physical action, it shifts the view when necessary to include the reader or collection tray, waits for the camera to settle, and restores the reading view afterward. The updated local `Tech Design MRT KIOSK (1).fig` is the design source, with trip checkout frames `1127:2707` and `1153:2693` providing the native 1200 x 900 measurements. The original image UV crops are retained for the MRT/LRT and payment logos. The two supplied recordings define the QR and collection illustrations. Figma uses SF Pro; because that font was not supplied or installed, the project retains its multilingual font with the reference sizes and spacing. The letter shapes therefore differ from SF Pro. At the user's latest request, page title bars consistently use the previous larger header size: 27 Unreal font units (36 design pixels). The EZ-Link reader instruction uses a larger 20-point font and fits down only when needed within its separate 35-pixel text area; the 27-point heading and grey bar dimensions are unchanged.

Background verification and screen renders are retained in `Saved/KioskUX/`; widget backups are in its `Backup/` directory. No desktop control is required to run the verification helper.

### Editable behavior graphs

All assets in this table are in `Content/UI/Kiosk/Blueprint/`. Open **Graph**, then select a function in **My Blueprint**. Functions are grouped into categories, with descriptions on their entry nodes. The game uses ordinary Blueprint nodes; no node calls the old project C++ module.

| Blueprint | Functions/settings to edit |
|---|---|
| `BP_KioskSession` | `EnterScreen`, `UpdateView`, `FrameCollection`, `LeaveScreen`; input focus, camera locking and shared account state. |
| `BP_KioskCameraMath` | `ScreenView`, `CollectionView`, action framing and blend calculations. |
| `BP_KioskActions` | `StartAction`, `Animate`, `Contact`, automatic payment/collection selection and EZ-Link retention. |
| `BP_KioskHandRig` | `PoseHand`; reaching/gripping arm pose and hidden body bones. |
| `BP_KioskPresentation` | Screen animations, `AnimateOutputs`, `AnimateLights`; drop/open timing defaults and sliding-door distance. |
| `BP_KioskScreenLogic` | `RefreshLineSearch`, `RefreshReferenceTheme`, calendar selection, typography, header navigation and button binding. |
| `BP_KioskRules`, `BP_KioskCalendarRules` | Money formatting, station filters, pass pricing, selected dates and validity. |
| `WBP_KioskRouteMap`, `BP_KioskRailNetwork` | Map drawing/input and route calculation. |
| `WBP_KioskTripSlider` | Designer track/thumb, OnPaint arrow icons, pointer/keyboard selection and animated thumb. |
| `WBP_KioskProp`, `WBP_KioskCollectionFeedback` | Physical card/note/ticket artwork and the collected-item toast. |
| `BP_KioskInteraction` | Terminal registration and the connection to physical device actions. |

The SMRT/EZ-Link pill uses `WBP_KioskInteractive.HeaderLogoHomeButton`, bound in `BP_KioskScreenLogic.Initialize`; its `HeaderLogoHome` function returns to language selection. Top-up Return uses `ReturnToOptions`. Both call `BP_KioskActions.NavigateAfterCardReturn`, which retrieves a retained card before changing pages. Reader placement is editable through `TransitCardRestOffset` and `TransitCardRestAngle` in that Blueprint.

`RefreshBalanceNotice` sets the balance label and amount below $3 to red and supplies the minimum tap-in notice. The physical `HelpButton3D` is the assistance control; screen assistance graphics are collapsed. `Begin_SingleUse` opens destination search before showing a map, and the map action reads "Confirm Trip". Calendar selection and its past-date backgrounds are editable in `RefreshPassSelection` and `RefreshCalendarBackground`; their sizes follow the supplied 1200 x 900 calendar PDF. The four rotating language prompts are editable defaults on `BP_KioskPresentation`.

`BP_KioskSession.BuildScreenView` uses the reference camera framing; its `LockedScreenHeightFraction` and offset defaults control the screen size and composition while keeping the physical help button and payment devices in view. `BP_KioskActions.SignifierLeadSeconds` gives the customer one second to see the green device light after the camera settles and before the hand starts. `PrepareCashProp` deliberately overpays with accepted notes. `PrepareCashFeed` and `CompleteCashFeed` feed the note short edge first and credit it after insertion. `FrameCashInsertion` shows the note from above before the green cue, so the insertion is visible.

`BP_KioskPresentation.CreateOutputItem` creates one physical body per ticket or receipt, plus one fictional green note representing all pending change. `AnimateOutputs` releases the items after the green cue, waits for the pile to settle, then opens the door. The landed state stays set until an item is lifted or new output is added, so small resting movements cannot restart the wait. The terminal's `Collection*Collision` boxes follow the actual bin's sloped floor, front floor, back wall, sides and front lip, using the `KioskDispensedItem` collision channel. `ConfigureTray` also applies those shapes to placed instances. Receipt proportions are preserved at a size that fits the shallow bin. `PrepareCollection`, `LiftItem`, `MoveHeldItem` and `FinishPhysicalPickup` carry the same body out, highest item first, without swapping artwork. `BP_KioskTerminal.TakeTicket` transfers one ticket while preserving the original transaction and selected travel dates; `TakeChange` collects the full change amount in one pickup. The change note is drawn with native UMG widgets by `WBP_KioskProp.BuildChangeNote` and uses no real currency image.

The kiosk camera applies `M_KioskDisplayTonemapper` through `BP_KioskSession.ConfigureKioskDisplayRendering`. It converts the scene back from pre-exposure and preserves the display colours instead of applying the film tone curve. The correction covers all kiosk pages, survives leaving and re-entering, and ends when the kiosk camera is released.

The physical screen draws 1280 x 900 at its original world scale. `WBP_KioskInteractive.PhysicalDisplayCanvas`, its page canvases and backgrounds use the full 1280-pixel width. Text and buttons retain their size; checkout columns and map headers have been repositioned to fit the wider display. The station browser uses four station cards per row and three line filters per row, with 44-pixel outer margins. The editable search field follows the source/destination header and matches its yellow start or orange destination background. The failure page retains the Figma red header, gray message panel, blue retry and Return Home buttons. All Return Home and calculator action buttons use dark blue. `CustomAmountPanel` contains the matching top-up keypad layout. The updated top-up page has two columns of lavender amount options, a blue selected option, and aligned current/after balance rows. Its existing header and red low-balance label/amount are retained; the minimum tap-in notice uses bold white text on grey. The trip-details direction icon follows the one-way/round-trip selector; round trips show opposing vertical arrows. `PaymentSuccessful_Hit_Receipt` uses the Figma dark blue (`#0C00BA`). The result pages use their Figma Return Home dimensions (259 x 59 pixels on success and 259 x 63 on failure), with a centred, black 15-pixel countdown below each button. The failure page uses the updated compact Try Again / Return Home pair and light-grey message panel; staff instructions appear in the contextual interaction hint. The return countdown pauses while a card or output is being collected, leaving the receipt control available, then allows ten seconds after collection. Staff assistance suspends the countdown and returning to the failure screen starts a fresh ten seconds.

`BP_KioskTerminal` has the `KioskPresentationBP` and `KioskActionsBP` components. Existing transaction and translation graphs remain in `Interaction/WBP_KioskInteractive`; its shared helper functions forward to `BP_KioskScreenLogic`. `Interaction/BP_KioskDocument.IsValidOnDate` validates the dates saved on each purchased pass. The five level kiosks keep their previous transforms and hardware components.

Conversion tools, previous C++ files and recovery copies are retained locally under `Saved/BlueprintConversion/`, outside the active project source. They are not required to run or edit the Blueprint game. Keep that folder for recovery; it is excluded from Git.

### Completion screens and payment demonstrations (20 November)

The three options use their full column widths for pointer hover, with intentional line breaks and multilingual fit boxes. Language hover boundaries follow the visible dividers. Returning from the first single-use-ticket destination search goes to the options screen; searches opened from a map still return to that map. The selected line filter is vertically centred, with its line colour behind white text. Clear filter and the calculator actions use blue backgrounds and white labels. Success/failure action pairs share aligned icon/caption regions and a measured text scale, so translated Return Home labels do not become smaller than the adjacent action. The Malay label is Kembali ke Utama; the countdown stays centred below it.

The successful-payment illustration follows `Tech Design MRT KIOSK (3).fig` and the supplied `change + tickets.mp4` recording: the ticket falls inside the clipped tray, change follows, and the translucent glass slides upward. The ticket indicator flashes white and green at the reference position. Single-use tickets and day passes paid with cash show Amount paid and Change together. Other payment methods keep their usual amount panel. The cash-entry screens still require an explicit Pay press after enough cash is inserted.

For an EZ-Link top-up, success first shows removing the card from the reader. Cash top-ups follow `ezlink.mp4`: New balance (green) and Change (black) remain visible together, with the instruction to collect the transit card and change. When change is due, the illustration repeats a nine-second cycle: 3.8 seconds of card removal, followed by 5.2 seconds of the cash-dispensing sequence. The summary stays unchanged throughout and the animation continues after physical collection. This sequence never displays a ticket. Card/QR top-ups keep the usual balance panel; exact cash top-ups show the combined summary with zero change and only the card-removal illustration. Ticket and day-pass success illustrations also keep looping after collection until the page closes. `BP_KioskActions.UpdateSuccessReadingPause` tracks physical collection separately and gives each collection stage three seconds of settled screen time; adjust `SuccessReadingSeconds` to change this pause.

Cancelled or failed EZ-Link transactions retrieve the retained card before another attempt. Try Again queues a retry until card pickup and any cash-refund collection finish, then returns to the reader prompt and replays card placement. `CancelledTopUpAction` preserves the existing device-action selector and overrides only the cancelled top-up flow; `UpdateCancelledTopUpRetry` opens the prompt once collection is complete. `RefreshEzLinkCashSummary` applies the video-specific summary after localization.

The three cash-entry pages center inserted cash and Remaining over the Pay button, with vertically aligned labels and values. Their translated button captions and icons are centered as a group. The QR status sits below the accepted-merchant row; result headings and their status icons share a center line. The QR heading and status share one centered column; fare and merchant labels share a left edge, while the amount and payment logos share the value column. The QR status wraps inside that column for longer translations.

Screen panels retain the Figma corner radii (15 pixels on the main cards and 20 on calendar cards). Shadows use the shared texture-free UI material `/Game/UI/Kiosk/Materials/M_KioskDropShadow`, with reusable instances in `Materials/Shadows`. The material exposes pixel dimensions, corner radius, blur, offset, colour, and opacity; it excludes the interior to preserve translucent panel colours. Figma action shadows use 25% black, a 5-pixel offset, and a 10-pixel blur; language and Return pills use the smaller 4-pixel effect. Missing illustration and header shadows use the same material, and full-page backdrops render beneath the shadows. Edit the material and its instances directly in Unreal Editor; no Python scripts are needed to edit, run, or package these assets. The shadow shader is stored inside the material's Custom node; no separate shader source file is required.

The QR phone uses a grey articulated thumb to press Pay. After submission the Pay control disappears, and success replaces the phone payment form with the Figma check-mark artwork and Transaction successful message. The phone result remains visible before the kiosk changes to its own result page.

All page title bars now use the same larger bold type: 27 Unreal font units (36 design pixels), with no per-page scale reduction. This includes the map/search, checkout, payment, result and card-balance headers. The language and option landing screens retain their larger introductory headings. The top-up subtitle remains separate from the main title. Header changes do not alter the cash-entry layout or its explicit Pay behaviour.

The opening language screen keeps four large clickable regions aligned with its four visible language cards. These are separate from the narrow header language selector used on later screens. After changing language layouts, check the button bounds in the Widget Designer and test clicks near every card edge in Play in Editor.

### Before handing off a kiosk change

1. Compile and save the changed page and shared widgets, then compile/save `WBP_KioskInteractive` so its embedded pages are up to date.
2. Run the kiosk in the level. Check the actual runtime screen, not just the Designer preview.
3. Switch through all four languages. Check long labels, wrapping, clipping, button labels and alignment at the kiosk's displayed size.
4. Test the relevant flow: for example, calendar to checkout, payment to success/failure, or map zoom to station selection. Include a multi-line interchange badge such as Dhoby Ghaut when changing station rows.
5. Review the changed files before committing. Keep temporary scripts, editor helpers and preview reports outside the project.

**Do not delete or clean the working project's `Saved/` folder.** It may contain local autosaves, backups, logs and recovery material that Git does not preserve. Prepare submission copies separately.
