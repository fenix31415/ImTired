#include "Utils.h"

const std::string races[] = {
	"Argonian", "Breton", "DarkElf", "HighElf", "Imperial", "Khajiit", "Nord", "Orc", "Redguard", "WoodElf", "Elder"
	,"Dremora"
};

bool is_human_race(std::string race)
{
	for (auto& r : races) {
		size_t found = race.find(r);
		if (found != std::string::npos) {
			return true;
		}
	}
	return false;
}
