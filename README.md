# Team01 Kiosk Prototype (Unreal)

Public transit ticket kiosk prototype for Technical Design Methods, Part III. Read this before you touch the project.

## 1. Versions (everyone must match)

| Tool | Version |
|---|---|
| Unreal Engine | 5.7.4 |
| Visual Studio | 2022, 17.14 or later, with the "Game development with C++" workload |
| Git | Git for Windows (latest), includes Git LFS |
| Project file | `Team01_Kiosk.uproject` |

Do not open the project in a different engine version. Opening it in a newer one upgrades the files, and you cannot go back.

The project is C++, but we build everything with Blueprints and widgets. Nobody needs to write C++.

## 2. First-time setup

1. Install the versions above. Run `git lfs install` once in a terminal.
2. In GitHub Desktop: File, Clone repository, URL tab, then enter `https://github.com/Technical-Design-Bloons-TD-6/MRT-Kiosk-Simulation`.
3. Pick an **empty folder with a short path**, for example `C:\Dev\MRT-Kiosk-Simulation`. Long paths cause Unreal build errors.
4. Right-click `Team01_Kiosk.uproject` and choose "Generate Visual Studio project files" (on Windows 11, use Shift + right-click if you don't see it).
5. Double-click the `.uproject`. The first open compiles for a few minutes. Say yes if it asks to rebuild.
6. Check it worked: Help, About Unreal Editor should say 5.7.4. The current editor startup map is `Content/Maps/L_Island`.

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
| `CashPayment`, `CashPaymentProgress`, `CashPaymentComplete` | Cash instructions, accepted notes, inserted cash, remaining amount and change. These are separate pages; check each state. |
| `PaymentSuccessful`, `PaymentFailed` | Result messages, receipt, retry and return-home controls. |
| `RequestAssistance` | Assistance screen layout. |

### Where the other kiosk parts live

Paths in this table are relative to the project root. In Unreal, `Content/` is shown as `/Game/` in asset references.

| Location | What it controls |
|---|---|
| `Content/UI/Kiosk/Interaction/WBP_KioskInteractive` | Main kiosk widget: screen switching, transaction state, translations, runtime text updates, button actions and map interaction. Open **Graph** for behavior; the page widgets supply the screen layouts. |
| `Content/UI/Kiosk/Interaction/BP_KioskTerminal` | Kiosk actor and the connection between the player, kiosk screen and simulated hardware. |
| `Content/UI/Kiosk/Interaction/WBP_KioskInteractPrompt` | Prompt shown when interacting with the kiosk. |
| `Content/UI/Kiosk/Interaction/WBP_KioskHardware` | Simulated reader, cash-slot and output controls. |
| `Content/UI/Kiosk/Interaction/BP_KioskInventory` and `WBP_PlayerInventory` | Inventory behavior and its on-screen presentation, including held cards, phone, cash and collected items. |
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
| `KioskDemoData.csv` / `DT_KioskDemoData` | Demo values such as `SingleFare`, `DayPassFare` and `InitialBalance`. The `Cents` column uses whole cents: `211` means `$2.11`. After editing the CSV, reimport the Data Table and verify the transaction in play. |
| `ST_KioskDemoRow` | Row structure for the demo Data Table (`Value` and `Cents`). Changing the structure can affect Blueprint readers. |
| `KioskStations.csv` | Station names, station codes and lines. Keep station IDs consistent with the runtime map targets and station-selection logic. |
| `KioskTranslations.csv` | Source text with English, Chinese, Malay and Tamil columns. Keep this consistent with the main widget's runtime translation functions. |

CSV files are source data, not automatically live UI updates. In particular, editing the station or translation CSV alone does not rebuild the existing Blueprint map targets or translation graphs.

### Map zoom and station selection

Map behavior is in **`WBP_KioskInteractive`**, alongside the page layouts:

| Function / widget | Purpose |
|---|---|
| `WheelZoom`, `MapZoomIn`, `MapZoomOut`, `MapFit` | Mouse-wheel routing, zoom steps and reset-to-fit behavior. |
| `MapZoomLevel`, `ApplyMapZoom` | Current zoom level and selection of the corresponding layout. |
| `ZoomLevel_0` through `ZoomLevel_3` | Map dimensions and artwork positioning for each zoom level. The current map widths are **992, 1440, 2000 and 3960**, respectively, in widget layout units. |
| `Zoom0Targets_...` through `Zoom3Targets_...` | Position and size of station click targets at each zoom level. Update these together with map dimensions so clicks remain aligned. |
| `MapContentSize`, `MapArtwork`, `MapHit_Station_...` | Runtime map content size, map image and station hit areas. |
| `MapApplyStation`, `MapConfirm`, `RefreshMap` | Applying a selected station, confirming the trip and refreshing map state. |

When changing zoom, check every level, both scroll directions, reset-to-fit and station selection. Enlarging only the image can leave the clickable station targets behind.

### Before handing off a kiosk change

1. Compile and save the changed page and shared widgets, then compile/save `WBP_KioskInteractive` so its embedded pages are up to date.
2. Run the kiosk in the level. Check the actual runtime screen, not just the Designer preview.
3. Switch through all four languages. Check long labels, wrapping, clipping, button labels and alignment at the kiosk's displayed size.
4. Test the relevant flow: for example, calendar to checkout, payment to success/failure, or map zoom to station selection. Include a multi-line interchange badge such as Dhoby Ghaut when changing station rows.
5. Review the changed files before committing. Keep temporary scripts, editor helpers and preview reports outside the project.

**Do not delete or clean the working project's `Saved/` folder.** It may contain local autosaves, backups, logs and recovery material that Git does not preserve. Prepare submission copies separately.
