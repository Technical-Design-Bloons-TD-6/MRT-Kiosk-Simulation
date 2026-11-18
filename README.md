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

The payment illustrations use editable **Designer > Animations** tracks: `PaymentExample_CardTapThenInsert` plays tap followed by insertion in one shared illustration area, `PaymentExample_InsertCash` feeds a vertically oriented note short edge first into the slot on the three cash pages, and `PaymentExample_TapPhone` moves the phone to the reader. Each sequence loops while its page is active. The page function `UpdatePaymentExample` and the main widget's `RefreshPaymentExamples` start/stop playback, so returning to a page starts its example again. Keep `Canvas_CashNoteClip` in place when editing the cash illustration; it hides the note as it enters the slot. The static `Img_image_31` overlay is hidden because its artwork contains a second note. Payment instructions, summary columns and cancel controls share a centered layout.

The card page also contains `Canvas_CardProcessingOverlay`, with the transaction-in-progress message, card-removal reminder and animated `CardProcessingSpinner`. `BeginCardProcessing` starts a 2.4-second simulated processing period before `CompleteCardProcessing` runs the existing approval flow. `RefreshCardProcessing` dismisses the overlay and cancels its timer when leaving the card page. The four `Translate_..._CardPayment` functions supply its translated labels.

Cash payment keeps accepting notes after the fare is covered. `CashCents` continues increasing and Remaining is clamped to zero. `BeginCashSettlement` resets a two-second timer on each sufficient note; `CompleteCashSettlement` finishes payment after that quiet period. Leaving cash payment clears the timer through `RefreshCashSettlement`. The Change summary row is hidden; the existing hardware change/refund accounting remains in place.

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

Edit the terminal Blueprint's `Model_...` components for the visible hardware. The hidden `...3D` components are interaction targets and must stay aligned with the model. The lower reader accepts the transit card; the upper reader accepts card/phone interactions; the payment keypad accepts the card payment interaction. The green help button opens assistance, and the cash slot receives automatically animated notes. Ticket, receipt and change collection use the left, centre and right sections of the bottom opening. `InteractiveScreen` replaces the model's striped screen placeholder.

`BP_KioskSession` shares transit balance and collected-document records across all five terminals and directs input to the kiosk the player approaches or aims at. Input stays with the current kiosk while its screen or an automatic action is active. Pending transactions and dispensed items stay at their own kiosk. Screen and payment calculations remain in Blueprints.

`KioskPresentationBP` on `BP_KioskTerminal` controls presentation at runtime. All 22 screens fade in on navigation. It also cycles the language/assistance prompts, brings in the ticketing options, blinks the empty station-search cursor, fades updated station results, demonstrates a transit-card scan, animates the success-screen ticket and flap, and gently animates the assistance icon. The QR and collection illustrations follow the supplied November 18 recordings: phone approach/confirmation/withdrawal, then ticket drop and a door that slides upward. Other Designer payment animations remain in place. Both start and destination keyboards use the same navigation behavior.

The EZ-Link card is placed on the lower reader and stays there during amount selection and bank-card, QR or cash payment. The hand withdraws empty after placement. After the top-up settles, it reaches back to retrieve the card. Changing payment method or requesting assistance keeps the card in place; cancelling the transaction or leaving retrieves it before unlocking the camera.

Physical output drops into the bottom tray over 0.65 seconds. The glass collection door slides straight upward over 0.4 seconds, preserving its rotation. Collection starts after the opening completes; the door stays open until the pickup animation finishes, then slides closed. A refresh or partial collection does not restart the drop. Adjust timings and `FlapSlideDistance` on `KioskPresentationBP`.

`KioskActionsBP` on `BP_KioskTerminal` runs the automatic demonstrations. Click the screen to move into a fixed reading view, then choose the purchase and payment method. Press Escape to return to movement. Right-drag remains available for panning the map. The player never selects or equips a card, phone or note. Entering transit-card balance places the EZ-Link card on the lower reader. Bank-card payment taps the upper reader, QR payment presents a phone, and cash payment inserts the required $50/$10/$5 notes in sequence. Existing Blueprint payment functions run at physical contact. Backing out before contact cancels the action without charging. A procedural Manny right arm reaches, grips and withdraws with each item.

After dispensing, the hand collects tickets, receipts and change automatically and a short message appears, such as "x1 ticket collected". The green physical help button opens the calling-for-assistance screen; it can be clicked with the cursor or targeted with F. Blue on-screen assistance buttons and their click targets are hidden. The calling screen's Back button remains available. There is no inventory HUD or equipment selection.

Physical card and ticket props reuse the project's Figma artwork, including the vector EZ-Link design and the ticket/day-pass exports. The extra $10 and $50 images are imported under `Content/UI/Kiosk/Textures/Payment`; their original SPECIMEN labels remain. Download sources are recorded in `ArtSource/Kiosk/PaymentReferences/Sources.txt`. The action graphs are in `BP_KioskActions`; prop/feedback widgets are `WBP_KioskProp` and `WBP_KioskCollectionFeedback`, under `Content/UI/Kiosk/Blueprint/`.

The four white bars on the upper reader have separate material slots in `SM_KioskReaderFourLights`, preserving the original geometry. During card processing they turn green one at a time, reaching all four before the existing payment timer completes. Completion/cancellation resets them to white. Accepted card/phone use flashes the matching indicator green; accepted cash keeps its indicator green during payment. The collection surround stays green while output is pending and returns white when empty. `M_KioskDeviceLight` provides per-terminal dynamic colour, so using one kiosk does not change another kiosk's lights.

The shared screen inherits the engine UserWidget class; `BP_KioskScreenLogic` holds its editable shared behavior. Day-pass dates toggle independently and remain selected while changing months. A new purchase starts with today selected; an empty selection cannot proceed. One pass covers exactly the selected days, including nonconsecutive dates, at the existing $15 daily fare per selected day. Checkout shows the earliest activation day and final expiry at midnight after the last selected day. The extra selected-date list is hidden at the user’s request; the pass still stores all selected dates. `BP_KioskDocument` inherits the engine Actor class and snapshots `ValidPassDates`; validation checks membership rather than accepting the entire span. Starting another purchase cannot change an issued pass.

The calendar places activation/expiry above the description, with smaller labels and larger dates. Its panel sizes and typography use updated Figma calendar frame `1094:2270`; selected dates share one blue and Confirm becomes green when a date is selected. The top-up calculator uses a 900 x 740 panel, 260 x 92 number buttons and rounded corners. `F_KioskMultilingual` combines Roboto, Noto Sans CJK SC and the existing Tamil faces across screen, translation and inventory text. The Chinese source font and SIL Open Font License are in `ArtSource/Fonts/NotoSansCJK`.
### Kiosk screen and camera updates (18 November)

`BP_KioskScreenLogic` keeps the calculator and station results consistent with the kiosk theme. Both search fields open the keyboard and show matching stations immediately as the player types. A Filter button toggles the colour-coded line choices; choosing a line narrows the results, and Clear filter restores search across all lines while keeping the query. Interchanges appear on each applicable line. Map/search header names sit directly beside the train logo and the visible line badges, without reserving space for absent interchange codes. The map Confirm control is green and opens checkout. The four cycling language prompts use a consistent, full-opacity 38-point font.

`BP_KioskScreenLogic.RefreshReferenceTheme` applies dark-blue payment buttons and dark-blue/white selected recharge amounts after language and state refreshes. It uses the original imported MRT and LRT pictograms, rounded code capsules, and the supplied checkout frame dimensions and typography. `WBP_KioskTripSlider` implements the lavender track, sliding blue selection and straight/curved arrow icons, with pointer dragging and keyboard selection. Payment hit targets follow their visible buttons. Configured fares and balances are rounded to the nearest whole dollar before calculation; payments, change and receipt labels display whole dollars with two decimal places (for example `$2.00`). Clicking an EZ-Link header logo returns to language selection when no payment or uncollected output is active. The physical green help button remains the assistance entry point; cancel, receipt, and map confirmation groups are retained even where the exported Figma widget name contains `Assistance`.

`WBP_KioskCrosshair` is a small white cross with a dark outline at the centre of the viewport. `BP_KioskSession.UpdateCrosshair` shows it during free movement and hides it while the kiosk camera is locked. `AimHitsScreen` checks the visible screen rectangle and rejects nearer obstacles; left-click enters the kiosk and Escape leaves. Both old E-key prompt components and their prompt widget are hidden.

`BP_KioskSession` positions the player and camera to frame the screen, upper payment readers and the complete physical green help button with a margin. The camera explicitly maintains horizontal FOV so the framing calculation stays correct across viewport aspect ratios. During a physical action, it shifts the view when necessary to include the reader or collection tray, waits for the camera to settle, and restores the reading view afterward. The updated local `Tech Design MRT KIOSK (1).fig` is the design source, with trip checkout frames `1127:2707` and `1153:2693` providing the native 1200 x 900 measurements. The original image UV crops are retained for the MRT/LRT and payment logos. The two supplied recordings define the QR and collection illustrations. Figma uses SF Pro; because that font was not supplied or installed, the project retains its multilingual font with the reference sizes and spacing. The letter shapes therefore differ from SF Pro. At the user's request, the grey title bars keep their previous Unreal scaling: trip details 1.6, day-pass details 1.8, top-up title 1.6 (matching trip details), and its subtitle 1.1.

Background verification and screen renders are retained in `Saved/KioskUX/`; widget backups are in its `Backup/` directory. No desktop control is required to run the verification helper.

### Editable behavior graphs

All assets in this table are in `Content/UI/Kiosk/Blueprint/`. Open **Graph**, then select a function in **My Blueprint**. Functions are grouped into categories, with descriptions on their entry nodes. The game uses ordinary Blueprint nodes; no node calls the old project C++ module.

| Blueprint | Functions/settings to edit |
|---|---|
| `BP_KioskSession` | `EnterScreen`, `UpdateView`, `FrameCollection`, `LeaveScreen`; input focus, camera locking and shared account state. |
| `BP_KioskCameraMath` | `ScreenView`, `CollectionView`, action framing and blend calculations. |
| `BP_KioskActions` | `StartAction`, `Animate`, `CommitContact`, automatic payment/collection selection and EZ-Link retention. |
| `BP_KioskHandRig` | `PoseHand`; reaching/gripping arm pose and hidden body bones. |
| `BP_KioskPresentation` | Screen animations, `AnimateOutputs`, `AnimateLights`; drop/open timing defaults and sliding-door distance. |
| `BP_KioskScreenLogic` | `RefreshLineSearch`, `RefreshReferenceTheme`, calendar selection, typography, header navigation and button binding. |
| `BP_KioskRules`, `BP_KioskCalendarRules` | Money formatting, station filters, pass pricing, selected dates and validity. |
| `WBP_KioskRouteMap`, `BP_KioskRailNetwork` | Map drawing/input and route calculation. |
| `WBP_KioskTripSlider` | Designer track/thumb, OnPaint arrow icons, pointer/keyboard selection and animated thumb. |
| `WBP_KioskProp`, `WBP_KioskCollectionFeedback` | Physical card/note/ticket artwork and the collected-item toast. |
| `BP_KioskInteraction` | Terminal registration and the connection to physical device actions. |

`BP_KioskTerminal` has the `KioskPresentationBP` and `KioskActionsBP` components. Existing transaction and translation graphs remain in `Interaction/WBP_KioskInteractive`; its shared helper functions forward to `BP_KioskScreenLogic`. `Interaction/BP_KioskDocument.IsValidOnDate` validates the dates saved on each purchased pass. The five level kiosks keep their previous transforms and hardware components.

Conversion tools, previous C++ files and recovery copies are retained locally under `Saved/BlueprintConversion/`, outside the active project source. They are not required to run or edit the Blueprint game. Keep that folder for recovery; it is excluded from Git.

### Before handing off a kiosk change

1. Compile and save the changed page and shared widgets, then compile/save `WBP_KioskInteractive` so its embedded pages are up to date.
2. Run the kiosk in the level. Check the actual runtime screen, not just the Designer preview.
3. Switch through all four languages. Check long labels, wrapping, clipping, button labels and alignment at the kiosk's displayed size.
4. Test the relevant flow: for example, calendar to checkout, payment to success/failure, or map zoom to station selection. Include a multi-line interchange badge such as Dhoby Ghaut when changing station rows.
5. Review the changed files before committing. Keep temporary scripts, editor helpers and preview reports outside the project.

**Do not delete or clean the working project's `Saved/` folder.** It may contain local autosaves, backups, logs and recovery material that Git does not preserve. Prepare submission copies separately.
