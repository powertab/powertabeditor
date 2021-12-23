# Change Log
All notable changes to this project will be documented in this file.

## [Unreleased]

Thanks to the following contributors who worked on this release:
- @cameronwhite

### Added
- Added support for tremolo bars (#8).
- .pt2 files are now 3-4x smaller in file size.
- For Linux users, the application can now be easily installed as a Snap package (https://snapcraft.io/powertabeditor).
- The macOS installers are now signed and notarized. This resolves the "developer cannot be verified" warnings when running for the first time.

### Changed
- Removed dependency on boost::filesystem. Instead, std::filesystem (C++17) is now used. See the README for updated build instructions.
- Removed dependency on RapidJSON with nlohmann-json. See the README for updated build instructions.

### Fixed
- Fixed an issue where stopping MIDI playback while a "let ring" was active could incorrectly keep the "let ring" active when restarting playback from the beginning (#337).
- Fixed a bug where the mixer's volume settings could be ignored when starting MIDI playback partway through the score (#335).
- Fixed an issue where the score's title could be scaled incorrectly when printed (#338).
- Fixed potential crashes on exit when the tuning dictionary failed to load (#342).

## [Alpha 15] - 2021-07-24

Thanks to the following contributors who worked on this release:
- @cameronwhite
- @pysomn

### Added
- Many behavior changes to improve the workflow of editing symbols such as tempo markers (#23, #192, #220).
  - These items can now be clicked on and selected, and Delete can then be pressed to remove the selected item.
  - Double-clicking an item will now launch the edit dialog. Previously, it was required to remove and then re-create the item.
- New preferences option to enable playing notes as they are entered (#47).
- New menu item `Help` -> `App Info` which contains information about the installation. This can be useful to include when reporting bugs (#329)

### Changed
- On macOS, the `Delete` key can now be used to remove notes rather than having to press `Fn+Delete`.
- The MIDI device is now initialized in the background, which should avoid some lag the first time playback is activated (#47).
- Qt 5.10 or higher is now required.

### Fixed
- Fixed a MIDI playback bug where adjacent repeated sections resulted in an empty bar being played (#289).
- Fixed a bug where the score's existing subtitle did not show up when opening the file information dialog.
- Fixed a bug where multi-bar rests were not drawn as a filled rectangle.
- Fixed a bug where the pan controls in the mixer had no effect (#276).
- Fixed a rare error when importing .ptb files (#332).

## [Alpha 14] - 2020-11-24

Thanks to the following contributors who worked on this release:
- @cameronwhite
- @psyomn
- @mostafa-nabil

### Added
- Added a bulk file conversion tool (#288, #212).
- Added a 32-bit installer for Windows in addition to the default 64-bit build (#312).
- Added a preference to select a light or dark score theme, in addition to the system default colors (#307).

### Changed

### Fixed
- Fixed issues with adjusting player volumes during MIDI playback (#276, #303).
- Fixed an issue where starting MIDI playback later in the score could prevent bends from being played (#311).
- Closing a tab that has modifications and isn't the active tab now works correctly (#304).
- Fixed multiple issues that could cause the Windows installer to fail (#305).
- Fixed issues where scores without any song information (title, artist info, etc) would be printed incorrectly (#313).
- Fixed printing of the song information at the top of the score, which previously was extremely large when printed (#313).

## [Alpha 13] - 2020-08-11

Thanks to the following contributors who worked on this release:
- @cameronwhite
- @mostafa-nabil
- @nbrunett

### Added
- Added a toolbox panel with common commands (#188, #300, @nbrunett).
- Added support for volume swells (#17).
- Added MIDI playback support for alterations of pace (#18).
- Added new commands (`Note -> Shift String Up` and `Note -> Shift String Down`) to shift notes up or down by a string (#20).
- Ties, hammer ons, pull offs, and slides can now be created between notes in adjacent systems (#27).
- Added a dark theme for the score view, based on the user's system theme (#286, @mostafa-nabil).
- Left hand fingerings now support thumbs. This is also now supported by the Guitar Pro 6/7 importers (#292).
- The song information can now store a subtitle. This is also now supported by the Guitar Pro 6/7 importers (#297).
- Several improvements to the view filter dialog. In particular, it now displays a list of the players that will be displayed when the filter is used (#295).
- Several improvements to the Guitar Pro 3/4/5 importer:
  - It now shares most of its code for converting to the Power Tab format with the GP6/7 importers, fixing a variety of layout / conversion issues.
  - Artificial and tapped harmonics are now imported.
  - Bends are now imported (#30).
  - Musical directions are now imported.
  - The initial tempo marker's name and visibility is now imported.
  - Wide vibrato, left hand fingerings, and the song's subtitle are now imported.
  - Fixed issues where imported tied notes could incorrectly have a fret number of 0 instead of the previous note's fret.

### Changed
- The `Position -> Shift Forward` and `Position -> Shift Backward` menu items have been renamed to `Insert Space` and `Remove Space`, to hopefully reduce confusion with the new string shifting commands (#20).
- Unit tests now depend on the [doctest](https://github.com/onqtam/doctest) library instead of `Catch2`. This is more lightweight (while still providing all of the necessary features) and is more widely available on Linux distros.

### Fixed
- Improved the size and alignment of the treble and bass clef glyphs.
- Improved standard notation layout when a note is staccato and also has an accent.
- Improved alignment of accents in the standard notation staff.
- Improved vertical positioning of trills and dynamics symbols.
- Fixed a bug where editing the zoom text caused it to turn black, making it unreadable with a dark system theme.
- Fixed a potential crash in the Edit View Filters dialog when a string pattern was invalid (#295).

## [Alpha 12] - 2020-06-11
### Added
- Added support for Guitar Pro 7 (`.gp`) files (#290).
- The Guitar Pro 6 (`.gpx`) importer has been rewritten and shares most of its code with the GP7 importer. It now has much more complete support for `.gpx` files, and a variety of bugs and missing features have been addressed (#290).
- Improved the Rest menu's behaviour to be more consistent with the Notes menu (#135).
- Beginning with this release, `.pt2` files with an unknown newer file format version (e.g. from a future release that adds additional data to the score format) will now be loaded to the best degree possible instead of refusing to load. (#294).
- `File -> Open` now allows selecting and opening multiple files.

### Changed
- Removed dependency on the Boost `program_options` library.

### Fixed
- Fixed a crash when the player assigned to a staff did not have enough strings (#243).
- Fixed issues that could cause notes to be held forever after interrupting MIDI playback.
- The `.pt2` importer is now more robust, and should better handle JSON data generated by other software (#294).
- Fixed an issue on Windows where the Stop button could cause playback to restart again (#296).
- Fixed an issue where slides and bends could lead to tempo drift during playback (#214).
- Fixed an issue where double-clicking to open a file could fail to open it in an existing instance of the program (#236).
- Fixed a potential crash during MIDI playback when musical directions were present.

## [Alpha 11] - 2020-05-28

Thanks to the following contributors who worked on this release:
- @cameronwhite
- @DontBelieveMe
- @iceseyes
- @jcranney
- @jlearman
- @Kadabash

### Added
- File information is now displayed at the top of the score (#49).
- The Delete key will now remove rests - previously, Ctrl+Delete was required (#202).
- Added support for left hand fingering hints (#261, #65).

### Fixed
- Fixed tempo drift issues during playback (#269).
- Fixed errors when loading or saving files that had non-ASCII characters in their path (#244).
- Fixed a bug in the score polisher when there were grace notes at the start of a bar.
- Fixed issues where the pause and stop buttons did not reliably respond to clicks during playback (#237).
- Fixed a bug where dots could be hidden when a note had an accidental (#242).
- Fixed a bug with the MIDI exporter that caused the file header to be invalid (#241).
- Fixed incorrect key signature position for the bass clef (#275).
- Fixed a bug where it was impossible to edit the number of repeats at the final barline in the score.
- Improved how the MIDI output on OSX is implemented (#285).
- The dark theme on Mac OSX is now supported for all widgets except the score view.
- Fixed many build system issues (e.g. #255, #282) and updated the required compiler and library versions.
  - A C++17 compiler is now required.
  - Third-party dependencies are no longer bundled as submodules, and instead are expected to be provided by the system package manager or a tool such as `vcpkg`. The README has been updated with the recommended build steps.
  - Moved to Github Actions for CI builds. See `.github/workflows/build.yml`.
  - Fixed errors when loading .pt2 files with newer versions of `RapidJSON`.
  - Various improvements to the OSX installer.

## [Alpha 10] - 2016-12-22
### Added
* An Ubuntu PPA is now available (https://launchpad.net/~powertab-developers/+archive/ubuntu/powertabeditor-stable).
* Added a dialog (under the Players menu) for adding custom filters for which players should be visible in the score. (#173).
* Added zoom support (#211, #213, #221).
* Added drag and drop support for opening files.
* Added delete commands to the Edit menu (#101).
* Added support for multi-line text items (#71).
* Added an option to stop MIDI playback and return to the initial location, instead of just pausing at the current location (#185).

### Changed
* Improved Linux integration and fixed several packaging issues (#178, #201)
    * Added a correct `install` target, which installs the executable to `$CMAKE_INSTALL_PREFIX/bin` and the tuning dictionary to `$CMAKE_INSTALL_PREFIX/share/powertab/powertabeditor`.
    * Custom tuning dictionaries are now saved to `$XDG_DATA_HOME/powertab/powertabeditor`, and application settings are saved under `$XDG_CONFIG_HOME/powertab`.
    * Added launcher support, with a .desktop file and application icon installed to `$CMAKE_INSTALL_PREFIX/share/applications` and `$CMAKE_INSTALL_PREFIX/share/icons/hicolor`.
    * Added file association support, installed to `$CMAKE_INSTALL_PREFIX/share/mime/packages`.
    * Building against the system versions of RapidJSON, pugixml, Catch, withershins, and RtMidi is now supported. They will be auto-detected and used instead of the submodules in the `external` directory.
    * Fixed missing version numbers in the titlebar when building from a tarball instead of a Git repository.
    * Running `make test` will now run the unit tests.

* Improved OS X integration
    * Key sequences (e.g. in the Customize Shortcuts dialog) are now displayed in the native format (matching what is seen in the menus) instead of showing Ctrl/Shift/etc
    * The tab bar and several dialogs now have a more native appearance.
    * Improved the appearance of the playback toolbar, mixer, and instrument panel.
    * Fixed a number of default keyboard shortcuts that were unusable due to conflicts with system-wide shortcuts.
    * Modifications to the tuning dictionary are now saved under `~/Library/Application Support` instead of overwriting the tuning dictionary in the application bundle.

* Application settings are now saved using a simple JSON format on Windows / Linux, and `NSUserDefaults` (.plist) on OS X.
* Improved the auto-scrolling behaviour when moving the caret's location (#191).
* Opening the same file multiple times will now switch to the tab where the file was already open for editing (#29).
* When adding a new staff, the number of strings now defaults to the current staff's number of strings (#193).
* When adding a new system, the number of staves is inherited from the previous system (#193).
* Removed the lower bpm limit for tempos, which fixes occasional v1.7 import errors (#206).

### Fixed
* Fixed issues with using `Backspace` as a keyboard shortcut (#166).
* Fixed a crash when removing the last staff in a system (#216).
* Fixed a potential crash when playing imported v1.7 files with alternate endings (#204).
* Fixed the Ukulele GCEA tuning (#229)
* Fixed a potential crash when playing an imported Guitar Pro file with empty bars (#230)

## [Alpha 9] - 2015-11-15
### Changed
* Rewritten MIDI player, which now shares the majority of its code with the MIDI file exporter and should produce identical MIDI events.

### Fixed
* Fixed MIDI playback issues on Windows 10 when using the Microsoft GS Wavetable Synth.
* Fixed a potential crash when merging the guitar and bass score from a v1.7 file (#183).

## [Alpha 8] - 2015-06-28
### Added
- MIDI files can now be exported (#55).

### Changed
- Rewritten score merger, which fixes numerous issues with importing v1.7 files (#153, #154).
- Grace notes are now drawn smaller and with a slash through the stem, and are no longer included in beaming patterns (#19).
- The Increase/Decrease Duration command now updates the active note duration that is used for new notes (#70).

### Fixed
- The scorer polisher now correctly handles grace notes (#103).
- Fixed a crash when there are no available MIDI output devices (#180).
- The note duration is now preserved when changing a note to a rest (#175).

## [Alpha 7] - 2015-04-26
### Added
- Added support for hiding staves via the View menu in the toolbar (#66).
- Added a Save command to the File menu (#21)
- Added support for opening files via Finder on OSX (#169)
- Added a command (Ctrl+Space) to start playback from the beginning of the current measure (#163)

### Changed
- The Print Preview dialog can now be maximized (#165)
- Player changes now display the actual player name instead of the player number (#161)

### Fixed
- Fixed crash when changing the number of strings in a staff (#170)

## [Alpha 6] - 2015-01-24
### Added
- Added printing support (#157).
- The number of strings in a staff can now be edited (#146).
- When a crash occurs, a dialog is now displayed that contains a crash report and a link to the bug tracker (#160).

### Changed
- Renamed a few commands to make them easier to find in the shortcut editor (#139).
- Player changes now indicate if there are no players assigned to a particular staff, instead of being invisible (#147).

### Fixed
- Fixed a crash when changing the number of strings for a player (#159).

## [Alpha 5] - 2014-12-27
### Added
* Added a shortcut (Shift+J) for the Polish Score command, and adjusted the names of some commands to make them easier to find in the keyboard shortcuts dialog (#139)

### Changed
* The bend dialog now allows arbitrarily long bend durations (#136)
* New instruments are now named the same way as players (#142)
* All clickable items in the score now have tooltips (#148)
* Improved the placement of slide and hammeron symbols (#152)
* The customize shortcuts dialog now checks for duplicate shortcuts (#7)
* The tuning dialog now tracks whether the current tuning matches one of the presets from the tuning dictionary (#141)

### Fixed
* Fixed various issues with scrolling through the presets in the tuning dialog (#122, #138)
* Fixed a bug with error handling in the Player Change dialog (#147)
* Fixed errors when loading Guitar Pro files on Linux (#156)
* Fixed a bug in the score polisher when dealing with multiple voices (#144)

## [Alpha 4] - 2014-9-20
### Added
- Added many new tunings to the tuning dictionary (including 7 and 8 string tunings), and cleaned up several redundant or incorrect tunings (#118, #119, #130, #114, #113, #112, #111, #122)
- Added a command to polish a single system (#78)
- The tuning dialog now allows users to revert to the original tuning after editing a preset (#121)
- Added additional fret options to the tapped harmonic dialog (#129)

### Changed
- Improved the positioning of rehearsal signs when the position spacing is large (#84)
- Newly-created files are now marked as modified so that users are prompted to save the files before exiting (#106)
- For groups of harmonics, the dotted line no longer stretches to the end of the staff (#128)
- The last used directory is now updated after saving a file, instead of only when opening a file (#137)
- Newly-created players and instruments now have unique names (#105, #107)
- Tapped harmonics are now updated when changing the fret number of a note (#129)
- The caret now turns gray to indicate if the score widget has lost focus (#123)

### Fixed
- Fixed errors with saving custom tunings to the tuning dictionary on Windows.
- Fixed a bug where the playback toolbar buttons' tooltips did not update after changing their keyboard shortcuts (#14)
- Fixed issues with the caret not redrawing properly after moving to another system (#87, #34)
- Fixed a bug where the caret could get stuck in an invalid position after polishing the score (#85)
- The score polisher now handles alternate endings correctly (#57)
- Files are now correctly marked as unmodified after being saved (#90)
- Fixed a bug that caused the score polisher to fail when rhythms from different staves/voices didn't exactly match up (#109)
- Fixed a bug that caused 16th notes to sometimes be drawn without beams (#98)
- Fixed a bug that caused the bend dialog to incorrectly set the release pitch (#93)
- Fixed a bug that caused bends to not be fully released during MIDI playback (#93)
- Fixed crashes when removing players and instruments (#97)
- Fixed a bug that caused the sharps checkbox to be incorrectly checked when editing a tuning (#120)
- Fixed a potential crash when deleting a note after changing voices (#124)
- Corrected the note heads used when drawing harmonics (#127)

## [Alpha 3] - 2014-08-30
### Added
* Added a score polisher command under the Edit menu. Imported files (e.g. Guitar Pro files) are now automatically formatted upon being loaded (#78, #26, #79).
* Bar numbers are now drawn at the start of each system (#1).
* Ties are now drawn in the standard notation staff (#38).
* The metronome can now be toggled on and off during MIDI playback (#73).

### Changed
* Pressing `Y` on an empty position will now insert a note that is tied to the previous note (#61).
* Ties, bends, hammer-ons, and slides are now drawn much more smoothly.

### Fixed
* Fixed a bug where the caret could become incorrectly positioned after adding or editing a note (#81).
* Fixed a bug where tied notes were not faded in the tab staff (#72).
* Fixed a bug with the importing of v1.7 files where symbols such as rehearsal signs were duplicated while expanding a repeated section (#75).
* Fixed a bug where a multi-bar rest of duration 1 could be created when importing v1.7 files.

## [Alpha 2] - 2014-08-17
### Added
* Added a 32-bit installer for Windows XP users. If you installed the previous alpha release, keep using the `win64` version.
* An OS X installer is now available.
* The buttons in the playback widget now have keyboard shortcuts, associated menu items, and more useful tooltips (#14)

### Changed
* Completely rewritten importer for Guitar Pro 3,4,5 files with numerous bug fixes and improvements
    * Grace notes are now imported from Guitar Pro files (#32)
    * Irregular groups are now imported from Guitar Pro files (#11)
    * Drawing the score is now significantly faster for large documents.
    * Added support for floating text (#12). In order to support flexible score layouts in the future, text is attached to a position in the system (instead of being at an absolute (x,y) position as in v1.7)
    * Improved the readability of the score by adding a small white border around tab notes, and switched to a lighter color for tab lines.
    * Improved MIDI playback for muted notes (#51)
    * Long beam groups (such as eight 16th notes in 4/4 time) are now subdivided for improved readability.
* Improved the rendering of alternate endings (#57)

### Fixed
* Listesso and triplet feel tempo markers are now drawn correctly (#44)
* Fixed issues where the automatic scrolling during MIDI playback became laggy (#56)
* Fixed some issues where alternate endings were ignored during MIDI playback (#63)
* Fixed a crash when deleting a note that was part of an irregular group (#39)

## [Alpha 1] - 2014-07-27
* The first alpha release!

[Unreleased]: https://github.com/powertab/powertabeditor/compare/2.0.0-alpha15...HEAD
[Alpha 15]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha15
[Alpha 14]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha14
[Alpha 13]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha13
[Alpha 12]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha12
[Alpha 11]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha11
[Alpha 10]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha10
[Alpha 9]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha9
[Alpha 8]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha8
[Alpha 7]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha7
[Alpha 6]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha6
[Alpha 5]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha5
[Alpha 4]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha4
[Alpha 3]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha3
[Alpha 2]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha2
[Alpha 1]: https://github.com/powertab/powertabeditor/releases/tag/2.0.0-alpha1
