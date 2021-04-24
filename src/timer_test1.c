#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> // for sleep()
// #include <conio.h> // for keyboard hit

// unsigned int MAX_TIME = 10 * 60; // 10min e.g. 600s
unsigned int MAX_TIME = 5; // 10min e.g. 600s

// game end because overtime
unsigned int GAME_END = 0;

// count timer
unsigned int player_black_time = 0;
unsigned int player_white_time = 0;

// count timer switch
unsigned int white_turn = 1;
unsigned int black_turn = 0;

void switch_player_timer()
{
   if (white_turn == 1)
   {
      white_turn = 0;
      black_turn = 1;
   }
   else if (black_turn == 1)
   {
      black_turn = 0;
      white_turn = 1;
   }
}

unsigned int return_time_black_player()
{
   return player_black_time;
}

unsigned int return_time_white_player()
{
   return player_white_time;
}

unsigned int remaining_time_white_player()
{
   return MAX_TIME - return_time_white_player();
}

unsigned int remaining_time_black_player()
{
   return MAX_TIME - return_time_black_player();
}

void check_over_time_limit()
{
   if (white_turn == 1)
   {
      if (player_white_time > MAX_TIME)
      {
         GAME_END = 1;
         printf("\nPlayer White lose because timeout");
         black_turn = 0;
         white_turn = 0;
      }
   }
   else if (black_turn == 1)
   {
      if (player_black_time > MAX_TIME)
      {
         GAME_END = 1;
         printf("\nPlayer Black lose because timeout");
         black_turn = 0;
         white_turn = 0;
      }
   }
}

void *time_counter(void *args)
{
   while (1)
   {
      if (white_turn == 1)
      {
         player_white_time++;
         check_over_time_limit();
         printf("\nCurrent White Player Time: %dsec", player_white_time);
      }
      else if (black_turn == 1)
      {
         player_black_time++;
         check_over_time_limit();
         printf("\nCurrent Black Player Time: %dsec", player_black_time);
      }

      if (white_turn == 0 && black_turn == 0)
      {
         break;
      }

      sleep(1);
   }
}

int main()
{
   pthread_t game_thread;
   pthread_t time_counter_thread;

   pthread_create(&time_counter_thread, NULL, time_counter, NULL);

   int curr;
   printf("Switch Player <s> or Stop with <q> :");
   while (1)
   {
      curr = getchar(); // stop the programm from terminating immediately
      if (curr == 'q')
      {
         return 0;
      }
      if (curr == 's')
      {
         check_over_time_limit();
         switch_player_timer();
      }
      sleep(0.1);
   }
}
