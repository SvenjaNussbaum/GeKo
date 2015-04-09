#include "GeKo_Gameplay/Object/Player.h"
#include <stdexcept>

Player::Player(std::string playerName)
{
	Player(playerName, glm::vec3(0.0f));
}

Player::Player(std::string playerName, glm::vec3 spawnPoint)
{
	m_type = ObjectType::PLAYER;
	m_class = ClassType::PLAYER;

	m_spawnPoint = spawnPoint;
	m_position = m_spawnPoint;
	m_name = playerName;

	m_hunger = 10;
	m_hungerMax = 10;
	m_health = 10;
	m_healthMax = 10;
	m_strength = 1;

	m_inventory = new Inventory();

	std::pair<States, bool> s(States::HUNGER, false);
	m_states.push_back(s); 
	s.first = States::HEALTH;
	s.second = true;
	m_states.push_back(s); 

	m_direction = glm::vec3(0.0, 0.0, -1.0);
	m_deltaTime = 0.0;
	m_phi = 0.0;
	m_theta = 2.0;
}

Player::Player(){
}

Player::~Player(){
}

glm::vec3 Player::getSpawnPoint()
{
	return m_spawnPoint;
}

void Player::move(glm::vec3 newPosition)
{
	m_position = newPosition;
}

void Player::moveForward(){
	m_position.x += m_speed* m_deltaTime *m_direction.x;
	m_position.y += m_speed* m_deltaTime *m_direction.y;
	m_position.z += m_speed* m_deltaTime *m_direction.z;

	notify(*this, Object_Event::OBJECT_MOVED);
}

void Player::moveBackward(){
	m_position.x -= m_speed* m_deltaTime *m_direction.x;
	m_position.y -= m_speed* m_deltaTime *m_direction.y;
	m_position.z -= m_speed* m_deltaTime *m_direction.z;

	notify(*this, Object_Event::OBJECT_MOVED);
}

void Player::moveLeft(){
	glm::vec3 directionOrtho = glm::cross(m_direction, glm::vec3(0,1,0));
	m_position.x -= m_speed* m_deltaTime*directionOrtho.x;
	m_position.y -= m_speed* m_deltaTime*directionOrtho.y;
	m_position.z -= m_speed* m_deltaTime*directionOrtho.z;

	notify(*this, Object_Event::OBJECT_MOVED);
}

void Player::moveRight(){
	glm::vec3 directionOrtho = glm::cross(m_direction, glm::vec3(0, 1, 0));
	m_position.x += m_speed* m_deltaTime*directionOrtho.x;
	m_position.y += m_speed* m_deltaTime*directionOrtho.y;
	m_position.z += m_speed* m_deltaTime*directionOrtho.z;

	notify(*this, Object_Event::OBJECT_MOVED);
}

void Player::turnLeft(){

	m_phi += m_speed* m_deltaTime;
	if (m_phi < 0) m_phi += 2 * glm::pi<float>();
	else if (m_phi > 2 * glm::pi<float>()) m_phi -= 2 * glm::pi<float>();

	m_direction.x = sin(m_theta) * cos(m_phi);
	// y-direction only needed for flying
	//m_direction.y = cos(m_theta);
	m_direction.z = -sin(m_theta) * sin(m_phi);

	//TODO: OBJECT_MOVED mit m_direction noch anpassen
	notify(*this, Object_Event::OBJECT_ROTATED);
}

void Player::turnRight(){
	m_phi -= m_speed* m_deltaTime;
	if (m_phi < 0) m_phi += 2 * glm::pi<float>();
	else if (m_phi > 2 * glm::pi<float>()) m_phi -= 2 * glm::pi<float>();

	m_direction.x = sin(m_theta) * cos(m_phi);
	// y-direction only needed for flying
	//m_direction.y = cos(m_theta);
	m_direction.z = -sin(m_theta) * sin(m_phi);

	notify(*this, Object_Event::OBJECT_ROTATED);
}

void Player::update(){
	if (m_health == 0){
		std::cout << "Player: Died" << std::endl;
		notify(*this, Object_Event::OBJECT_STOPPED);
		setStates(States::HEALTH, false);
	}
	if (getStates(States::HEALTH)){ 
		std::cout << "<<<<<<<< UpdateMethod <<<<<<<<" << std::endl;
		updateStates();
		std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	}
}

void Player::rotateView(float leftRight, float upDown)
{
	m_viewDirection = glm::vec3(0.0, 0.0, -1.0);
	m_viewDirection = glm::rotate(m_viewDirection, upDown, glm::cross(m_viewDirection, glm::vec3(0.0f, -1.0f, 0.0f)));
	m_viewDirection = glm::rotate(m_viewDirection, leftRight, glm::vec3(0.0f, -1.0f, 0.0f));
	m_viewDirection.y = glm::clamp(m_viewDirection.y, -0.5f, 0.5f);
	notify(*this, Object_Event::OBJECT_ROTATED);
}

float Player::getPhi(){
	return m_phi;
}

void Player::updateSourcesInMap()
{
	for (std::map<Soundtype, std::string>::iterator i = m_soundMap.begin(); i != m_soundMap.end(); ++i)
	{
			m_sfh->updateSourcePosition(i->second , m_position);
	}
}

std::string Player::getSourceName(Soundtype type)
{
	try{
		return m_soundMap.at(type);
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "No Sound with this type was set!" <<std::endl;
		return "oor";
	}
}

void Player::setSourceName(Soundtype type , std::string sourceName, const char* filepath)
{
		m_sfh->generateSource(sourceName, m_position, filepath);
		m_soundMap.insert(std::pair<Soundtype, std::string>(type, sourceName));
}