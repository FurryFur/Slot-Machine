#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <Windows.h>

const int g_kSLOT_MIN = 2;
const int g_kSLOT_MAX = 7;
const int g_kNUM_SLOTS = 3;

int g_arriSlot[g_kNUM_SLOTS];
int g_iPlayerChips;

std::random_device g_rd;     // only used once to initialise (seed) engine
std::mt19937 g_rng(g_rd());    // random-number engine used (Mersenne-Twister in this case)
std::uniform_int_distribution<int> g_uniR(g_kSLOT_MIN, g_kSLOT_MAX); // guaranteed unbiased

void RandomizeSlotState()
{
	for (int i = 0; i < g_kNUM_SLOTS; i++)
		g_arriSlot[i] = g_uniR(g_rng);
}

void Init()
{
	g_iPlayerChips = 2000;
	RandomizeSlotState();
}

void GotoXY(int _iX, int _iY)
{
	COORD point;
	point.X = _iX;
	point.Y = _iY;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
}

void ClearScreen()
{
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize,
		coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize,
		coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
}

void ClearRight()
{
	HANDLE _hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(_hScreen, &csbi);

	DWORD _dwCharsToClear = csbi.dwSize.X - csbi.dwCursorPosition.X;
	DWORD _dwWritten;
	FillConsoleOutputCharacter(_hScreen, TEXT(' '), _dwCharsToClear, csbi.dwCursorPosition, &_dwWritten);
}

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

void PrintCentre(std::stringstream& _ss)
{
	std::string line;
	while (std::getline(_ss, line))
	{
		HANDLE _hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		GetConsoleScreenBufferInfo(_hScreen, &csbi);

		int _iX = (csbi.dwSize.X - line.length()) / 2;
		int _iY = csbi.dwCursorPosition.Y;

		GotoXY(_iX, _iY);
		if (!_ss.eof())
			std::cout << line << std::endl;
		else
			std::cout << line;
	}
	
	_ss.clear();
	_ss.str("");
}

void PrintCentre(const std::string _sStr)
{
	std::stringstream _ss;
	_ss << _sStr;
	PrintCentre(_ss);
}

void DrawSlots()
{
	GotoXY(0, 9);
	std::stringstream _ss;
	_ss << "+---+---+---+" << std::endl;
	PrintCentre(_ss);
	_ss << "| " << Wrap(g_arriSlot[0] - 1) << " | " << Wrap(g_arriSlot[1] - 1) << " | " << Wrap(g_arriSlot[2] - 1) << " |" << std::endl;
	PrintCentre(_ss);
	_ss << "+---+---+---+" << std::endl;
	PrintCentre(_ss);
	_ss << "| " << g_arriSlot[0] << " | " << g_arriSlot[1] << " | " << g_arriSlot[2] << " |" << std::endl;
	PrintCentre(_ss);
	_ss << "+---+---+---+" << std::endl;
	PrintCentre(_ss);
	_ss << "| " << Wrap(g_arriSlot[0] + 1) << " | " << Wrap(g_arriSlot[1] + 1) << " | " << Wrap(g_arriSlot[2] + 1) << " |" << std::endl;
	PrintCentre(_ss);
	_ss << "+---+---+---+" << std::endl;
	PrintCentre(_ss);
}

void DrawSlotMachine()
{
	// Draw the machine 
	std::stringstream _ss;
	_ss << R"(
  /---------------------------\  
 /   _______________________   \ 
/   /                       \   \
|  |                         |  |
|  |                         |  |
|  |                         |  |
|   \_______________________/   |
|                               |
|                            o  |
|                           /   |
|                          /    |
|                         /     |
|                               |
|                               |
|                               |
|                               |
|                               |
+_______________________________+)";
	PrintCentre(_ss);
	DrawSlots();
	

	// Show Player chips
	std::cout << std::endl;
	_ss << "Player Chips: $" << g_iPlayerChips << std::endl << std::endl;
	PrintCentre(_ss);

	// Place cursor after machine
	std::cout << std::endl;
}

void DisplayMenu()
{
	PrintCentre("1) Play Slots\n");
	PrintCentre("2) Quit Slot Machine\n");
	PrintCentre("3) Credits\n");
}

void DisplayMsg(std::string _sStr)
{
	GotoXY(0, 5);
	std::stringstream _ss;
	_ss << _sStr;
	PrintCentre(_ss);
}

bool ValidateInput(std::string _sInput, std::string _sRegex)
{
	std::regex _rRegex(_sRegex);
	return (std::regex_match(_sInput, _rRegex));
}

std::string GetValidatedInput(std::string _sRegex)
{
	std::stringstream _ss;
	std::string _sInput;
	while (true)
	{
		// Save Cursor position.
		HANDLE _hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(_hScreen, &csbi);
		int _iX = csbi.dwCursorPosition.X;
		int _iY = csbi.dwCursorPosition.Y;

		std::cin >> _sInput;
		if (std::regex_match(_sInput, std::regex(_sRegex)))
			return _sInput;
		else
		{
			// Print error at message possition
			DisplayMsg("Invalid Input");

			// Reset cursor position
			GotoXY(_iX, _iY);

			// Clear previous input
			ClearRight();
		}
	}
}

void ShowCredits()
{

}


int main()
{
	std::stringstream _ss;
	Init();

	while (true)
	{
		ClearScreen();
		DrawSlotMachine();
		DisplayMenu();

		PrintCentre("Select A Menu Option: ");
		std::string _sMenuSelection = GetValidatedInput("[123]");

		ClearScreen();
		DrawSlotMachine();

		if (_sMenuSelection == "1")
		{
			if (g_iPlayerChips > 0)
			{
				_ss << "Enter amount to bet: ";
				PrintCentre(_ss);
				int _iPlayerBet = std::stoi(GetValidatedInput("[[:digit:]]+"));
				if (_iPlayerBet > g_iPlayerChips)
				{
					_ss << "You don't have enough chips for that" << std::endl;
					PrintCentre(_ss);
				}
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

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}