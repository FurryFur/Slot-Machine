#include <iostream>
#include <random>

const int g_kSLOT_MIN = 2;
const int g_kSLOT_MAX = 7;

int g_iSlot1;
int g_iSlot2;
int g_iSlot3;

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
	std::cout << "|" << Wrap(g_iSlot1 - 1) << "|" << Wrap(g_iSlot2 - 1) << "|" << Wrap(g_iSlot3 - 1) << "|" << std::endl;
	std::cout << "|" << g_iSlot1 << "|" << g_iSlot2 << "|" << g_iSlot3 << "|" << std::endl;
	std::cout << "|" << Wrap(g_iSlot1 + 1) << "|" << Wrap(g_iSlot2 + 1) << "|" << Wrap(g_iSlot3 + 1) << "|" << std::endl;
}

int main()
{
	int player_chips = 1000;

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(g_kSLOT_MIN, g_kSLOT_MAX); // guaranteed unbiased

	g_iSlot1 = uni(rng);
	g_iSlot2 = uni(rng);
	g_iSlot3 = uni(rng);

	DrawSlotMachine();

	int iTemp;
	std::cin >> iTemp;
}