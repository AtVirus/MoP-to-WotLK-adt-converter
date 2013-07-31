#pragma once

class ConsoleProgress
{
public:
	static void printProgress(int curValue, int maxValue) {
		printf("\r ");
		int numBars = (int)((curValue / (float)maxValue) * 40.0f);
		for(int i = 0; i < numBars; ++i)
			printf("=");

		for(int i = numBars; i < 40; ++i)
			printf(" ");

		printf(" %.02f%%", ((float)curValue / maxValue) * 100.0f);

	}
};