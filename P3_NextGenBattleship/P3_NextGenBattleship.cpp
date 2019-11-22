#include <iostream>
#include <string>
#include <dos.h> //for delay
#include <conio.h> //for getch()
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const int BOARD_WIDTH = 20;
const int BOARD_HEIGHT = 10;
const int SHIP_TYPES = 4;

const char isWATER = '~';
const char isHIT = 'X';
const char isSHIP = 'S';
const char isMISS = 'O';

//Function for loading and displaying images
void displayImage(string image_name) {
	Mat img;
	img = imread(image_name);
	imshow(image_name, img);
	waitKey(0);
}

struct POINT {
	//Horizontal and vertical coordinates
	int X;
	int Y;
};

struct SHIP {
	//Ship name
	string name;
	//Total points on the grid
	int length;
	//Coordinates of those points
	POINT onGrid[4]; //0-5 max length of biggest ship
	//Whether or not those points are a "hit"
	bool hitFlag[4];
}ship[SHIP_TYPES];

struct PLAYER {
	char grid[BOARD_WIDTH][BOARD_HEIGHT];
}player[3];

enum DIRECTION { //The enum direction can either get the value horizontal or vertical
	HORIZONTAL,
	VERTICAL }; 

struct PLACESHIPS {
	DIRECTION direction;
	SHIP shipType;
};

bool gameRunning = false;

//Functions
void LoadShips();
void ResetBoard();
void DrawBoard(int);
PLACESHIPS UserInputShipPlacement();
bool UserInputAttack(int&, int&, int);
bool GameOverCheck(int);

int main()
{
	LoadShips();
	ResetBoard();
	displayImage("grid.png");
	//displayImage("carrier.png");
	
	//Loop through each player and place ships
	for (int aPlayer = 1; aPlayer < 3; ++aPlayer)
	{
		//Loop through each ship type to place
		for (int thisShip = 0; thisShip < SHIP_TYPES; ++thisShip)
		{
			//Display board
			system("cls");
			DrawBoard(aPlayer);
			//Instructions
			cout << "\n";
			cout << "(Player " << aPlayer << ")\n\n";
			cout << "Horizontal: 0, Vetical: 1, then enter X and Y coordinates\n";
			cout << "Ship to place: " << ship[thisShip].name << " which has a length of " << ship[thisShip].length << "\n";
			cout << "Where do you want it placed? (specify direction first): \n";

			//Get input from user and loop until good data is returned
			PLACESHIPS aShip;
			aShip.shipType.onGrid[0].X = -1;
			while (aShip.shipType.onGrid[0].X == -1)
			{
				aShip = UserInputShipPlacement();
			}

			//Combine user data with "this ship" data
			aShip.shipType.length = ship[thisShip].length;
			aShip.shipType.name = ship[thisShip].name;

			//Add the FIRST grid point to the current player's game board
			player[aPlayer].grid[aShip.shipType.onGrid[0].X][aShip.shipType.onGrid[0].Y] = isSHIP;

			//Determine ALL grid points based on length and direction
			for (int i = 1; i < aShip.shipType.length; ++i)
			{
				if (aShip.direction == HORIZONTAL) {
					aShip.shipType.onGrid[i].X = aShip.shipType.onGrid[i - 1].X + 1;
					aShip.shipType.onGrid[i].Y = aShip.shipType.onGrid[i - 1].Y;
				}
				if (aShip.direction == VERTICAL) {
					aShip.shipType.onGrid[i].Y = aShip.shipType.onGrid[i - 1].Y + 1;
					aShip.shipType.onGrid[i].X = aShip.shipType.onGrid[i - 1].X;
				}

				//Add the REMAINING grid points to our current players game board
				player[aPlayer].grid[aShip.shipType.onGrid[i].X][aShip.shipType.onGrid[i].Y] = isSHIP;
			}
			//Loop back through each ship type
		}
		//Loop back through each player
	}

	//Game part after ships have been placed by both palyers
	gameRunning = true;
	int thisPlayer = 1;
	do {
		//Because we are ATTACKING now, the opposite players board is the display board
		int enemyPlayer;
		if (thisPlayer == 1) enemyPlayer = 2;
		if (thisPlayer == 2) enemyPlayer = 1;
		system("cls");
		DrawBoard(enemyPlayer);

		//Get attack coords from this player
		bool goodInput = false;
		int x, y;
		while (goodInput == false) {
			goodInput = UserInputAttack(x, y, thisPlayer);
		}

		//Check board; if a ship is there, set as HIT.. otherwise MISS
		if (player[enemyPlayer].grid[x][y] == isSHIP) player[enemyPlayer].grid[x][y] = isHIT;
		if (player[enemyPlayer].grid[x][y] == isWATER) player[enemyPlayer].grid[x][y] = isMISS;

		//Check to see if the game is over
		//If 0 is returned, nobody has won yet
		int aWin = GameOverCheck(enemyPlayer);
		if (aWin != 0) {
			gameRunning = false;
			break;
		}
		//Alternate between each player as we loop back around
		thisPlayer = (thisPlayer == 1) ? 2 : 1;
	} while (gameRunning);

	system("cls");
	cout << "\n\nPLAYER " << thisPlayer << "WON\n\n\n\n";

	system("pause");
	return 0;
}


bool GameOverCheck(int enemyPLAYER)
{
	bool winner = true;
	//Loop through enemy board
	for (int w = 0; w < BOARD_WIDTH; ++w) {
		for (int h = 0; h < BOARD_HEIGHT; ++h) {
			//If any ships remain, game is NOT over
			if (player[enemyPLAYER].grid[w][h] = isSHIP)
			{
				winner = false;
				return winner;
			}
		}
	}
	//If we get here, somebody won, game over!
	return winner;
}


bool UserInputAttack(int& x, int& y, int theplayer)
{
	cout << "\nPLAYER " << theplayer << ", ENTER COORDINATES TO ATTACK: ";
	bool goodInput = false;
	cin >> x >> y;
	if (x < 0 || x >= BOARD_WIDTH) return goodInput;
	if (y < 0 || y >= BOARD_HEIGHT) return goodInput;
	goodInput = true;
	//delay(500);

	return goodInput;
}

PLACESHIPS UserInputShipPlacement()
{
	int d, x, y;
	PLACESHIPS tmp;
	//Using this as a bad return
	tmp.shipType.onGrid[0].X = -1;
	//Get 3 integers from user
	cin >> d >> x >> y;
	if (d != 0 && d != 1) return tmp;
	if (x < 0 || x >= BOARD_WIDTH) return tmp;
	if (y < 0 || y >= BOARD_HEIGHT) return tmp;
	//Good data
	tmp.direction = (DIRECTION)d;
	tmp.shipType.onGrid[0].X = x;
	tmp.shipType.onGrid[0].Y = y;
	return tmp;
}

void LoadShips()
{
	//Sets the default data for the ships
	ship[0].name = "Scout Ship"; ship[0].length = 2;
	ship[1].name = "Artillery Ship"; ship[1].length = 3;
	ship[2].name = "The Destroyer"; ship[2].length = 4;
	ship[3].name = "Aircraft Carrier"; ship[3].length = 5;
}
void ResetBoard()
{
	//Loop through each player
	for (int plyr = 1; plyr < 3; ++plyr)
	{
		//For each grid point, set contents to 'water'
		for (int w = 0; w < BOARD_WIDTH; ++w) {
			for (int h = 0; h < BOARD_HEIGHT; ++h) {
				player[plyr].grid[w][h] = isWATER;
			}
		}
		//Loop back to next player
	}
}

void DrawBoard(int thisPlayer)
{	
	//Array that contains the letters
	char gridLetters[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T' };

	//Draws the board for a player (thisPlayer)
	cout << "PLAYER " << thisPlayer << "'s GAME BOARD\n";
	cout << "----------------------\n";

	//Loop through the board with and draw the numbers
	cout << "   "; //left-padding for number row on top
	for (int w = 0; w < BOARD_WIDTH; ++w) {
		if (w < BOARD_WIDTH)
			cout << w << "  ";
	};
		
	cout << "\n";

	//Loop through each grid point and display to console
	for (int h = 0; h < BOARD_HEIGHT; ++h) {
		for (int w = 0; w < BOARD_WIDTH; ++w) {
			//Displaying the letters
			
			if (w == 0 ) {
				cout << gridLetters[h] << "  ";
			}
			if (w > 10) {
				cout << " ";
			}
			//Display contents of this grid (if game isn't running yet, we are placing ships
			//so display the ships
			if (gameRunning == false) cout << player[thisPlayer].grid[w][h] << "  ";
			//Don't show ships, BUT show damage if it's hit
			if (gameRunning == true && player[thisPlayer].grid[w][h] != isSHIP)
			{
				cout << player[thisPlayer].grid[w][h] << "  ";
			}
			else if (gameRunning == true && player[thisPlayer].grid[w][h] == isSHIP)
			{
				cout << isWATER << "  ";
			}
			//If we have reached the border = line feed
			if (w == BOARD_WIDTH - 1) cout << "\n";
		}
	}
}
