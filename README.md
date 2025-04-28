# trucker-hook
A mod for the game Trucker that lets you set any custom screen resolution and improves the positioning of UI elements on widescreen displays. It works by injecting a custom DLL in place of d3d8.dll. The hook is compatible with the game executable with MD5 hash ```...e9bace1c```.

## Features
- Support for any custom game resolution
- Improved UI scaling and alignment

## Known Issues
- The main menu is still not properly aligned
- Some custom resolutions may cause the game to fail to launch (this can be fixed by installing [DxWrapper](https://github.com/elishacloud/dxwrapper))

## Installation
1. Download the latest release from the Releases section on the right
2. Find your game installation directory (by default: ```C:\Program Files (x86)\Mirage Interactive\Trucker```) and place the downloaded file there
3. Run the game!

## Author

masuo / izawartka

## Trucker Enhancer

Trucker-hook is bundled with and can be easily installed using [Trucker Enhancer](https://github.com/zbieracz2000/Trucker-Enhancer) by [zbieracz2000](https://github.com/zbieracz2000).
