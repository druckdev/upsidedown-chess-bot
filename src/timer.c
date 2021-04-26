#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> // for sleep()
// #include <conio.h> // for keyboard hit

// unsigned int MAX_TIME = 10 * 60; // 10min e.g. 600s
unsigned int MAX_TIME = 10 * 60 * 1000; // 10min in millisecond
// game ends because overtime or because of checkmate or bot goes full skynet, whatever comes first
unsigned int GAME_END = 0;

// count timer
unsigned int bot_current_time = 0;

// count timer switch
// 1 our turn -> count time
// 0 not our turn -> not counting
unsigned int bot_turn = 0;

// to know when we should start counting our time again and when to stop
void switch_bot_timer()
{
    if (bot_turn == 1)
    {
        bot_turn = 0; // stop counting
    }
    else
    {
        bot_turn = 1; // start counting again
    }
}

// gives back the current bot time
unsigned int return_time_current_bot()
{
    return bot_current_time;
}

// gives back remaining time until bot lose the game
unsigned int remaining_time_current_bot()
{
    return MAX_TIME - return_time_current_bot();
}

// see method name
void check_over_time_limit()
{
    if (bot_current_time > MAX_TIME)
    {
        bot_turn = 0; // stop counting
        printf("\nBot lose because timeout");
        printf("\nEnding game");
        GAME_END = 1; // end the game
    }
}

// time counting thread
void *time_counter(void *args)
{
    // time loop
    while (1)
    {
        if (bot_turn == 1)
        {
            bot_current_time++;
            check_over_time_limit();
            if (bot_current_time % 1000 == 0) // for test purpose
            {
                printf("\nCurrent current bot time: %d s", bot_current_time / 1000);
            }
        }

        if (GAME_END) // ending when our time runs out see check_over_time_limit()
        {
            break; // jump out of while
        }
        usleep(1 * 1000); // usleep takes microseconds
    }
    printf("\nEnding thread");
    pthread_exit(0); // stop thread
}

int main()
{
    pthread_t game_thread;
    pthread_t time_counter_thread;

    pthread_create(&time_counter_thread, NULL, time_counter, NULL);

    int curr;
    printf("Switch bot <s> or Stop with <q> : ");
    while (1)
    {
        curr = getchar(); // stop the programm from terminating immediately
        if (curr == 'q')
        {
            return 0;
        }
        if (curr == 's')
        {
            switch_bot_timer();
            printf("\nbot turn %d", bot_turn);
        }
        sleep(0.1);
    }
}