Here's a pure C++ game logic implementation for a Ninja action game:

cpp
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Constants
const float PLAYER_SPEED = 300.0f;
const float SHURIKEN_SPEED = 500.0f;
const float ENEMY_SPEED = 150.0f;
const float SPAWN_RATE = 1.5f;
const int MAX_ENEMIES = 20;
const int MAX_SHURIKENS = 10;
const float ATTACK_COOLDOWN = 0.3f;

// Forward declarations
struct Shuriken;
struct Enemy;
struct GameState;

// ========== STRUCTS ==========
struct Vector2 {
    float x, y;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    float length() const {
        return std::sqrt(x*x + y*y);
    }
    
    void normalize() {
        float len = length();
        if (len > 0) {
            x /= len;
            y /= len;
        }
    }
};

struct Ninja {
    Vector2 position;
    Vector2 velocity;
    int health = 100;
    int maxHealth = 100;
    float attackCooldown = 0;
    bool isAlive = true;
    int kills = 0;
    
    void update(float dt, const GameState& state);
    void takeDamage(int amount);
    void reset();
};

struct Shuriken {
    Vector2 position;
    Vector2 direction;
    bool active = false;
    float lifetime = 2.0f;
    
    void update(float dt);
    bool isOutOfBounds(float worldWidth, float worldHeight) const;
};

struct Enemy {
    Vector2 position;
    Vector2 velocity;
    int health = 30;
    int maxHealth = 30;
    bool active = false;
    float damageCooldown = 0;
    
    void update(float dt, const Ninja& ninja, GameState& state);
    void takeDamage(int amount);
    bool isCollidingWithNinja(const Ninja& ninja) const;
};

struct GameState {
    Ninja player;
    std::vector<Shuriken> shurikens;
    std::vector<Enemy> enemies;
    
    int score = 0;
    float timeSinceLastSpawn = 0;
    float gameTime = 0;
    bool isGameOver = false;
    
    // World boundaries
    float worldWidth = 1600;
    float worldHeight = 900;
    
    GameState() {
        shurikens.resize(MAX_SHURIKENS);
        enemies.resize(MAX_ENEMIES);
        reset();
    }
    
    void reset();
    void spawnEnemy();
    Shuriken* getAvailableShuriken();
    Enemy* getAvailableEnemy();
};

// ========== NINJA METHODS ==========
void Ninja::update(float dt, const GameState& state) {
    if (!isAlive) return;
    
    // Update position with boundary checking
    position = position + velocity * dt;
    position.x = std::clamp(position.x, 0.0f, state.worldWidth);
    position.y = std::clamp(position.y, 0.0f, state.worldHeight);
    
    // Update attack cooldown
    if (attackCooldown > 0) {
        attackCooldown -= dt;
    }
}

void Ninja::takeDamage(int amount) {
    if (!isAlive) return;
    
    health -= amount;
    if (health <= 0) {
        health = 0;
        isAlive = false;
    }
}

void Ninja::reset() {
    position = Vector2(400, 300);
    velocity = Vector2(0, 0);
    health = maxHealth;
    isAlive = true;
    kills = 0;
    attackCooldown = 0;
}

// ========== SHURIKEN METHODS ==========
void Shuriken::update(float dt) {
    if (!active) return;
    
    position = position + direction * SHURIKEN_SPEED * dt;
    lifetime -= dt;
    
    if (lifetime <= 0) {
        active = false;
    }
}

bool Shuriken::isOutOfBounds(float worldWidth, float worldHeight) const {
    return position.x < 0 || position.x > worldWidth || 
           position.y < 0 || position.y > worldHeight;
}

// ========== ENEMY METHODS ==========
void Enemy::update(float dt, const Ninja& ninja, GameState& state) {
    if (!active) return;
    
    // Move towards ninja
    Vector2 toNinja = ninja.position - position;
    float distance = toNinja.length();
    
    if (distance > 0) {
        toNinja.normalize();
        velocity = toNinja * ENEMY_SPEED;
        position = position + velocity * dt;
    }
    
    // Check collision with ninja
    if (isCollidingWithNinja(ninja) && damageCooldown <= 0) {
        damageCooldown = 1.0f;
    }
    
    if (damageCooldown > 0) {
        damageCooldown -= dt;
    }
    
    // Deactivate if out of bounds
    if (position.x < -100 || position.x > state.worldWidth + 100 ||
        position.y < -100 || position.y > state.worldHeight + 100) {
        active = false;
    }
}

void Enemy::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        active = false;
    }
}

bool Enemy::isCollidingWithNinja(const Ninja& ninja) const {
    if (!ninja.isAlive) return false;
    
    float dx = position.x - ninja.position.x;
    float dy = position.y - ninja.position.y;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    return distance < 40.0f; // Collision radius
}

// ========== GAME STATE METHODS ==========
void GameState::reset() {
    player.reset();
    score = 0;
    gameTime = 0;
    timeSinceLastSpawn = 0;
    isGameOver = false;
    
    for (auto& shuriken : shurikens) {
        shuriken.active = false;
    }
    
    for (auto& enemy : enemies) {
        enemy.active = false;
    }
}

void GameState::spawnEnemy() {
    Enemy* enemy = getAvailableEnemy();
    if (!enemy) return;
    
    // Random edge spawn
    int side = rand() % 4;
    switch(side) {
        case 0: // Top
            enemy->position = Vector2(rand() % (int)worldWidth, -50);
            break;
        case 1: // Right
            enemy->position = Vector2(worldWidth + 50, rand() % (int)worldHeight);
            break;
        case 2: // Bottom
            enemy->position = Vector2(rand() % (int)worldWidth, worldHeight + 50);
            break;
        case 3: // Left
            enemy->position = Vector2(-50, rand() % (int)worldHeight);
            break;
    }
    
    enemy->health = enemy->maxHealth;
    enemy->active = true;
    enemy->damageCooldown = 0;
}

Shuriken* GameState::getAvailableShuriken() {
    for (auto& shuriken : shurikens) {
        if (!shuriken.active) {
            return &shuriken;
        }
    }
    return nullptr;
}

Enemy* GameState::getAvailableEnemy() {
    for (auto& enemy : enemies) {
        if (!enemy.active) {
            return &enemy;
        }
    }
    return nullptr;
}

// ========== MAIN UPDATE FUNCTION ==========
void updateGameState(GameState& state, float dt, 
                     const Vector2& inputDirection, 
                     bool wantsToAttack,
                     bool wantsToDash) {
    
    if (state.isGameOver) return;
    
    state.gameTime += dt;
    
    // Update player
    state.player.velocity = inputDirection * PLAYER_SPEED;
    state.player.update(dt, state);
    
    // Handle attack
    if (wantsToAttack && state.player.attackCooldown <= 0 && state.player.isAlive) {
        Shuriken* shuriken = state.getAvailableShuriken();
        if (shuriken) {
            shuriken->position = state.player.position;
            
            // Attack in input direction, or forward if no input
            if (inputDirection.length() > 0) {
                shuriken->direction = inputDirection;
                shuriken->direction.normalize();
            } else {
                shuriken->direction = Vector2(1, 0); // Default right
            }
            
            shuriken->active = true;
            shuriken->lifetime = 2.0f;
            state.player.attackCooldown = ATTACK_COOLDOWN;
        }
    }
    
    // Update shurikens and check collisions
    for (auto& shuriken : state.shurikens) {
        if (!shuriken.active) continue;
        
        shuriken.update(dt);
        
        if (shuriken.isOutOfBounds(state.worldWidth, state.worldHeight)) {
            shuriken.active = false;
            continue;
        }
        
        // Check collision with enemies
        for (auto& enemy : state.enemies) {
            if (!enemy.active) continue;
            
            float dx = shuriken.position.x - enemy.position.x;
            float dy = shuriken.position.y - enemy.position.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            
            if (distance < 25.0f) { // Hit radius
                enemy.takeDamage(10);
                shuriken.active = false;
                
                if (!enemy.active) {
                    state.player.kills++;
                    state.score += 100;
                }
                break;
            }
        }
    }
    
    // Update enemies and check collisions with player
    int activeEnemies = 0;
    for (auto& enemy : state.enemies) {
        if (!enemy.active) continue;
        
        enemy.update(dt, state.player, state);
        
        // Check collision with player
        if (enemy.isCollidingWithNinja(state.player) && enemy.damageCooldown <= 0) {
            state.player.takeDamage(10);
            enemy.damageCooldown = 1.0f;
        }
        
        if (enemy.active) activeEnemies++;
    }
    
    // Spawn new enemies
    state.timeSinceLastSpawn += dt;
    if (state.timeSinceLastSpawn >= SPAWN_RATE && activeEnemies < MAX_ENEMIES) {
        state.spawnEnemy();
        state.timeSinceLastSpawn = 0;
        
        // Increase spawn rate over time
        if (state.gameTime > 30) {
            state.timeSinceLastSpawn = -0.5f; // Faster spawns
        }
    }
    
    // Check game over
    if (!state.player.isAlive) {
        state.isGameOver = true;
    }
    
    // Bonus score for surviving
    state.score += (int)(dt * 10);
}


**Usage Example:**
cpp
// In your main game loop:
GameState gameState;
float deltaTime = 1.0f/60.0f; // 60 FPS

// Input example (these would come from your input system)
Vector2 inputDir(1.0f, 0.0f); // Moving right
bool wantsAttack = true;
bool wantsDash = false;

updateGameState(gameState, deltaTime, inputDir, wantsAttack, wantsDash);

// Access game state:
int currentScore = gameState.score;
bool isGameOver = gameState.isGameOver;
int playerHealth = gameState.player.health;


This implementation includes:
- Ninja player with movement, attacking, and health
- Shuriken projectiles with collision detection
- Enemy AI that chases the player
- Dynamic spawning system
- Score tracking with kill bonuses and time bonus
- Game state management
- Clean separation from rendering/engine code

The code is structured to receive input and delta time, then update all game logic accordingly.