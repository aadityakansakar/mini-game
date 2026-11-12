# LAST ZONE - BATTLE ROYALE ISLAND MAP DESIGN SPECIFICATION
**Map Name:** `Island_Main` ("Last Island" / Остров Последний)  
**Map Dimensions:** 8,000 meters x 8,000 meters (8 km x 8 km, 64 sq km total area)  
**World Partition Grid:** 64 x 64 cells (128m cell size) with Hierarchical Level of Detail (HLOD)  
**Terrain Technology:** Unreal Engine 5 Landscape System with Nanite tessellation, Chaos Landscape Physical Materials, Virtual Texturing (RVT).

---

## 1. MAJOR LANDMARKS & POINTS OF INTEREST (POI)

### 1.1. Airfield & Military Base ("Camp Vanguard") - South Island
- **Location:** X: [1000m, 3500m], Y: [-3800m, -2000m]
- **Tier:** High-Tier Loot (Military grade rifles, sniper attachments, Level 3 vests/helmets)
- **Structures:**
  - 2,400m concrete runway with taxiways and hangars.
  - 4x hardened subterranean underground bunker tunnels.
  - Air traffic control tower (5 stories, 360-degree sniper vantage).
  - Radar dishes, barracks, motor pool with guaranteed 4x4 Jeep spawns.

### 1.2. Central Metropolis ("Novo-Grad") - Center Island
- **Location:** X: [-800m, 1200m], Y: [-600m, 1400m]
- **Tier:** Ultra-High Density Urban Loot
- **Structures:**
  - Multi-story residential block apartments with interior rooms, stairs, and balconies.
  - Central Regional Hospital (3 wings, extensive medical supply generation).
  - Police Station with weapons armory and holding cells.
  - Commercial strip and bank with vault and rooftops.

### 1.3. Industrial Naval Port ("Port Kholod") - Northeast Coast
- **Location:** X: [2200m, 3800m], Y: [1800m, 3600m]
- **Tier:** High-Tier Industrial Loot
- **Structures:**
  - Container terminal with stacked shipping containers for intense close-quarters combat.
  - 2x cargo container freight ships docked alongside wharves.
  - Industrial gantry cranes (accessible via ladders for snipers).
  - Boat slipways with patrol boats and speedboats.

### 1.4. Mount Sokol Radar Observatory - Northwest Peak
- **Location:** X: [-3500m, -1800m], Y: [1200m, 3200m]
- **Elevation:** +420 meters above sea level
- **Tier:** Medium-High Sniper Tier
- **Structures:**
  - Massive geodesic radome structure.
  - Weather telemetry station and communication array.
  - Winding serpentine mountain roads with switchbacks and sheer cliff edges.

### 1.5. River Networks & Bridges
- River "Volitsa" splits the island from North to South, requiring players to either:
  - Cross over 3 major suspension highway bridges (choke points for ambushes).
  - Swim across (risking slow movement and vulnerability).
  - Navigate patrol boats up river channels.

---

## 2. BIOMES & VEGETATION
- **Pine & Spruce Dense Forests:** High canopy cover with scattered boulders and fallen logs providing natural cover.
- **Agricultural Farmlands:** Wheat fields, hay bales, wooden barns, and tractors.
- **Coastal Sandy Beaches & Rocky Shoals:** Open beachheads with shipwrecks and sea caves.
- **Wetland Marshes:** Shallow water bodies with reeds and tall grass slowing vehicle traversal.

---

## 3. RENDERING & TECHNICAL CONFIGURATION
- **Nanite Geometry:** Enabled for all rock formations, cliff faces, concrete structures, and architectural meshes.
- **Lumen Global Illumination:** Hardware ray tracing enabled for realistic bounce lighting through building windows and tree canopies.
- **Virtual Shadow Maps (VSM):** Pixel-perfect contact shadows from every blade of grass, foliage leaf, and fence post.
- **Volumetric Fog & Sky Atmosphere:** Real-time Rayleign/Mie scattering linked to `ADayNightWeatherCycle`.
- **Landscape Physical Materials:**
  - `PM_Grass`: Moderate friction, quiet footsteps.
  - `PM_Mud`: Reduced vehicle traction, heavy squelch audio.
  - `PM_Asphalt`: Maximum wheel grip and vehicle acceleration.
  - `PM_Sand`: High wheel slip.
  - `PM_Water`: Buoyancy activation, splash Niagara particles and sound.

