#include "Tank.h"

Tank::Tank(sf::Texture const & texture, std::vector<sf::Sprite>& wallSprites)
: m_texture(texture),
m_wallSprites(wallSprites),
m_health(10)
{
	initSprites(); //call the function to set up sprites
}

void Tank::update(double dt, bool targetsAlive[], int & score, float& accu, TankAi& aiTank)
{
	m_previousPosition = m_tankBase.getPosition(); //set the previous tank position to the current position
	m_previousTurretPosition = m_turret.getPosition(); //set the previous turret position to the current position

	handleKeyInput(); //call the function to handle input 

	m_tankBase.setRotation(m_rotation); //set the tank rotation 
	//get the new tank position by finding its direction multiplied by the speed
	double newTankX = m_tankBase.getPosition().x + cos(m_rotation* MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
	double newTankY = m_tankBase.getPosition().y + sin(m_rotation* MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);


	m_tankBase.setPosition(newTankX, newTankY); //set the tank position

	m_turret.setRotation(m_rotation+m_turretRotation); //set the turret rotation
	//get the new turret position by finding its direction multiplied by the speed
	double newTurretX = m_turret.getPosition().x + cos(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
	double newTurretY = m_turret.getPosition().y + sin(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);


	m_turret.setPosition(newTurretX, newTurretY); //set the turret position

	m_speed=std::clamp(m_speed, MIN_SPEED, MAX_SPEED); //set up a clam so the speed isn't more than max speed or less than min speed

	m_speed *= FRICTION; //multiply the speed by friction to slow down the tank

	if (checkWallCollision()) //check for wall collision
	{
		deflect(); //call the deflect function
	}
	else //else
	{
		m_enablerotation = true; //enable rotation
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		requestFire();
	}
	      
	if (m_fireRequested)
	{
		m_shootTimer -= dt;
		if (m_shootTimer <= 0)
		{
			m_shootTimer = s_TIME_BETWEEN_SHOTS;
			m_fireRequested = false;
		}
	}
	
	m_aiTank.first = aiTank.getBaseSprite();
	m_aiTank.second = aiTank.getTurretSprite();
	if (m_pool.update(dt, m_wallSprites, m_aiTank) > 0)
	{
		aiTank.takeDamage();
	}

}

void Tank::render(sf::RenderWindow & window) 
{
	//m_pool.render(window);
	window.draw(m_tankBase); //draw the tank base
	window.draw(m_turret); //draw the turret
}

void Tank::setPosition(sf::Vector2f & pos)
{
	int randPos = rand() % 4; //get a random number for the tank position

	if (randPos == 0) //top left
	{
		pos = { pos.x, pos.y }; //set the pos to pos
	}
	if (randPos == 1) //top right
	{
		pos = { ScreenSize::WIDTH - pos.x, pos.y }; //set the pos the lenght of the window - position
	}
	if (randPos == 2) //bottom left
	{
		pos = { pos.x, ScreenSize::HEIGHT - pos.y }; //set the pos the heigh of the window - position
	}
	if (randPos == 3) //bottom right
	{
		pos = { ScreenSize::WIDTH - pos.x, ScreenSize::HEIGHT - pos.y }; 
	}

	m_tankBase.setPosition(pos); //set the tank pos
	m_turret.setPosition(pos); //set the turret pos
	
}

void Tank::increaseSpeed()
{
	m_previousSpeed = m_speed; //set the previous speed to the current speed

	if (m_speed < 100.0) //if speed is less than 100
	{
		m_speed += 1; //add 1 to the speed
	}
}

void Tank::decreaseSpeed()
{
	m_previousSpeed = m_speed; //set the previous speed to the current speed

	if (m_speed > -100.0) //if speed is more than -100
	{
		m_speed -= 1; //minus 1 from the speed
	}
}

void Tank::increaseRotation()
{
	m_previousRotation = m_rotation; //set the previous rotation to the current rotation

	m_rotation += 1; //add 1 to the rotation
	if (m_rotation == 360.0) //if the rotation is 360
	{
		m_rotation = 0; //set the rotation to 0
	}
}


void Tank::decreaseRotation()
{
	m_previousRotation = m_rotation; //set the previous rotation to the current rotation

	m_rotation -= 1; //minus 1 from the rotation
	if (m_rotation == 0.0) //if the rotation is 0
	{
		m_rotation = 359.0; //set the rotation to 359
	}
}

void Tank::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  //if the right arrow is pressed
	{
		increaseRotation(); //increase the rotation
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) //if the left arrow is pressed
	{
		decreaseRotation(); //decrease the rotation
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) //if the up arrow is pressed
	{
		increaseSpeed(); //increase speed
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) //if the down arrow is pressed
	{
		decreaseSpeed(); //decrease speed
	}

	if (m_enablerotation) //if rotation is enabled
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) //if x is pressed
		{
			m_centeringTurret = false; //set centering turret to false
			increaseTurretRotation(); //increase the turret rotation
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) //if z is pressed
		{
			m_centeringTurret = false; //set centering turret to false
			decreaseTurretRotation(); //decrease the turret rotation
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) //if c is pressed
		{
			m_centeringTurret = true; //set centering turret to true
		}
		if (m_centeringTurret) //if centering turret is true
		{
			centreTurret(); //call the centre turret function
		}
	}
}

void Tank::increaseTurretRotation()
{
	m_previousTurretRotation = m_turretRotation; //set the previous turret rotation to the current turret rotation

	m_turretRotation += 1; //add 1 to the turret rotation
	if (m_turretRotation == 360.0) //if turret rotation is 360
	{
		m_turretRotation = 0; //set the turret rotation to 0
	}
}

void Tank::decreaseTurretRotation()
{
	m_previousTurretRotation = m_turretRotation; //set the previous turret rotation to the current turret rotation

	if (m_turretRotation == 0.0) //if the turret rotation is 0
	{
		m_turretRotation = 359.0; //set the turret rotation to 359
	}
	m_turretRotation -= 1; //minus 1 from the turret rotation
}

void Tank::centreTurret()
{
	if (m_turretRotation != 0 && m_turretRotation < 180) //if the turret rotation is not 0 and less then 180
	{
		decreaseTurretRotation(); //call the decrease turret rotation
	}
	else //else
	{
		increaseTurretRotation(); //call the increase turret rotation
	}
	if(m_turretRotation==0) //if turret rotation is 0
	{
		m_centeringTurret = false; //set the centering turret to false
	}
}

bool Tank::checkWallCollision()
{

	for (sf::Sprite const& sprite : m_wallSprites)
	{
		//Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			return true;
		}
	}

	return false;
}

void Tank::deflect()
{
	//In case tank was rotating
	adjustRotation();

	//If tank was moving
	if (m_speed != 0)
	{
		//Temporarily disable turret rotations on collision.
		m_enablerotation = false;
		//Back up to position in previous frame.
		m_tankBase.setPosition(m_previousPosition);
		m_turret.setPosition(m_previousTurretPosition);
		//Apply small force in opposite direction of travel
		if (m_previousSpeed < 0)
		{
			m_speed = 8;
		}
		else
		{
			m_speed = -8;
		}
	}
}

void Tank::adjustRotation()
{
	//If tank was rotating
	if (m_rotation != m_previousRotation)
	{
		if (m_rotation > m_previousRotation)
		{
			m_rotation = m_previousRotation - 1;
		}
		else
		{
			m_rotation = m_previousRotation + 1;
		}
	}

	//If Turret was rotating while tank was moving
	if (m_turretRotation != m_previousTurretRotation)
	{
		//Set the turret rotation back to its pre-collision value.
		m_turretRotation = m_previousTurretRotation;
	}
}

void Tank::requestFire()
{
	m_fireRequested = true;
	if (m_shootTimer == s_TIME_BETWEEN_SHOTS)
	{
		sf::Vector2f tipOfTurret(m_turret.getPosition().x + 2.0f, m_turret.getPosition().y);
		tipOfTurret.x += std::cos(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		tipOfTurret.y += std::sin(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		m_pool.create(m_texture, tipOfTurret.x, tipOfTurret.y, m_turret.getRotation());
	}
}

sf::Vector2f Tank::getPosition() const
{
	return m_tankBase.getPosition();
}

void Tank::takeDamage()
{
	int typeOfDamage = rand() % 2;

	if (typeOfDamage == 0)
	{
		m_health--;
	}
	else if (typeOfDamage == 1)
	{
		MAX_SPEED -= 0.1;
	}
}

int Tank::getHealth()
{
	return m_health;
}

void Tank::setHealth()
{
	m_health = 10;
}

void Tank::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);

	topBorder.setPosition(0, 0);
	bottomBorder.setPosition(0, ScreenSize::HEIGHT);
	leftBorder.setPosition(0, 0);
	rightBorder.setPosition(ScreenSize::WIDTH, 0);
}