// Author -- ybaheti 18th Nov 2021

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

//define LOG_ENABLED

#ifdef LOG_ENABLED
#define LOG(log){cerr << log << '\n';}
#else
#define LOG(log) while(false);
#endif

typedef enum critical
{
	CORNER = 2,
	EDGE = 3,
	INNER = 4
}CRIT;

typedef enum class cell
{
	EMPTY = 0,
	SELF,
	OPP,
	SELF_SINK,
	OPP_SINK
}CELL;

typedef enum class safeTypeOp
{
	EXPLODE,
	ADD
}S_OP;

int g_rows;
int g_cols;
vector<vector<pair<CELL, int>>> g_matrix;
vector<pair<int, int>> g_self_sinks;
int g_max_sink;

// ----------------------------------------------------------------------------------------------
// ------------------------------FUNCTION DECLARATIONS-------------------------------------------
// ----------------------------------------------------------------------------------------------
void printBoard();
void createBoard();
string printCell(const CELL c);
bool checkIfCriticalMass(const int x, const int y);

void processExplosion(const int x, const int y);
void updateBoard(const CELL cellType, const int x, const int y);

bool checkIfCriticalPoint(const int x, const int y);
bool check(const int x, const int y, S_OP op);
pair<int, int> performSafeMove();



// ----------------------------------------------------------------------------------------------
// ------------------------------FUNCTION DEFINITIONS--------------------------------------------
// ----------------------------------------------------------------------------------------------

// function definitions
void createBoard()
{
	for (int i = 0; i < g_rows; i++)
	{
		for (int j = 0; j < g_cols; j++)
		{
			g_matrix[i][j] = make_pair(CELL::EMPTY, 0);
		}
	}
}

void printBoard()
{
	cerr << "----------------------------------------\n";
	for (int i = 0; i < g_rows; i++)
	{
		for (int j = 0; j < g_cols; j++)
		{
			cerr << printCell(g_matrix[i][j].first) << '|' << g_matrix[i][j].second << " ";
			std::cerr.clear();
		}
		cerr << '\n';
	}
	cerr << "----------------------------------------\n";
}

string printCell(const CELL c)
{
	switch (c)
	{
	case CELL::EMPTY:
		return "EMPTY";
	case CELL::SELF:
		return "SELF";
	case CELL::SELF_SINK:
		return "SELF_SINK";
	case CELL::OPP:
		return "OPP";
	case CELL::OPP_SINK:
		return "OPP_SINK";
	}
}

bool checkIfCriticalMass(const int x, const int y)
{
	int cellVal = g_matrix[x][y].second;

	// corner 
	if ((x == 0 && y == 0) && cellVal >= CRIT::CORNER) return true;
	else if ((x == 0 && y == (g_cols - 1)) && cellVal >= CRIT::CORNER)return true;
	else if ((x == g_rows - 1) && y == 0 && cellVal >= CRIT::CORNER)return true;
	else if ((x == g_rows - 1 && y == g_cols - 1) && cellVal >= CRIT::CORNER)return true;

	//edge cells 
	else if (x == 0 && cellVal >= CRIT::EDGE)return true;
	else if ((x == g_rows - 1) && cellVal >= CRIT::EDGE)return true;

	else if (y == 0 && cellVal >= CRIT::EDGE)return true;
	else if ((y == g_cols - 1) && cellVal >= CRIT::EDGE)return true;

	// inner cell
	if (cellVal >= CRIT::INNER)return true;

	return false;
}

bool checkIfCriticalPoint(const int x, const int y)
{
	LOG("critpoint " + to_string(x) + " | " + to_string(y));
	// corner 
	if ((x == 0 && y == 0) && g_matrix[x][y].second == (CRIT::CORNER - 1)) return true;
	else if ((x == 0 && y == (g_cols - 1)) && g_matrix[x][y].second == (CRIT::CORNER - 1))return true;
	else if ((x == g_rows - 1) && y == 0 && g_matrix[x][y].second == (CRIT::CORNER - 1))return true;
	else if ((x == g_rows - 1 && y == g_cols - 1) && g_matrix[x][y].second == (CRIT::CORNER - 1))return true;

	//edge cells 
	else if (x == 0 && g_matrix[x][y].second == (CRIT::EDGE - 1))return true;
	else if ((x == g_rows - 1) && g_matrix[x][y].second == (CRIT::EDGE - 1))return true;

	else if (y == 0 && g_matrix[x][y].second == (CRIT::EDGE - 1))return true;
	else if ((y == g_cols - 1) && g_matrix[x][y].second == (CRIT::EDGE - 1))return true;

	// inner cell
	if (g_matrix[x][y].second == (CRIT::INNER - 1))return true;

	return false;
}

void processExplosion(const int x, const int y)
{
	// todo take care of extra orbs present in cell due to multiple explosion 
	// the suboptimal handling of the system bot
	LOG("-----------EXPLOSION---------------");
	LOG(to_string(x) + " || " + to_string(y));
	// get the current cell type so we can use later for reflection and chain reaction
	CELL currCellType = g_matrix[x][y].first;
	int currCellValue = g_matrix[x][y].second;

	// there might be a much more better looking way to handle cases
	// but i dont have time 
	// if else it is....:)

	//corner cells
	if (x == 0 && y == 0 && g_matrix[x][y].second >= CRIT::CORNER)
	{
		g_matrix[x + 1][y].second += 1;
		g_matrix[x + 1][y].first = currCellType;

		g_matrix[x][y + 1].second += 1;
		g_matrix[x][y + 1].first = currCellType;

		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::CORNER) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::CORNER));
	}
	else if ((x == 0 && y == (g_cols - 1)) && g_matrix[x][y].second >= CRIT::CORNER)
	{
		g_matrix[x + 1][y].second += 1;
		g_matrix[x + 1][y].first = currCellType;

		g_matrix[x][y - 1].second += 1;
		g_matrix[x][y - 1].first = currCellType;

		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::CORNER) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::CORNER));
	}
	else if ((x == g_rows - 1) && y == 0 && g_matrix[x][y].second >= CRIT::CORNER)
	{
		g_matrix[x - 1][y].second += 1;
		g_matrix[x - 1][y].first = currCellType;

		g_matrix[x][y + 1].second += 1;
		g_matrix[x][y + 1].first = currCellType;

		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::CORNER) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::CORNER));
	}
	else if (x == g_rows - 1 && y == g_cols - 1 && g_matrix[x][y].second >= CRIT::CORNER)
	{
		g_matrix[x - 1][y].second += 1;
		g_matrix[x - 1][y].first = currCellType;

		g_matrix[x][y - 1].second += 1;
		g_matrix[x][y - 1].first = currCellType;

		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::CORNER) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::CORNER));
	}

	//edge cells 
	else if (x == 0 && g_matrix[x][y].second >= CRIT::EDGE)
	{
		// make cell empty or residue from extra as it exploded
		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::EDGE) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::EDGE));

		g_matrix[x][y - 1].second += 1;
		g_matrix[x][y - 1].first = currCellType;

		g_matrix[x][y + 1].second += 1;
		g_matrix[x][y + 1].first = currCellType;


		if (g_matrix[x + 1][y].first == CELL::SELF_SINK || g_matrix[x + 1][y].first == CELL::OPP_SINK)
		{
			if (g_matrix[x + 1][y].second == g_max_sink)
			{
				//reflected orb back
				g_matrix[x][y].first = currCellType;
				g_matrix[x][y].second += 1;
			}
			else if (g_matrix[x + 1][y].first == currCellType)
			{
				g_matrix[x + 1][y].second++;
			}
		}
		else
		{
			g_matrix[x + 1][y].first = currCellType;
			g_matrix[x + 1][y].second++;
		}
	}
	else if (x == g_rows - 1 && g_matrix[x][y].second >= CRIT::EDGE)
	{
		// make cell empty or residue from extra as it exploded
		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::EDGE) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::EDGE));

		g_matrix[x][y - 1].second += 1;
		g_matrix[x][y - 1].first = currCellType;

		g_matrix[x][y + 1].second += 1;
		g_matrix[x][y + 1].first = currCellType;


		if (g_matrix[x - 1][y].first == CELL::SELF_SINK || g_matrix[x - 1][y].first == CELL::OPP_SINK)
		{
			if (g_matrix[x - 1][y].second == g_max_sink)
			{
				//reflected orb back
				g_matrix[x][y].first = currCellType;
				g_matrix[x][y].second += 1;
			}
			else if (g_matrix[x - 1][y].first == currCellType)
			{
				g_matrix[x - 1][y].second++;
			}
		}
		else
		{
			g_matrix[x - 1][y].first = currCellType;
			g_matrix[x - 1][y].second++;
		}
	}
	else if (y == 0 && g_matrix[x][y].second >= CRIT::EDGE)
	{
		// make cell empty or residue from extra as it exploded
		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::EDGE) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::EDGE));

		g_matrix[x - 1][y].second += 1;
		g_matrix[x - 1][y].first = currCellType;

		g_matrix[x + 1][y].second += 1;
		g_matrix[x + 1][y].first = currCellType;


		if (g_matrix[x][y + 1].first == CELL::SELF_SINK || g_matrix[x][y + 1].first == CELL::OPP_SINK)
		{
			if (g_matrix[x][y + 1].second == g_max_sink)
			{
				//reflected orb back
				g_matrix[x][y].first = currCellType;
				g_matrix[x][y].second += 1;
			}
			else if (g_matrix[x][y + 1].first == currCellType)
			{
				g_matrix[x][y + 1].second++;
			}
		}
		else
		{
			g_matrix[x][y + 1].first = currCellType;
			g_matrix[x][y + 1].second++;
		}

	}
	else if (y == g_cols - 1 && g_matrix[x][y].second >= CRIT::EDGE)
	{
		// make cell empty or residue from extra as it exploded
		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::EDGE) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::EDGE));

		g_matrix[x - 1][y].second += 1;
		g_matrix[x - 1][y].first = currCellType;

		g_matrix[x + 1][y].second += 1;
		g_matrix[x + 1][y].first = currCellType;


		if (g_matrix[x][y - 1].first == CELL::SELF_SINK || g_matrix[x][y - 1].first == CELL::OPP_SINK)
		{
			if (g_matrix[x][y - 1].second == g_max_sink)
			{
				//reflected orb back
				g_matrix[x][y].first = currCellType;
				g_matrix[x][y].second += 1;
			}
			else if (g_matrix[x][y - 1].first == currCellType)
			{
				g_matrix[x][y - 1].second++;
			}
		}
		else
		{
			g_matrix[x][y - 1].first = currCellType;
			g_matrix[x][y - 1].second++;
		}
	}

	//inner cell
	if (g_matrix[x][y].second >= CRIT::INNER)
	{
		// make cell empty or residue from extra as it exploded
		g_matrix[x][y] = make_pair(max(0, currCellValue - CRIT::INNER) ? currCellType : CELL::EMPTY, max(0, currCellValue - CRIT::INNER));

		int row = x;
		int col = y;
		for (const auto dir : { make_pair(-1,0), make_pair(1,0), make_pair(0,-1), make_pair(0,1) })
		{
			row += dir.first;
			col += dir.second;
			if (row >= 0 && row < g_rows && col >= 0 && col < g_cols)
			{
				if (g_matrix[row][col].first == CELL::SELF_SINK || g_matrix[row][col].first == CELL::OPP_SINK)
				{
					if (g_matrix[row][col].second == g_max_sink)
					{
						//reflected orb back
						g_matrix[x][y].first = currCellType;
						g_matrix[x][y].second += 1;
					}
					else if (g_matrix[row][col].first == currCellType)
					{
						g_matrix[row][col].second++;
					}
				}
				else
				{
					g_matrix[row][col].first = currCellType;
					g_matrix[row][col].second++;
				}
			}
			row = x;
			col = y;
		}
	}

}

void updateBoard(const CELL cellType, const int x, const int y)
{
	LOG("CELL : " + printCell(cellType) + " coordinates : " + to_string(x) + "|| " + to_string(y));

	// maintain queue of all the critical mass cooridinates
	// and blow them all in sequence
	queue<pair<int, int>> criticalMassQueue;

	// first add the orb to the x,y coordinate and which type cell
	g_matrix[x][y].first = cellType;
	g_matrix[x][y].second += 1;

	

	// check for explosion first
	if (checkIfCriticalMass(x, y))
		processExplosion(x, y);

	// keep simulating the board until it is stable
	while (true)
	{
		// system board processing
		// iterate the matrix and keep adding to queue list of critical mass coords
		for (int i = 0; i < g_rows; i++)
		{
			for (int j = 0; j < g_cols; j++)
			{
				if (checkIfCriticalMass(i, j))criticalMassQueue.push(make_pair(i, j));
			}
		}

		if (criticalMassQueue.empty())
			break;

		while (!criticalMassQueue.empty())
		{
			pair<int, int> tp = criticalMassQueue.front();
			processExplosion(tp.first, tp.second);
			criticalMassQueue.pop();
		}
	}
}

// main function which performs our moves [dummy]
pair<int, int> performMoveDummy()
{
	for (int i = 0; i < g_rows; i++)
	{
		for (int j = 0; j < g_cols; j++)
		{
			if (g_matrix[i][j].first == CELL::EMPTY || g_matrix[i][j].first == CELL::SELF)
			{
				updateBoard(CELL::SELF, i, j);
				return make_pair(i, j);
			}
		}
	}
	return { -1,-1 };
}

// main function which performs our moves [optimally i hope]
// safe tactic
// explode if critical mass found next to other critical mass

// check for either add or explode operation
// called based on wether the source cell is crit mass or normal
bool check(const int x, const int y, S_OP op)
{
	LOG(" CELL CHECK  " + to_string(x) + " || " + to_string(y) + " opType : " + (op == S_OP::ADD ? "ADD" : "EXPLODE"));
	int row = x;
	int col = y;
	for (const auto dir : { make_pair(-1,0), make_pair(1,0), make_pair(0,-1), make_pair(0,1) })
	{
		row += dir.first;
		col += dir.second;
		if (row >= 0 && row < g_rows && col >= 0 && col < g_cols)
		{
			if (g_matrix[row][col].first == CELL::OPP && checkIfCriticalPoint(row, col))
			{
				if (op == S_OP::EXPLODE)
				{
					return true;
				}
				else if (op == S_OP::ADD)
				{
					return false;
				}
			}
		}
		row = x;
		col = y;
	}

	//no crit mass found
	if (op == S_OP::ADD) return true;
	// same but dont explode
	return false;

}

/* Tactic
	Traverse the matrix till you first find any of our critical mass cells
	if they are next to another critical mass cell then explode
	Traverse again to see any empty or self cell and check if SAFE
		SAFE--> not adjacent to a critical mass cell diagonal is fine .

	if none of the above conditions are satisfied, put the orb in the first cell we find empty.
	If no cell is found empty, put it in our cell

	// TODO
	// queue of crit masses to process first ?
	// random starts
	// sink cell priority
	// add to sink cell if crit point reached
	// blow up biggest crit mass first ?
	// blow up crit point next to biggest opp crit point
*/
pair<int, int> performSafeMove()
{

	pair<int, int> firstEmpty = { -1,-1 };
	pair<int, int> firstCritPoint = { -1,-1 };
	pair<int, int> firstNormal = { -1,-1 };

	// check for crit masses to explode
	// todo maitain cache queue of crit masses 
	for (int i = 0; i < g_rows; i++)
	{
		for (int j = 0; j < g_cols; j++)
		{
			if (firstEmpty.first == -1 && g_matrix[i][j].first == CELL::EMPTY)
			{
				firstEmpty = make_pair(i, j);
			}
			if (firstNormal.first == -1 && g_matrix[i][j].first == CELL::SELF)
			{
				firstNormal = make_pair(i, j);
			}
			if (g_matrix[i][j].first == CELL::SELF && checkIfCriticalPoint(i, j))
			{
				if (firstCritPoint.first == -1)
				{
					firstCritPoint = make_pair(i, j);
				}
				if (check(i, j, S_OP::EXPLODE))
				{
					updateBoard(CELL::SELF, i, j);
					return make_pair(i, j);
				}
			}
		}
	}

	// safe additions
	// CURRENT IMPLEMENTATION !!
	/*
	for (int i = 0; i < g_rows; i++)
	{
		for (int j = 0; j < g_cols; j++)
		{
			if (g_matrix[i][j].first == CELL::SELF || g_matrix[i][j].first == CELL::EMPTY)
			{
				if (check(i, j, S_OP::ADD))
				{
					updateBoard(CELL::SELF, i, j);
					return make_pair(i, j);
				}
			}
		}
	}
	*/
	// todo --> add to crit if safe ? or not
	/* DOES NOT ADD TO CRIT IF SAFE*/
	for (int i = 0; i < g_rows; i++)
	{
		for (int j = 0; j < g_cols; j++)
		{
			if (g_matrix[i][j].first == CELL::SELF || g_matrix[i][j].first == CELL::EMPTY)
			{
				if (!checkIfCriticalPoint(i,j) && check(i, j, S_OP::ADD))
				{
					updateBoard(CELL::SELF, i, j);
					return make_pair(i, j);
				}
			}
		}
	}
	// nothing worked above
	if (firstCritPoint.first != -1)
	{
		updateBoard(CELL::SELF, firstCritPoint.first, firstCritPoint.second);
		return firstCritPoint;
	}
	if (firstEmpty.first != -1)
	{
		updateBoard(CELL::SELF, firstEmpty.first, firstEmpty.second);
		return firstEmpty;
	}
	if (firstNormal.first != -1)
	{
		updateBoard(CELL::SELF, firstNormal.first, firstNormal.second);
		return firstNormal;
	}

	return make_pair(0, 0);
}


int main()
{

	string inputStr;
	cin >> inputStr;

	if (!inputStr.compare("BOARD_INIT"))
	{
		cin >> g_rows >> g_cols;
		cout << 0 << '\n';
		LOG("Rows : " + to_string(g_rows) + "| Cols : " + to_string(g_cols));
	}
	else
	{
		LOG("INVALID INPUT");
		return 0;
	}
	g_matrix = vector<vector<pair<CELL, int>>>(g_rows, vector<pair<CELL, int>>(g_cols));
	createBoard();

	g_self_sinks = vector<pair<int, int>>();

	//input sink cells
	while (true)
	{
		cin >> inputStr;
		std::cin.clear();
		if (!inputStr.compare("YOUR_SINK"))
		{
			int x, y;
			cin >> x >> y;
			g_matrix[x][y] = make_pair(CELL::SELF_SINK, 0);
			g_self_sinks.push_back(make_pair(x, y));
			cout << 0 << '\n';
		}
		else if (!inputStr.compare("OPPONENT_SINK"))
		{
			int x, y;
			cin >> x >> y;
			g_matrix[x][y] = make_pair(CELL::OPP_SINK, 0);
			cout << 0 << '\n';
		}
		else if (!inputStr.compare("MAX_CAPACITY"))
		{
			int x;
			cin >> x;
			g_max_sink = x;
			cout << 0 << '\n';
		}
		else
		{
			break;
		}
	}

	while (true)
	{
		if (!inputStr.compare("MAKE_MOVE"))
		{
			pair<int, int> move = performSafeMove();
			if (move.first == -1)
			{
				LOG("No more moves left. you lose!");
				break;
			}
			LOG("Made first move");
			cout << move.first << " " << move.second << '\n';
		}
		else if (!inputStr.compare("OPPONENT_MOVE"))
		{
			std::cin.clear();
			std::cerr.clear();
			int ox, oy;
			cin >> ox >> oy;

			LOG(" OPPONENT MOVE " + to_string(ox) + "|" + to_string(oy));

			//Perform opponent Move 
			updateBoard(CELL::OPP, ox, oy);

			pair<int, int> move = performSafeMove();

			cout << move.first << " " << move.second << '\n';
			LOG("MOVE MADE by SELF " + to_string(move.first) + " " + to_string(move.second));
		}
		else if (!inputStr.compare("VALIDATE_BOARD"))
		{
			cout << 0 << endl;
		}
		cin >> inputStr;
	}


	return 0;
}