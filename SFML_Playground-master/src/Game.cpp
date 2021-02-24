#include "Game.h"
#include <iostream>
#include <ctime>

// Updates per milliseconds
static double const MS_PER_UPDATE = 10.0;

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::WIDTH, ScreenSize::HEIGHT, 32), "SFML Playground", sf::Style::Default)
	, m_tank(m_texture, m_wallSprites),
	m_aiTank(m_texture, m_wallSprites),
	m_hud(m_font)
{
	srand(time(nullptr));

	m_window.setVerticalSyncEnabled(true);
	
	int currentLevel = 1; //set the current level
	
	//Will generate an exception if level loading fails

	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception& e)
	{
		std::cout << "Level loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}
	//Will generate exception when texture loading fails
	try
	{
		if (!m_texture.loadFromFile(".//resources//images//SpriteSheet.png"))
		{
			std::string s("Error loading texture");
			throw std::exception(s.c_str());
		}
	}
	catch (std::exception)
	{
		std::cout << "Error loading texture";
	}

	try
	{
		if (!m_targetTexture.loadFromFile(".//resources//images//E-100.png"))
		{
			std::string s("Error loading texture");
			throw std::exception(s.c_str());
		}
	}
	catch (std::exception)
	{
		std::cout << "Error loading texture";
	}

	try
	{
		if (!m_textFont.loadFromFile(".//resources//fonts//arial.ttf"))
		{
			std::string s("Error loading font");
			throw std::exception(s.c_str());
		}

		if (!m_font.loadFromFile("./resources/fonts/arial.ttf"))
		{
			std::string s("Error loading font");
			throw std::exception(s.c_str());
		}
	}
	catch (std::exception)
	{
		std::cout << "Error loading font";
	}

	try
	{
		if (!m_bgTexture.loadFromFile(m_level.m_background.m_fileName))
		{
			std::string s("ERror loading texture");
			throw std::exception(s.c_str());
		}

		m_bgSprite.setTexture(m_bgTexture);
	}
	catch (std::exception)
	{
		std::cout << "Error loading texture";
	}

	//set up the scoreboard location
	m_ScoreBoard = ".//resources//data//scores.txt";

	m_tank.setPosition(m_level.m_tank.m_position); //set the tank position
	
	generateWalls(); //call the function to generate walls
	generateTargets(); //call the function to generate targets

	m_time = 60; //set the time to 60
	m_timerText.setFont(m_textFont); //set the timer text font
	m_timerText.setPosition(450.0f, 0.0f); //set the timer text position

	m_targetDuration = 10; //set the target duration

	m_targetText.setFont(m_textFont); //set the alive timer text font 
	m_targetText.setPosition(0.0f, 0.0f); //set the alive timer text position

	m_healthText.setFont(m_textFont); //set the score text font
	m_healthText.setPosition(700.0f, 0.0f); //set the score text position


	m_accuracy = 100; //set accuracy to 100

	m_accuracyText.setFont(m_textFont); //set the accuracy text font
	m_accuracyText.setPosition(950.0f, 0.0f); //set the accuracy text position

	alpha.a = 100; //set the alpha to 100

	m_score = 0; //set the score to 0

	m_extraTime = 0; //set extra time to 0


	m_message.setFont(m_textFont); //set the message font
	m_message.setPosition(450.0f, 200.0f); //set the message position

	m_scoreBox.setFillColor(sf::Color{ 0,0,0,100 }); //set the score box fill colour
	m_scoreBox.setPosition(300.0f, 100.0f); //set the score box position
	m_scoreBox.setSize(sf::Vector2f(800.0f, 600.0f)); //set the score box size

	m_message1.setFont(m_textFont); //set the message 1 font
	m_message1.setPosition(450.0f, 100.0f); //set the message 1 position
	m_message1.setString("Press Esc to exit game"); //set the message 1 string

	m_message2.setFont(m_textFont); //set the message 2 font
	m_message2.setPosition(450.0f, 150.0f);//set the message 2 position
	m_message2.setString("Press Space to restart the game"); //set the message 2 string

	//Populate the obstacle list and set the AI tank position
	m_aiTank.init(m_level.m_aiTank.m_position);

}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Int32 lag = 0;

	while (m_window.isOpen())
	{
		sf::Time dt = clock.restart();

		lag += dt.asMilliseconds();

		processEvents();

		while (lag > MS_PER_UPDATE)
		{
			update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}
		update(MS_PER_UPDATE);

		render();
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}

		processGameEvents(event);
	}
	
}

////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_window.close();
			break;
		default:
			break;
		}
	}
}

void Game::generateWalls()
{

	sf::IntRect wallRect(2, 129, 33, 23);
	//Create the Walls
	for (ObstacleData const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0, wallRect.height / 2.0);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}

void Game::generateTargets()
{
	//Create the targets
	for (TargetData const& target : m_level.m_targets)
	{
		int offset = target.m_randomOffset;

		int randomOffset = rand() % (offset*2) + (-50);


		sf::Sprite sprite;
		sprite.setTexture(m_targetTexture);
		sprite.setScale(0.5f, 0.5f);
		sprite.setPosition(target.m_position.x + randomOffset, target.m_position.y + randomOffset);
		sprite.setRotation(target.m_rotation);
		m_targets.push_back(sprite);
	}
}

void Game::saveScores()
{
	std::ofstream outputFile; //create an output file stream
	outputFile.open(m_ScoreBoard, outputFile.app); //open the output file from the scoreboard string location and append the output
	if (outputFile.is_open()) //check if the file was opened
	{
		outputFile << "Score: " << std::to_string(m_score)<<"  Accuracy: " << std::to_string(m_accuracy); //Add core and accuracy to the file
		outputFile << std::endl; //make a new line

		outputFile.close(); //close the file
	}
}

void Game::getScores()
{
	m_scores.clear(); //clear the score string
	std::ifstream inputFile; //create and input file 
	inputFile.open(m_ScoreBoard); //open the input file from the scoreboard string location

	if (inputFile.is_open()) //check if file is open
	{
		while (!inputFile.eof()) //while the file is open
		{
			std::getline(inputFile, scores); //get the line and append it to scores
			m_scores += scores + "\n"; //add the scores and a new line to the text 
		}
	}
	m_message.setString(m_scores); //set the message string
}

void Game::restartGame()
{
	m_time = 60; //set the time to 60
	m_accuracy = 100; //set accuracy to 100
	targetsCollected = 0; //set the score to 0
	m_tank.setPosition(m_level.m_tank.m_position); //set the tank position
	m_tank.setHealth();
	m_aiTank.init(m_level.m_aiTank.m_position);

	for (int i = 0; i < m_targets.size(); i++) //loop for all the targets
	{

		//m_targets[i].setColor(sf::Color::White); //clear the target colour
		targetsAlive[i] = false;

	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{

	if (m_gameState==GameState::GAME_RUNNING) //if the game is not over
	{
		
		m_aiTank.update(m_tank, dt);

		if (m_time >= 1) //if time is more than or equal to 1
		{
			if (m_timer.getElapsedTime().asSeconds() >= 1) //if the timer is more than or equal to 1
			{
				m_timer.restart(); //restart the timer
				m_time--; //minus 1 from the timer
			}
		}

		m_targetText.setString("Targets: " + std::to_string(targetsCollected) + " out of " + std::to_string(MAX_TARGETS)); //set the alive timer text string
		m_timerText.setString("Time Left: " + std::to_string(m_time)); //set the timer text string
		m_healthText.setString("Health: " + std::to_string(m_tank.getHealth())); //set the score text string
		m_accuracyText.setString("Accuracy: " + std::to_string(static_cast<int>(m_accuracy)) + "%"); //set the accuracy text string

		m_tank.update(dt, targetsAlive, m_score, m_accuracy, m_aiTank); //call the tank update function

		for (int i = 0; i < m_targets.size(); i++) //loop for all the targets
		{
			if (m_time == m_level.m_targets[i].m_spawnTime) //if the time is equal to the target spawn time
			{
				targetsAlive[i] = true; //set the target alive to true
			}
		}

		if (m_time == 0) //if time is equal to 0
		{
			saveScores(); //call the save score function
		}

		if (m_aiTank.collidesWithPlayer(m_tank))
		{
			m_tank.takeDamage();
		}
		if (m_aiTank.m_hitTarget)
		{
			m_aiTank.m_hitTarget = false;
			m_tank.takeDamage();
		}
		if (m_tank.getHealth() <= 0)
		{
			m_gameState = GameState::GAME_LOSE;
		}
		for (int i = 0; i < m_targets.size(); i++)
		{
			if (targetsAlive[i])
			{
				if (CollisionDetector::collision(m_tank.getTurret(), m_targets[i]) ||
					CollisionDetector::collision(m_tank.getBase(), m_targets[i]))
				{
					targetsAlive[i] = false;
					targetsCollected++;
				}
			}
		}
		if (targetsCollected >= MAX_TARGETS)
		{
			m_gameState = GameState::GAME_WIN;
		}
	}
	else if(m_gameState==GameState::GAME_WIN)//other wise
	{
		getScores(); //call the get scores function

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) //if escape is pressed
		{
			m_window.close(); //close the window / game
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) //if space is pressed 
		{
			restartGame(); //restart the game
			m_gameState = GameState::GAME_RUNNING;
		}
	}
	else if (m_gameState == GameState::GAME_LOSE)//other wise
	{
		if (m_restartTimer.getElapsedTime().asSeconds() >= dt)
		{
			restartGame();
			m_gameState = GameState::GAME_RUNNING;
			m_restartTimer.restart();
		}
	}

	m_hud.update(m_gameState);
}

////////////////////////////////////////////////////////////
void Game::render()
{
	if (m_gameState == GameState::GAME_RUNNING) //if its not game over
	{
		m_window.clear(sf::Color(0, 0, 0, 0)); //clear the screen 

		m_window.draw(m_bgSprite); //draw the background sprite
		m_tank.render(m_window); //call the tank draw function
		m_aiTank.render(m_window);

		for (auto const& obstacle : m_wallSprites) //auto for loop for all the obstacles
		{
			m_window.draw(obstacle); //draw the obstacles
		}
		
		for (int i = 0; i < m_targets.size(); i++) //loop for all the targets
		{
			//if the target is alive and the time is less than the target spawn time
			if (m_time <= m_level.m_targets[i].m_spawnTime && targetsAlive[i]) 
			{
				m_window.draw(m_targets[i]); //draw the targets
			}
		}

		m_window.draw(m_timerText); //draw the timer text
		m_window.draw(m_targetText); //draw the alive timer text

		m_window.draw(m_healthText); //draw the score text

		m_window.draw(m_accuracyText); //draw the accuracy text

	}
	else if(m_gameState==GameState::GAME_WIN)//otherwise
	{
		m_window.draw(m_scoreBox); //draw the score box
		m_window.draw(m_message); //draw the message
		m_window.draw(m_message1); //draw the message 1
		m_window.draw(m_message2); //draw the message 2
	}
	else if (m_gameState == GameState::GAME_LOSE)//otherwise
	{

	}

	m_hud.render(m_window);

	m_window.display(); //display the window
}





