git push -u origin main# LAST ZONE - Interactive Battle Royale Simulation & Playable Runtime
# Mirrors C++ architecture of LastZone (UE5) with 50 Players, Chaos Zone, AI & Ballistics

import cv2
import numpy as np
import time
import math
import random

# Canvas & Window settings
WINDOW_NAME = "LAST ZONE - 50 Player Battle Royale (Playable Simulation)"
SCREEN_W, SCREEN_H = 1280, 720
MAP_SIZE = 4000.0 # 4km x 4km map coordinate space

# Colors (BGR)
COLOR_BG = (18, 28, 20)           # Dark tactical green terrain
COLOR_WATER = (85, 45, 20)        # Deep ocean/river blue
COLOR_ROAD = (45, 50, 48)         # Asphalt
COLOR_BUILDING = (60, 65, 60)     # Concrete structures
COLOR_SAFE_ZONE = (230, 160, 30)  # Electric cyan/blue circle
COLOR_NEXT_ZONE = (220, 220, 220) # White target circle
COLOR_PLAYER = (50, 220, 50)      # Player green
COLOR_BOT = (60, 60, 220)         # Enemy red
COLOR_BULLET = (50, 220, 255)     # Tracer yellow
COLOR_LOOT = (0, 215, 255)        # Gold pickup
COLOR_HUD_BG = (20, 22, 20)
COLOR_TEXT = (240, 240, 240)
COLOR_GOLD = (30, 215, 255)
COLOR_RED = (40, 40, 230)

class Bullet:
    def __init__(self, x, y, dx, dy, speed, damage, is_player):
        self.x = x
        self.y = y
        self.dx = dx
        self.dy = dy
        self.speed = speed
        self.damage = damage
        self.is_player = is_player
        self.life = 1.8 # seconds

class LootItem:
    def __init__(self, x, y, item_type, name):
        self.x = x
        self.y = y
        self.item_type = item_type # 'weapon', 'ammo', 'med', 'armor'
        self.name = name

class Character:
    def __init__(self, char_id, name, x, y, is_player=False):
        self.id = char_id
        self.name = name
        self.x = x
        self.y = y
        self.vx = 0.0
        self.vy = 0.0
        self.angle = 0.0 # radians
        self.is_player = is_player
        self.health = 100.0
        self.armor = 50.0
        self.max_health = 100.0
        self.is_alive = True
        self.kills = 0
        self.ammo_clip = 30
        self.ammo_reserve = 120
        self.speed = 220.0
        self.shoot_cooldown = 0.0
        self.target_enemy = None
        self.state = "looting" # 'looting', 'moving_zone', 'engaging'

class BattleRoyaleGame:
    def __init__(self):
        self.player = Character(0, "Player (You)", 2000.0, 2000.0, is_player=True)
        self.bots = []
        for i in range(1, 50):
            bx = random.uniform(500.0, 3500.0)
            by = random.uniform(500.0, 3500.0)
            self.bots.append(Character(i, f"Soldier_{i:02d}", bx, by))
        
        self.all_characters = [self.player] + self.bots
        self.bullets = []
        self.loot_items = []
        self.killfeed = [] # (text, time)
        
        # Spawn initial ground loot across island
        item_names = [("weapon", "LZ-416 AR"), ("ammo", "5.56mm Box"), ("med", "First Aid"), ("armor", "Vest Lv.2"), ("weapon", "LZ-12 Shotgun")]
        for _ in range(180):
            lx = random.uniform(300.0, 3700.0)
            ly = random.uniform(300.0, 3700.0)
            itype, iname = random.choice(item_names)
            self.loot_items.append(LootItem(lx, ly, itype, iname))

        # Safe Zone Phases matching C++ SafeZoneManager
        self.zone_center = [2000.0, 2000.0]
        self.zone_radius = 1800.0
        self.next_center = [2000.0, 2000.0]
        self.next_radius = 1200.0
        self.zone_phase = 1
        self.phase_timer = 45.0 # seconds before shrink
        self.is_shrinking = False
        self.shrink_duration = 30.0
        self.shrink_elapsed = 0.0
        self.prev_center = list(self.zone_center)
        self.prev_radius = self.zone_radius
        self.zone_dps = 2.0

        # Game state
        self.match_time = 0.0
        self.game_over = False
        self.victory = False
        self.mouse_world_x = 2000.0
        self.mouse_world_y = 2000.0
        self.mouse_pressed = False

    def advance_zone(self):
        self.zone_phase += 1
        self.is_shrinking = False
        self.phase_timer = max(20.0, 45.0 - (self.zone_phase * 4))
        self.zone_dps += 1.5

        # Choose new next circle inside current circle
        self.prev_center = list(self.zone_center)
        self.prev_radius = self.zone_radius
        self.next_radius = max(80.0, self.zone_radius * 0.65)
        
        max_offset = max(0.0, self.zone_radius - self.next_radius)
        ang = random.uniform(0, math.tau)
        dist = random.uniform(0, max_offset * 0.8)
        self.next_center = [self.zone_center[0] + math.cos(ang) * dist, self.zone_center[1] + math.sin(ang) * dist]

    def update(self, dt, keys):
        if self.game_over:
            return

        self.match_time += dt

        # Safe zone logic
        if not self.is_shrinking:
            self.phase_timer -= dt
            if self.phase_timer <= 0:
                self.is_shrinking = True
                self.shrink_elapsed = 0.0
        else:
            self.shrink_elapsed += dt
            alpha = min(1.0, self.shrink_elapsed / self.shrink_duration)
            self.zone_center[0] = (1 - alpha) * self.prev_center[0] + alpha * self.next_center[0]
            self.zone_center[1] = (1 - alpha) * self.prev_center[1] + alpha * self.next_center[1]
            self.zone_radius = (1 - alpha) * self.prev_radius + alpha * self.next_radius

            if alpha >= 1.0:
                self.advance_zone()

        # Update Player Controls
        if self.player.is_alive:
            vx, vy = 0.0, 0.0
            move_speed = self.player.speed
            if keys.get(ord('w')) or keys.get(ord('W')): vy -= 1.0
            if keys.get(ord('s')) or keys.get(ord('S')): vy += 1.0
            if keys.get(ord('a')) or keys.get(ord('A')): vx -= 1.0
            if keys.get(ord('d')) or keys.get(ord('D')): vx += 1.0

            # Sprint
            if keys.get(225) or keys.get(16): # Shift key
                move_speed *= 1.45

            mag = math.hypot(vx, vy)
            if mag > 0:
                self.player.x += (vx / mag) * move_speed * dt
                self.player.y += (vy / mag) * move_speed * dt

            # Look toward mouse
            dx = self.mouse_world_x - self.player.x
            dy = self.mouse_world_y - self.player.y
            self.player.angle = math.atan2(dy, dx)

            # Shooting
            if self.player.shoot_cooldown > 0:
                self.player.shoot_cooldown -= dt

            if self.mouse_pressed and self.player.shoot_cooldown <= 0:
                if self.player.ammo_clip > 0:
                    self.player.ammo_clip -= 1
                    self.player.shoot_cooldown = 0.11 # ~550 RPM
                    bx = self.player.x + math.cos(self.player.angle) * 22
                    by = self.player.y + math.sin(self.player.angle) * 22
                    # Small spread
                    spread = random.uniform(-0.04, 0.04)
                    ang = self.player.angle + spread
                    self.bullets.append(Bullet(bx, by, math.cos(ang), math.sin(ang), 1100.0, 36.0, True))
                else:
                    self.reload_character(self.player)

            # Reload key R
            if keys.get(ord('r')) or keys.get(ord('R')):
                self.reload_character(self.player)

            # Loot pickup (Walk over loot)
            for item in self.loot_items[:]:
                if math.hypot(self.player.x - item.x, self.player.y - item.y) < 45.0:
                    if item.item_type == "med":
                        self.player.health = min(self.player.max_health, self.player.health + 40.0)
                    elif item.item_type == "armor":
                        self.player.armor = 100.0
                    elif item.item_type == "ammo":
                        self.player.ammo_reserve += 60
                    self.loot_items.remove(item)

        # Update AI Bots
        for bot in self.bots:
            if not bot.is_alive:
                continue

            if bot.shoot_cooldown > 0:
                bot.shoot_cooldown -= dt

            # Safe zone distance
            dist_to_center = math.hypot(bot.x - self.zone_center[0], bot.y - self.zone_center[1])
            is_outside = dist_to_center > self.zone_radius

            # AI State Decision
            nearest_enemy = None
            closest_dist = 650.0 # Vision range 65m
            for other in self.all_characters:
                if other != bot and other.is_alive:
                    d = math.hypot(bot.x - other.x, bot.y - other.y)
                    if d < closest_dist:
                        closest_dist = d
                        nearest_enemy = other

            if is_outside or dist_to_center > self.next_radius * 0.9:
                # Move toward safe zone
                target_x, target_y = self.next_center
                angle_to_zone = math.atan2(target_y - bot.y, target_x - bot.x)
                bot.angle = angle_to_zone
                bot.x += math.cos(angle_to_zone) * bot.speed * dt
                bot.y += math.sin(angle_to_zone) * bot.speed * dt
            elif nearest_enemy:
                # Engage enemy
                dx = nearest_enemy.x - bot.x
                dy = nearest_enemy.y - bot.y
                bot.angle = math.atan2(dy, dx)
                
                # Move towards / strafe
                if closest_dist > 250.0:
                    bot.x += math.cos(bot.angle) * bot.speed * 0.6 * dt
                    bot.y += math.sin(bot.angle) * bot.speed * 0.6 * dt
                
                # Bot shooting
                if bot.shoot_cooldown <= 0:
                    bot.shoot_cooldown = random.uniform(0.18, 0.45)
                    spread = random.uniform(-0.12, 0.12)
                    bang = bot.angle + spread
                    bx = bot.x + math.cos(bang) * 20
                    by = bot.y + math.sin(bang) * 20
                    self.bullets.append(Bullet(bx, by, math.cos(bang), math.sin(bang), 950.0, 24.0, False))
            else:
                # Wander / patrol
                if random.random() < 0.02:
                    bot.angle += random.uniform(-1.0, 1.0)
                bot.x += math.cos(bot.angle) * bot.speed * 0.4 * dt
                bot.y += math.sin(bot.angle) * bot.speed * 0.4 * dt

            # Clamp bots inside map
            bot.x = max(100.0, min(MAP_SIZE - 100.0, bot.x))
            bot.y = max(100.0, min(MAP_SIZE - 100.0, bot.y))

        # Update Bullets & Collisions
        for b in self.bullets[:]:
            b.x += b.dx * b.speed * dt
            b.y += b.dy * b.speed * dt
            b.life -= dt
            if b.life <= 0:
                self.bullets.remove(b)
                continue

            # Check character hit
            for target in self.all_characters:
                if not target.is_alive:
                    continue
                if b.is_player and target.is_player:
                    continue
                if (not b.is_player) and (not target.is_player):
                    # Bots can shoot each other with 50% probability
                    if random.random() < 0.5:
                        continue

                dist = math.hypot(b.x - target.x, b.y - target.y)
                if dist < 18.0: # Character hit radius
                    dmg = b.damage
                    if target.armor > 0:
                        absorbed = dmg * 0.4
                        target.armor = max(0.0, target.armor - dmg)
                        dmg -= absorbed

                    target.health -= dmg
                    if b in self.bullets:
                        self.bullets.remove(b)

                    if target.health <= 0 and target.is_alive:
                        target.is_alive = False
                        killer_name = "Player" if b.is_player else "AI Bot"
                        if b.is_player:
                            self.player.kills += 1
                        self.add_killfeed(killer_name, target.name, "LZ-416")

                    break

        # Safe Zone Damage over Time
        for char in self.all_characters:
            if char.is_alive:
                dist = math.hypot(char.x - self.zone_center[0], char.y - self.zone_center[1])
                if dist > self.zone_radius:
                    char.health -= self.zone_dps * dt
                    if char.health <= 0:
                        char.is_alive = False
                        self.add_killfeed("The Zone", char.name, "Zone Collapse")

        # Check win / lose
        alive_count = sum(1 for c in self.all_characters if c.is_alive)
        if not self.player.is_alive and not self.game_over:
            self.game_over = True
            self.victory = False
        elif alive_count <= 1 and self.player.is_alive and not self.game_over:
            self.game_over = True
            self.victory = True

    def reload_character(self, char):
        needed = 30 - char.ammo_clip
        if needed > 0 and char.ammo_reserve > 0:
            load = min(needed, char.ammo_reserve)
            char.ammo_clip += load
            char.ammo_reserve -= load

    def add_killfeed(self, killer, victim, weapon):
        self.killfeed.append((f"{killer} [{weapon}] {victim}", time.time()))
        if len(self.killfeed) > 5:
            self.killfeed.pop(0)

    def render(self):
        frame = np.full((SCREEN_H, SCREEN_W, 3), COLOR_BG, dtype=np.uint8)

        # Camera centered on player
        cam_x = self.player.x
        cam_y = self.player.y

        def world_to_screen(wx, wy):
            sx = int((wx - cam_x) + SCREEN_W * 0.5)
            sy = int((wy - cam_y) + SCREEN_H * 0.5)
            return sx, sy

        # Draw grid lines for terrain scale
        grid_step = 250
        start_x = int((cam_x - SCREEN_W * 0.5) // grid_step * grid_step)
        start_y = int((cam_y - SCREEN_H * 0.5) // grid_step * grid_step)
        for gx in range(start_x, start_x + SCREEN_W + grid_step * 2, grid_step):
            sx, _ = world_to_screen(gx, 0)
            if 0 <= sx < SCREEN_W:
                cv2.line(frame, (sx, 0), (sx, SCREEN_H), (25, 38, 28), 1)
        for gy in range(start_y, start_y + SCREEN_H + grid_step * 2, grid_step):
            _, sy = world_to_screen(0, gy)
            if 0 <= sy < SCREEN_H:
                cv2.line(frame, (0, sy), (SCREEN_W, sy), (25, 38, 28), 1)

        # Draw POI Military Bases and Buildings
        pois = [
            ("Camp Vanguard Airfield", 2250, 1500, 300, 180),
            ("Novo-Grad Metropolis", 2000, 2000, 260, 260),
            ("Port Kholod", 3000, 2700, 240, 190),
            ("Mount Sokol Radar", 1200, 1000, 180, 180)
        ]
        for name, px, py, pw, ph in pois:
            sx, sy = world_to_screen(px, py)
            cv2.rectangle(frame, (sx - pw//2, sy - ph//2), (sx + pw//2, sy + ph//2), COLOR_BUILDING, -1)
            cv2.rectangle(frame, (sx - pw//2, sy - ph//2), (sx + pw//2, sy + ph//2), (80, 90, 80), 2)
            cv2.putText(frame, name, (sx - pw//2 + 8, sy - ph//2 + 22), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (160, 180, 160), 1)

        # Draw Safe Zones (Next Zone: White, Current Zone: Cyan)
        nsx, nsy = world_to_screen(self.next_center[0], self.next_center[1])
        cv2.circle(frame, (nsx, nsy), int(self.next_radius), COLOR_NEXT_ZONE, 2, cv2.LINE_AA)

        csx, csy = world_to_screen(self.zone_center[0], self.zone_center[1])
        cv2.circle(frame, (csx, csy), int(self.zone_radius), COLOR_SAFE_ZONE, 3, cv2.LINE_AA)

        # Draw Loot Items
        for item in self.loot_items:
            lsx, lsy = world_to_screen(item.x, item.y)
            if -20 < lsx < SCREEN_W + 20 and -20 < lsy < SCREEN_H + 20:
                cv2.circle(frame, (lsx, lsy), 5, COLOR_LOOT, -1)

        # Draw Bullets
        for b in self.bullets:
            bsx, bsy = world_to_screen(b.x, b.y)
            if 0 <= bsx < SCREEN_W and 0 <= bsy < SCREEN_H:
                end_x = int(bsx - b.dx * 12)
                end_y = int(bsy - b.dy * 12)
                cv2.line(frame, (bsx, bsy), (end_x, end_y), COLOR_BULLET, 2)

        # Draw Characters
        for char in self.all_characters:
            if not char.is_alive:
                continue
            cx, cy = world_to_screen(char.x, char.y)
            if -30 < cx < SCREEN_W + 30 and -30 < cy < SCREEN_H + 30:
                col = COLOR_PLAYER if char.is_player else COLOR_BOT
                # Body circle
                cv2.circle(frame, (cx, cy), 12, col, -1)
                cv2.circle(frame, (cx, cy), 12, (20, 20, 20), 2)
                # Gun barrel line
                gx = int(cx + math.cos(char.angle) * 22)
                gy = int(cy + math.sin(char.angle) * 22)
                cv2.line(frame, (cx, cy), (gx, gy), (180, 180, 180), 3)

                # Small health bar over head
                hp_w = 24
                hp_pct = max(0.0, char.health / char.max_health)
                cv2.rectangle(frame, (cx - hp_w//2, cy - 20), (cx + hp_w//2, cy - 16), (40, 40, 40), -1)
                cv2.rectangle(frame, (cx - hp_w//2, cy - 20), (cx - hp_w//2 + int(hp_w * hp_pct), cy - 16), (40, 200, 40), -1)

        # HUD Overlay - Top Bar (Dark military style)
        cv2.rectangle(frame, (0, 0), (SCREEN_W, 60), COLOR_HUD_BG, -1)
        cv2.line(frame, (0, 60), (SCREEN_W, 60), (45, 65, 45), 2)

        alive_count = sum(1 for c in self.all_characters if c.is_alive)
        cv2.putText(frame, "LAST ZONE", (24, 38), cv2.FONT_HERSHEY_DUPLEX, 0.85, COLOR_GOLD, 2)
        cv2.putText(frame, f"ALIVE: {alive_count}/50", (260, 38), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (50, 220, 50), 2)
        cv2.putText(frame, f"KILLS: {self.player.kills}", (460, 38), cv2.FONT_HERSHEY_SIMPLEX, 0.7, COLOR_TEXT, 2)

        # Zone Timer
        state_str = "SHRINKING" if self.is_shrinking else "SAFE TIME"
        timer_val = max(0, int(self.shrink_duration - self.shrink_elapsed if self.is_shrinking else self.phase_timer))
        cv2.putText(frame, f"ZONE: {state_str} [{timer_val:02d}s]", (630, 38), cv2.FONT_HERSHEY_SIMPLEX, 0.65, COLOR_SAFE_ZONE, 2)

        # Bottom Bar HUD - Player Health, Armor, Weapon
        cv2.rectangle(frame, (SCREEN_W//2 - 280, SCREEN_H - 95), (SCREEN_W//2 + 280, SCREEN_H - 15), COLOR_HUD_BG, -1)
        cv2.rectangle(frame, (SCREEN_W//2 - 280, SCREEN_H - 95), (SCREEN_W//2 + 280, SCREEN_H - 15), (45, 65, 45), 2)

        # Health bar (Red to Green)
        hp_ratio = max(0.0, self.player.health / 100.0)
        cv2.putText(frame, f"HP: {int(self.player.health)}", (SCREEN_W//2 - 260, SCREEN_H - 68), cv2.FONT_HERSHEY_SIMPLEX, 0.55, COLOR_TEXT, 1)
        cv2.rectangle(frame, (SCREEN_W//2 - 180, SCREEN_H - 80), (SCREEN_W//2 + 50, SCREEN_H - 62), (30, 30, 30), -1)
        cv2.rectangle(frame, (SCREEN_W//2 - 180, SCREEN_H - 80), (SCREEN_W//2 - 180 + int(230 * hp_ratio), SCREEN_H - 62), (40, 200, 60), -1)

        # Armor bar (Cyan)
        arm_ratio = max(0.0, self.player.armor / 100.0)
        cv2.putText(frame, f"ARM: {int(self.player.armor)}", (SCREEN_W//2 - 260, SCREEN_H - 35), cv2.FONT_HERSHEY_SIMPLEX, 0.55, COLOR_TEXT, 1)
        cv2.rectangle(frame, (SCREEN_W//2 - 180, SCREEN_H - 47), (SCREEN_W//2 + 50, SCREEN_H - 29), (30, 30, 30), -1)
        cv2.rectangle(frame, (SCREEN_W//2 - 180, SCREEN_H - 47), (SCREEN_W//2 - 180 + int(230 * arm_ratio), SCREEN_H - 29), (220, 160, 40), -1)

        # Ammo Counter & Weapon Name
        cv2.putText(frame, "LZ-416 (5.56mm)", (SCREEN_W//2 + 75, SCREEN_H - 62), cv2.FONT_HERSHEY_SIMPLEX, 0.6, COLOR_GOLD, 2)
        cv2.putText(frame, f"{self.player.ammo_clip} / {self.player.ammo_reserve}", (SCREEN_W//2 + 75, SCREEN_H - 32), cv2.FONT_HERSHEY_DUPLEX, 0.75, (255, 255, 255), 2)

        # Top-Right Killfeed
        now = time.time()
        ky = 90
        for text, ktime in self.killfeed[-5:]:
            if now - ktime < 7.0:
                cv2.putText(frame, text, (SCREEN_W - 380, ky), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 200), 1)
                ky += 22

        # Minimap (Top Right Radar Widget)
        mm_size = 150
        mm_x = SCREEN_W - mm_size - 15
        mm_y = 15
        cv2.rectangle(frame, (mm_x, mm_y), (mm_x + mm_size, mm_y + mm_size), (15, 20, 15), -1)
        cv2.rectangle(frame, (mm_x, mm_y), (mm_x + mm_size, mm_y + mm_size), (60, 80, 60), 2)

        # Scale map coords to minimap
        def to_mm(wx, wy):
            mx = int(mm_x + (wx / MAP_SIZE) * mm_size)
            my = int(mm_y + (wy / MAP_SIZE) * mm_size)
            return mx, my

        # Safe zones on minimap
        cx, cy = to_mm(self.zone_center[0], self.zone_center[1])
        cr = int((self.zone_radius / MAP_SIZE) * mm_size)
        cv2.circle(frame, (cx, cy), cr, COLOR_SAFE_ZONE, 1)

        # Player on minimap
        px, py = to_mm(self.player.x, self.player.y)
        cv2.circle(frame, (px, py), 3, (50, 255, 50), -1)

        # Crosshair cursor at mouse
        msx, msy = world_to_screen(self.mouse_world_x, self.mouse_world_y)
        cv2.drawMarker(frame, (msx, msy), COLOR_GOLD, cv2.MARKER_CROSS, 16, 2)

        # End of Match Screen
        if self.game_over:
            overlay = frame.copy()
            cv2.rectangle(overlay, (0, 0), (SCREEN_W, SCREEN_H), (0, 0, 0), -1)
            cv2.addWeighted(overlay, 0.75, frame, 0.25, 0, frame)

            if self.victory:
                cv2.putText(frame, "WINNER WINNER CHICKEN DINNER!", (SCREEN_W//2 - 380, SCREEN_H//2 - 60), cv2.FONT_HERSHEY_DUPLEX, 1.4, COLOR_GOLD, 3)
                cv2.putText(frame, f"#1 / 50 SURVIVORS  |  KILLS: {self.player.kills}", (SCREEN_W//2 - 220, SCREEN_H//2), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 255, 255), 2)
            else:
                cv2.putText(frame, "ELIMINATED - BETTER LUCK NEXT TIME", (SCREEN_W//2 - 390, SCREEN_H//2 - 60), cv2.FONT_HERSHEY_DUPLEX, 1.3, COLOR_RED, 3)
                rank = sum(1 for c in self.all_characters if c.is_alive) + 1
                cv2.putText(frame, f"RANK #{rank} / 50  |  KILLS: {self.player.kills}", (SCREEN_W//2 - 180, SCREEN_H//2), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 255, 255), 2)

            cv2.putText(frame, "Press 'Q' to Exit  |  'R' to Play Again", (SCREEN_W//2 - 200, SCREEN_H//2 + 65), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (180, 180, 180), 2)

        return frame

def mouse_callback(event, x, y, flags, param):
    game = param
    # Convert screen coords to world coords
    cam_x = game.player.x
    cam_y = game.player.y
    game.mouse_world_x = (x - SCREEN_W * 0.5) + cam_x
    game.mouse_world_y = (y - SCREEN_H * 0.5) + cam_y

    if event == cv2.EVENT_LBUTTONDOWN:
        game.mouse_pressed = True
    elif event == cv2.EVENT_LBUTTONUP:
        game.mouse_pressed = False

def run_game():
    cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_AUTOSIZE)
    game = BattleRoyaleGame()
    cv2.setMouseCallback(WINDOW_NAME, mouse_callback, game)

    keys = {}
    prev_time = time.time()

    print("\n==========================================")
    print("      LAST ZONE - BATTLE ROYALE GAME      ")
    print("==========================================")
    print("Controls:")
    print("  W, A, S, D  : Move character")
    print("  Mouse Move  : Aim crosshair")
    print("  Left Click  : Shoot (LZ-416 Assault Rifle)")
    print("  R           : Reload")
    print("  Shift       : Sprint")
    print("  Walk over gold dots to pick up Ammo, Meds, Armor")
    print("  Q / ESC     : Quit simulation")
    print("==========================================\n")

    while True:
        curr_time = time.time()
        dt = min(0.1, curr_time - prev_time)
        prev_time = curr_time

        # Update game loop
        game.update(dt, keys)

        # Render frame
        frame = game.render()
        cv2.imshow(WINDOW_NAME, frame)

        # Key handling
        key = cv2.waitKey(16) & 0xFF
        if key == ord('q') or key == ord('Q') or key == 27: # Q or ESC
            break
        elif key == ord('r') or key == ord('R'):
            if game.game_over:
                game = BattleRoyaleGame()
                cv2.setMouseCallback(WINDOW_NAME, mouse_callback, game)
            else:
                game.reload_character(game.player)
        elif key != 255:
            keys[key] = True

        # Clear key inputs that aren't being held
        for k in list(keys.keys()):
            if k not in [ord('w'), ord('W'), ord('a'), ord('A'), ord('s'), ord('S'), ord('d'), ord('D')]:
                keys[k] = False

    cv2.destroyAllWindows()

if __name__ == "__main__":
    run_game()

