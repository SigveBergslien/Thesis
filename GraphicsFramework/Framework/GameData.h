#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_


class GameData
{
	public:
		GameData();
		~GameData();

		inline unsigned int getCrystalsCollected() const { return (crystalsCollected); }
		inline void setCrystalsCollected(unsigned int cc) { crystalsCollected = cc; }

		inline unsigned int getEnemiesKilled() const { return (enemiesKilled); }
		inline void setEnemiesKilled(unsigned int ek) { crystalsCollected = ek; }

		inline float getMana() const { return (mana); }
		inline void setMana(float m) { mana = (m > maxMana) ? maxMana : m; }
		inline void increaseMana() { (mana + 0.1f > maxMana) ? maxMana : mana += 0.3f; }

		inline void setNumberStartingPookas(int num){numberOfStartingPookas = num;}
		inline int getNumberStartingPookas(){return numberOfStartingPookas;}

		inline void setNumberOfPookas(int num){numberOfPookas = num;}
		inline int getNumberOfPookas(){return numberOfPookas;}

		inline void setNumberExtraPookas(int num){numberExtraPookas = num;}
		inline int getNumberExtraPookas(){return numberExtraPookas;}

		inline void setNumberPredators(int num){numberOfPredators = num;}
		inline int getNumberPredators(){return numberOfPredators;}

		inline void setNumberTrees(int num){numberOfTrees = num;}
		inline int getNumberTrees(){return numberOfTrees;}

		inline void setNumberMushrooms(int num){numberOfMushrooms = num;}
		inline int getNumberMushrooms(){return numberOfMushrooms;}

		inline void setNumberFlowers(int num){numberOfFlowers = num;}
		inline int getNumberFlowers(){return numberOfFlowers;}

		inline void setLastScore(int num){lastScore = num;}
		inline int getLastScore(){return lastScore;}


		inline void incrementLevel() {level++;}	
		inline int getLevel(){return level;}
	protected:

	private:

		unsigned int crystalsCollected;
		unsigned int enemiesKilled;
		unsigned int timeTaken;
		float mana;
		static const unsigned int maxMana = 500;
		int lastScore;
		int level;
		int numberOfPookas;
		int numberExtraPookas;
		int numberOfPredators;
		int numberOfTrees;
		int numberOfMushrooms;
		int numberOfFlowers;
		int numberOfStartingPookas;
};

#endif