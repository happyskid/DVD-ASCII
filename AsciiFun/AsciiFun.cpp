#include <iostream>
#include <string>
#include <windows.h>
#include <chrono>
#include <thread>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void maxsc()
{
	HWND Hwnd = GetForegroundWindow();
	ShowWindow(Hwnd, SW_MAXIMIZE);
}

void clear() 
{
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

int main(int argc, const char* argv[])
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
	maxsc();

	if (argc < 2)
	{
		std::cout << "Missing argument 1\n";
		return 1;
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int w, h, c, densityLen;
	unsigned int channels = 4;
	unsigned char* data = stbi_load(argv[1], &w, &h, &c, channels);
	COORD topLeft = { 0, 0 };

	std::string density = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,.";
	std::vector<std::string> img{};

	int position[] = { 8,13 };
	int velocity[] = { 1,1 };
	int edges[] = { 125,50 };
	int colorRange = 15;

	char borderChar = '#';
	char emptyChar = ' ';

	// Clear console buffer
	FlushConsoleInputBuffer(hConsole);

	if (data == NULL)
	{
		std::cout << "Error loading image\n";
		return 1;
	}

	densityLen = density.size();

	// Save image ascii to memory
	for (int j = 0; j < h; ++j)
	{
		std::string line = "";
		for (int i = 0; i < w; ++i)
		{
			unsigned char* pxl = data + (i + w * j) * channels;

			if (c > 3 && pxl[3] < 20)
			{
				line += emptyChar;
				continue;
			}

			// Get avg rgb (brightness) and get the density from it
			float avg = ((((float)pxl[0] + (float)pxl[1] + (float)pxl[2]) / 3) / 255) * densityLen;
			line += density[(int)avg];
		}
		img.push_back(line);
	}

	std::cout << "Press any key to start\n\n";
	std::cin.get();

	while (true)
	{
		std::string frame;
		int end = edges[0] - position[0];

		for (int i = 0; i < position[1]; ++i)
		{
			frame += borderChar;
			for (int j = 0; j < edges[0] + w; ++j)
			{
				if (i == 0)
				{
					frame += borderChar;
					continue;
				}
				frame += emptyChar;
			}
			frame += "#\n";
		}

		for (auto& element : img) {
			frame += borderChar;
			for (int j = 0; j < position[0]; ++j)
			{
				frame += emptyChar;
			}
			frame += element;
			for (int j = 0; j < end; ++j)
			{
				frame += emptyChar;
			}
			frame += "#\n";
		}

		for (int i = 0; i < edges[1] - position[1]; ++i)
		{
			frame += borderChar;
			for (int j = 0; j < edges[0] + w; ++j)
			{
				if (i >= edges[1] - position[1]-1)
				{
					frame += borderChar;
					continue;
				}
				frame += emptyChar;
			}
			frame += "#\n";
		}

		// New frame
		SetConsoleCursorPosition(hConsole, topLeft);
		std::cout << frame;

		if (position[0] >= edges[0] || position[0] <= 0)
		{
			velocity[0] = -velocity[0];
			SetConsoleTextAttribute(hConsole, rand() % colorRange+1);
		}

		if (position[1] >= edges[1] || position[1] <= 0)
		{
			velocity[1] = -velocity[1];
			SetConsoleTextAttribute(hConsole, rand() % colorRange+1);
		}

		position[0] += velocity[0];
		position[1] += velocity[1];

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}