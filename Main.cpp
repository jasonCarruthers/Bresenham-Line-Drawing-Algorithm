/*
 * Author: Jason Carruthers
 * Last modified: 1/13/16
 */
//#include <Windows.h> //Necessary? Comment out when running on Linux.
#include <GL/glut.h>
#include <math.h>
#include <list>
#include <ctime>


/*
* Classes
*/
class Color
{
public:
	enum RGBColors
	{
		Red,
		Green,
		Blue,
		Num__RGBColors,
	};
public:
	//Constructors
	Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f)
	{
		mRGB[(int)Red] = red;
		mRGB[(int)Green] = green;
		mRGB[(int)Blue] = blue;
	}

	//Accessors
	float GetR() const
	{
		return mRGB[(int)Red];
	}
	float GetG() const
	{
		return mRGB[(int)Green];
	}
	float GetB() const
	{
		return mRGB[(int)Blue];
	}

	//Mutators
	void Set(const Color &color)
	{
		mRGB[(int)Red] = color.GetR();
		mRGB[(int)Green] = color.GetG();
		mRGB[(int)Blue] = color.GetB();
	}
	void Set(float newRed, float newGreen, float newBlue)
	{
		mRGB[(int)Red] = newRed;
		mRGB[(int)Green] = newGreen;
		mRGB[(int)Blue] = newBlue;
	}
	void SetR(float newRed)
	{
		mRGB[(int)Red] = newRed;
	}
	void SetG(float newGreen)
	{
		mRGB[(int)Green] = newGreen;
	}
	void SetB(float newBlue)
	{
		mRGB[(int)Blue] = newBlue;
	}

	//Overloaded operators
	float &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__RGBColors)
			return mRGB[(int)Red]; //If index out of bounds, return the red value by default.
		return mRGB[index];
	}

private:
	float mRGB[3];
};

class Vector2I
{
public:
	enum VectorElements
	{
		X,
		Y,
		Num__Elements,
	};
public:
	//Constructors
	Vector2I(int x = 0, int y = 0)
	{
		mArr[X] = x;
		mArr[Y] = y;
	}

	//Accessors
	int GetX() const
	{
		return mArr[X];
	}
	int GetY() const
	{
		return mArr[Y];
	}

	//Mutators
	void SetX(int newX)
	{
		mArr[X] = newX;
	}
	void SetY(int newY)
	{
		mArr[Y] = newY;
	}

	//Overloaded operators
	int &operator[](int index)
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}
	const int &operator[](int index) const
	{
		if (index < 0 || index >= (int)Num__Elements)
			return mArr[X]; //If index is out of bounds, return the first element by default.
		return mArr[index];
	}

private:
	int mArr[Num__Elements];
};

class Line
{
public:
	Line(Vector2I left, Vector2I right)
	{
		mLeftPt = left;
		mRightPt = right;
	}
public:
	Vector2I mLeftPt;
	Vector2I mRightPt;
};

/*
 * Global variables
 */
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const unsigned int SLEEP_DURATION = 100;
unsigned const int NUM_RAND_LINES = 10;
float *pixelBuffer;
Color lineColor;



/*
 * Function prototypes
 */
void CreateRandomLine(std::list<Line> &lineList);
void CreateRandomLines(std::list<Line> &lineList);
void CreateStaticLines(std::list<Line> &lineList);
void Display();
Color GetRandomColor();
void SetPixel(int x, int y);
void SetPixel(int x, int y, const Color &color);
bool IsLineLegal(int leftX, int leftY, int rightX, int rightY);
void DrawLine(const Vector2I &leftPt, const Vector2I &rightPt, const Color &color);
void DrawLine(int leftX, int leftY, int rightX, int rightY);
void DrawVerticalLine(int x, int y, int dy);
void DrawHorizontalLine(int x, int y, int dx);
void DrawFortyFiveDegreeLine(int x, int y, int dx);



/*
 * Functions
 */
int main(int argc, char *argv[])
{
	//Seed the random number generator
	srand(((static_cast<int>(time(0)))));

	//Allocate new pixel buffer, need initialization!!
	pixelBuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
	//Set the line color
	lineColor.Set(1.0f, 0.0f, 0.0f);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	//Set window size to WINDOW_WIDTH*WINDOW_HEIGHT
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//Set window position
	glutInitWindowPosition(0, 0);

	//Create and set main window title
	int mainWindow = glutCreateWindow("Hello Graphics!!");
	glClearColor(0, 0, 0, 0); //Clears the buffer of OpenGL; sets the background color to black.
	//Sets display function
	glutDisplayFunc(Display);

	glutMainLoop();//Main display loop, will display until terminate
	return 0;
}

void CreateRandomLines(std::list<Line> &lineList)
{
	for (int i = 0; i < NUM_RAND_LINES; i++)
		CreateRandomLine(lineList);
}

void CreateStripingLines(std::list<Line> &lineList)
{
	/*
	 * There are some bugs with these conditionals, but the effect the lines make
	 * is cool so maybe keep the code and understand why it draws what it does.
	 */

	for (int i = 0; i <= (WINDOW_WIDTH + WINDOW_HEIGHT)/2; i++)
	{
		if (i < WINDOW_HEIGHT/2)
			lineList.push_back(Line(Vector2I(0, WINDOW_HEIGHT/2 - 1 - i * 2), Vector2I(1 + i * 2, WINDOW_HEIGHT/2 - 1)));
		else if (i < (WINDOW_HEIGHT/2 + (WINDOW_WIDTH - WINDOW_HEIGHT)/2))
			lineList.push_back(Line(Vector2I((i-WINDOW_HEIGHT/2)*2, 0), Vector2I(1 + i * 2, WINDOW_HEIGHT/2 - 1)));
		else
			lineList.push_back(Line(Vector2I((i - WINDOW_HEIGHT/2) * 2, 0), Vector2I(WINDOW_WIDTH/2 - 1, WINDOW_HEIGHT/2 - 1 - (i - WINDOW_WIDTH/2 - WINDOW_HEIGHT/2)*2)));
	}
}

void CreateRandomLine(std::list<Line> &lineList)
{
	/*
	 * Choose two random coordinates as a means for "creating" a random line.
	 * The first coordinate is chosen as a random position along the left window edge.
	 * The second coordinate is chosen as a random position along the right window edge.
	 */
	Vector2I firstCoord = Vector2I(0, rand() % WINDOW_HEIGHT);
	Vector2I secondCoord = Vector2I(WINDOW_WIDTH - 1, rand() % WINDOW_HEIGHT);
	lineList.push_back(Line(firstCoord, secondCoord));
}

void CreateStaticLines(std::list<Line> &lineList)
{
	//A firework
	lineList.push_back(Line(Vector2I(750, 599), Vector2I(750, 400))); //Vertical line
	lineList.push_back(Line(Vector2I(799, 550), Vector2I(600, 550))); //Horizontal line
	lineList.push_back(Line(Vector2I(799, 599), Vector2I(599, 399))); //|m| = 1
	lineList.push_back(Line(Vector2I(799, 599), Vector2I(400, 400))); //|m| < 1, m > 0
	lineList.push_back(Line(Vector2I(0, 599), Vector2I(400, 400))); //|m| < 1, m < 0
	lineList.push_back(Line(Vector2I(799, 599), Vector2I(600, 200))); //|m| > 1, m > 0
	lineList.push_back(Line(Vector2I(0, 599), Vector2I(200, 200))); //|m| > 1, m < 0

	//The letter J
	lineList.push_back(Line(Vector2I(0, 400), Vector2I(200, 400)));  //-
	lineList.push_back(Line(Vector2I(100, 400), Vector2I(200, 0))); //|
	lineList.push_back(Line(Vector2I(0, 100), Vector2I(200, 0)));    //_

	//The letter A
	lineList.push_back(Line(Vector2I(200, 0), Vector2I(300, 400)));  ///
	lineList.push_back(Line(Vector2I(250, 200), Vector2I(350, 200)));//-
	lineList.push_back(Line(Vector2I(300, 400), Vector2I(400, 0)));  //\

	//The letter C
	lineList.push_back(Line(Vector2I(400, 400), Vector2I(599, 300)));//-
	lineList.push_back(Line(Vector2I(400, 400), Vector2I(400, 0)));  //|
	lineList.push_back(Line(Vector2I(400, 0), Vector2I(599, 100)));    //_
}

//Main display loop, this function will be called again and again by OpenGL
void Display()
{
	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	//Push lines onto a list
	std::list<Line> stripingList;
	std::list<Line> randomLineList;
	std::list<Line> staticLineList;
	//CreateStripingLines(stripingList);
	//Draw lines on the striping lines list
	//for (std::list<Line>::const_iterator iter = stripingList.begin(); iter != stripingList.end(); ++iter)
	//	DrawLine(iter->mLeftPt, iter->mRightPt, Color(0.0f, 0.0f, 1.0f));
	CreateRandomLines(randomLineList);
	//Draw lines on the random lines list
	for (std::list<Line>::const_iterator iter = randomLineList.begin(); iter != randomLineList.end(); ++iter)
		DrawLine(iter->mLeftPt, iter->mRightPt, GetRandomColor());
	CreateStaticLines(staticLineList);
	//Draw lines on the random lines list and the static lines list
	for (std::list<Line>::const_iterator iter = randomLineList.begin(); iter != randomLineList.end(); ++iter)
		DrawLine(iter->mLeftPt, iter->mRightPt, Color(1.0f, 0.0f, 0.0f));
	for (std::list<Line>::const_iterator iter = staticLineList.begin(); iter != staticLineList.end(); ++iter)
		DrawLine(iter->mLeftPt, iter->mRightPt, Color(1.0f, 1.0f, 1.0f));

	//Draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);

	//Window refresh
	glFlush();
}

Color GetRandomColor()
{
	/*
	* Randomize the color of every pixel drawn.
	* Set R, G, and B to a random number between 0.0 and 1.0, inclusive.
	*/
	float newR = (rand() % 11) / 10.0f;
	float newG = (rand() % 11) / 10.0f;
	float newB = (rand() % 11) / 10.0f;
	return Color(newR, newG, newB);
}

void SetPixel(int x, int y)
{
	//Update the pixelBuffer
	for (int color = 0; color < Color::Num__RGBColors; color++)
		pixelBuffer[x * (int)Color::Num__RGBColors + y * WINDOW_WIDTH * (int)Color::Num__RGBColors + color] = lineColor[color]; //See pgs. 146-147 to optimize this.


	/*
	* Both of these functions update the pixels onscreen, so that each time a new pixel
	* is "added to" pixelBuffer, it immediately draws onscreen. This way, the lines
	* animate as their drawn to the screen.
	*/
	//Draws pixel on screen, width and height must match pixel buffer dimension
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
	//Window refresh
	glFlush();
	//Sleep(SLEEP_DURATION);

	//Sleep(SLEEP_DURATION);
}

void SetPixel(int x, int y, const Color &color)
{
	lineColor.Set(color);
	SetPixel(x, y);
}

bool IsLineLegal(int leftX, int leftY, int rightX, int rightY)
{
	//If any of the coordinates are out of bounds, exit the function.
	return !(leftX < 0 || leftX >= WINDOW_WIDTH ||
		rightX < 0 || rightX >= WINDOW_WIDTH ||
		leftY < 0 || leftY >= WINDOW_HEIGHT ||
		rightY < 0 || rightY >= WINDOW_HEIGHT);
}

void DrawVerticalLine(int x, int y, int dy) //Vertical line
{
	for (int i = 0; i < dy; i++)
	{
		SetPixel(x, y);
		y++;
	}
}

void DrawHorizontalLine(int x, int y, int dx) //Horizontal line
{
	for (int i = 0; i < dx; i++)
	{
		SetPixel(x, y);
		x++;
	}
}

void DrawFortyFiveDegreeLine(int x, int y, int dx) //Line with |m| = 1
{
	for (int i = 0; i < dx; i++)
	{
		SetPixel(x, y);
		x++;
		y++;
	}
}

void DrawLine(const Vector2I &leftPt, const Vector2I &rightPt, const Color &color)
{
	lineColor.Set(color);
	DrawLine(leftPt[Vector2I::X], leftPt[Vector2I::Y], rightPt[Vector2I::X], rightPt[Vector2I::Y]);
}

//Bresenham line-drawing procedure
void DrawLine(int leftX, int leftY, int rightX, int rightY)
{
	



	//If any of the coordinates are out of bounds, exit the function.
	if (!IsLineLegal(leftX, leftY, rightX, rightY))
		return;

	//If the two points are the same, draw one point and exit the function
	if (leftX == rightX && leftY == rightY)
	{
		SetPixel(leftX, leftY);
		return;
	}

	//Init variables
	int dx = (int)fabs((float)(rightX - leftX)), dy = (int)fabs((float)(rightY - leftY));
	int twoDy = 2 * dy, twoDyMinusTwoDx = 2 * (dy - dx);
	int twoDx = 2 * dx, twoDxMinusTwoDy = 2 * (dx - dy);
	int p = 2 * dy - dx;
	int x = leftX, y = leftY;
	bool slopeAbsIsLessThanOne = (dy <= dx);
	bool slopeIsPos = !(((rightX - leftX) < 0) ^ ((rightY - leftY) < 0));

	//Determine which endpoint to use as start position
	if (leftX > rightX)
	{
		x = rightX;
		y = rightY;
	}

	//Plot the first point
	SetPixel(x, y);

	//Check for vertical or horizontal line, and also check for line with |m| = 1.
	//If any of the three exist, exit function after drawing.
	if (dx == 0 && dy > 0) //Vertical line
	{
		y = (leftY < rightY) ? leftY : rightY;
		DrawVerticalLine(x, y, dy);
		return;
	}
	else if (dx > 0 && dy == 0) //Horizontal line
	{
		DrawHorizontalLine(x, y, dx);
		return;
	}
	else if (dx == dy) //Line with |m| = 1
	{
		DrawFortyFiveDegreeLine(x, y, dx);
		return;
	}

	
	//Draw the Bresenham line
	if ( slopeAbsIsLessThanOne ) //Cases where |m| < 1
	{
		for (int i = 0; i < dx; i++)
		{
			x++;
			if (p < 0)
				p += twoDy;
			else
			{
				y += (slopeIsPos) ? 1 : -1;
				p += twoDyMinusTwoDx;
			}
			SetPixel(x, y);
		}
	}
	else //if( !slopeAbsIsLessThanOne ) //Cases where |m| > 1
	{
		p = 2 * dx - dy;

		for (int i = 0; i < dy; i++)
		{
			y += (slopeIsPos) ? 1 : -1;
			if (p < 0)
				p += twoDx;
			else
			{
				x++;
				p += twoDxMinusTwoDy;
			}
			SetPixel(x, y);
		}
	}
}