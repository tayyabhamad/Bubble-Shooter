
#ifndef WORD_SHOOTER_CPP

#define WORD_SHOOTER_CPP
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "SOIL/SOIL.h"
#include <thread>
#include "util.h"

using namespace std;
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define ABS(A) ((A) < (0) ? -(A) : (A))

#define FPS 60
const int row = 9, col = 15;
int grid[row][col] = {0};
bool isWord_PlaceHolder[row][col] = {0};

string *dictionary;

int dictionarysize = 369646;

#define KEY_ESC 27
const int bradius = 30;

int width = 900, height = 900;

int byoffset = bradius;

int nfrows = 2;

int score = 0;

int **board;

int bwidth = 130;

int bheight = 10;

int bsx,
	bsy;

const int nalphabets = 26;

const int MAX_WORDS = 100;
int *wordStartX;
int *wordStartY;
int *wordDirection;
int *wordLength;
int wordCount = 0;
const int MAX_PLAYERS = 10;
int mouseX, mouseY;

int shootX, shootY;
float currentX = width / 2;
float currentY = 40;
bool isshoot = false;
bool ischoose = false;
float directionX = 1.0;
float directionY = 1.0;
float speed = 40.0;

int cAlphabet;
int ucAlphabet;

bool isloading = true, isplay = false, isMenu = false, isSetting = false, isLeaderBoard = false, isExit = false;
bool islogin = false, isusername = false;
bool isBlast = false, ispause = false;
bool isgameover = false;
int oversec = 10;
string username = "";
int soundcirclex = 606, musiccirclex = 606;
int minutes = 2, second = 0;
const int max_word = 10;
string foundwords[max_word] = {""};
Mix_Music *backgroundMusic = NULL;
Mix_Chunk *touchSound = NULL;

enum alphabets
{

	AL_A,
	AL_B,
	AL_C,
	AL_D,
	AL_E,
	AL_F,
	AL_G,
	AL_H,
	AL_I,
	AL_J,
	AL_K,
	AL_L,
	AL_M,
	AL_N,
	AL_O,
	AL_P,
	AL_Q,
	AL_R,
	AL_S,
	AL_T,
	AL_U,
	AL_W,
	AL_X,
	AL_y,
	AL_Z

};
Mix_Chunk *sound;
int svolume = 120, mvolume;
GLuint texture[nalphabets];
GLuint logo;
GLuint tid[nalphabets];

string tnames[] = {"a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",

				   "k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",

				   "x.bmp", "y.bmp", "z.bmp"};

GLuint mtid[nalphabets];

void DrawAlphabet(const alphabets &cname, int sx, int sy, int radius = 30)
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_TRIANGLE_FAN);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.5f, 0.5f);
	glVertex2f(sx, sy);
	const int num_segments = 100;
	for (int i = 0; i <= num_segments; i++)
	{

		float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);

		float x = radius * cosf(theta);
		float y = radius * sinf(theta);

		float tx = 0.5f + 0.5f * cosf(theta);
		float ty = 0.5f + 0.5f * sinf(theta);

		float gradientFactor = 0.8f + 0.2f * cosf(theta);
		glColor4f(gradientFactor, gradientFactor, 1.0f, 0.9f);
		glTexCoord2f(tx, ty);
		glVertex2f(sx + x, sy + y);
	}

	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void DrawCircleslines(int startX, int startY, int radius, int lineLength, int circleSpacing, float *colors)
{

	float directionX = mouseX - startX;
	float directionY = startY - mouseY;

	float magnitude = sqrt(directionX * directionX + directionY * directionY);

	float unitX = directionX / magnitude;
	float unitY = directionY / magnitude;

	int numCircles = lineLength / circleSpacing;

	float currentX = startX;
	float currentY = startY;

	for (int i = 0; i < numCircles; ++i)
	{

		DrawCircle(currentX, currentY, radius * 1.0000 / i, colors);

		currentX += unitX * circleSpacing;
		currentY += unitY * circleSpacing;
	}
}

void loadLeaderboard(string userarr[], int scoresarr[])
{
	int playerCount = 0;
	ifstream file("data/leaderboard.txt");
	playerCount = 0;

	if (!file)
	{
		cerr << "Leaderboard file does not exist. A new file will be created.\n";
		return;
	}
	while (file >> userarr[playerCount] >> scoresarr[playerCount])
	{
		playerCount++;
		if (playerCount >= MAX_PLAYERS)
			break;
	}
	file.close();
}

void printStates()
{
	cout << "*****************************************************" << endl;
	cout << "isloading: " << (isloading ? "true" : "false") << endl;
	cout << "isplay: " << (isplay ? "true" : "false") << endl;
	cout << "isMenu: " << (isMenu ? "true" : "false") << endl;
	cout << "isSetting: " << (isSetting ? "true" : "false") << endl;
	cout << "isLeaderBoard: " << (isLeaderBoard ? "true" : "false") << endl;
	cout << "isExit: " << (isExit ? "true" : "false") << endl;
	cout << "islogin: " << (islogin ? "true" : "false") << endl;
	cout << "isusername: " << (isusername ? "true" : "false") << endl;
	cout << "isBlast: " << (isBlast ? "true" : "false") << endl;
	cout << "ispause: " << (ispause ? "true" : "false") << endl;
	cout << "*****************************************************" << endl;
}

void saveLeaderboard(const string userarr[], const int scoresarr[], int playerCount)
{
	ofstream file("/data/leaderboard.txt");

	for (int i = 0; i < playerCount; i++)
	{
		file << userarr[i] << " " << scoresarr[i] << "\n";
	}

	file.close();
}

void updateLeaderboard(int playerCount)
{
	string userarr[MAX_PLAYERS];
	int scoresarr[MAX_PLAYERS];
	loadLeaderboard(userarr, scoresarr);
	int position = playerCount;
	for (int i = 0; i < playerCount; i++)
	{
		if (score > scoresarr[i])
		{
			position = i;
			break;
		}
	}

	if (position >= MAX_PLAYERS)
		return;

	for (int i = MAX_PLAYERS - 1; i > position; i--)
	{
		if (i < playerCount)
		{
			userarr[i] = userarr[i - 1];
			scoresarr[i] = scoresarr[i - 1];
		}
	}

	userarr[position] = username;
	scoresarr[position] = score;

	if (playerCount < MAX_PLAYERS)
		playerCount++;
	saveLeaderboard(userarr, scoresarr, playerCount);
}

void renderImage(const char *imagePath, float x, float y, float width, float height)
{
	glEnable(GL_TEXTURE_2D);
	GLuint texture = SOIL_load_OGL_texture(
		imagePath,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y);

	if (texture == 0)
	{
		printf("Failed to load texture from '%s': %s\n", imagePath, SOIL_last_result());
		return;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x, y);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x + width, y);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(x + width, y + height);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x, y + height);
	glEnd();

	glDeleteTextures(1, &texture);
}
int awidth = 60, aheight = 60;

bool isSubWord(string dictionary, string word[], int &count)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < word[i].length() - 2; j++)
		{
			for (int k = j + 2; k < word[i].length() - 1; k++)
			{
				string sWord = word[i].substr(j, k - j);
				if (dictionary == sWord)
				{
					for (int l = j; l < k; l++)
					{
						score += 1;
						grid[i][l] = rand() % 26;
						isWord_PlaceHolder[i][l] = true;
					}
					count++;
					return true;
				}
			}
		}
	}
	return false;
}

void getRowWords(string words[])
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < col; j++)
		{
			char latter = grid[i][j] + 'a';
			words[i] += latter;
		}
	}
}

void initial_adjustment()
{
	bool grid[row][col] = {false};
	string words[4] = {"", "", "", ""};
	getRowWords(words);

	int count = 0;
	do
	{

		for (int i = dictionarysize - 1; i >= 0; i--)
		{
			if (isSubWord(dictionary[i], words, count))
			{
				if (count >= 4)
				{
					break;
				}
			}
		}
	} while (count == 0);
}

void Initialize_Grid()
{

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (i < 2)
				grid[i][j] = rand() % 26;
			else
				grid[i][j] = -1;
		}
	}

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (grid[i][j] != -1)
			{
				char a = grid[i][j] + 'a';
				cout << a << "\t";
			}
		}
		cout << endl;
	}

	int num = 0;
	initial_adjustment();
	cAlphabet = rand() % 26;
	ucAlphabet = rand() % 26;
}

void collectWord(int dx, int dy, int gridX, int gridY, string &word)
{
	int x = gridX, y = gridY;
	word = "";
	while (x >= 0 && x < col && y >= 0 && y < row && grid[y][x] != -1)
	{
		word += 'a' + grid[y][x];
		x += dx;
		y += dy;
	}
}

void GetAllWords(string words[], int gridX, int gridY)
{
	if (gridX < 0 || gridY < 0 || gridX >= col || gridY >= row || grid[gridY][gridX] == -1)
	{
		return;
	}

	string horizontalRight, horizontalLeft, verticalDown, verticalUp;
	string diagonalRightDown, diagonalRightUp, diagonalLeftDown, diagonalLeftUp;

	collectWord(1, 0, gridX, gridY, horizontalRight);
	collectWord(-1, 0, gridX, gridY, horizontalLeft);
	collectWord(0, 1, gridX, gridY, verticalDown);
	collectWord(0, -1, gridX, gridY, verticalUp);
	collectWord(1, 1, gridX, gridY, diagonalRightDown);
	collectWord(-1, -1, gridX, gridY, diagonalLeftUp);
	collectWord(-1, 1, gridX, gridY, diagonalLeftDown);
	collectWord(1, -1, gridX, gridY, diagonalRightUp);
	words[0] = horizontalLeft + horizontalRight.substr(1);
	words[1] = verticalUp + verticalDown.substr(1);
	words[2] = diagonalLeftUp + diagonalRightDown.substr(1);
	words[3] = diagonalRightUp + diagonalLeftDown.substr(1);
}

void resetGrid()
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			isWord_PlaceHolder[i][j] = false;
		}
	}
}

bool isWordFound(string words[], int gridX, int gridY)
{
	int wordno = 0;
	bool wordFound = false;
	cout << "Grid Y :" << gridY << " Gridx: " << gridX << endl;
	for (int dir = 0; dir < 4; ++dir)
	{
		string currentWord = words[dir];
		if (currentWord.empty() || currentWord.length() < 2)
			continue;
		for (int d = 0; d < dictionarysize; ++d)
		{
			string dictWord = dictionary[d];

			if (currentWord == dictWord)
			{
				foundwords[wordno] = dictWord;
				cout << currentWord << endl;
				int x = gridX, y = gridY;
				for (int pos = 0; pos < dictWord.length(); ++pos)
				{

					if (x >= 0 && x < col && y >= 0 && y < row)
					{
						if (isWord_PlaceHolder[y][x] == false)
						{
							isWord_PlaceHolder[y][x] = true;
						}
						cout << char(grid[x][y] + 'a') << " : " << x << " " << y << endl;

						switch (dir)
						{
						case 0:
							x++;
							break;
						case 1:
							y++;
							break;
						case 2:
							x--;
							y--;
							break;
						case 3:
							x++;
							y--;
							break;
						}
					}
					else
					{

						break;
					}
				}

				wordStartX[wordCount] = gridX;
				wordStartY[wordCount] = gridY;
				wordDirection[wordCount] = dir;
				wordLength[wordCount] = dictWord.length();
				wordCount++;

				wordFound = true;
			}
		}
	}

	return wordFound;
}

void reset_wordlist()
{
	for (int i = 0; i < max_word; ++i)
	{
		foundwords[i] = "";
	}
}

void RemoveFoundWords()
{

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (isWord_PlaceHolder[i][j] == true)
			{
				score += 1;
				grid[i][j] = -1;
				isWord_PlaceHolder[i][j] = false;
			}
		}
	}
}

void CheckAndRemoveWords(int gridX, int gridY)
{
	resetGrid();
	string words[4];
	GetAllWords(words, gridX, gridY);
	if (isWordFound(words, gridX, gridY))
	{
		int count = 0;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				if (isWord_PlaceHolder[i][j] && grid[i][j] != -1)
				{
					count++;
				}
			}
		}
		if (count > 1)
		{
			RemoveFoundWords();
		}
	}
	ofstream file;
	file.open("data/" + username + ".txt", ios::app);
	for (int i = 0; i < max_word; i++)
	{
		if (foundwords[i] != "")
		{
			file << foundwords[i] << endl;
		}
	}
	file.close();
	reset_wordlist();
}

void RegisterTextures()
{

	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;

	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile)
	{

		cout << " Couldn't Read the Image Data file ";
	}

	int length;

	ifile.read((char *)&length, sizeof(int));

	data.resize(length, 0);

	for (int i = 0; i < nalphabets; ++i)
	{

		/*if (i == 0) {

		int length = data.size();

		ofile.write((char*) &length, sizeof(int));

		}*/

		ifile.read((char *)&data[0], sizeof(char) * length);

		mtid[i] = tid[i];

		glBindTexture(GL_TEXTURE_2D, tid[i]);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,

						GL_LINEAR_MIPMAP_NEAREST);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		bool wrap = true;

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,

						wrap ? GL_REPEAT : GL_CLAMP);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,

						wrap ? GL_REPEAT : GL_CLAMP);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,

						  GL_UNSIGNED_BYTE, &data[0]);
	}

	ifile.close();
}

void DrawReflectedLine(float startX, float startY, float angle, float color[], float strength, int recursionDepth)
{

	if (recursionDepth > 20)
		return;

	float dx = cos(Deg2Rad(angle));
	float dy = sin(Deg2Rad(angle));
	float t;
	float intersectX, intersectY;

	t = (height - startY) / dy;
	intersectX = startX + t * dx;
	intersectY = startY + t * dy;

	if (intersectX >= 0 && intersectX <= width)
	{

		DrawLineAtAngle(startX, startY, angle, t, color, strength);

		DrawReflectedLine(intersectX, intersectY, -angle, color, strength, recursionDepth + 1);
		return;
	}

	t = (width - startX) / dx;
	intersectX = startX + t * dx;
	intersectY = startY + t * dy;

	if (intersectY >= 0 && intersectY <= height)
	{

		DrawLineAtAngle(startX, startY, angle, t, color, strength);

		DrawReflectedLine(intersectX, intersectY, 180.0f - angle, color, strength, recursionDepth + 1);
		return;
	}

	t = -startX / dx;
	intersectX = startX + t * dx;
	intersectY = startY + t * dy;

	if (intersectY >= 0 && intersectY <= height)
	{

		DrawLineAtAngle(startX, startY, angle, t, color, strength);

		DrawReflectedLine(intersectX, intersectY, 180.0f - angle, color, strength, recursionDepth + 1);
		return;
	}

	t = -startY / dy;
	intersectX = startX + t * dx;
	intersectY = startY + t * dy;

	if (intersectX >= 0 && intersectX <= width)
	{

		DrawLineAtAngle(startX, startY, angle, t, color, strength);

		DrawReflectedLine(intersectX, intersectY, -angle, color, strength, recursionDepth + 1);
		return;
	}
}

void ChooseAlphabet()
{
	cAlphabet = ucAlphabet;
	ucAlphabet = rand() % 26;
}

void Display_Grid()
{

	int g_width = width / 15;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (grid[i][j] != -1)
				DrawAlphabet((alphabets)grid[i][j], (g_width * j + g_width / 2), (height - g_width * (i + 1)) - g_width / 4 - 70, g_width / 2);
		}
	}
}

void DrawCannon(int sx, int sy)
{
	if (isshoot && ischoose)
	{
		ChooseAlphabet();
		ischoose = false;
	}

	float baseX = sx;
	float baseY = sy + 40;

	float dx = mouseX - baseX;
	float dy = baseY - mouseY;
	float angle = atan2(dy, dx) * 180.0f / M_PI;
	if (!isshoot)
	{

		DrawCircleslines(baseX, baseY, 20, height / 2, 50, colors[RED]);
	}

	float barrelWidth = 65;
	float barrelHeight = 120;
	DrawRotatedRectangle(baseX, baseY, angle - 90, barrelWidth, barrelHeight, colors[DARK_BLUE]);
	DrawCircle(baseX, baseY, 55, colors[DARK_BLUE]);
	DrawRotatedRectangle(baseX, baseY, angle - 90, barrelWidth - 15, barrelHeight - 10, colors[BLUE]);
	DrawCircle(baseX, baseY, 45, colors[BLUE]);
	int alphabetRadius = 30;
	DrawAlphabet((alphabets)cAlphabet, baseX, baseY, alphabetRadius);
}

void GameOver()
{
	isplay = false;
	isshoot = false;
	isgameover = true;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);
	cout << username << " " << score << endl;
	renderImage("images/gameover.png", 0, 0, width, height);
	DrawString(width / 2 - 50, height / 2 + 20, width, height, username, 0, colors[GOLD]);
	DrawString(width / 2, height / 2 - 100, width, height, to_string(score), 0, colors[GOLD]);
	glutPostRedisplay();
}

void Game()
{
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	renderImage("images/play.png", 0, 0, width, height);

	Display_Grid();

	if (isshoot)
	{
		DrawAlphabet((alphabets)cAlphabet, currentX, currentY, width / 30);
	}

	DrawCannon((width / 2), 0);

	glutPostRedisplay();
}
float loadingProgress = 0.1f;
float loadingSpeed = 0.01f;
bool pausedAtCenter = false;
int pauseCounter = 0;

int dotCount = 0;
float dotTimer = 0.0f;
float dotUpdateSpeed = 1.0f;
string loadingName = "Loading";
void Menu()
{
	isMenu = true;
	glColor3f(1.0f, 1.0f, 1.0f);
	renderImage("images/menu.png", 0, 0, width, height);
}

void Loading()
{
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	renderImage("images/loading.png", 0, 0, width, height);

	float upperX = width * 0.25;
	float upperY = height * 0.3;
	float upperWidth = width / 2;
	float upperHeight = height * 0.05 + 10;
	float borderRadius = 28;

	DrawRoundRect(upperX, upperY, upperWidth, upperHeight, colors[DARK_GOLDEN_ROD], borderRadius);

	static float lowerWidth = 0.0f;
	float lowerX = upperX + 5;
	float lowerY = upperY + 5;
	float lowerHeight = upperHeight - 10;
	float lowerBorderRadius = 23;

	lowerWidth = (upperWidth - 10) * loadingProgress;

	string dots = string(dotCount, '.');

	dotTimer += 0.1f;
	if (dotTimer >= dotUpdateSpeed)
	{
		dotTimer = 0.0f;
		dotCount = (dotCount + 1) % 4;
	}

	DrawRoundRect(lowerX, lowerY, lowerWidth, lowerHeight, colors[GOLD], lowerBorderRadius);
	DrawString(445, 292, width, height, to_string(int(loadingProgress * 100)) + "%", 0, colors[WHITE]);

	loadingProgress += loadingSpeed;
	if (loadingProgress > 1.0f)
	{
		loadingProgress = 1.0f;
		isloading = false;
		islogin = true;
	}
}

void login()
{

	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	renderImage("images/username.png", 0, 0, width, height);
	DrawString(400, 455, width, height, username == "" ? "Username" : username, 0, colors[GOLDEN_ROD]);
}

bool ismusic = true, issound = true;
void UpdateMusicVolumes()
{
	Mix_VolumeMusic(mvolume);

	Mix_VolumeChunk(touchSound, svolume);
}
void InitializeAudio()
{
	if (Mix_OpenAudio(24100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return;
	}

	// Load background music
	backgroundMusic = Mix_LoadMUS("sound/music.mp3");
	if (backgroundMusic == NULL)
	{
		std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return;
	}

	// Play background music indefinitely
	Mix_PlayMusic(backgroundMusic, -1);
	Mix_VolumeMusic(50);

	// UpdateMusicVolumes(); // Set initial volume

	// Load touch sound effect
	touchSound = Mix_LoadWAV("sound/touch2.mp3");
	if (touchSound == NULL)
	{
		std::cerr << "Failed to load touch sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return;
	}
}
void PlayTouchSound()
{
	Mix_PlayChannel(-1, touchSound, 0); // Play touch sound on a free channel
}
void Settings()
{
	isSetting = true;
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	renderImage("images/setting.png", 0, 0, width, height);

	if (!ismusic)
	{
		DrawLine(668, 665, 623, 619, 3, colors[GOLDEN_ROD]);
	}
	if (!issound)
	{
		DrawLine(623, 512, 668, 559, 3, colors[GOLDEN_ROD]);
	}
	DrawLine(344, 637, musiccirclex, 637, 3, colors[GOLDEN_ROD]);
	DrawLine(344, 535, soundcirclex, 535, 3, colors[GOLDEN_ROD]);
	DrawCircle(musiccirclex, 637, 10, colors[GOLDEN_ROD]);
	DrawCircle(soundcirclex, 535, 10, colors[GOLDEN_ROD]);
	glutPostRedisplay();
}

void timer()
{

	glColor3f(1, 1, 1);
	DrawRoundRect(20, height - 110, width - 40, 100, colors[DARK_BLUE], 50);
	DrawRoundRect(30, height - 100, width - 60, 80, colors[BLUE], 40);
	DrawString(107, height - 75, width, height, username, 0, colors[WHITE]);
	DrawString(width / 2 - 50, height - 75, width, height, "score " + to_string(score), 0, colors[WHITE]);
	DrawString(width / 2 + 200, height - 75, width, height, "Time Left:  " + to_string(minutes) + " : " + to_string(second), 0, colors[RED]);
}

void Play()
{
	isloading = false;
	isplay = true;
	isMenu = false;
	isSetting = false;
	isLeaderBoard = false;
	isExit = false;
	ispause = false;
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	if (!ispause)
	{

		Game();
		timer();
	}
	glutPostRedisplay();
}

void Exit()
{
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);
	renderImage("images/exit.png", 0, 0, width, height);
}

void DisplayFunction()
{

	if (minutes <= 0 && second <= 0)
	{
		int num = 0;
		cout << "TIme is over" << endl;
		isshoot = false;
		isplay = false;
		ispause = true;
	}
	if (isgameover)
	{
		GameOver();
	}
	if (isloading)
	{
		Loading();
	}
	if (islogin)
	{
		login();
	}

	if (isMenu)
	{
		Menu();
	}
	if (isSetting)
	{
		Settings();
	}
	if (isplay)
	{
		Play();
	}
	if (isExit)
	{
		Exit();
	}

	glutSwapBuffers();
}

void SetCanvasSize(int width, int height)
{

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glOrtho(0, width, 0, height, -1, 1);
}

void NonPrintableKeys(int key, int x, int y)
{

	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/)
	{
	}

	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/)
	{
	}

	else if (key == GLUT_KEY_UP /*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/)
	{
	}

	else if (key == GLUT_KEY_DOWN /*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/)
	{
	}
}

void MouseMoved(int x, int y)
{
	if (!ispause)
	{
		if (isplay && !isshoot)
		{

			mouseX = x;
			mouseY = y - height;
		}
	}
	glutPostRedisplay();
}

void MouseClicked(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		PlayTouchSound();

		printStates();
	}
	y = height - y;

	if (!ispause)
	{
		if (isplay && !isshoot && state == GLUT_DOWN)
		{

			shootX = x;
			shootY = y;
			isshoot = true;

			float dx = shootX - currentX;
			float dy = shootY - currentY;
			float magnitude = sqrt(dx * dx + dy * dy);

			directionX = dx / magnitude;
			directionY = dy / magnitude;
		}
	}

	if (isMenu && state == GLUT_DOWN)
	{
		if (x > 333 && x < 570 && y > 338 && y < 437)
		{
			cout << "is inside" << endl;

			isloading = false;
			isplay = true;
			isMenu = false;
			isSetting = false;
			isLeaderBoard = false;
			isExit = false;
			ispause = true;
		}

		if (x > 342 && x < 565 && y > 562 && y < 617)
		{

			isloading = false;
			isplay = false;
			isMenu = false;
			isSetting = false;
			isLeaderBoard = true;
			isExit = false;
		}

		if (x > 335 && x < 570 && y < 231 && y > 179)
		{

			isloading = false;
			isplay = false;
			isMenu = false;
			isSetting = true;
			isLeaderBoard = false;
			isExit = false;
		}
		if (x > 754 && x < 864 && y > 39 && y < 98)
		{

			isloading = false;
			isplay = false;
			isMenu = false;
			isSetting = false;
			isLeaderBoard = false;
			isExit = true;
		}

		else
		{
			cout << "is outside" << endl;
		}
	}

	if (isSetting && state == GLUT_DOWN)
	{
		if (x > 344 && x < 606 && y > 625 && y < 645)
		{
			musiccirclex = x;
			mvolume = ((musiccirclex - 344) * MIX_MAX_VOLUME) / 242;
			cout << "Music slider clicked at: " << musiccirclex << endl;
			UpdateMusicVolumes();
		}

		if (x > 344 && x < 606 && y > 520 && y < 545)
		{
			soundcirclex = x;
			cout << "Sound slider clicked at: " << soundcirclex << endl;
			svolume = ((soundcirclex - 344) * MIX_MAX_VOLUME) / 242;
			UpdateMusicVolumes();
		}

		if (x > 623 && x < 668 && y > 619 && y < 665 && state == GLUT_DOWN)
		{
			ismusic = !ismusic;
			musiccirclex = ismusic ? 606 : 344;
			mvolume = ((musiccirclex - 344) * MIX_MAX_VOLUME) / 242;
			cout << "Music toggled: " << (ismusic ? "On" : "Off") << endl;
		}

		if (x > 623 && x < 668 && y > 512 && y < 559 && state == GLUT_DOWN)
		{
			issound = !issound;
			soundcirclex = issound ? 606 : 344;
			svolume = ((soundcirclex - 344) * MIX_MAX_VOLUME) / 242;
			cout << "Sound toggled: " << (issound ? "On" : "Off") << endl;
		}
		if (x > 371 && x < 546 && y < 451 && y > 409 && state == GLUT_DOWN)
		{

			isloading = false;
			isplay = false;
			isMenu = true;
			isSetting = false;
			isLeaderBoard = false;
			isExit = false;
		}
	}

	if (isExit && state == GLUT_DOWN)
	{
		if (x > 484 && x < 693 && y > 320 && y < 380)
		{
			exit(0);
		}

		if (x > 223 && x < 445 && y > 322 && y < 372)
		{

			isloading = false;
			isplay = false;
			isMenu = true;
			isSetting = false;
			isLeaderBoard = false;
			isExit = false;
		}
	}

	if (islogin && state == GLUT_DOWN)
	{
		if (x > 285 && x < 615 && y > 423 && y < 491)
		{
			isusername = true;
		}
		else
		{
			isusername = false;
		}
		if (x > 330 && x < 547 && y > 282 && y < 330)
		{

			isloading = false;
			isplay = false;
			isMenu = true;
			isSetting = false;
			isLeaderBoard = false;
			isExit = false;
			islogin = false;
		}
	}

	glutPostRedisplay();
}

void PrintableKeys(unsigned char key, int x, int y)
{
	if (isusername && islogin)
	{
		if (key != 8)
			username += string(1, key);
		else
		{
			username = username.substr(0, username.length() - 1);
		}
	}

	if (key == KEY_ESC)
	{

		exit(1);
	}
}

bool HandleWallCollision(float nextX, float nextY, float &lastValidX, float &lastValidY)
{
	int storeY, storeX;
	int g_width = width / col;
	int g_height = height / row;
	if (nextX <= 10 || nextX >= width - 10)
	{
		directionX = -directionX;
		return false;
	}

	int gridX = nextX / g_width;
	int gridY = (height - nextY) / g_height;

	if ((gridY == 0 && grid[0][gridX] == -1))
	{

		grid[0][gridX] = cAlphabet;
		int num = 1000;
		while (num > 0)
		{
			num--;
		}

		CheckAndRemoveWords(gridX, 0);
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				cout << grid[i][j] << "\t";
			}
			cout << endl;
		}
		currentX = lastValidX;
		currentY = lastValidY;

		return true;
	}
	if (grid[gridY - 1][gridX] != -1 || grid[gridY][gridX + 1] != -1 || grid[gridY][gridX - 1] != -1)
	{
		grid[gridY][gridX] = cAlphabet;
		int num = 1000;
		while (num > 0)
		{
			num--;
		}
		CheckAndRemoveWords(gridX, gridY);
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				cout << grid[i][j] << "\t";
			}
			cout << endl;
		}
		currentX = lastValidX;
		currentY = lastValidY;

		return true;
	}

	lastValidX = nextX;
	lastValidY = nextY;

	return false;
}

void moveAlphabet()
{

	float nextX = currentX + directionX * speed;
	float nextY = currentY + directionY * speed;

	float lastValidX = currentX;
	float lastValidY = currentY;

	if (HandleWallCollision(nextX, nextY, lastValidX, lastValidY))
	{
		isshoot = false;

		currentX = width / 2;
		currentY = 40;

		ChooseAlphabet();
	}
	else
	{
		currentX = nextX;
		currentY = nextY;

		DrawAlphabet((alphabets)cAlphabet, currentX, currentY, width / 4);
	}
}

void Timer(int m)
{
	if (!ispause)
	{

		if (isshoot)
		{
			moveAlphabet();
		}
		else
		{

			currentX = width / 2;
			currentY = 40;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(1000.0 / FPS, Timer, 0);
}

void clock(int m)
{
	if (isplay)
	{

		if (!ispause && (minutes * 60 + second) > 1)
		{
			second--;
			if (second <= 0)
			{
				second = 60;
				minutes--;
			}
		}
		else
		{
			int num = 0;
			GameOver();
		}
	}
	if (isgameover)
	{

		if (oversec == 1)
		{
			updateLeaderboard(0);
		}
		oversec--;
		if (oversec < 0)
		{
			exit(0);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1000.0, clock, 0);
}

void Reshape(int w, int h)
{
	width = w;
	height = h;

	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char *argv[])
{
	InitRandomizer();
	InitializeAudio();
	dictionary = new string[dictionarysize];
	wordStartX = new int[MAX_WORDS];
	wordStartY = new int[MAX_WORDS];
	wordDirection = new int[MAX_WORDS];
	wordLength = new int[MAX_WORDS];

	ReadWords("words_alpha.txt", dictionary);
	Initialize_Grid();

	for (int i = 0; i < 5; ++i)

		cout << " word " << i << " =" << dictionary[i] << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Name : Tayyab Hamad  Roll NO: 23i-0053");
	SetCanvasSize(width, height);
	updateLeaderboard(0);
	RegisterTextures();

	glutDisplayFunc(DisplayFunction);
	glutSpecialFunc(NonPrintableKeys);
	glutReshapeFunc(Reshape);

	glutKeyboardFunc(PrintableKeys);
	glutMouseFunc(MouseClicked);

	glutReshapeFunc(Reshape);

	glutPassiveMotionFunc(MouseMoved);
	glutTimerFunc(1000.0 / FPS, Timer, 0);
	glutTimerFunc(1000.0, clock, 0);

	glutMainLoop();

	return 1;
}

#endif
