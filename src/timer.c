#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TOTAL_TIME 10 * (60 * 1000) // total time 10 minutes in ms

void timeDelay(ms)
{
    clock_t delay = ms + clock();
    while (delay > clock())
        ;
}

void start_timer(void) // get start time
{
    long counter = 0;
    int flag = 0;
    while (1)
    {
        timeDelay(1);
        if (counter < TOTAL_TIME && flag == 0)
        {
            counter++;
            continue;
        }
        else
        {
            printf("Time is out!\n");
            break;
        }
    }
}

void pause_timer() // pause time counting after one move
{
    while (!flag == 0)
    {
        sleep();
    }
}

void continue_timer()
{
    while (flag == 0)
        start_timer();
}

int get_remaining_move_time() //calculate remaining time
{

    float timeLeft = (float)TOTAL_TIME;
    clock_t start_t, end_t;

    while (1)
    {
        start_t = clock();

        generate_moves();
        evaluate_moves();
        choose_move();
        execute_move();

        end_t = clock();

        float used_t = (long double)(end_t - start_t) / CLOCKS_PER_SEC;
        timeLeft = (int)(((timeLeft / 1000) - used_t) * 1000);

        if (/* condition */)
        {
            break; // jump out of the loop and return the left time
        }
    }

    return timeLeft;
}

void stop_timer()
{
}