# ModularityPractice
Practice repository for modular and reusable game systems.
These systems are design-agnostic and are meant to fit in as many projects as possible with minimal modification.

NOTE: These systems may not be complete or have bugs I have not encountered yet. I use these systems in my personal projects, but are developed here so it is easier to plug-and-play them into each game I work on.

I will NOT be separating each plugin into their own repository, I can't be bothered with all the work behind that, and each of these plugins are supposed to play well together and have one, unified example project and documentation with all of them in use.

Majority is designed to stay inside of C++, but some things are handled in Blueprints where it makes sense. This is meant to be a healthy mix of C++ and blueprints.

What this repo hosts (That I own/created, see below for external plugins.):

---
- `FlowExtension`, which will NOT work without the Flow Graph plugin (Linked below). This extension is meant to provide a base that some games using the Flow Graph plugin will need. This includes:
	- Quest system.
	- Dialogue system.
   	- Other unrelated nodes, such as portals to reduce wire spaghetti.
    - NOTE: The Flow plugin in this repo is slightly out of date until more work around AIFlow is made. Check the Flow discord to stay up to date.

---
 - `TagFacts`, allows you to store gameplay tags with values to serve as a "fact system". This kind of system is very popular among triple A studios.
   	- Can also be repurposed for an achievement system.

---
 - `LayeredUI`, inspired by Lyra's UI layering, but expands on the idea and tries to simplify the internal logic.
 	- Allows you to layer widgets on the screen using tags instead of the usual AddToViewport function.
  	- Add a widget to another widget without creating hard references using `Slots`.

---
 - `ObjectTags`, a simple layer for widgets, actors and objects where gameplay tags are added, with delegates going off notifying if a tag has been added or removed.

---
 - `Relations`, a simple plugin that brings a "reputation" or "experience" system to players, but is tied to a data asset, which is meant to be a entity.
	 - This can display if a entity is "friendly" or "hated" by the player character in your game, similar to WoW's reputation system. This system supports relationships with multiple entities.
  	 - Can be repurposed for a leveling system.

Note: This has not been tested too much, the project this was meant for has been shelved and thus I haven't battle-tested this plugin that much.

---
 - `PerformanceDirector`, brings similar use cases to the internal plugin "Significance Manager" but brings it into a component and simplifies implementation.
 	- Can greatly improve performance in scenes with a lot of actors. See Wiki for more information.

---
 - `TagMetadata`, giving you a simple system to associate any kind of variable to a gameplay tag.
   - This is an extension of GameplayTags and does not replace it, this can work alongside any other plugin that uses GameplayTags.

---
- `Ambient Dialogue Manager` manages ambient dialogue in open world games where multiple NPC's might want to play ambient dialogue audio.
  - Balances audio for lower priority ambient dialogues.
  - Ensures the same ambient dialogue isn't repeated by separate NPC's.

---
- `EditorAssistant` provides a few quality of life improvements and editor tools.
  - Allows any class from a plugin or the base game to override thumbnails for classes and data assets in the content browser.
  - Allows any class from a plugin or the base game to add a custom category and entry into the content browser context menu.
  - Provides a base editor utility widget to act as a hub for all your editor utility widgets and comes with a few common tools that will benefit most projects.

---
# External plugins
These are plugins that are not mine but are utilized in this project, these plugins are amazing and developed by incredibly talented people.
 - `FlowGraph` - https://github.com/MothCocoon/FlowGraph
 - `EditorScriptingTools` - https://github.com/HoussineMehnik/UE4-EditorScriptingToolsPlugin
 - `UnrealImGui` - https://github.com/IDI-Systems/UnrealImGui/
 - `ImGuiBlueprint` - https://github.com/TheEnbyWitch/ImGuiBlueprint

# Notice:
- I will NOT be supporting multiplayer in any of these plugins, I am done with multiplayer programming for the foreseeable future.
Keep in mind, some of these plugins do not need multiplayer support, such as `LayeredUI`
- I don't take on many requests unless I see myself using the requested features. These are plugins that already work for my projects.
- The example project might not be well-organized or maintained. The wiki is the primary source of documentation.

# Links:
- [Marketplace](https://www.unrealengine.com/marketplace/en-US/profile/Varian+Daemon)
- [Twitch](https://www.twitch.tv/variann_)
