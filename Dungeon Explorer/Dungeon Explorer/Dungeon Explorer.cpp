#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

// Character class as a base for Player and Enemy classes
class Character {
public:
    Character(int health, int attack) : health(health), attack(attack), gold(0) {}

    // Getters
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getGold() const { return gold; }
    int getExp() const { return experience; }
    int getLevel() const { return level; }
    int getMaxHealth() const { return maxHealth; }

    // Setters
    void setHealth(int health) { this->health = health; }
    void addGold(int amount) { gold += amount; }
    void takeDamage(int damage) { health -= damage; }
    void addExp(int exp) { experience += exp; }
    void addLevel(int l) { level += l; }
    void addAttack(int att) { attack += att; }
    void addMax(int max) { maxHealth += max; }

protected:
    int maxHealth = 20;
    int health;
    int attack;
    int gold;
    int experience = 0;
    int level = 0;

};

// Player class derived from Character class
class Player : public Character {
public:
    Player(int health, int attack) : Character(health, attack), gold(0), experience(0), level(1) {}

    // Getters
    int getGold() const { return gold; }
    int getExp() const { return experience; }
    int getLevel() const { return level; }
    int getRestCounter() const { return restCounter; }
    int getReqExp() const { return requiredExp; }

    // Setters
    void addGold(int amount) { gold += amount; }
    void addLevel(int l) { level += l; }
    void addExp(int exp) {
        experience += exp;
        checkLevelUp();
    }
    void decreaseRestCounter() { restCounter -= 1; }
    void resetRestCounter() { restCounter = 5; }

private:
    // Level up logic
    void checkLevelUp() {
        while (experience >= requiredExp) {
            experience -= requiredExp;
            level += 1;
            attack += 1;
            addMax(5);
            resetRestCounter();
            health = maxHealth;
            cout << "You leveled up! You are now level " << level << "!" << endl;
            requiredExp = static_cast<int>(requiredExp * 1.25);
        }
    }
    int requiredExp = 100;
    int gold;
    int experience;
    int level;
    int restCounter = 5;
};

// Enemy class derived from Character class
class Enemy : public Character {
public:
    Enemy(int health, int attack, int experience, int gold, const string& type) : Character(health, attack), type(type) {

        this->experience = experience;
        this->gold = gold;
    }
    // Getters
    string getType() const { return type; }
    int getGold() const { return gold; }
    int getExp() const { return experience; }


private:
    string type;
    int gold;
    int experience;
};

// Generates a vector of Enemies
vector<Enemy> generateEnemies() {
    vector<Enemy> enemies = {
        Enemy(5, 1, 20, 1, "Goblin"),
        Enemy(10, 2, 30, 2, "Orc"),
        Enemy(15, 4, 40, 4, "Troll"),
        Enemy(20, 6, 60, 10, "Dragon")
    };
    return enemies;
}

// Selects an enemy based on the player's level
Enemy getEnemyBasedOnLevel(const vector<Enemy>& enemies, int playerLevel) {
    int enemyIndex = min(playerLevel - 1, static_cast<int>(enemies.size()) - 1);
    return enemies[enemyIndex];
}

// Save player data to a file
void savePlayerData(const Player& player, const string& filename) {
    ofstream myfile(filename);
    if (myfile.is_open()) {
        myfile << player.getHealth() << " " << player.getAttack() << " " << player.getGold() << " " << player.getExp() << " " << player.getLevel() << " " << player.getRestCounter() << " " << player.getReqExp() << endl;
        myfile.close();
    }
    else {
        cerr << "Unable to open file for writing!" << endl;
    }
}

// Load player data from a file
Player loadPlayerData(const string& filename) {
    ifstream myfile(filename);
    if (myfile.is_open()) {
        int health, attack, gold, experience, level, restCounter, requiredExp;
        myfile >> health >> attack >> gold >> experience >> level >> restCounter >> requiredExp;
        myfile.close();

        Player loadedPlayer(health, attack);
        loadedPlayer.addGold(gold);
        loadedPlayer.addExp(experience);
        loadedPlayer.addLevel(level - 1);
        loadedPlayer.resetRestCounter();
        loadedPlayer.getReqExp();

        return loadedPlayer;
    }
    else {
        cerr << "Unable to open file for reading!" << endl;
        return Player(20, 2);
    }
}

int main() {
    // Seed the random number generator
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(static_cast<unsigned>(seed));
    uniform_int_distribution<int> attack_distribution(0, 2); // Determines range of random values applied to attack

    // Initialize game data
    vector<Enemy> enemies = generateEnemies();
    Player player(20, 2);

    int shop1 = 5;
    int shop2 = 10;
    int shop3 = 15;

    // Print introduction text
    cout << "You have always been fascinated by tales of adventure and heroism. From a young age, you have dreamed of exploring dungeons, fighting monsters, and claiming treasure.\n\n"
        << "As you grow older, you honed your skills, becoming a formidable warrior in your own right. When news of a dungeon filled with goblins, trolls, orcs, and dragons reached your ears, you knew you had to take on the challenge.\n\n"
        << "So picking up your pack and twirling your trusty blade, you set out to explore the depths of the dungeon and claim its riches.\n\n"
        << "Welcome to the dungeon!\n" << endl;


    // Main game loop
    while (player.getHealth() > 0) {
        // Print player status and options
        cout << "\nYou have " << player.getHealth() << " health, " << player.getGold() << " gold, " << player.getExp() << "/" << player.getReqExp() << " experience and you are level " << player.getLevel() << "." << endl;
        cout << "1. Explore the dungeon" << endl;
        cout << "2. Rest and heal" << endl;
        cout << "3. Visit the shop" << endl;
        cout << "4. Save the game" << endl;
        cout << "5. Load the game" << endl;
        cout << "6. Quit the game" << endl;

        int choice;
        int shopChoice = 0;
        bool bought = false;
        cin >> choice;

        // Handle player choice (Switch was used here to promote efficiency, so only one part of the code has to run with each input)
        switch (choice) {
        case 1:
        {
            // Spawn enemy based on player's level
            Enemy enemy = getEnemyBasedOnLevel(enemies, player.getLevel());
            cout << "\nYou explore the dungeon and encounter a " << enemy.getType() << "!" << endl;

            // Combat loop
            while (player.getHealth() > 0 && enemy.getHealth() > 0) {
                // Player attacks enemy
                int playerDamage = player.getAttack() + attack_distribution(generator);
                cout << "You attack the " << enemy.getType() << " and deal " << playerDamage << " damage." << endl;
                enemy.takeDamage(playerDamage);

                // Check if enemy is defeated
                if (enemy.getHealth() <= 0) {
                    cout << "You defeat the " << enemy.getType() << " and gain " << enemy.getGold() << " gold!" << endl;
                    player.addGold(enemy.getGold());
                    int gainedExp = enemy.getExp();
                    cout << "You gained " << gainedExp << " experience points!" << endl;
                    player.addExp(gainedExp);
                    break;
                }

                // Enemy attacks player
                int enemyDamage = enemy.getAttack() + attack_distribution(generator);
                cout << "The " << enemy.getType() << " attacks you and deals " << enemyDamage << " damage." << endl;
                player.takeDamage(enemyDamage);

                // Check if player is defeated
                if (player.getHealth() <= 0) {
                    cout << "Sadly, the valiant efforts of your hero have come to an end. The dungeon claims another brave adventurer, and your legacy will be remembered by all who dare to follow in your footsteps.\n\n"
                        << "With heavy hearts, we bid farewell to you, and hope that your next adventure will be met with greater success. Game Over!\n\n"
                        << "Your score: " << player.getLevel() << endl;
                    break;
                }
            }
        }

        break;
        case 2:
            // Rest and heal the player if rests are available
            if (player.getRestCounter() > 0) {
                cout << "\nYou rest and regain 2 health." << endl;
                player.setHealth(min(player.getHealth() + 2, player.getMaxHealth()));
                player.decreaseRestCounter();
                cout << "Rests remaining: " << player.getRestCounter() << endl;
            }
            else {
                cout << "\nYou have no rests remaining. You must level up to reset your rest counter." << endl;
            }
            break;
        case 3:
            // Shop functionality
            bought = false;
            while (shopChoice != 4) {
                // Print shop options
                cout << "\nYou have " << player.getHealth() << " health, " << player.getGold() << " gold, " << player.getExp() << "/" << player.getReqExp() << " experience and you are level " << player.getLevel() << "." << endl;
                cout << "1. Sharpen your sword(+1 Damage) - " << shop1 << " Gold" << endl;
                cout << "2. Drink health potion(+5 Max Health) - " << shop2 << " Gold" << endl;
                cout << "3. Drink Exp Potion(+150 Experience) - " << shop3 << " Gold" << endl;
                cout << "4. Leave shop" << endl;
                cin >> shopChoice;

                // Handle player choice in shop
                if ((shopChoice == 1) && (player.getGold() >= shop1)) {
                    player.addAttack(1);
                    player.addGold(-shop1);
                    shop1 += 5;
                    bought = true;
                    cout << "\nYou sharpened your sword! Your attack increased by 1." << endl;
                }
                if ((shopChoice == 2) && (player.getGold() >= shop2)) {
                    player.addMax(5);
                    player.addGold(-shop2);
                    shop2 += 10;
                    bought = true;
                    cout << "\nYou drank a health potion! Your max health increased by 5." << endl;
                }
                if ((shopChoice == 3) && (player.getGold() >= shop3)) {
                    player.addLevel(1);
                    player.addGold(-shop3);
                    shop3 += 15;
                    bought = true;
                    cout << "\nYou drank an Exp potion!" << endl;
                    player.addExp(150);
                }
                if (shopChoice == 4) {
                    break;
                }
                if ((shopChoice < 1) || (shopChoice > 4)) {
                    cout << "\nInvalid choice!" << endl;
                    bought = true;
                }
                if (bought == false) {
                    cout << "Not enough coins for that!" << endl;
                }
            }
            break;
        case 4:
        {
            // Save game
            cout << "Please enter a name with no spaces\n";
            string name;
            cin >> name;
            string filename = name + ".txt";
            cout << "Filename is " << filename << "\n";
            savePlayerData(player, filename);
            cout << "Game saved!" << endl;
        }
        break;
        case 5:
        {
            // Load game
            cout << "Please enter the name you used to save your game with no spaces\n";
            string name;
            cin >> name;
            string filename = name + ".txt";
            cout << "Filename is " << filename << "\n";
            player = loadPlayerData(filename);
            cout << "Game loaded!" << endl;
        }
        break;
        case 6:
            // Quit the game
            cout << "\nThanks for playing!" << endl;
            return 0;
        default:
            // Handle invalid choices
            cout << "\nInvalid choice!" << endl;
            break;
        }
    }

    return 0;
}