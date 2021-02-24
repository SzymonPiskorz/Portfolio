#pragma once
#include <SFML/Graphics.hpp>
#include "MathUtility.h"
#include "CollisionDetector.h"
#include "ScreenSize.h"
#include "ProjectilePool.h"
#include "TankAI.h"


/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	Tank(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites);
	void update(double dt, bool targetsAlive[], int & score, float& accu, TankAi& aiTank);
	void render(sf::RenderWindow & window);
	void setPosition(sf::Vector2f & pos);

	void takeDamage();

	/// <summary>
	/// @brief Increases the speed by 1, max speed is capped at 100.
	/// </summary>
	void increaseSpeed();
	/// <summary>
	/// @brief Decreases the speed by 1, min speed is capped at -100.
	/// </summary>
	void decreaseSpeed();
	/// <summary>
	/// @brief Increases the rotation by 1 degree, wraps to 0 degrees after 359.
	/// </summary>
	void increaseRotation();
	/// <summary>
	/// @brief Decreases the rotation by 1 degree, wraps to 359 degrees after 0.
	/// </summary>
	void decreaseRotation();
	/// <summary>
	/// @brief Processes control keys and applies speed/rotation as appropiate
	/// </summary>
	void handleKeyInput();
	/// <summary>
	/// @brief Increases the rotation by 1 degree, wraps to 0 degrees after 359.
	/// </summary>
	void increaseTurretRotation();
	/// <summary>
	/// @brief Decreases the rotation by 1 degree, wraps to 359 degrees after 0.
	/// </summary>
	void decreaseTurretRotation();
	/// <summary>
	/// @brief Centres the turret to the tank
	/// </summary>
	void centreTurret();
	/// <summary>
	/// @brief Check for collisions between the tank and walls.
	/// 
	/// </summary>
	/// <returns>True is collision detected between tank and wall.</returns>
	bool checkWallCollision();
	/// <summary>
	/// @brief Stops the tank if moving and applies a small increase in speed in the opposite direction of travel.
	/// If the tank speed is currently 0, the rotation is set to a value that is less than the previous rotation value
	///  (scenario: tank is stopped and rotates into a wall, so it gets rotated towards the opposite direction).
	/// If the tank is moving, further rotations are disabled and the previous tank position is restored.
	/// The tank speed is adjusted so that it will travel slowly in the opposite direction. The tank rotation
	///  is also adjusted as above if necessary (scenario: tank is both moving and rotating, upon wall collision it's 
	///  speed is reversed but with a smaller magnitude, while it is rotated in the opposite direction of it's 
	///  pre-collision rotation).
	/// </summary> 
	void deflect();

	void adjustRotation();


	/// <summary>
	/// @brief Generates a request to fire a projectile.
	/// A projectile will be fired from the tip of the turret only if time limit between shots
	/// has expired. Fire requests are not queued.
	/// </summary>
	void requestFire();
	
	/// <summary>
	/// @brief Reads the player tank position.
	/// <returns>The tank base position.</returns>
	/// </summary>
	sf::Vector2f getPosition() const;

	sf::Sprite getTurret() const { return m_turret; }
	sf::Sprite getBase() const { return m_tankBase; }

	int getHealth();
	void setHealth();
private:
	void initSprites(); //function to setup sprites
	sf::Sprite m_tankBase; //sprite for the tank base
	sf::Sprite m_turret; //sprite for the turret 
	sf::Texture const & m_texture; //constand texture vaiable

	double m_speed{ 0.0 }; //variable for the speed of the tank
	double m_rotation{ 0.0 }; //variable for the rotation of the tank
	double MAX_SPEED{ 50.0 }; //constatnt variable for the max speed of the tank
	double const MIN_SPEED{ -50.0 }; //constatnt variable for the min speed of the tank
	double const FRICTION{ 0.99 }; //constatnt variable for the friction
	double m_turretRotation{ 0.0 }; //variable for the rotation of the turret
	bool m_centeringTurret{ false }; //bool for centering the turret
	
	std::vector<sf::Sprite>& m_wallSprites; //a reference vector of sprites for the waals

	bool m_enablerotation{ true }; //bool to enable rotation
	sf::Vector2f m_previousPosition{ 0.0f, 0.0f }; //vector to keep the previous position of the tank
	double m_previousSpeed{ 0.0 }; //variable to keep the previous speed of the tank
	double m_previousRotation{ 0.0 }; //variable to keep the previous rotation of the tank
	double m_previousTurretRotation{ 0.0 }; //variable to keep the previous rotation of the turret
	sf::Vector2f m_previousTurretPosition{ 0.0f, 0.0f }; //vector to keep the previous position of the turret
	ProjectilePool m_pool;

	bool m_fireRequested{ false };
	static int const s_TIME_BETWEEN_SHOTS{ 800 };
	int m_shootTimer{ 800 };

	std::pair<sf::Sprite, sf::Sprite> m_aiTank;

	int m_health;

	sf::Sprite topBorder;
	sf::Sprite bottomBorder;
	sf::Sprite leftBorder;
	sf::Sprite rightBorder;
};
