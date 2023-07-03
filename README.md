# ModularityPractice
Practice repository for modular and reusable game systems.

What this provides:

---
- Flow extension plugin, which will NOT work without the Flow Graph plugin. This extension is meant to provide a base that most games using the Flow Graph plugin will need. This includes:
	- Quest system
	- Dialogue system

---
 - TagFacts, a plugin that allows you to store gameplay tags with values to serve as a "fact system". This kind of system is very popular among triple A studios.

---
 - LayeredUI, inspired by Lyra's UI layering system using CommonUI, but isn't... Well to put it bluntly, my version is not shit.

---
 - ObjectTags, a simple layer for widgets, actors and objects where gameplay tags are added, with delegates going off notifying if a tag has been added or removed.

---
 - Relations, a simple plugin that brings a "reputation" or "experience" system to players, but is tied to a data asset, which is meant to be a character.
	 - This can display if a character is "friendly" or "hated" by the player character in your game.
