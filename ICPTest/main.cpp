#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <chrono>

#include "data.h"
#define PI 3.14159265 / 180
#define NUM 360

//using namespace std;
//using namespace std::chrono;

struct Scan
{
	int16_t points[360][2];
	int16_t X;
	int16_t Y;
	int16_t R;
};
struct Point
{
	int16_t X;
	int16_t Y;
};

void drawpoints(SDL_Renderer *renderer, uint16_t *points, int16_t x, int16_t y);
void drawpoints(SDL_Renderer *renderer, Scan *scan);
void calculateCenter(uint16_t *points, int16_t x, int16_t y, int16_t *cx, int16_t *cy);
void drawCenter(SDL_Renderer *renderer, int16_t x, int16_t y, bool mass);
void drawlines(SDL_Renderer *renderer, uint16_t *points, int16_t x, int16_t y);
void findClosesedPoint(int16_t X, int16_t Y, int16_t X2, int16_t Y2, uint16_t *points, int16_t *disx, int16_t *disy);
void findClosesedPoint(Point *point, Scan *scan, int16_t *disx, int16_t *disy);
Scan arrayToScan(int16_t X, int16_t Y, uint16_t *points);
void matchScan(Scan *scan1, Scan *scan2);
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

Scan scan1;
Scan scan2;
uint16_t points[NUM];
uint16_t points2[NUM];
int16_t x = 0, y = 0;
int16_t x2 = 0, y2 = 0;
int16_t cenx = 0, ceny = 0;
int16_t cenx2 = 0, ceny2 = 0;
int mouseX, mouseY;
SDL_Event event;
bool pointSelect = true;
bool change;
bool running;
bool done = false;
uint16_t counter;

int main(int, char **) //int main(int argc, char **argv)
{
	std::cout << "ICP Test" << std::endl;
	running = true;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		running = false;
	}
	if (SDL_CreateWindowAndRenderer(1920, 1080, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0)
	{
		running = false;
	}
	SDL_SetWindowTitle(window, "ICP Test visualiser");
	uint16_t x = 1920 / 2, y = 1080 / 2;
	uint16_t x2 = 1920 / 2, y2 = 1080 / 2;

	scan1 = arrayToScan(x, y, data1);
	scan2 = arrayToScan(x2, y2, data2);

	scan1.R = 90;

	int16_t val = 100;
	uint8_t randomVal = 10;

	SDL_Rect cornerRect;
	cornerRect.x = 0;
	cornerRect.y = 0;
	cornerRect.w = 10;
	cornerRect.h = 10;

	SDL_Rect cornerRect2;
	cornerRect2.x = 20;
	cornerRect2.y = 0;
	cornerRect2.w = 10;
	cornerRect2.h = 10;

	while (running)
	{
		SDL_SetRenderDrawColor(renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_GetMouseState(&mouseX, &mouseY);

		if (change)
		{
			if (pointSelect)
			{
				scan1.X = mouseX;
				scan1.Y = mouseY;
			}
			else
			{
				scan2.X = mouseX;
				scan2.Y = mouseY;
			}
			done = false;
		}
		if (!change || !pointSelect)
		{ // move to same point
			if (!done)
			{
				SDL_SetRenderDrawColor(renderer, 244, 244, 100, SDL_ALPHA_OPAQUE);

				std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); //start time

				matchScan(&scan1, &scan2);
				done = true;
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now(); // end time

				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count(); //calculate how long did it take to match
				//std::cout << '\r';
				std::cout << "time taken: " << duration/1000 << std::endl;
			}
		}

		//draw indication corner
		SDL_SetRenderDrawColor(renderer, (255 / (!change + 1)) * pointSelect, (255 / (!change + 1)) * !pointSelect, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &cornerRect);

		//draw first scan
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // draw in Green
		drawpoints(renderer, &scan1);
		drawCenter(renderer, scan1.X, scan1.Y, 0);

		//draw second scan
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE); // draw in Red
		drawpoints(renderer, &scan2);
		drawCenter(renderer, scan2.X, scan2.Y, 0);

		if (false) // slowdown program
			for (volatile uint16_t i = 0; i < 1000; i++)
				for (volatile uint16_t i = 0; i < UINT16_MAX; i++)
					;

		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{
					done = false;
				}
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					pointSelect = !pointSelect;
				}
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					change = !change;
				}
				if (event.button.button == SDL_BUTTON_MIDDLE)
				{

				}
			}
		}
	}

	std::cout << std::endl;

	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
	}
	if (window)
	{
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	return 0;
}

void drawpoints(SDL_Renderer *renderer, Scan *scan)
{
	for (int i = 0; i < 360; i++)
	{
		SDL_Rect srcrect;
		srcrect.x = scan->X + scan->points[i][0];
		srcrect.y = scan->Y + scan->points[i][1];
		srcrect.w = 2;
		srcrect.h = 2;
		SDL_RenderFillRect(renderer, &srcrect);
	}
}

void drawCenter(SDL_Renderer *renderer, int16_t x, int16_t y, bool mass)
{
	SDL_RenderDrawLine(renderer, x - 5, y, x + 5, y);
	SDL_RenderDrawLine(renderer, x, y - 5, x, y + 5);
	if (mass)
	{
		SDL_RenderDrawLine(renderer, x - 5, y - 5, x + 5, y + 5);
		SDL_RenderDrawLine(renderer, x - 5, y + 5, x + 5, y - 5);
	}
}

void findClosesedPoint(Point *point, Scan *scan, int16_t *disx, int16_t *disy)
{
	const uint16_t use = 360;
	const uint8_t divide = 360 / use;

	uint16_t closedDis = UINT16_MAX;

	uint16_t closesed = 0;

	for (int i = 0; i < use; i++)
	{
		if (scan->points[i * divide][0] || scan->points[i * divide][1])
		{
			int16_t testX = scan->points[i * divide][0] + scan->X;
			int16_t testY = scan->points[i * divide][1] + scan->Y;

			uint16_t dis = sqrt(pow(point->X - testX, 2) + pow(point->Y - testY, 2));

			if (dis < closedDis)
			{

				closedDis = dis;
				closesed = i;
				*disx = point->X - testX;
				*disy = point->Y - testY;
				//drawCenter(renderer,point->X,point->Y,0);
			}
		}
	}
	//SDL_RenderDrawLine(renderer, point->X,point->Y,scan->points[closesed][0] + scan->X,scan->points[closesed][1] + scan->Y);
	//drawCenter(renderer,point->X,point->Y,1);
	//drawCenter(renderer,testX,testY,0);
	//	std::cout <<  "x: "  << point->X << " y: " << point->Y <<  " x2: "  << scan->X << " y2: " << scan->Y << std::endl;
}

Scan arrayToScan(int16_t X, int16_t Y, uint16_t *points)
{
	Scan scan;
	scan.X = X;
	scan.Y = Y;
	scan.R = 0;
	for (uint16_t i = 0; i < 360; i++)
	{
		float angle = i * PI;
		scan.points[i][0] = (sin(angle) * points[i]);
		scan.points[i][1] = (cos(angle) * points[i]);
	}

	return scan;
}

void matchScan(Scan *scan1, Scan *scan2)
{
	const float tol = 2;
	int32_t totalDistancex = 0;
	int32_t totalDistancey = 0;
	uint16_t counter = 20;
	float dis = 0.0;
	do
	{
		totalDistancex = 0;
		totalDistancey = 0;

		const uint16_t number = 360;
		for (int i = 0; i < number; i++)
		{
			if (scan2->points[i][0] != 0 || scan2->points[i][1] != 0)
			{
				int16_t disx = 0;
				int16_t disy = 0;

				Point point;

				point.X = scan2->points[i][0] + scan2->X;
				point.Y = scan2->points[i][1] + scan2->Y;

				findClosesedPoint(&point, scan1, &disx, &disy);

				totalDistancex += disx;
				totalDistancey += disy;
			}
		}

		totalDistancex /= number;
		totalDistancey /= number;

		scan1->X += totalDistancex;
		scan1->Y += totalDistancey;

		counter--;
		dis = sqrtf( pow(totalDistancex, 2) + pow(totalDistancey, 2));

	} while( dis > tol && counter != 0);
}