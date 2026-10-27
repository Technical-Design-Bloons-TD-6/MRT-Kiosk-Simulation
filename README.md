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
6. Check it worked: Help, About Unreal Editor should say 5.7.4. The project should open on `L_Main`.

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
