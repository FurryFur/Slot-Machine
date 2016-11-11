#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <Windows.h>

const int g_kiSLOT_MIN = 2;
const int g_kiSLOT_MAX = 7;
const int g_kiNUM_SLOTS = 3;

// Mask used when doing text highlighting for slots that contribute to a win
const int g_kiSIZE_HIGHLIGHT_MASK = 13;
bool g_bHighlightMask[g_kiSIZE_HIGHLIGHT_MASK] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
const int g_kiHIGHLIGHT_MASK_SLOT_IDX[g_kiNUM_SLOTS] = { 2, 6, 10 };

int g_arriSlot[g_kiNUM_SLOTS];
int g_iPlayerChips;

std::random_device g_rd;     // only used once to initialise (seed) engine
std::mt19937 g_rng(g_rd());    // random-number engine used (Mersenne-Twister in this case)
std::uniform_int_distribution<int> g_uniR(g_kiSLOT_MIN, g_kiSLOT_MAX); // guaranteed unbiased

void GenRandSlotState(int (*_arriSlot)[g_kiNUM_SLOTS])
{
	for (int i = 0; i < g_kiNUM_SLOTS; i++)
		(*_arriSlot)[i] = g_uniR(g_rng);
}

void SetAll(bool* _bpArray, int const _iArraySize, bool const _bValue = true)
{
	if (_bpArray == nullptr)
		return;

	for (int i = 0; i < _iArraySize; i++)
	{
		_bpArray[i] = _bValue;
	}
}

void Init()
{
	g_iPlayerChips = 2000;
	GenRandSlotState(&g_arriSlot);
	SetAll(g_bHighlightMask, g_kiSIZE_HIGHLIGHT_MASK, false);
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
	HANDLE _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(_hConsole, &csbi);

	DWORD _dwCharsToClear = csbi.dwSize.X - csbi.dwCursorPosition.X;
	DWORD _dwWritten;
	FillConsoleOutputCharacter(_hConsole, TEXT(' '), _dwCharsToClear, csbi.dwCursorPosition, &_dwWritten);
}

int Wrap(int _iN)
{
	int _iRange = g_kiSLOT_MAX - g_kiSLOT_MIN + 1;
	int _iIdx = (_iN - g_kiSLOT_MIN) % _iRange;
	if (_iIdx < 0)
	{
		return g_kiSLOT_MAX + 1 + _iIdx;
	}
	else
	{
		return g_kiSLOT_MIN + _iIdx;
	}
}

void PrintCentre(std::stringstream& _ss, 
                 int _iAttributes = -1,
                 bool const * _bpMask = nullptr, 
                 int const _iSizeMask = 0)
{
	std::string line;
	while (std::getline(_ss, line))
	{
		HANDLE _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO _csbi;

		GetConsoleScreenBufferInfo(_hConsole, &_csbi);

		int _iX = (_csbi.dwSize.X - line.length()) / 2;
		int _iY = _csbi.dwCursorPosition.Y;

		GotoXY(_iX, _iY);
		for (int i = 0; i < line.size(); i++)
		{
			// Color text
			if (_bpMask == nullptr && _iAttributes >= 0)
				SetConsoleTextAttribute(_hConsole, _iAttributes);
			if (_bpMask != nullptr && _iAttributes >= 0 && i < _iSizeMask)
			{
				if (_bpMask[i])
					SetConsoleTextAttribute(_hConsole, _iAttributes);
			}

			std::cout << line[i];

			// Reset text color
			SetConsoleTextAttribute(_hConsole, _csbi.wAttributes);
		}
		if (!_ss.eof())
		{
			std::cout << std::endl;
		}
	}
	
	_ss.clear();
	_ss.str("");
}

void PrintCentre(const std::string& _sStr,
                 int _iAttributes = -1,
                 bool const * _bpMask = nullptr,
                 int const _iSizeMask = 0)
{
	std::stringstream _ss;
	_ss << _sStr;
	PrintCentre(_ss, _iAttributes, _bpMask, _iSizeMask);
}

// Generate and set the mask to use for highlighting slot numbers inside ascii art
void GenSetHighlightMask(bool const (&_arrbWinningSlot)[g_kiNUM_SLOTS])
{
	for (int i = 0; i < g_kiNUM_SLOTS; i++)
	{
		if (_arrbWinningSlot[i])
			g_bHighlightMask[g_kiHIGHLIGHT_MASK_SLOT_IDX[i]] = true;
		else
			g_bHighlightMask[g_kiHIGHLIGHT_MASK_SLOT_IDX[i]] = false;
	}
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
	PrintCentre(_ss, 2, g_bHighlightMask, g_kiSIZE_HIGHLIGHT_MASK);
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

void DisplayMsg(const std::string& _sStr)
{
	GotoXY(0, 5);
	std::stringstream _ss;
	_ss << _sStr;
	PrintCentre(_ss);
}

bool ValidateInput(const std::string& _sInput, const std::string& _sRegex)
{
	std::regex _rRegex(_sRegex);
	return (std::regex_match(_sInput, _rRegex));
}

std::string GetValidatedInput(const std::string& _sRegex)
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
	PrintCentre("Everything: Lance Chaney");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << '.';
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << '.';
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << '.';
}

// Checks if any propositions in an array are true
bool Any(bool const * _bpArray, int const _iArraySize)
{
	if (_bpArray == nullptr)
		return false;

	for (int i = 0; i < _iArraySize; i++)
	{
		if (_bpArray[i] == true)
			return true;
	}
	return false;
}

enum WinCondition {
	g_kwcNONE,
	g_kwcTWO_SAME,
	g_kwcALL_SAME,
	g_kwcALL_SEVEN,
};

WinCondition CheckWinConditions(int const _iNumSlotsToCheck = g_kiNUM_SLOTS, bool (*_arrbWinningSlot)[g_kiNUM_SLOTS] = nullptr)
{
	SetAll(*_arrbWinningSlot, g_kiNUM_SLOTS, false);

	if (_iNumSlotsToCheck == g_kiNUM_SLOTS)
	{
		// Check for all sevens
		bool _bAllSeven = true;
		for (int i = 0; i < g_kiNUM_SLOTS; i++)
		{

			if (g_arriSlot[i] != 7)
			{
				_bAllSeven = false;
				break;
			}
		}
		if (_bAllSeven)
		{
			SetAll(*_arrbWinningSlot, g_kiNUM_SLOTS);
			return g_kwcALL_SEVEN;
		}

		// Check for all same
		bool _bAllSame = true;
		for (int i = 1; i < g_kiNUM_SLOTS; i++)
		{
			if (g_arriSlot[i] != g_arriSlot[0])
			{
				_bAllSame = false;
				break;
			}
		}
		if (_bAllSame)
		{
			SetAll(*_arrbWinningSlot, g_kiNUM_SLOTS);
			return g_kwcALL_SAME;
		}
	}

	// Check for two same
	for (int i = 0; i < _iNumSlotsToCheck; i++)
	{
		for (int j = i + 1; j < _iNumSlotsToCheck; j++)
		{
			if (g_arriSlot[i] == g_arriSlot[j])
			{
				if (_arrbWinningSlot != nullptr)
				{
					(*_arrbWinningSlot)[i] = true;
					(*_arrbWinningSlot)[j] = true;
				}
				return g_kwcTWO_SAME;
			}
		}
	}

	// No win conditions have been met
	return g_kwcNONE;
}

void AnimateToSlotState(int const (&_arriNewSlotState)[g_kiNUM_SLOTS])
{
	const int _kiMIN_TICKS = 30;
	const int _kiMIN_TICKS_SLOT_INCR = 25;
	const std::chrono::milliseconds _kmsTICK_FRAME_TIME(50);

	// REPEAT until all slots have stopped spinning
	int _iTicksPassed = 0;
	int _iFirstSpinningSlot = 0;
	while (_iFirstSpinningSlot < g_kiNUM_SLOTS)
	{
		// For each slot
		for (int i = _iFirstSpinningSlot; i < g_kiNUM_SLOTS; i++)
		{
			// Tick the slot unless the minimum number of ticks have
			// passed and the slot has arrived at the target value
			if (_iTicksPassed >= (_kiMIN_TICKS + _kiMIN_TICKS_SLOT_INCR * i)
				&& g_arriSlot[i] == _arriNewSlotState[i])
			{
				_iFirstSpinningSlot++;

				// Check win conditions
				bool _arrbWinningSlot[g_kiNUM_SLOTS];
				WinCondition _wcResult = CheckWinConditions(i + 1, &_arrbWinningSlot);

				// Highlight slots contributing to a win
				GenSetHighlightMask(_arrbWinningSlot);

				continue;
			}
			else
			{
				g_arriSlot[i] = Wrap(g_arriSlot[i] - 1);
			}
		}

		// Draw slots
		DrawSlots();

		std::this_thread::sleep_for(_kmsTICK_FRAME_TIME);
		_iTicksPassed++;
	}
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

		SetAll(g_bHighlightMask, g_kiSIZE_HIGHLIGHT_MASK, false);
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

					// Get new slot state
					int _arriNewSlotState[g_kiNUM_SLOTS];
					GenRandSlotState(&_arriNewSlotState);

					AnimateToSlotState(_arriNewSlotState);

					// Check win conditions
					WinCondition _wcResult = CheckWinConditions();
					int _iWinnings;
					switch (_wcResult)
					{
					case g_kwcTWO_SAME:
						_iWinnings = _iPlayerBet * 3;
						g_iPlayerChips += _iWinnings;
						DisplayMsg("+$" + std::to_string(_iWinnings));
						break;
					case g_kwcALL_SAME:
						_iWinnings = _iPlayerBet * 5;
						g_iPlayerChips += _iWinnings;
						DisplayMsg("+$" + std::to_string(_iWinnings));
						break;
					case g_kwcALL_SEVEN:
						_iWinnings = _iPlayerBet * 10;
						g_iPlayerChips += _iWinnings;
						DisplayMsg("JACKPOT! +$" + std::to_string(_iWinnings));
						break;
					case g_kwcNONE:
					default:
						DisplayMsg("-$" + std::to_string(_iPlayerBet));
						break;
					}
				}
			}
			else
				PrintCentre("You have no chips left to play with");

			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		else if (_sMenuSelection == "2")
			break;
		else if (_sMenuSelection == "3")
			ShowCredits();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}