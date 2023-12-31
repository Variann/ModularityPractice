# ModularityPractice
Practice repository for modular and reusable game systems.
These systems are design-agnostic and are meant to fit in as many projects as possible with minimal modification.

NOTE: These systems may not be complete or have bugs I have not encountered yet. I use these systems in my personal projects, but are developed here so it is easier to plug-and-play them into each game I work on.

Majority is designed to stay inside of C++, but some things are handled in Blueprints where it makes sense. This is meant to be a healthy mix of C++ and blueprints.

What this provides:

---
- Flow extension plugin, which will NOT work without the Flow Graph plugin. This extension is meant to provide a base that most games using the Flow Graph plugin will need. This includes:
	- Quest system.
	- Dialogue system.
   	- Other unrelated nodes, such as portals to reduce wire spaghetti.

---
 - TagFacts, a plugin that allows you to store gameplay tags with values to serve as a "fact system". This kind of system is very popular among triple A studios.
   	- Can also be repurposed for an achievement system.

---
 - LayeredUI, inspired by Lyra's UI layering, but takes a different approach.
 	- Allows you to layer widgets on the screen using tags instead of the usual AddToViewport function.

---
 - ObjectTags, a simple layer for widgets, actors and objects where gameplay tags are added, with delegates going off notifying if a tag has been added or removed.

---
 - Relations, a simple plugin that brings a "reputation" or "experience" system to players, but is tied to a data asset, which is meant to be a entity.
	 - This can display if a entity is "friendly" or "hated" by the player character in your game, similar to WoW's reputation system.
  	 - Can be repurposed for a leveling system. 

---
 - PerformanceDirector, brings similar use cases to the internal plugin "Significance Manager" but brings it into a component and simplifies implementation.
 	- Can greatly improve performance in scenes with a lot of actors. See Wiki for more information.

---
 - ColorTags, allows you to have a centralized location for common colors throughout your project by adding additional config settings.
   - Improves mod support as people can now just modify a .ini file to configure common colors, such as item rarity colors or UI colors to their liking.

---
- Ambient Dialogue Manager manages ambient dialogue in open world games where multiple NPC's might want to play ambient dialogue audio.
  - Balances audio for lower priority ambient dialogues.
  - Ensures the same ambient dialogue isn't repeated by separate NPC's.

---
# External plugins
These are plugins that are not mine, but I utilize to bring my plugins to the next level.
 - FlowGraph - https://github.com/MothCocoon/FlowGraph
 - EditorScriptingTools - https://github.com/HoussineMehnik/UE4-EditorScriptingToolsPlugin
 - UnrealImGui - https://github.com/IDI-Systems/UnrealImGui/
 - ImGuiBlueprint - https://github.com/TheEnbyWitch/ImGuiBlueprint
