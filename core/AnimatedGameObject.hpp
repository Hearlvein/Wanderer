#pragma once

#include "GameObject.hpp"
#include "Animation.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

class AnimatedGameObject : public virtual GameObject
{
public:
	AnimatedGameObject()    // needs to be loaded with loadFromFile
	{}
	~AnimatedGameObject() {}

	bool loadFromFile(const std::string& filename)
	{
		using namespace std;
		ifstream animationFile(filename);

		if (animationFile)
		{
			string currentAnimationName;

			string prefixes[6] = {
				"#",
				"-",
				"frameCount: ",
				"durations: ",
				"subTextureCoords: ",
				"hitboxCoords: "
			};

			string line;
			while (getline(animationFile, line))
			{
				if (line.empty())	// empty line
				{
					//cout << "empty line in animation file: " << filename << endl;
					continue;
				}
				else if (line.substr(0, prefixes[0].size()) == prefixes[0])	// comment line
				{
					//cout << "comment line in animation file: " << filename << endl;
					continue;
				}
				else if (line.substr(0, prefixes[1].size()) == prefixes[1])	// animation name
				{
					currentAnimationName = line.substr(prefixes[0].size());
					m_animations[currentAnimationName].m_name = currentAnimationName;
					//cout << "setting new animation: " << currentAnimationName << endl;
				}
				else if (line.substr(0, prefixes[2].size()) == prefixes[2])	// frame count
				{
					istringstream lineStream(line.substr(prefixes[2].size()));
					int n;
					lineStream >> n;
					m_animations[currentAnimationName].m_frameCount = n;
					//cout << "setting frame count: " << n << endl;
				}
				else if (line.substr(0, prefixes[3].size()) == prefixes[3])	// durations
				{
					istringstream lineStream(line.substr(prefixes[3].size()));
					float f;
					while (lineStream >> f)	// must be called m_frameCount times
						m_animations[currentAnimationName].m_frameDurations.push_back(f);

					//cout << "setting durations"<< endl;
				}
				else if (line.substr(0, prefixes[4].size()) == prefixes[4])	// subTextures coords
				{
					istringstream lineStream(line.substr(prefixes[4].size()));
					int x, y, w, h;
					while (lineStream >> x >> y >> w >> h)	// must be called 4 * m_frameCount times
						m_animations[currentAnimationName].m_subTextureCoords.push_back({ x, y, w, h});
				}
				else if (line.substr(0, prefixes[5].size()) == prefixes[5])	// hitboxes coords
				{
					istringstream lineStream(line.substr(prefixes[5].size()));
					float x, y, w, h;
					while (lineStream >> x >> y >> w >> h)	// must be called 4 * m_frameCount times
                        m_animations[currentAnimationName].m_hitboxCoords.push_back({ x, y, w, h });
				}
			}

			// print data
			/*for (const auto& [name, animation] : m_animations)
            {
                cout << name << endl;
                cout << animation.m_frameCount << endl;
                for (float duration : animation.m_frameDurations)
                    cout << duration << " ";
                cout << endl;
                for (auto& subTextureCoords : animation.m_subTextureCoords)
                    cout << subTextureCoords.left << " " << subTextureCoords.top << " "
                    << subTextureCoords.width << " " << subTextureCoords.height << " ";
                cout << endl;
                for (auto& hitboxCoords : animation.m_hitboxCoords)
                    cout << hitboxCoords.x << " " << hitboxCoords.y << " "
                    << hitboxCoords.w << " " << hitboxCoords.h << " ";
                cout << endl;
            }*/

			return true;
		}
		else
		{
			cout << "failed to create animation file stream" << endl;
			return false;
		}
	}

	void setCurrentAnimationName(const std::string& newCurrentAnimationName)
	{
		// not already playing specified animation and existing key
		if (m_currentAnimationName != newCurrentAnimationName)
		{
			if (m_animations.find(newCurrentAnimationName) != m_animations.end())
			{
				m_currentAnimationName = newCurrentAnimationName;
				m_animations[m_currentAnimationName].reset();   // reset animation data from last play
			}
			else
				std::cout << newCurrentAnimationName << " is unknown" << std::endl;
		}
	}

	void increaseElapsedTime(float modifier)
	{
		auto& currentAnimation = m_animations[m_currentAnimationName];

	    currentAnimation.increaseElapsedTime(modifier);

	    if (currentAnimation.frameChanged())
        {
            auto& newTextureRect = getCurrentTextureRect();
            /* std::cout << "new texture rect: " << newTextureRect.left << " " << newTextureRect.top << " " <<
                newTextureRect.width << " " << newTextureRect.height << std::endl; */
            sf::Vector2f newHitboxSize(getRelativeHitbox().w, getRelativeHitbox().h);
            setTextureRect(newTextureRect/*, &newHitboxSize*/);
        }
	}

	const sf::IntRect& getCurrentTextureRect() const
	{
		return m_animations.at(m_currentAnimationName).getCurrentSubTextureCoords();
	}

	const Box& getRelativeHitbox() const
	{
		// hitbox position is relative to subTexture position. (position not included)
		// {0, 0, .., ..}: hitbox has the same position than the subTexture
		return m_animations.at(m_currentAnimationName).getCurrentRelativeHitboxCoords();
	}

private:
	// current animation state
	std::string m_currentAnimationName;

	// animations list
	std::map<std::string, Animation> m_animations;
};
