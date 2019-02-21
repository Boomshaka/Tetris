#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <cstring>
#include <cstdio>
#include <time.h>
#include <cstdlib>
#include <curses.h>
#include <unistd.h>
#include <chrono>

#define FIELD_WIDTH 12
#define FIELD_HEIGHT 22
#define BLOCK_WIDTH 4
#define BLOCK_HEIGHT 4

char field[FIELD_HEIGHT][FIELD_WIDTH];
char display[FIELD_HEIGHT][FIELD_WIDTH];

enum {
  TYPE_I,
  TYPE_O,
  TYPE_S,
  TYPE_Z,
  TYPE_J,
  TYPE_L,
  TYPE_T,
  TYPE_MAX
};
enum {
  ANGLE_0,
  ANGLE_90,
  ANGLE_180,
  ANGLE_270,
  ANGLE_MAX
};

char blockShapes[TYPE_MAX][ANGLE_MAX][BLOCK_HEIGHT][BLOCK_WIDTH] = {
  //TYPE_I,
  {
    //ANGLE_0,
    {
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0,
    },
    //ANGLE_90,
    {
      0, 0, 0, 0,
      1, 1, 1, 1,
      0, 0, 0, 0,
      0, 0, 0, 0
    },
    //ANGLE_180,
    {
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
    },
    //ANGLE_270,
    {
      0, 0, 0, 0,
      0, 0, 0, 0,
      1, 1, 1, 1,
      0, 0, 0, 0,
    }
  },
  //TYPE_O,
  {
      //ANGLE_0
      {
        0, 1, 1, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_90
      {
        0, 1, 1, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_180
      {
        0, 1, 1, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_270
      {
        0, 1, 1, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      }
  },
  //TYPE_S,
  {
      //ANGLE_0
      {
        0, 0, 1, 1,
        0, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_90
      {
        0, 1, 0, 0,
        0, 1, 1, 0,
        0, 0, 1, 0,
        0, 0, 0, 0,
      },
      //ANGLE_180
      {
        0, 0, 0, 0,
        0, 0, 1, 1,
        0, 1, 1, 0,
        0, 0, 0, 0,
      },
      //ANGLE_270
      {
        0, 0, 1, 0,
        0, 0, 1, 1,
        0, 0, 0, 1,
        0, 0, 0, 0,
      }
  },
  //TYPE_Z,
  {
      //ANGLE_0
      {
        0, 1, 1, 0,
        0, 0, 1, 1,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_90
      {
        0, 0, 1, 0,
        0, 1, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_180
      {
        0, 0, 0, 0,
        0, 1, 1, 0,
        0, 0, 1, 1,
        0, 0, 0, 0,
      },
      //ANGLE_270
      {
        0, 0, 0, 1,
        0, 0, 1, 1,
        0, 0, 1, 0,
        0, 0, 0, 0,
      }
  },
  //TYPE_J,
  {
      //ANGLE_0
      {
        0, 0, 1, 0,
        0, 0, 1, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
      },
      //ANGLE_90
      {
        0, 0, 0, 0,
        0, 1, 1, 1,
        0, 0, 0, 1,
        0, 0, 0, 0,
      },
      //ANGLE_180
      {
        0, 0, 1, 1,
        0, 0, 1, 0,
        0, 0, 1, 0,
        0, 0, 0, 0,
      },
      //ANGLE_270
      {
        0, 1, 0, 0,
        0, 1, 1, 1,
        0, 0, 0, 0,
        0, 0, 0, 0,
      }
  },
  //TYPE_L,
  {
      //ANGLE_0
      {
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 1, 0,
        0, 0, 0, 0,
      },
      //ANGLE_90
      {
        0, 0, 1, 0,
        1, 1, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_180
      {
        1, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_270
      {
        0, 0, 0, 0,
        1, 1, 1, 0,
        1, 0, 0, 0,
        0, 0, 0, 0,
      }
  },
  //TYPE_T,
  {
      //ANGLE_0
      {
        0, 1, 1, 1,
        0, 0, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_90
      {
        0, 1, 0, 0,
        0, 1, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
      },
      //ANGLE_180
      {
        0, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 1, 1,
        0, 0, 0, 0,
      },
      //ANGLE_270
      {
        0, 0, 0, 1,
        0, 0, 1, 1,
        0, 0, 0, 1,
        0, 0, 0, 0,
      }
  }
};


int blockType, blockAngle, blockX, blockY, numLineErased;
double timeLevel = 1000;
int difficulty = 0;
int score = 0;

void scoreKeep(int numLineErased){
  score += 40 * numLineErased * numLineErased * numLineErased;
  difficulty = score / 200;
  timeLevel = timeLevel - difficulty * 500;
}

void blockify(){
  memcpy(display, field, sizeof(field)); // creates a copy field for display
  for (int i = 0; i < BLOCK_HEIGHT; i++){
    for (int j = 0; j < BLOCK_WIDTH; j++){
      display[blockY + i][blockX + j] |= blockShapes[blockType][blockAngle][i][j];
    }
  }
  clear();
  refresh();

  for (int i = 0; i < FIELD_HEIGHT; i++){
    for (int j = 0; j < FIELD_WIDTH; j++){
      printw(display[i][j] ? "8" : " ");
      refresh();
    }
    printw("\n");
    refresh();
  }
  printw("Score: %u", score);
  refresh();
}

bool isBlockMovable(int blockX, int blockY, int blockType, int blockAngle){
  for (int i = 0; i < BLOCK_HEIGHT; i++){
    for (int j = 0; j < BLOCK_WIDTH; j++){
      if (blockShapes[blockType][blockAngle][i][j] && field[blockY + i][blockX + j])
        return false;
    }
  }
  return true;
}

void eraseLine(int lineNum){
  for (int k = 1; k < FIELD_WIDTH - 1; k++){
    field[lineNum][k] = 0;
  }
  for (int i = lineNum; i > 0; i--){
    for (int j = 1; j < FIELD_WIDTH - 1; j++){
      field[i][j] = field[i - 1][j];
    }
  }
}

bool isLineErasable(int i){
    for (int j = 1; j < FIELD_WIDTH - 1; j++){
      if (!field[i][j]){
        return false;
      }
    }
    return true;
}

void checkEveryLine(){
  for (int i = 0; i < FIELD_HEIGHT - 1; i++){
    if (isLineErasable(i)){
      numLineErased++;
      eraseLine(i);
      i--;
    }
  }
  scoreKeep(numLineErased);
  numLineErased = 0;
}

void resetBlock(){
  blockX = 5;
  blockY = 0;
  blockType = rand() % TYPE_MAX;
  blockAngle = rand() % ANGLE_MAX;
}

double time_in_millisec(){
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

int kbhit(){
  int ch = getch();

  if (ch != ERR) {
    ungetch(ch);
    return 1;
  }
  else{
    return 0;
  }
}

int main () {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);
  memset(field, 0, sizeof(field));
  for (int i = 0; i < FIELD_HEIGHT; i++){
    field[i][0] = field[i][FIELD_WIDTH - 1] = 1;
  }
  for (int i = 0; i < FIELD_WIDTH; i++) {
    field[FIELD_HEIGHT - 1][i] = 1;
  }
  resetBlock();
  //double t = time_in_millisec();
  time_t t = time(NULL);
  while (true){
    if (kbhit()){
      switch(getch()){
        //case 'w':
        case 'a':
          if(isBlockMovable(blockX-1, blockY, blockType, blockAngle)){
            blockX--;
          }
          break;
        case 's':
          if(isBlockMovable(blockX, blockY + 1, blockType, blockAngle)){
            blockY++;
          }
          break;
        case 'd':
          if(isBlockMovable(blockX+1, blockY, blockType, blockAngle)){
            blockX++;
          }
          break;
        case 0x20:
          if(isBlockMovable(blockX, blockY, blockType, (blockAngle + 1) % ANGLE_MAX)){
            blockAngle = (blockAngle + 1) % ANGLE_MAX;
          }
      }
      blockify();
    }
    if (t != time(NULL)){//t + timeLevel < time_in_millisec()){
      //t = time_in_millisec();
      t = time(NULL);
      if(!isBlockMovable(blockX, blockY + 1, blockType, blockAngle)){
        for (int i = 0; i < BLOCK_HEIGHT; i++){
          for (int j = 0; j < BLOCK_WIDTH; j++){
            field[blockY + i][blockX + j] |= blockShapes[blockType][blockAngle][i][j];
          }
        }
        checkEveryLine();
        resetBlock();
      }
      else{
        blockY++;
      }
      blockify();
    }

  }




  //getchar();
}
