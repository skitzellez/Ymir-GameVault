<p align="center">
  <img src="https://raw.githubusercontent.com/skitzellez/xbox-game-vault/main/GameVault/art/logo-banner.png" alt="Game Vault" width="720">
</p>

<h3 align="center">Game Vault · Ymir</h3>

<p align="center">
  <b>Sega Saturn games on your Xbox, with the controller.</b><br>
  The standalone Ymir Saturn emulator for <a href="https://github.com/skitzellez/xbox-game-vault">Game Vault</a> on <b>Xbox Series X|S</b> in Dev Mode:
  pick a Saturn game in Game Vault and it plays here, then you are back in Game Vault.
</p>

<!-- gv-buttons:start -->
<p align="center">
  <a href="https://github.com/skitzellez/Ymir-GameVault/releases/latest"><img alt="DOWNLOAD Ymir" src="https://img.shields.io/badge/DOWNLOAD-Ymir-107C10?style=for-the-badge&logo=xbox&logoColor=white"></a>
  &nbsp;
  <a href="https://github.com/skitzellez/xbox-game-vault"><img alt="Get Game Vault" src="https://img.shields.io/badge/GET-Game%20Vault-107C10?style=for-the-badge&logo=xbox&logoColor=white"></a>
  &nbsp;
  <a href="https://discord.gg/RmhhWgguvw"><img alt="Join the Game Vault Discord" src="https://img.shields.io/badge/JOIN-Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white"></a>
  &nbsp;
  <a href="https://www.patreon.com/Skitzelles"><img alt="Support on Patreon" src="https://img.shields.io/badge/SUPPORT-Patreon-F96854?style=for-the-badge&logo=patreon&logoColor=white"></a>
</p>
<!-- gv-buttons:end -->

<p align="center">
  <img src="xbox/Assets/SplashScreen.png" alt="Ymir" width="420">
</p>

## 🎮 What it is

[Ymir](https://github.com/StrikerX3/Ymir) is a modern, accurate Sega Saturn emulator by StrikerX3. This app is the full
standalone Ymir, made for Xbox Series X|S in Dev Mode. Game Vault sends your Saturn games to it by itself.

**No games or BIOS files are included.** Bring your own, and only play what you have the right to play.

## 1️⃣ 2️⃣ 3️⃣ How to play

| | |
|---|---|
| **1** | Install [Game Vault](https://github.com/skitzellez/xbox-game-vault). Its setup adds this app, or press RT in Game Vault to add it with your phone. |
| **2** | Put a Saturn BIOS from a console you own (`sega_101.bin` or `mpr-17933.bin`) in the `Game Vault\bios` folder on your games USB drive. Ymir finds it by itself. |
| **3** | Put your Saturn games (`.cue` + `.bin`, `.chd`, `.iso`, `.ccd`, `.mds`) on the drive, open the **SATURN** shelf in Game Vault and press A. |

## 🕹️ Controls

| Controller | Saturn pad |
|---|---|
| D-pad / Left stick | D-pad |
| X / A / B | A / B / C |
| LB / Y / RB | X / Y / Z |
| LT / RT | L / R |
| Menu | Start |
| Hold View + Menu | Quit, back to Game Vault |

## 📜 Credits and licence

- Ymir: © StrikerX3 and contributors, GPL-3.0: see [LICENSE](LICENSE).
- SDL, ImGui and the other libraries keep their own licences; the app carries them in its `licenses` folder.
- Xbox port: Skitzelles, in `xbox/`, `apps/ymir-sdl3/src/gv/` and the changes marked "Game Vault" in the source.

This is a modified version of Ymir, not made or supported by the Ymir project. Please report problems with this app to Game Vault, not to them.

<!-- gv-family:start -->
## 🎮 The Game Vault family

Game Vault is one launcher plus emulators built to work with it. **Install [Game Vault](https://github.com/skitzellez/xbox-game-vault) and it sets these up for you**; press **RT** in Game Vault to update them all.

**Apps**

| Project | What it is | Runs on |
|---|---|---|
| [Game Vault](https://github.com/skitzellez/xbox-game-vault) | The launcher: plug in a USB drive, pick a game, play. Sets up everything below by itself. | Xbox Series X&#124;S |
| [Game Vault for Android](https://github.com/skitzellez/game-vault-android) | The same game library for phones, tablets and Android TV (beta). | Android |
| [Game Vault for iPhone](https://github.com/skitzellez/game-vault-ios) | The Game Vault look on iPhone (early test). | iPhone |

**Emulators built for Game Vault**

| Project | What it is | Runs on |
|---|---|---|
| [xemu for Xbox Dev Mode](https://github.com/skitzellez/xemu-uwp) | Original Xbox | Xbox Series X&#124;S |
| [Xenia for Game Vault](https://github.com/skitzellez/Xenia-GameVault) | Xbox 360 | Xbox Series X&#124;S |
| [XBSX2 for Game Vault](https://github.com/skitzellez/XBSX2-GameVault) | PlayStation 2 | Xbox Series X&#124;S |
| [Dolphin for Game Vault](https://github.com/skitzellez/Dolphin-GameVault) | GameCube and Wii | Xbox Series X&#124;S |
| [Game Vault · Wii U (Cemu)](https://github.com/skitzellez/Cemu-GameVault) | Wii U | Xbox Series X&#124;S |
| [RPCS3 for Game Vault](https://github.com/skitzellez/RPCS3-GameVault) | PlayStation 3 | Xbox Series X&#124;S |
| [Game Vault · PS Vita](https://github.com/skitzellez/Vita-GameVault) | PlayStation Vita (Vitox / Vita3K) | Xbox Series X&#124;S |
| [Flycast for Game Vault](https://github.com/skitzellez/Flycast-GameVault) | Sega NAOMI, NAOMI 2 and Atomiswave arcade | Xbox Series X&#124;S |
| [RetroArch for Game Vault](https://github.com/skitzellez/RetroArch-GameVault) | PlayStation, Dreamcast, arcade, classic consoles and computers (80+ systems) | Xbox Series X&#124;S, Android |
| [PPSSPP for Game Vault](https://github.com/skitzellez/PPSSPP-GameVault) | PSP | Xbox Series X&#124;S |
| [**Game Vault · Ymir**](https://github.com/skitzellez/Ymir-GameVault) 📍 *you are here* | Sega Saturn | Xbox Series X&#124;S |
| [Game Vault · Azahar](https://github.com/skitzellez/Azahar-GameVault) | Nintendo 3DS | Xbox Series X&#124;S |
| [Game Vault · Ikemen GO](https://github.com/skitzellez/IkemenGO-GameVault) | MUGEN fighting games | Xbox Series X&#124;S |
| [Game Vault · Visual Pinball](https://github.com/skitzellez/VPinball-GameVault) | Pinball tables | Xbox Series X&#124;S |
| [Supermodel for Game Vault](https://github.com/skitzellez/Supermodel-GameVault) | Sega Model 3 arcade (inside RetroArch) | Xbox Series X&#124;S |
| [Game Vault · Arcade](https://github.com/skitzellez/Arcade-GameVault) | Windows arcade games (early test) | Xbox Series X&#124;S |
| [Game Vault · Flash](https://github.com/skitzellez/Flash-GameVault) | Flash games | Xbox Series X&#124;S |
| [Game Vault · OpenBOR](https://github.com/skitzellez/OpenBOR-GameVault) | OpenBOR beat 'em up games | Xbox Series X&#124;S |
| [Gambatte for Game Vault](https://github.com/skitzellez/Gambatte-GameVault) | Game Boy link cable in online rooms | Xbox Series X&#124;S, Android |
| [X1 BOX for Game Vault](https://github.com/skitzellez/x1box-gamevault) | Original Xbox | Android |

**Extras**

| Project | What it is | Runs on |
|---|---|---|
| [gamevault.fyi](https://gamevault.fyi) | The Game Vault website | Any browser |
| [Compatibility list](https://skitzellez.github.io/game-vault-compatibility/) | How games run, from real player reports | Any browser |
| [Free games](https://github.com/skitzellez/game-vault-free-games) | Freeware and shareware for the FREE GAMES tab | Game Vault |
| [Game Vault · Web](https://github.com/skitzellez/Web-GameVault) | Websites from the WEB menu, with the controller | Xbox Series X&#124;S |
| [Fallout 1in2 for Xbox](https://github.com/skitzellez/Fallout1in2-Xbox) | The original Fallout, made for the controller | Xbox Series X&#124;S |
| [Game Vault updates](https://github.com/skitzellez/game-vault-updates) | Update data Game Vault reads when it starts | Game Vault |

**💬 Help, news and bug reports:** [gamevault.fyi](https://gamevault.fyi), the [Game Vault Discord](https://discord.gg/RmhhWgguvw) (or this repo's Issues).
Please don't take problems with these builds to the original emulator teams: they don't make or support them.
<!-- gv-family:end -->

## ❤️ Support

Game Vault and its emulators are free and stay free. If you'd like to help, you can support it on **[Patreon](https://www.patreon.com/Skitzelles)**. ❤️
