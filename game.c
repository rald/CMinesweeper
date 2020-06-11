#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>

#define ENT_KEY 13
#define ESC_KEY 27
#define UP_KEY 328
#define DN_KEY 336
#define LF_KEY 331
#define RT_KEY 333

#define MINE 9
#define NONE 10

void clrscr(void) {
	COORD coord={0,0};
	DWORD chars;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD conSize;
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	
	GetConsoleScreenBufferInfo(console,&csbi);
	conSize=csbi.dwSize.X*csbi.dwSize.Y;
	FillConsoleOutputCharacter(console,' ',conSize,coord,&chars);
	GetConsoleScreenBufferInfo(console,&csbi);
	FillConsoleOutputAttribute(console,csbi.wAttributes,conSize,coord,&chars);
	SetConsoleCursorPosition(console,coord);
}

void gotoxy(int x,int y) {
	COORD coord={x-1,y-1};
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(console,coord);
}

void textattr(WORD attrib) {
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console,attrib);
}

void setMines(int board[],int w,int h,int numMines) {
	for(int i=0;i<w*h;i++) {
		board[i]=NONE;
	}
	for(int i=0;i<numMines;i++) {
		board[i]=MINE;
	}
	for(int i=w*h-1;i>0;i--) {
	 int j=rand()%(i+1);
	 int t=board[i];
	 board[i]=board[j];
	 board[j]=t;
	}
}

void printBoard(int board[],int w,int h,int cx,int cy,int x,int y, bool showMines) {
	CONSOLE_CURSOR_INFO cci;

	cci.dwSize=99;
	cci.bVisible=FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cci);

	for(int j=0;j<h;j++) {
		for(int i=0;i<w;i++) {
			gotoxy(cx+i+1,cy+j+1);
			switch(board[j*w+i]) {
				case MINE: 
					textattr(BACKGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);					
					if(showMines) {
						if(i==x && j==y) printf("X"); else printf("*"); 
					} else { 
						printf(" "); 
					}
					break;
				case NONE: 
					textattr(BACKGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);					
					printf(" "); 
					break;
				case 0: 
					textattr(BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE);					
					printf(" "); 
					break;
				case 1: case 2: case 3: case 4: 
				case 5: case 6: case 7: case 8:
					textattr(BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE);					
					printf("%d",board[j*w+i]);
					break;
			}
		}
	}
	textattr(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
	printf("\n");

	cci.dwSize=99;
	cci.bVisible=TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cci);

}

bool valid(int w,int h,int x,int y) {
	return x>=0 && x<w && y>=0 && y<h;
}

void sweep(int board[],int w,int h,int x,int y,bool flags[],int *numOpen) {
	if(valid(w,h,x,y)) {
		int z=y*w+x;
		if(!flags[z]) {
			flags[z]=true;
			(*numOpen)++;
			int n=0;
			for(int j=-1;j<=1;j++) {
				for(int i=-1;i<=1;i++) {
					if(i || j) {
						if(valid(w,h,x+i,y+j)) {
							if(board[(y+j)*w+(x+i)]==MINE) n++;
						}
					}
				}
			}
			board[z]=n;
			if(n==0) {
				for(int j=-1;j<=1;j++) {
					for(int i=-1;i<=1;i++) {
						if(i || j) {
							if(valid(w,h,x+i,y+j)) {
								sweep(board,w,h,x+i,y+j,flags,numOpen); 
							}
						}
					}
				}			
			} 
		}
	}
}

int main(void) {

	int w=10,h=10;
	int x=0,y=0;

	int board[w*h];
	bool flags[w*h];
	int numMines=10;
	int numGoals=w*h-numMines;
	int numOpen=0;

	bool quit=false;
	int key;

	srand(time(NULL));

	for(int i=0;i<w*h;i++) flags[i]=false;

	setMines(board,w,h,numMines);

	CONSOLE_CURSOR_INFO cci;

	cci.dwSize=99;
	cci.bVisible=TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cci);

	clrscr();
	printBoard(board,w,h,0,0,x,y,false);

	while(!quit) {
		gotoxy(x+1,y+1);
		key=getch();
		if(key==0 || key==224) key=getch()+256;
		switch(key) {
			case ESC_KEY: clrscr(); quit=true; break;
			case ENT_KEY: 
				if(board[y*w+x]==MINE) {
					printBoard(board,w,h,0,0,x,y,true);
					quit=true;
				} else if(board[y*w+x]==NONE) {
					sweep(board,w,h,x,y,flags,&numOpen); 
					if(numOpen==numGoals) {
						printBoard(board,w,h,0,0,x,y,true);
						printf("Finished!\n");
						quit=true;
					} else {
						printBoard(board,w,h,0,0,x,y,false);				
					}
				}
				break;
			case UP_KEY: if(y>0) y--; break;
			case DN_KEY: if(y<h-1) y++; break;
			case LF_KEY: if(x>0) x--; break;
			case RT_KEY: if(x<w-1) x++; break;
		}
	}
	printf("Game Over.\n");

	cci.dwSize=7;
	cci.bVisible=TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cci);

	return 0;
}
