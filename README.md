# LAST ZONE
### AAA High-End Realistic 3D Battle Royale Game

**LAST ZONE** is a tactical, realistic 50-player 3D Battle Royale game developed in Unreal Engine 5 using C++, Chaos Physics, Nanite, Lumen Global Illumination, Virtual Shadow Maps, and the Enhanced Input System.

---

## 🎮 Key Features

- **Realistic Ballistics & Gunplay:** Bullet drop, muzzle velocity, drag, procedural recoil patterns, stance spread modifiers, and server-authoritative hit registration.
- **Chaos Vehicles:** 4x4 Off-road Jeep, Military Truck, Motorbike with dynamic lean, and Coastal Patrol Boat with buoyancy physics.
- **Battle Royale Circle & Safe Zone:** Dynamic multi-phase shrinking electrical storm barrier with mathematical inner circle packing and damage-over-time ticks.
- **Intelligent AI Bots:** Full behavior tree-driven opponents equipped with sight and hearing perception, looting state machines, tactical cover finding, and humanized burst shooting.
- **Modular Inventory & Armor System:** Backpack weight management, multi-tier armor vests and helmets that mitigate incoming ballistic damage with physical degradation.
- **Supply Drop Air Drops:** Air cargo crates deployed via parachute with red smoke signal flares and high-tier military gear.
- **Cross-Platform PC & Android:** Custom scalable graphics presets, dynamic resolution scaling, and responsive on-screen mobile virtual controls.

---

## ⌨️ Default Controls

| Action | PC (Keyboard & Mouse) | Mobile (Touch Interface) | Gamepad |
|---|---|---|---|
| **Move** | W / A / S / D | Left Virtual Stick | Left Thumbstick |
| **Look / Aim** | Mouse Movement | Right Screen Swipe | Right Thumbstick |
| **Fire** | Left Mouse Button | Fire Touch Button | Right Trigger |
| **Aim Down Sights (ADS)** | Right Mouse Button | ADS Toggle Button | Left Trigger |
| **Sprint** | Left Shift | Sprint Toggle | Left Stick Click |
| **Jump / Vault** | Spacebar | Jump Button | Face Button Bottom |
| **Crouch** | C | Crouch Button | Face Button Right |
| **Prone** | Z | Prone Button | D-Pad Down |
| **Reload** | R | Reload Button | Face Button Left |
| **Interact / Loot** | F | Interact Button | Face Button Top |
| **Inventory** | Tab | Bag Icon | Special Right |
| **Weapon Slots** | 1 / 2 / 3 | Quick Slot Buttons | D-Pad Left / Right |

---

## 📁 Source Code Structure

- `Source/LastZone/Core/`: Master `LastZoneGameMode`, `LastZoneGameState`, `LastZonePlayerState`, `LastZonePlayerController`, and `LastZoneGameInstance`.
- `Source/LastZone/Character/`: Player locomotion (`ALastZoneCharacter`), animation instance (`ULastZoneAnimInstance`), and components (`HealthComponent`, `ArmorComponent`, `StaminaComponent`, `InventoryComponent`).
- `Source/LastZone/Weapons/`: Weapon base, projectile ballistics (`ALastZoneProjectile`), hitscan, assault rifles, shotguns, throwable grenades (`ALastZoneGrenadeProjectile`), and melee weapons.
- `Source/LastZone/Vehicles/`: Wheeled vehicles (`ALastZoneCarVehicle`), bikes (`ALastZoneBikeVehicle`), and watercraft (`ALastZoneBoatVehicle`).
- `Source/LastZone/Zone/`: Safe zone shrink orchestrator (`ASafeZoneManager`) and 3D barrier visualizer (`ASafeZoneVisualizer`).
- `Source/LastZone/Loot/`: Ground item pickups, weighted loot generator, and parachute supply drop crates.
- `Source/LastZone/AI/`: AI Controller with Perception, AI Character, and Behavior Tree tasks (`BTTask_FindLoot`, `BTTask_MoveToZone`, `BTTask_EngageTarget`, `BTTask_TakeCover`, `BTService_EvaluateTactics`).
- `Source/LastZone/UI/`: In-game HUD (`ULastZoneMainHUDWidget`), minimap radar, inventory screen, killfeed, match results, mobile controls, and main menu.
- `Source/LastZone/Environment/`: Real-time 24h day/night cycle, atmospheric scattering, and volumetric fog controller (`ADayNightWeatherCycle`).

---

## 🚀 How to Run

### Option 1: Playable Simulation (Immediate - Ready Right Now)
Double-click [`Run_Game.bat`](file:///c:/Users/User/Desktop/pubg/Run_Game.bat) or execute from the terminal:
```bash
python PlayLastZone.py
```
This launches the full 50-player Battle Royale loop with real-time AI opponents, shrinking safe zones, ballistics, looting, and victory conditions directly on your screen.

### Option 2: Unreal Engine 5 Project
1. Right-click [`LastZone.uproject`](file:///c:/Users/User/Desktop/pubg/LastZone.uproject) and choose **Generate Visual Studio project files**.
2. Open `LastZone.sln` in Visual Studio and build solution (**Development Editor** | **Win64**).
3. Press **F5** to launch into the Unreal Engine 5 Editor and hit **Play In Editor (PIE)**.

---

## 🛠️ Build and Setup

Refer to [DEVELOPMENT_STATUS.md](file:///c:/Users/User/Desktop/pubg/DEVELOPMENT_STATUS.md) for full setup instructions, build target configurations, and dedicated server deployment guides.

