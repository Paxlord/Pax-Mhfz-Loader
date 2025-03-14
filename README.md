# Pax's MHFZ Mod Loader

General-purpose mod loader for Monster hunter frontier Z. My mods are stored in this repo : https://github.com/Paxlord/Pax-MHFZ-Mod-Repository

If you encounter any issue with the loader or any of my mods, feel free to raise an issue or shoot me a dm on Discord at @pax_777

This loader uses a plugin architecture to dynamically load compatible mods present in the "/mods" folder in the main game folder. For more information about potentially creating your own mods please check this repo : [paxloader-basemod](https://github.com/Paxlord/paxloader-basemod)
_(Please note that creating mods in this game requires some decent knowledge of C++ and x86 ASM, and even if this loader makes it easier it's still really hard to do.)_

### **CAUTION : This loader only and will only support High-grade edition, if you try to run it with Low-Grade edition you will either completely crash or not load anything.** 

## Features

- Dynamic loading of compatible mods
- Hooking of DX9 and auto-injection of ImGUI, the context is passed to each mod.
- Basic mod versioning checks
- Basic mod loader versioning checks
- Possibility to define a required mod list 
- Possibility to allow specific mods

### Future plans

At this point, I feel like the loader is in a pretty good spot and can do its job just fine. Here's a few missing features that I'm planning or thinking about : 
- I have not yet found a decent "Update" function that would run only while in the lobby and could be hooked without too much side-effects
- Better memory cleanup, I'm not a c++ dev so i'm still figuring out the whole cleaning-up your own memory part.
- I'm tinkering with the possibility to have the loader hit a /get endpoint on the current running server adress to get the config. This would remove any possibility from a malicious player to tinker with the way the loader is loading mods.
- More config options as they come up...

## How to use
### Installation
- Check the Release of this repo (link here : https://github.com/Paxlord/Pax-Mhfz-Loader/releases) and download the latest paxloader.zip archive
- Unzip and drop the content of the ```paxloader.zip``` archive inside your mhfz folder, . When launching the game, you should see a console and a new window in the game called "Mod Menu".
![image](https://github.com/Paxlord/Pax-Mhfz-Loader/assets/19719025/0ac8f259-ed4c-443d-b196-0abff7dbfaf9)


### Uninstallation

Simply delete or remove ```pax.dll``` and ```version.dll``` from the game's folder.

### Update

Unless specified otherwise you should only have to replace ```pax.dll``` with the new one to update.

## Contributing

I'm not by any means a C++ dev, the code might not be as clean and readable as it could be. Despite that, any contribution is more than welcome, same goes for feature suggestions.

## Configuration

If a ```config.json``` file is located in the same folder as the main pax.dll. The loader will read it and look for some config options. 

Here is an example config :

```js
{
  //Used to check if the loader should actively close the game if LGE is detected
  "no_lge":  false,
  
  //Required allow you to set a list of mods that have to be loaded or the client will be closed
  "required": [
    { "name"  :  "weapon-unbloater", "version":  "0.1.1" },
    { "name"  :  "camera-tweaker", "version":  "any" }
  ],
  
  //Allowed acts like a "white-list" of mods that are allowed but not required.
  "allowed": [
    { "name"  :  "weapon-unbloater", "version":  "0.1.1" },
    { "name"  :  "camera-tweaker", "version":  "any" }
  ]
}
```
## Known Issues
- Problem: Sometimes the game will just get stuck in place at startup, happens rarely (~once out of 20-25 game launches)
  - Solution: Closing the game and restarting should work.   
- Problem: Very rarely the game crashes at startup (~once out of 15 launches)
  - Solution: Starting the game back up should do the trick.
