#include "ProjectilePool.h"

////////////////////////////////////////////////////////////
void ProjectilePool::create(sf::Texture const & texture, double x, double y, double rotation)
{
	// If no projectiles available, simply re-use the next in sequence.
	if (m_poolFull)
	{
		m_nextAvailable = (m_nextAvailable + 1) % s_POOL_SIZE;
	}
	
	m_projectiles.at(m_nextAvailable).init(texture, x, y, rotation);
}

////////////////////////////////////////////////////////////
int ProjectilePool::update(double dt, std::vector<sf::Sprite> & wallSprites, std::pair<sf::Sprite, sf::Sprite> aiTankSprites)
{	
	// The number of active projectiles.
	int activeCount = 0;
	// Assume the pool is not full initially.
	m_poolFull = false;
	int m_hitCount = 0;

	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		m_result = m_projectiles.at(i).update(dt, wallSprites, aiTankSprites);

		if(m_result.first)
		{
			// If this projectile has expired, make it the next available.
			m_nextAvailable = i;
		}
		else
		{
			// So we know how many projectiles are active.
			activeCount++;
		}

		if (m_result.second)
		{
			m_hitCount++;
		}
	}
	// If no projectiles available, set a flag.
	if (s_POOL_SIZE == activeCount)
	{		
		m_poolFull = true;
	}

	return m_hitCount;
}

////////////////////////////////////////////////////////////
void ProjectilePool::render(sf::RenderWindow & window)
{
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		// If projectile is active...
		if (m_projectiles.at(i).inUse())
		{
			window.draw(m_projectiles.at(i).m_projectile);
		}
	}
}