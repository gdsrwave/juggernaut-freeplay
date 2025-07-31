# JFP 1.6.7 (July 30, 2025)
- Bugfixes and touch-ups

# JFP 1.6.6 (July 23, 2025)
- Added logo and changelog

# JFP 1.6.5 (July 23, 2025)
- Attempted cross-platform support
- Cleaner GD console outputs

# JFP 1.6.4 (July 21, 2025)
- Codebase style check

# JFP 1.6.3 (July 21, 2025)
- Added random Color Mode option
- Added random Corridor Rules option
- Added option to shuffle your local songs
- Fixed glow corners on transitions in `heinous` theme
- Bugfixes

# JFP 1.6.1 (July 21, 2025)
- Adjusted size portal positions
- Support for tagging themes as big/miniwave only
- Added Finish Line option
- Added endscreen distance display
- Added LRD corridor rule (low + respectful density)
- Added random bg texture option
- Added ability for themes to disable default slopes
- Bugfixes

# JFP 1.6.0 (July 18, 2025)
- New theme: `gandhi`
- Update `gandhi`, `heinous` and `moonmen` themes to support miniwave
- JFPT language support for floor/ceiling targeted matching
- JFPT keyword "override" changed to "define"
- Added definitions for start, start-mini, endup-mini, enddown-mini
- JFPT creator tools in editor's Edit menu
- Removed attributes 155/156 from all level data

# JFP 1.5.3 (July 14, 2025)
- Added option import button/popup to JFP menu

# JFP 1.5.1 (July 13, 2025)
- Ported option string features from jfp.py
- Added option code label to JFP menu

# JFP 1.5.0 (July 9, 2025)
- Added miniwave
- Added size portals (Type A and Type B transitions)
- Expanded portal, hazard etc. options to work on miniwave
- Added Corridor Widening option
- Added total-played/ack-disclaimer/global-seed saved values

# JFP 1.4.7 (July 6, 2025)
- Mid corridor portals on Aggressive
- Fixed editor JFP bugs (or added preventative measures)
- Added Icon Kit pulldown
- Refined Theme options section
- Added BGM to JFP menu
- Bugfixes and minor additions

# JFP 1.4.6 (July 5, 2025)
- Added "Juggernaut" corridor rule
- Added Portal Inputs option

# JFP 1.4.4 (July 5, 2025)
- New theme: `ninecircles`
- Overhauled `heinous` and `moonmen` themes
- Modified progress bar to show traveled meters
- Added info button to JFP menu
- Removed theme list button from JFP menu

# JFP 1.4.1 (July 3, 2025)
- New theme: `moonmen`
- Added JFPT "override" keyword
- Added overrides for base, endup, enddown
- Added "not matching" functionality to JFPT
- Added Reload Themes button
- Added theme list button to JFP menu
- Existing gen techniques compatibility with themes
- Added corridor height (C) to block parsing
- Bugfixes

# JFP 1.4.0 (July 3, 2025)
- Added theme engine (metadata, color channels, corridor matching, block parsing/XY arithmetic)
- New theme: `heinous`
- Broke out/added color string generation

# JFP 1.3.2 (June 29, 2025)
- Level data internal reworks/theme prep
- Ported remaining options to Ninja/StringGen
- Removed Teleportals option
- Bugfixes
- Ninja gravity portals support
- Ninja Corridor Rules fix

# JFP 1.3.1 (June 29, 2025)
- Initial Ninja format (JFP custom structure generation rather than direct level data building)
- Fix returning to JFP menu correctly from level

# JFP 1.3.0 (June 22, 2025)
- Code structure massive overhaul (was previously one main.cpp and basically a direct copy of jfp.py)
- Repo reorganization
- Data structures/Ninja prep
- Added JFP menu layer/LMAO button
- Bugfixes

# JFP 1.2.4 (June 20, 2025)
- Fixed color hue picker algorithm
- Make level objects Dont Fade Dont Enter
- Fixed teleportal spawning scheme
- Bugfixes

# JFP 1.2.3 (June 10, 2025)
- Fixed incorrect corridor length
- Ported Teleportals, Speed Changes, Fake Portals and Portal/Speed Diffs
- Changed difficulties from Beginner/Intermediate/Expert to Light, Balanced and Aggressive

# JFP 1.2.2 (June 8, 2025)
- Ported Corridor Spikes, Fuzz
- Expanded soundtrack
- Bugfixes

# JFP 1.2.1 (June 8, 2025)
- Ported Color Mode and JFP soundtrack
- Added Dabbink Mode (auto-generate new every attempt)
- Disable Debug option by default
- Fixed Dabbink bugs and memory leaks

# JFP 1.52p (September 9, 2024)
- Bugfixes

# JFP 1.51p (June 21, 2024)
- Added Hidden Visibility mode
- Beginner, Intermediate and Expert gravity portal diffs
- Beginner, Intermediate and Expert speed change diffs
- Bugfixes

# JFP 1.50p (June 19, 2024)
- Added and moved all options to config.ini file
- Added option strings/codes (encoded options) export and import system
- Bugfixes

# JFP 1.41p (June 13, 2024)
- Added Night Mode color mode
- Bugfixes

# JFP 1.40p (June 12, 2024)
- Added teleportals option / TELEPORT flag
- Added UPSIDE_DOWN start flag
- Added FAKE_PORTALS flag

# JFP 1.30p (June 7, 2024)
- Added speed portal spawning
- Added SPEED_CHANGES, MIN_SPEED and MAX_SPEED variables
- Added SPIKES flag (corridor spikes)
- Added FUZZ flag (ground spikes)
- Randomized background color hue
- Added ALL_BG_COLORS flag

# JFP 1.21p (June 2, 2024)
- 80+ songs added to soundtrack
- No Geode version because of 2.206

# JFP 1.2.0 / 1.20p (June 1, 2024)
- Added Low Visibility option
- REMOVESPAM now incorporates ZIGLIMIT automatically

# JFP 1.12 / 1.12p (May 31, 2024)
- United the RNG systems between Python and Geode versions (a seed generates the same result on both)
- Corridor, portals, song etc. now have separate RNG paths
- Added gravity portals option
- Added seed option to Geode
- Changed seeds to 32-bit number

# JFP 0.52 (May 30, 2024)
- Removed script pausing at the end
- Added PAUSE flag
- Initial jfp.ahk macro release

# JFP 1.00 (May 28, 2024)
- Initial JFP geode release
- Added Waveman button to created levels
- Port existing features to cpp/Geode

# JFP 0.51 (May 22, 2024)
- Bugfixes

# JFP 0.5 (May 22, 2024)
- Added REMOVESPAM flag (NS)
- Added CORRIDOR_HEIGHT variable
- Added MARKER_INTERVAL variable
- Added soundtrack

# JFP 0.41 (May 22, 2024)
- Added ZIGLIMIT flag

# JFP 0.4 (May 22, 2024)
- Added zigzag limit (NZ)

# JFP 0.3 (May 22, 2024)
- Added ending connectors
- Added CORNERS flag

# JFP 0.2 (May 22, 2024)
- Added MARKS meter marks flag
- Changed level name format
- Print seed in console

# Juggernaut Freeplay (May 21, 2024)
- Initial jfp.py
- Added Juggernaut corridor generation
- Added SPEED variable
- Added FILENAME variable
- Added LENGTH variable
- Added optional RNG seed input

# Bronze Age (May 2, 2024)
- Short ingame randomized wave corridor (104754464)