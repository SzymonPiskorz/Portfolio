#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "LevelLoader.h"
#include "Tank.h"
#include <fstream>
#include "TankAI.h"
#include "GameState.h"
#include "HUD.h"

/// <summary>
/// @author RP
/// @date June 2017
/// @version 1.0
/// 
/// </summary>

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop results (lag) is stored. If this value is 
	///  greater than the notional time for one loop (MS_PER_UPDATE), then additional updates will be 
	///  performed until the lag is less than the notional time for one loop.
	/// The target is one update and one render cycle per game loop, but slower PCs may 
	///  perform more update than render operations in one loop.
	/// </summary>
	void run();

protected:
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	void processGameEvents(sf::Event&);

	/// <summary>
	/// @brief Creates the wall sprites and loads them into a vector.
	/// Note that sf::Sprite is considered a light weight class, so
	/// storing copies (instead od pointers to sf::Sprite) in std::vector is acceptable.
	/// </summary>
	void generateWalls();

	//generate targets and put them into a vector
	void generateTargets();

	//function to save the player scores
	void saveScores();

	//function to get the player scores
	void getScores();

	//function to restart the game
	void restartGame();


	// main window
	sf::RenderWindow m_window;
	LevelData m_level; //variable for the level
	sf::Texture m_bgTexture; //variable for the  background texture
	sf::Sprite m_bgSprite; //background sprite
	Tank m_tank; //tank object
	//An instance representing the AI controlled tank
	TankAi m_aiTank;
	sf::Texture m_texture; //texture variable
	sf::Font m_textFont; //font for the game
	sf::Text m_timerText; //timer text 
	sf::Clock m_timer; //timer to help controll the game
	int m_time; //time variable to display the remaining time
	sf::Texture m_targetTexture; //texture variable for the target
	int m_targetDuration; //target duration variable
	sf::Clock m_tankTimer;

	sf::Text m_healthText; //text variable for the score
	sf::Text m_accuracyText; //text variable for the accuracy

	//Wall sprites
	std::vector<sf::Sprite> m_wallSprites;
	//target sprites
	std::vector<sf::Sprite> m_targets;
	std::vector<sf::Vector2f>m_tankPos; //tank pos

	bool targetsAlive[10] = { false, false, false, false, false, false, false, false, false, false }; //array for the targets alive

	sf::Text m_targetText; //text variable for the alive timer
	sf::Clock m_aliveTimer; //a clock for the alive timer
	int m_aliveTime; // variable to help display alive time

	sf::Color alpha; //colour variable to control the alpha

	int m_score; //variable for the score

	int m_extraTime; //variable for the extra time

	float m_accuracy; //float for the accuracy

	std::string m_ScoreBoard; //string for the scoreboard text file location
	std::string m_scores; //string to get scores into 1 string
	std::string scores; //string to get text out of the file

	sf::Text m_message; //text variable
	sf::Text m_message1; //text variable
	sf::Text m_message2; //text variable

	

	sf::RectangleShape m_scoreBox; //rectangle shape for a score box


	//The initial state set to game running
	GameState m_gameState{ GameState::GAME_RUNNING };

	//The font used for the hud
	sf::Font m_font;

	//The game HUD instance
	HUD m_hud;
	
	sf::Clock m_restartTimer;

	int targetsCollected{ 0 };

	int const MAX_TARGETS{ 10 };

};
