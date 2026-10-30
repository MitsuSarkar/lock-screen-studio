# Publishing Lock Screen Studio 0.0.1

Submitted by Mitsu (`MitsuSarkar`): [Windhawk pull request #6770](https://github.com/ramensoftware/windhawk-mods/pull/6770).
The [project repository](https://github.com/MitsuSarkar/lock-screen-studio) and all three screenshots are public.
Catalog availability is pending maintainer review and merge. GitHub currently requires maintainer approval before the first-contributor validation workflows can run.
The steps below document the submission process for reference.

## 1. Host the screenshots

Create a public repository named `lock-screen-studio` under your GitHub account.
Upload the three files in `docs/screenshots` to the same path on its `main` branch.
The standalone mod description already points to these planned URLs:

- `https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots/pixel-lock-screen.png`
- `https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots/modern-flip.png`
- `https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots/sonoma.png`

These URLs will only work after the files are uploaded. Open each link while signed out to check that it displays the image. If you choose another repository/path, regenerate with `tools/extend.ps1 -ScreenshotBaseUrl 'https://raw.githubusercontent.com/OWNER/REPO/main/PATH'` and keep that value for future builds. Windhawk's embedded readme accepts images hosted on raw.githubusercontent.com or i.imgur.com.

The screenshots show Pixel clock/weather, Modern Flip and Sonoma respectively. They show your customized layouts, not guaranteed preset defaults. Wallpaper isn't bundled.

## 2. Check the release file

Use the complete `lock-screen-studio.wh.cpp` in the project root. Metadata is:

```text
@id      lock-screen-studio
@version 0.0.1
@author  Mitsu
@github  https://github.com/MitsuSarkar
```

Compile it in Windhawk, check the Details tab renders all screenshots, and confirm your clock, weather and media settings still work. Check disabling/re-enabling the mod and switching to Windows default. Keep the existing MIT and OFL notices: the inherited engine and font data require them. Version 0.0.1 is the initial public version; private development versions were never published. Future public updates must increase the version.

## 3. Submit to the official catalog

1. Sign in to GitHub as `MitsuSarkar` and fork [ramensoftware/windhawk-mods](https://github.com/ramensoftware/windhawk-mods).
2. In your fork, create a branch such as `add-lock-screen-studio`.
3. Add **only** `mods/lock-screen-studio.wh.cpp`, using the complete release source from this workspace. Do not add the DLL, screenshots, generator, or development files to this pull request.
4. Commit with `Add Lock Screen Studio 0.0.1`.
5. Open a pull request to the upstream repository's default branch. Suggested title: **Add Lock Screen Studio**. Paste `docs/PULL_REQUEST.md` as the description.
6. Check automated results and respond to the maintainers' review. Submission is not automatic acceptance. Once merged and deployed, verify the catalog entry and install it through Windhawk.

Keep the pull request to one mod file. The `@github` value must match the submitting account. These requirements come from the [official submission instructions](https://github.com/ramensoftware/windhawk-mods#submitting-a-new-mod) and the [mod authoring guide](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod).
