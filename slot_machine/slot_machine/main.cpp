#include <iostream>
#include <random>
#include <string>
#include <regex>
#include <chrono>
#include <thread>

const int g_kSLOT_MIN = 2;
const int g_kSLOT_MAX = 7;
const int g_kNUM_SLOTS = 3;

int g_arriSlot[g_kNUM_SLOTS];
int g_iPlayerChips;

std::random_device g_rd;     // only used once to initialise (seed) engine
std::mt19937 g_rng(g_rd());    // random-number engine used (Mersenne-Twister in this case)
std::uniform_int_distribution<int> g_uniR(g_kSLOT_MIN, g_kSLOT_MAX); // guaranteed unbiased

int Wrap(int _iN)
{
	int _iRange = g_kSLOT_MAX - g_kSLOT_MIN + 1;
	int _iIdx = (_iN - g_kSLOT_MIN) % _iRange;
	if (_iIdx < 0)
	{
		return g_kSLOT_MAX + 1 + _iIdx;
	}
	else
	{
		return g_kSLOT_MIN + _iIdx;
	}
}

void DrawSlotMachine()
{
	std::cout << "|" << Wrap(g_arriSlot[0] - 1) << "|" << Wrap(g_arriSlot[1] - 1) << "|" << Wrap(g_arriSlot[2] - 1) << "|" << std::endl;
	std::cout << "|" << g_arriSlot[0] << "|" << g_arriSlot[1] << "|" << g_arriSlot[2] << "|" << std::endl;
	std::cout << "|" << Wrap(g_arriSlot[0] + 1) << "|" << Wrap(g_arriSlot[1] + 1) << "|" << Wrap(g_arriSlot[2] + 1) << "|" << std::endl;
}

void RandomizeSlotState()
{
	for (int i = 0; i < g_kNUM_SLOTS; i++)
		g_arriSlot[i] = g_uniR(g_rng);
}

void Init()
{
	g_iPlayerChips = 1000;
	RandomizeSlotState();
}

void DisplayMenu()
{
	std::cout << "1) Play Slots" << std::endl;
	std::cout << "2) Quit Slot Machine" << std::endl;
	std::cout << "3) Credits" << std::endl;
}

bool ValidateInput(std::string _sInput, std::string _sRegex)
{
	std::regex _rRegex(_sRegex);
	return (std::regex_match(_sInput, _rRegex));
}

std::string GetValidatedInput(std::string _sRegex)
{
	std::string _sInput;
	while (true)
	{
		std::cin >> _sInput;
		if (std::regex_match(_sInput, std::regex(_sRegex)))
			return _sInput;
		else
			std::cout << "Invalid input" << std::endl;
	}
}

void ShowCredits()
{

}


int main()
{
	Init();

	while (true)
	{
		system("cls");

		DrawSlotMachine();
		std::cout << "Player's chips: " << g_iPlayerChips << std::endl;
		DisplayMenu();

		std::string _sMenuSelection = GetValidatedInput("^[123]$");

		if (_sMenuSelection == "1")
		{
			if (g_iPlayerChips > 0)
			{
				std::cout << "Enter amount to bet: ";
				int _iPlayerBet = std::stoi(GetValidatedInput("[[:digit:]]+"));
				if (_iPlayerBet > g_iPlayerChips)
					std::cout << "You don't have enough chips for that" << std::endl;
				else
				{
					g_iPlayerChips -= _iPlayerBet;
					RandomizeSlotState();
				}
			}
			else
				std::cout << "You have no chips left to play with" << std::endl;
		}
		else if (_sMenuSelection == "2")
			break;
		else if (_sMenuSelection == "3")
			ShowCredits();

		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}