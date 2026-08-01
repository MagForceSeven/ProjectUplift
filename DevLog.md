# Project Uplift

A log of the updates to Project Uplift.
Detailed descriptions of (some) things available in the ReadMe.

## May 2026
* Initial release/publication
* General source code framework
* General content layout
* Initial feature plugin support
* Basic Game Loop: Shell -> Strategy <-> Tactical
	* Basic content to support game loop like maps
* Save Games
* Assets for Difficulty selection and tracking during game
* Pause Menu
	* Only Resume, Quit, Main Menu currently work
	
## July 2026
* First pass implementation of Heroes and Hero Classes
* Integration of the Starfire Messenger for broadcasting gameplay (fairly limited right now) related events
	* Currently the addition of Heroes to the Roster, broadcasting that the strategy or tactical game is ready for play or that the BattleData is about to be destroyed
* UI and VM's to support Heroes and player management of Heroes
	* This is the initial integration of the ActorViewModels module from StarfireGame
* Added a new Game Feature Plugin for testing DLC integration
	* Includes a Game Feature Action that is used for incorporating DLC information into new or in-progress games
* A unique random number stream for gameplay randomness checks (this is persistent and meant to make randomness "consistent" when loading saves)
* Created a mechanism for determining the assets which will need bundle changes for the current game mode
* Added a GUID to the campaign for uniquely identifying it against other campaigns (useful for save management and telemetry)
* Improvements to the World Type based World Subsystems
* Update to 5.8.0 Engine version (and corresponding changes to Starfire Plugins)

## August 2026
* First pass UI Implementation for Load Game, Save Game & Continue buttons in Shell & Pause Menu
	* New screens for Load Game & Save Game
	* New supporting widgets for save game entries and lists
	* New dialog box for prompting for user input to name manual saves
	* New View Models tracking the set of all save games and views of individual saves
* Updates to the save slot naming conventions
	* Add the slot name directly to the header because platforms don't always use the slot name 1:1 as the filename
* Updates & centralization of the display naming functionality
	* Most save requests can now skip providing one and a reasonable one is generated (more work here as more gameplay is implemented)
	* Track user entry of the display name so that overwriting saves can preserve user names, but regenerates automated names
	* Also first pass at the extra display info included for entries
* Add Travel & Checkpoints as actual save game types (even though they're never written to disk that way)
	* Save Type is updated on checkpoints if they're ever saved to disk
* Remove some Campaign Save utilities that aren't really needed after implementing the view models
* Two! save game version bumps!
* Add an autosave to Strategy