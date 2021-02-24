#include "TankAi.h"

////////////////////////////////////////////////////////////
TankAi::TankAi(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites)
	: m_aiBehaviour(AiBehaviour::PATROL_MAP)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
	, m_health(10)
{
	// Initialises the tank base and turret sprites.
	initSprites();

	vision.append(sf::Vertex{ sf::Vector2f(-1,-1), sf::Color::Black });

	for (int i = 1; i <= 10; i++)
	{
		vision.append(sf::Vertex{ sf::Vector2f(-1,-1), sf::Color::Black });
	}

	vision.append(sf::Vertex{ m_tankBase.getPosition(), sf::Color::White });
	vision.append(sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y * acos(FOV / 2) + 200) , sf::Color::White });
	vision.append(sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y + 200), sf::Color::White });
	vision.append(sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y * acos(FOV) + 200),  sf::Color::White });
	vision.append(sf::Vertex{ m_tankBase.getPosition(), sf::Color::White });
}

////////////////////////////////////////////////////////////
void TankAi::update(Tank const & playerTank, double dt)
{
	sf::Vector2f vectorToPlayer = seek(playerTank.getPosition());

	sf::Vector2f acceleration;

	switch (m_aiBehaviour)
	{
	case AiBehaviour::ATTACK_PLAYER:
		m_steering += thor::unitVector(vectorToPlayer);
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		acceleration = m_steering / MASS;
		//m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		m_velocity = MathUtility::truncate(m_velocity + acceleration, MAX_SPEED);
		break;
	case AiBehaviour::STOP:
		m_velocity = sf::Vector2f(0, 0);
		//motion->m_speed = 0;
		break;
	case AiBehaviour::PATROL_MAP:
		m_steering += thor::unitVector(seek(sf::Vector2f( rand () % ScreenSize::WIDTH, rand() % ScreenSize::HEIGHT)));
		m_steering += collisionAvoidance();
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		acceleration = m_steering / MASS;
		//m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		m_velocity = MathUtility::truncate(m_velocity + acceleration, MAX_SPEED);
		break;
	default:
		break;
	}

	if (m_tankBase.getPosition().x < 0 || m_tankBase.getPosition().x > ScreenSize::WIDTH)
	{
		m_velocity.x *= -1;
	}
	if (m_tankBase.getPosition().y < 0 || m_tankBase.getPosition().y > ScreenSize::HEIGHT)
	{
		m_velocity.y *= -1;
	}

	lookForPlayer(playerTank.getPosition());

	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

	auto currentRotation = m_rotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}
	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_rotation = static_cast<int>((m_rotation) + 1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		m_rotation = static_cast<int>((m_rotation) - 1) % 360;
	}
	updateMovement(dt);

	if (m_aiBehaviour == AiBehaviour::ATTACK_PLAYER)
	{
		requestFire();
	}
	

	if (m_fire)
	{
		m_shootTimer -= dt;
		if (m_shootTimer <= 0)
		{
			m_shootTimer = s_TIME_BETWEEN_SHOTS;
			m_fire = false;
		}
	}

	if (m_pool.update(dt, m_wallSprites, std::pair<sf::Sprite, sf::Sprite>(playerTank.getBase(), playerTank.getTurret())) > 0)
	{
		m_hitTarget = true;
	}
}

////////////////////////////////////////////////////////////
void TankAi::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	m_pool.render(window);
	window.draw(m_tankBase);
	window.draw(m_turret);
	window.draw(vision);
}

////////////////////////////////////////////////////////////
void TankAi::init(sf::Vector2f position)
{
	m_tankBase.setPosition(position);
	m_turret.setPosition(position);

	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().width * 1.5f);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
	m_health = 10;
}
bool TankAi::collidesWithPlayer(Tank const& playerTank) const
{
	// Checks if the AI tank has collided with the player tank.
	if (CollisionDetector::collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_tankBase, playerTank.getBase()))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::seek(sf::Vector2f playerPosition) const
{
	// This line is simply a placeholder...
	return sf::Vector2f(playerPosition - m_tankBase.getPosition());
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::collisionAvoidance()
{
	auto headingRadians = thor::toRadian(m_rotation);
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle();
	sf::Vector2f avoidance(0, 0);
	if (mostThreatening.getRadius() != 0.0)
	{		
		avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
		avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
	}
	else
	{
		avoidance *= 0.0f;
	}
	return avoidance;
}

////////////////////////////////////////////////////////////
const sf::CircleShape TankAi::findMostThreateningObstacle()
{
	// The initialisation of mostThreatening is just a placeholder...
	sf::CircleShape mostThreatening;

	for (auto circle : m_obstacles)
	{
		bool collides = MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, circle);

		float distance = MathUtility::distance(m_ahead, circle.getPosition());

		if (distance > (0.95 * circle.getRadius()) &&
			distance < (1.05 * circle.getRadius()))
		{
			continue;
		}

		if (collides && (mostThreatening.getRadius() == 0.0f || (MathUtility::distance(m_tankBase.getPosition(), circle.getPosition()) <
			MathUtility::distance(m_tankBase.getPosition(), mostThreatening.getPosition()))))
		{
			mostThreatening = circle;
		}
	}

	return mostThreatening;
}

sf::Sprite TankAi::getBaseSprite()
{
	return m_tankBase;
}

sf::Sprite TankAi::getTurretSprite()
{
	return m_turret;
}

void TankAi::takeDamage()
{
	m_health--;
}

int TankAi::getHealth()
{
	return m_health;
}

void TankAi::lookForPlayer(sf::Vector2f t_player)
{
	vision[0] = sf::Vertex{ m_tankBase.getPosition(), sf::Color::Black };
	vision[1] = sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y + (FOV/2)*200) , sf::Color::Black };
	vision[2] = sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y + (FOV / 2) * 200) , sf::Color::Black };
	vision[3] = sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y * 200), sf::Color::Black };
	vision[4] = sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y * 200), sf::Color::Black };
	vision[5] = sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y + (FOV) * 200),  sf::Color::Black };
	vision[6] = sf::Vertex{ sf::Vector2f(m_tankBase.getPosition().x, m_tankBase.getPosition().y + (FOV) * 200),  sf::Color::Black };
	vision[7] = sf::Vertex{ m_tankBase.getPosition(), sf::Color::Black };
	vision[8] = sf::Vertex{ m_tankBase.getPosition(), sf::Color::Black };
	vision[9] = sf::Vertex{ m_tankBase.getPosition(), sf::Color::Black };



	//if inside the cone and less then the spot distance
	if (thor::length(t_player - m_tankBase.getPosition()) <= SPOT_DISTANCE)
	{
		m_aiBehaviour = AiBehaviour::ATTACK_PLAYER;
	}
	else
	{
		m_aiBehaviour = AiBehaviour::PATROL_MAP;
	}
}

////////////////////////////////////////////////////////////
void TankAi::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(103, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
}


////////////////////////////////////////////////////////////
void TankAi::updateMovement(double dt)
{
	double speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000),
		m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000));
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_rotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation+m_turretRotation);
}

void TankAi::requestFire()
{
	m_fire = true;
	if (m_shootTimer == s_TIME_BETWEEN_SHOTS)
	{
		sf::Vector2f tipOfTurret(m_turret.getPosition().x + 2.0f, m_turret.getPosition().y);
		tipOfTurret.x += std::cos(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		tipOfTurret.y += std::sin(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		m_pool.create(m_texture, tipOfTurret.x, tipOfTurret.y, m_turret.getRotation());
	}
}