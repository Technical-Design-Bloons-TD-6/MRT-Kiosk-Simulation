# Team01 Kiosk Prototype (Unreal)

Public transit ticket kiosk prototype for Technical Design Methods, Part III. This file is the team's rules. Read it before you touch the project.

## 1. Versions (everyone must match)

| Tool | Version |
|---|---|
| Unreal Engine | 5.7.4 (change this line if the team agrees on another version) |
| Visual Studio | 2022, 17.14 or later, with the "Game development with C++" workload |
| Git | Git for Windows (latest), includes Git LFS |
| Project file | `Team01_Kiosk.uproject` |

Do not open the project in a different engine version. Opening it in a newer one upgrades the files, and you cannot go back.

## 2. First-time setup

1. Install the versions above. Run `git lfs install` once in a terminal.
2. In GitHub Desktop: File, Clone repository, URL tab, then enter `https://github.com/Technical-Design-Bloons-TD-6/Kiosk-Simulation`.
3. Pick an **empty folder with a short path**, for example `C:\Dev\Kiosk-Simulation`. Long paths cause Unreal build errors.
4. Right-click `Team01_Kiosk.uproject` and choose "Generate Visual Studio project files" (use Shift + right-click on Windows 11 if you don't see it).
5. Double-click the `.uproject`. The first open compiles for a few minutes. Say yes if it asks to rebuild.
6. In the editor: File, Connect To Source Control, choose Git LFS 2, tick the file locking option, and check the user name matches your GitHub name.

## 3. Daily workflow

- Everyone works on `main`. No personal branches.
- **Pull** when you start. **Commit small** and **push** before you stop.
- Close the editor before you commit, so files are not changing underneath you.
- Commit with a clear message, like `Day pass: calendar screen`.
- If a push fails or something looks wrong, tell the group chat before you try to fix it.

## 4. Rules for binary files

Blueprints, widgets and levels (`.uasset`, `.umap`) are binary. Git cannot merge them, so the last push wins and the other person's changes are lost.

- **One owner per file.** Only edit your own screens and assets. Ask the owner for changes.
- **Lock before you edit anything shared:** the main level, navigation, the UI kit and the master material. Submitting from the editor commits, pushes and releases your lock.
- Say in the group chat when you lock something shared, and unlock it when you finish.
- Do not rename, move or delete shared assets without telling the group. It breaks links.
- Keep fares, station names and text in a CSV under `Content/Data` and import it as a Data Table. CSV is plain text, so it merges.
- Do not commit builds, packaged output or large source files (`.blend`, `.psd`, screen recordings). Keep those on the shared drive.

## 5. Roles and ownership

Fill in the names. If two people want the same role, go by who built that screen in Figma.

| Role | Owns | Person | Backup |
|---|---|---|---|
| Repo and integration lead | Repo, README, plugin, final submission folder | | |
| UI kit | Header, Return button, language bar, buttons, help arrow, fonts, colours | | |
| Navigation and main level | The Blueprint that moves between screens, 10-second timeouts, `L_Main` | | |
| Entry and help | Language select, ticketing options, help, receipt prompt | | |
| Single-use flow | Map, search, trip type, checkout | | |
| Top-up flow | Card reader screen, amounts, checkout | | |
| Day pass flow | Calendar, checkout | | |
| Payment and outcomes | Card, QR and cash screens, success and failure | | |

Build order: UI kit and a navigation skeleton with placeholder screens go in first. Nobody starts a real screen until both are pushed.

## 6. Environment

Each environment piece is its own file with one owner. Environment roles go to whoever has the lightest screen load.

| Role | Owns | Person |
|---|---|---|
| Environment lead | `L_Lighting`, the master material and colour palette, the soft post-process look | |
| Kiosk model | The kiosk mesh, the screen area, `L_Kiosk` | |
| Station and props | Floor, walls, benches, plants, signage, `L_Station` and `L_Props` | |

- `L_Main` is the persistent level. It stays almost empty and only gains sub-levels. One owner (navigation).
- Sub-levels: `L_Kiosk`, `L_Station`, `L_Props`, `L_Lighting`. Never edit someone else's sub-level.
- Turn on One File Per Actor (External Actors) when you create each level.
- Each prop is its own asset. Others only place it.
- Order: greybox first (plain cubes for sizes), then the master material, then kiosk, station and props in parallel, then lighting last.
- Keep scope small. The grade is on the kiosk UX and design principles.

## 7. Folders and names

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

## 8. Submission checklist

- The `Source` folder holds only `Config`, `Content`, `Plugins` (if present), `Source` and the `.uproject`. Delete all other folders first.
- Test the cleaned folder on a machine that has not opened the project before.
- Naming format: `ID_AssessmentTitle.zip`, containing one folder `ID_AssessmentTitle`, with files named `ID_Filename.extension`.
- Video: 1920x1080, MP4, 5 minutes at most.
- Build is optional. If included: windowed, 1920x1080, Windows EXE, in a `Build` folder.
