#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sched.h>

#include "lab10.h"


char * dishes[] = {"Vanilla", "Chocolate", "Strawberry", "Mint Chocolate Chip",
                        "Halva", "Baklava"};

static int customer_in_seat = -1;
static int customer_served_food = 0;
static int served_customers = 0;
static int customer_order = -1;

static pthread_mutex_t lock;


static void waiter_serve_check(int customer_id)
{
    if (customer_id > MAX_CUSTOMERS || customer_id < 0) {
        printf("VIOLATION: Attempted to serve a non-existent customer %d!\n", customer_id);
        exit(1);
    }
}


static char WSC[] = "VIOLATION: Attempted to seat customer %d while currently serving customer %d!\n";
static char WSCA[] = "VIOLATION: Attempted to seat customer %d but it is already seated!\n";

static void waiter_seat_check(int customer_id)
{
    waiter_serve_check(customer_id);

    if (customer_in_seat != -1) {
        if (customer_in_seat != customer_id)
            printf(WSC, customer_id, customer_in_seat);
        else
            printf(WSCA, customer_id);
        exit(1);
    }
}


static char WNS[] = "VIOLATION: Attempted to serve food to customer %d but no customer is being served!\n";
static char WSF[] = "VIOLATION: Attempted to serve food to customer %d but customer %d is being served!\n";
static char WWO[] = "VIOLATION: Customer %d was served %s when it ordered %s!\n";


static void waiter_food_check(int customer_id, int dish)
{
    waiter_serve_check(customer_id);

    if (customer_in_seat == -1) {
        printf(WNS, customer_id);
        exit(1);
    }
    if (customer_in_seat != customer_id) {
        printf(WSF, customer_id, customer_in_seat);
        exit(1);
    }
    if (customer_order == -1) {
        printf("VIOLATION: Waiter served dish but order was not taken!\n");
        exit(1);
    }
    if (dish != customer_order) {
        printf(WWO, customer_id, dishes[dish], dishes[customer_order]);
        exit(1);
    }
}


static char SEC[] = "VIOLATION: Waiter served customer %d before customer %d was done!\n";


void seat_customer(int customer_id)
{
    sched_yield();

    pthread_mutex_lock(&lock);

    printf("Waiter is seating customer %d\n", customer_id);
    waiter_seat_check(customer_id);
    customer_in_seat = customer_id;

    pthread_mutex_unlock(&lock);
}


void bring_food_to_customer(int customer_id, int dish)
{
    sched_yield();

    pthread_mutex_lock(&lock);
    printf("Waiter brings order of %s to customer %d\n", dishes[dish], customer_id);
    waiter_food_check(customer_id, dish);
    customer_served_food = 1;
    pthread_mutex_unlock(&lock);
}


void ready_to_seat_customer(int customer_id)
{
    printf("Customer %d is ready to be seated\n", customer_id);
}


static char CWO[] = "VIOLATION: Customer %d placed an order but customer %d is being served!\n";

void order_placed_by_customer(int customer_id, int dish)
{
    sched_yield();

    pthread_mutex_lock(&lock);

    printf("Customer %d has placed order for %s\n", customer_id, dishes[dish]);
    if (customer_in_seat == -1) {
        printf("VIOLATION: Customer %d placed an order but the table is empty!\n", customer_id);
        exit(1);
    }
    if (customer_in_seat != customer_id) {
        printf(CWO, customer_id, customer_in_seat);
        exit(1);
    }

    customer_order = dish;
    pthread_mutex_unlock(&lock);
}


void service_completed_for_customer(int customer_id)
{
    sched_yield();

    pthread_mutex_lock(&lock);

    printf("Customer %d is done eating\n", customer_id);
    served_customers++;
    customer_in_seat = -1;
    customer_order = -1;
    customer_served_food = 0;

    pthread_mutex_unlock(&lock);
}


static int select_dish(void)
{
    if (VANILLA_ONLY)
        return 0;
    return rand() % (sizeof(dishes) / sizeof(char *));
}



static void * customer_routine(void * args)
{
    int arg = (int)(long)args;
    int dish = select_dish();
    customer(arg, dish);

    return NULL;
}


static void * waiter_routine(void * args)
{
    while (served_customers < MAX_CUSTOMERS)
        waiter();
    return NULL;
}


int main(void)
{
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

    initialize();

    pthread_t waiter_tid;
    pthread_create(&waiter_tid, NULL, waiter_routine, NULL);

    pthread_t tids[MAX_CUSTOMERS];
    for (int i = 0; i < MAX_CUSTOMERS; i++)
        pthread_create(&tids[i], NULL, customer_routine, (void*)(long)i);

    for (int i = 0; i < MAX_CUSTOMERS; i++)
        pthread_join(tids[i], NULL);

    pthread_join(waiter_tid, NULL);

    return EXIT_SUCCESS;
}
