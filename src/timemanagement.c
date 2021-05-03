#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> // for sleep()
// #include <conio.h> // for keyboard hit

unsigned int MAX_TIME = 10 * 60 * 1000;  // 10min in millisecond
unsigned int MOVE_TIME_LIMIT = 4 * 1000; // timelimit for each move 1min
unsigned int GAME_END = 0;
unsigned int temp_counter = 0;
unsigned int kumulative_counter = 0;
unsigned int move_counter = 0;
unsigned int bot_turn = 0;

// gives back the current bot time
unsigned int return_time_current_bot()
{
    return kumulative_counter;
}

// see method name
void is_timeout()
{
    move_counter = kumulative_counter - temp_counter;
    if (temp_counter > MAX_TIME || move_counter > MOVE_TIME_LIMIT)
    {
        bot_turn = 0; // stop counting
        printf("\nmove time = %d", move_counter);
        GAME_END = 1; // end the game
    }
}
// game over
void stop_timer()
{
    if (GAME_END == 1)
    {
        printf("\nthread ending...");
        pthread_exit(0);
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
            kumulative_counter++;
            is_timeout();
            if (kumulative_counter % 1000 == 0)
            {
                printf("\nkumulative time: %d s", kumulative_counter / 1000);
            }
        }
        if (GAME_END == 1)
        {
            stop_timer();
        }
        usleep(1 * 1000); // usleep takes microseconds
    }
}

void start_timer()
{
    //pthread_t game_thread;
    pthread_t time_counter_thread;
    pthread_create(&time_counter_thread, NULL, time_counter, NULL);
}

unsigned int get_remaining_move_time()
{
    return MAX_TIME - return_time_current_bot();
}

void pause_timer()
{
    if (GAME_END == 0)
    {
        bot_turn = 0;
        temp_counter = kumulative_counter;
    }
}

void continue_timer()
{
    if (GAME_END == 0)
        bot_turn = 1;
}

int main()
{
    start_timer();
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
            printf("\nbot = %d", bot_turn);
            if (bot_turn == 1)
            {
                pause_timer();
                printf("\nbot = %d, pause...", bot_turn);
                printf("\ntotal past time = %d", kumulative_counter);
            }
            else
            {
                continue_timer();
            }
            printf("\ntotal time left = %d", get_remaining_move_time());
            printf("\nmove time = %d", move_counter);
        }
        sleep(0.1);
        if (GAME_END == 1)
        {
            break;
        }
    }
}
