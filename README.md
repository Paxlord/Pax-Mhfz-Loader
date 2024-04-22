# Pax's MHFZ Mod Loader

General-purpose mod loader for Monster hunter frontier Z.

If you encounter any issue with the loader or any of my mods, feel free to raise an issue or shoot me a dm on Discord at @pax_777

This loader uses a plugin architecture to dynamically load compatible mods present in the "/mods" folder in the main game folder. For more information about potentially creating your own mods please check this repo : [paxloader-basemod](https://github.com/Paxlord/paxloader-basemod)
_(Please note that creating mods in this game requires some decent knowledge of C++ and x86 ASM, and even if this loader makes it easier it's still really hard to do.)_

### **CAUTION : This loader only and will only support High-grade edition, if you try to run this loader with Low-Grade edition you will either completely crash or not load anything.** 

## Installation

Drop ```pax.dll``` and ```version.dll``` inside your mhfz folder. When launching the game, you should see a console and a new window in the game called "Mod Menu".

## Features

- Dynamic loading of compatible mods
- Hooking of DX9 and auto-injection of ImGUI, the context is passed to each mod.
- Basic mod versioning checks
- Basic mod loader versioning checks
- Possibility to define a required mod list 
- Possibility to allow specific mods

## Contributing

I'm not a C++ dev. So the code might not be as clean and readable as it could be. Contribution are more than welcome same for feature suggestions.

## Config.json

If a config.json file is present in the same folder as pax.dll. The loader will read it. Here is an example config :

```js
{
//Used to check if the loader should actively close the game if LGE is detected
"no_lge":  false,

//Required allow you to set a list of mods that have to be loaded or the client will be closed
"required": [
{ "name"  :  "weapon-unbloater", "version":  "0.1.1" }
{ "name"  :  "camera-tweaker", "version":  "any" },],

//Allowed acts like a "white-list" of mods that are allowed but not required.
"allowed": [
{ "name"  :  "weapon-unbloater", "version":  "0.1.1" }
{ "name"  :  "camera-tweaker", "version":  "any" },]
}
```
