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