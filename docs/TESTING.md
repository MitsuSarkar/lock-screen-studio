# Runtime validation before release

Version 0.6 adds Studio replacement cards after native movement remained pinned.
Check actual weather text and live rendering before treating this as validated:

- [ ] Select Studio mode; leave Weather enabled in Windows Settings.
- [ ] Match title, temperature/unit, condition/warning and image to native data.
- [ ] Verify -300/+300 move the independent card as expected with room on screen.
- [ ] Confirm source visuals and invisible native click targets are hidden only
      after the replacement is attached; unavailable data leaves native visible.
- [ ] Verify widget themes, font overrides, anchors and row scaling.
- [ ] Change native data/locations or wait for an update; the mirror refreshes.
- [ ] Switch back to native or disable mod; original opacity/hit testing return.
- [ ] Repeated lock/unlock and source-tree rebuild do not duplicate cards.

Version 0.5: user photo shows weather clipping inside its parent at widgetX=-300.
Regressions cover a narrower parent viewport even when the full card fits on the
display, and the difference when the complete widget-only viewport moves.

- [ ] Test -300 and +300 with outer mode and keep-on-screen enabled.
- [ ] Confirm full card, outline, weather icon and text remain visible together.
- [ ] Confirm moving widgets leaves the clock, media and wallpaper in place.
- [ ] Compare the logged motion-host name/type across stock and themed widgets.
- [ ] Choose different fonts for clock, date, weather and media independently.
- [ ] Check stock, Pixel, typography and legacy clock designs; preserve icon glyphs.
- [ ] Empty the font fields and disable mod; original fonts/opacity are restored.
- [ ] Test narrow/long labels and the custom-font Hollow layout at scaled DPI.

Version 0.4.1: confirm that only one outline surrounds each card, using the
reported Minimal Outline weather screenshot as a regression reference. Test
Pixel/Mac/Smoke as well, multiple widgets, and media with album art. Confirm
inner wrappers are transparent, small controls keep their own chrome, and stock
restoration restores every original border/background. Try hex RGB and ARGB
overrides and an opacity of 100. Build passed; these visual checks are pending.

Version 0.4: compiled x64. Automated placement tests cover 30,000 generated
cases for screen edges, oversized groups, scaled transforms and repeated refresh.
The following UI checks remain pending:

- [ ] Each media/widget preset changes its card surface and text colors.
- [ ] Mac Light remains readable; Mac Dark/Nord/Catppuccin/Gruvbox contrast works.
- [ ] Pixel single-line weather/city/title values update with native data.
- [ ] Wrapped/unsupported text remains readable; weather icons retain their fonts.
- [ ] Move weather far left/right/up/down with keep-on-screen enabled.
- [ ] Compare outer/panel movement; check for parent clipping on this build.
- [ ] Reset offsets to zero, resize/scaling change, and lock/unlock repeatedly.
- [ ] Switch pixel to stock; no invisible original labels or leftover overlays.
- [ ] Disable/unload restores all native backgrounds, text, offsets and opacity.
- [ ] Measure idle CPU with visible and hidden cards.

Version 0.3.1: x64 compilation passes. User reports version 0.3 working, with a
few-second delay before media styling. The new media refresh runs ahead of clock
throttling and separately from full scans. Runtime checks still pending:

- [ ] Lock while music is playing: measure the stock-to-custom card interval.
- [ ] Switch media themes while locked: settings apply without the clock delay.
- [ ] Start/stop playback while locked; replaced cards are discovered quickly.
- [ ] Change tracks/album art; newly created child elements receive the skin.
- [ ] Reset media to stock or disable mod; original properties are restored.
- [ ] Compare visible/hidden idle CPU; verify no clock refresh-rate regression.
- [ ] Repeat lock/unlock, settings reload and unload with media present.

Version 0.3: eight new pixel clock/date compositions compile with the x64
Windhawk compiler. An SVG preview uses the same generated glyph outlines for
visual inspection, but does not prove XAML rendering in LockApp. The user reports
version 0.2 working properly.

- [ ] Test every new Pixel preset, including complete pixel dates and shadows.
- [ ] Check Night's minute colors and separator; Hollow's date/clock spacing.
- [ ] Switch between Pixel and old themes repeatedly; check for retained visuals.
- [ ] Verify 12-hour AM/PM, seconds, date rollover and localized date fallback.
- [ ] Verify no font installation is required in LockApp.
- [ ] Verify pixel anchors, date/clock sizes, offsets and battery updates.

Version 0.2: x64 compilation passes. User reports version 0.1 working for clock
and media positioning, but widget movement failed. Retest WidgetGroupPanel
discovery and each media skin after updating. Test play/pause/next and album art,
skin-to-stock restoration, and widget offset reset to zero. With slow widget
loading, ensure fallback and inner panel offsets do not add together. Review
diagnostic logs for the actual selected group; do not assume the name matches.

Status: standalone x64 compilation passed on 2026-10-30 with Windhawk 1.7.3's
compiler. Windhawk editor compilation and hands-on validation remain pending.
Do not mark a runtime check complete from a successful
compile alone. Record Windows build, Windhawk version, display resolution and DPI.

- [ ] Windhawk compiles the standalone mod without errors.
- [ ] With other LockApp stylers disabled, logs show mod initialization,
      UI hook installation, tree attachment and clock injection.
- [ ] Lock/unlock five times without duplicate clocks, hangs or crashes.
- [ ] All six Studio themes render with correct date, battery and colors.
- [ ] Original six designs still update; Windows stock option restores its clock.
- [ ] Test 12/24-hour mode, seconds on/off, midnight/date rollover and resume.
- [ ] Save settings while unlocked and while locked; new values appear correctly.
- [ ] Test independent positions for clock/date/battery/text; include every anchor.
- [ ] Test at 100%, 150% and 200% scaling; check clipping and long localized dates.
- [ ] Custom font installed for all users resolves in LockApp.
- [ ] Widget hide/movement works; check diagnostic names if it does not.
- [ ] Media hide/movement works when media playback is active.
- [ ] Disable/re-enable while locked and unlocked; native content is restored.
- [ ] Failed clock attachment keeps the native clock visible.
- [ ] No stuck input; clicking/dragging the lock screen still opens sign-in.
- [ ] Measure idle CPU with seconds disabled and enabled.

If nothing changes, enable Windhawk debug logging and this mod's `diagnostics`.
Distinguish failure stages: no initialization means injection; initialization but
no hook means window discovery; hook but no attachment means tree discovery;
attachment but no injection means XAML construction. Save the full log and build
number. The one-time tree dump lists names/types, not text content.

Known release risks: private element names; inherited synchronous UI dispatch can
wait on an unresponsive UI thread; inherited worker stops if its CoreWindow is
destroyed. The fallback unload path can leave visual references retained if UI
teardown cannot run. These need live stress testing before a public release.

### 0.6.1 themed weather icons
- Compile verified; live lock-screen visual checks remain manual.
- In Studio cards mode, check Pixel, Minimal, Mac Light/Dark, Nord, Catppuccin and Gruvbox.
- A weather card with a temperature must show an icon even without a native Image.Source.
- Verify condition changes update the icon; unknown/localized text uses a thermometer.
- Check media and non-weather cards retain their original artwork.


### 0.6.2 weather condition and placement
- Automated: 15 condition/accessibility-label/alert-exclusion/manual-override cases passed.
- Manual: verify Automatic follows the native condition and updates on data refresh.
- Check all eight placements, positive/negative X/Y, size 24/160 on narrow cards, and Show weather icon off.
- Verify Pixel uses angular artwork and Minimal uses outline artwork.
- No live LockApp validation performed for this release.


### 0.6.3 widget theme defaults
- Switch themes with Customize widget theme colors and surface off; saved hex overrides must not leak across themes.
- Check Mac Light versus Mac Dark, Nord slate/cyan, Mocha mauve and Gruvbox brown/yellow.
- Enable customization and verify saved colors, opacity and corners apply again.
- Windows default must remove the Studio mirror and restore native source opacity and hit testing.
- Pixel must show one stepped silhouette with no square background at its cut corners.
- Live visual verification remains manual.

