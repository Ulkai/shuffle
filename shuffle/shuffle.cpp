#include "stdafx.h"
#include "malloc.h"
#include "string.h"
#include "time.h"
#include "math.h"

const int MAX_LETTERS = 30;


class Shelf {
public:
	static int letterCount;
	static char firstLetter;
	static char lastLetter;
	static int maxGen;
	char letters[MAX_LETTERS];
	int gen;

public:
	Shelf(const char *initLetters) {

		gen = 1;
		letterCount = (int)strlen(initLetters);
		memcpy(letters, initLetters, letterCount + 1);	
		firstLetter = 127;
		for (char *l = letters; *l; l++) {
			if (*l < firstLetter) {
				firstLetter = *l;
			}
		}
		lastLetter = firstLetter + letterCount - 1;

		maxGen = 12;
	}

	Shelf(const Shelf shelf, int moveLetters, int fromPos, int toPos) {

		gen = shelf.gen + 1;

		int from, to;
		if (fromPos < toPos) {
			from = fromPos;
			to = toPos;
		} else {
			from = toPos;
			to = fromPos + moveLetters;
			moveLetters = fromPos - toPos;
		}
		
		memcpy(letters, shelf.letters, from);
		memcpy(letters + from, shelf.letters + from + moveLetters, to - from - moveLetters);
		memcpy(letters + to - moveLetters, shelf.letters + from, moveLetters);
		memcpy(letters + to, shelf.letters + to, letterCount - to);
		letters[letterCount] = 0;

		//printf("%.2d %s(%d) -(%d)> %s(%d)\n", gen, shelf.letters, shelf.entropy(), moveLetters, letters, entropy());
	}

	bool checkShuffleEntropy() {
		time_t timer;
		time(&timer);

		bool solution = shuffleEntropy();

		double seconds = difftime(time(NULL), timer);
		printf("\n%.f seconds\n", seconds);
		if (!solution) {
			printf("...no solution\n");
		}
		return solution;
	}

	bool shuffleEntropy() {

		const int MAX_SOLUTIONS = 4000;
		struct solution {
			int src, dst, num, cost;
		} solutions[MAX_SOLUTIONS];

		int minCost = 0;
		int count = 0;

		for (int src = 0; src < letterCount - 2; src++) {
			for (int num = 2; num < letterCount - src; num++) {
				for (int dst = src + num + 1; dst < letterCount + 1; dst++) {
					int cost = entropyChangeCost(src, dst, num);
					if (cost < -2)  {
						solutions[count].dst = dst;
						solutions[count].src = src;
						solutions[count].num = num;
						solutions[count].cost = cost;
						count ++;
						if (count > MAX_SOLUTIONS) {
							printf("Overflow\n");
							return false;
						}
					}
				}
			}
		}

		for (int src = 1; src < letterCount - 2; src++) {
			for (int num = 2; num < letterCount - src + 1; num++) {
				for (int dst = 0; dst < src; dst++) {
					int cost = entropyChangeCost(src, dst, num);
					if (cost < -2)  {
						solutions[count].dst = src + num;
						solutions[count].src = dst;
						solutions[count].num = src - dst;
						solutions[count].cost = cost;
						count++;
						if (count > MAX_SOLUTIONS) {
							printf("Overflow\n");
							return false;
						}
					}
				}
			}
		}
		

		for (int i = 0; i < count; i++) {

			Shelf shelf(*this, solutions[i].num, solutions[i].src, solutions[i].dst);
			if (shelf.entropy() == 0 || (shelf.gen < maxGen && shelf.shuffleEntropy())) {
				char str[100];
				char *dst = str;
				char *src = letters;
				dst[0] = 0;

				strncat_s(dst, sizeof(str), src, solutions[i].src);
				strcat_s(dst, sizeof(str), " ");
				src += solutions[i].src;

				strncat_s(dst, sizeof(str), src, solutions[i].num);
				strcat_s(dst, sizeof(str), " ");
				src += solutions[i].num;

				strncat_s(dst, sizeof(str), src, solutions[i].dst - solutions[i].src - solutions[i].num);
				strcat_s(dst, sizeof(str), " ");
				src += solutions[i].dst - solutions[i].src - solutions[i].num;

				strncat_s(dst, sizeof(str), src, letterCount - solutions[i].dst);

				printf("%.2d %s\n", gen, str );
				return true;
			}
		}

		return false;
	}

	bool check() const {
		const char *c = letters;
		char a = 'a';
		while (*c) {
			if (a != *c) {
				return false;
			}
			a++;
			c++;
		}
		return true;
	}

	int entropy() const {
		int sum = 0;
		for (int i = 0; i <= letterCount; i++) {
			sum += lettersEntropy(i - 1, i);
		}
		return sum;
	}

	int entropyChangeCost(int src, int dst, int num) const {
		int beforeEntropy
			= lettersEntropy(src - 1, src)
			+ lettersEntropy(dst - 1, dst)
			+ lettersEntropy(src + num - 1, src + num);

		int afterEntropy
			= lettersEntropy(src - 1, src + num)
			+ lettersEntropy(dst - 1, src)
			+ lettersEntropy(src + num - 1, dst);

		return afterEntropy - beforeEntropy;
	}

	int lettersEntropy(int lPos, int rPos) const {
		char lVal;
		char rVal;

		if (lPos == -1) {
			lVal = firstLetter - 1;
		} else {
			lVal = letters[lPos];
		}

		if (rPos == letterCount) {
			rVal = lastLetter + 1;
		} else {
			rVal = letters[rPos];
		}

		int diff = rVal - lVal;

		if (diff == 1) {
			return 0;
		} else {
			return 1;
		}
	}
};

int Shelf::letterCount = 0;
char Shelf::firstLetter = 'a';
char Shelf::lastLetter;
int Shelf::maxGen;

int main()
{
	//char origLetters[] = "aedbc";
	char origLetters[] = "xisvbejcugodkhqwnlmarftp";

	Shelf shelf(origLetters);

	printf("check %s\n\n", origLetters);

	shelf.checkShuffleEntropy();

     return 0;
}

