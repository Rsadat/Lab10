#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "lab10.h"


extern char * dishes[];

static int served_customers;

static int customer_waiting = -1;
static int customer_seated = -1;
static int order_placed = -1;
static int waiting_order = -1;
static int service = -1;
pthread_mutex_t mlock;
pthread_cond_t customer_ready;
pthread_cond_t waiter_ready;
pthread_cond_t order_ready;





void initialize(void)
{
    // add initializations here
    
    pthread_mutex_init(&mlock,NULL);

    pthread_cond_init(&customer_ready, NULL);
    pthread_cond_init(&waiter_ready, NULL);
    pthread_cond_init(&order_ready, NULL);

    served_customers = 0;

}


void customer(int id, int dish)
{
    // signal customer ready to be served
    ready_to_seat_customer(id);

    // TODO 1: try to get ownership of the lock
    customer_waiting = id;

    pthread_mutex_lock(&mlock);
    // TODO 2: Signal to the waiter that customer is waiting
    pthread_cond_signal(&customer_ready);

    pthread_mutex_unlock(&mlock);
    // TODO: 4 Wait to be seated
    while(customer_seated != id){
	pthread_cond_wait(&waiter_ready, &mlock);
    }
    pthread_mutex_unlock(&mlock);

    // TODO: 6 place order and signal waiter
    order_placed_by_customer(id, dish);
    order_placed = dish;
    pthread_mutex_lock(&mlock);
    pthread_cond_signal(&customer_ready);
    //waiting for food to come
    while(waiting_order == -1){
	pthread_cond_wait(&order_ready, &mlock);
    }
    pthread_mutex_unlock(&mlock);

    service_completed_for_customer(id);
    service = 0;
    pthread_mutex_lock(&mlock);
    pthread_cond_signal(&customer_ready);

    pthread_mutex_unlock(&mlock);
}


void waiter(void)
{
    if (served_customers >= MAX_CUSTOMERS)
        return;

    // seat waiting customer

    // TODO 3: replace busy loop with predicate/cond_wait construct
    while (customer_waiting == -1){
	pthread_cond_wait(&customer_ready, &mlock);
    }
    int customer = customer_waiting;

    // TODO 5: seat the customer and then signal the customer thread
    seat_customer(customer);
    customer_seated = customer;
    
    pthread_cond_signal(&waiter_ready);
    pthread_mutex_unlock(&mlock);

    // TODO 6: wait for customer to place order then determine the order number
    while(order_placed == -1){
	pthread_cond_wait(&customer_ready, &mlock);
    }

    pthread_mutex_unlock(&mlock);

    bring_food_to_customer(customer, order_placed); // second arg should be zero in starter
    waiting_order = 0;

    pthread_mutex_lock(&mlock);
    pthread_cond_signal(&order_ready);

    while(service == -1){
	pthread_cond_wait(&customer_ready, &mlock);
    }
    pthread_mutex_unlock(&mlock);
    served_customers++;
    return;
}
