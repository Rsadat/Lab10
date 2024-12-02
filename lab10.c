#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "lab10.h"


extern char * dishes[];

static int served_customers;

static int customer_waiting = -1;


void initialize(void)
{
    // add initializations here
}


void customer(int id, int dish)
{
    // signal customer ready to be served
    ready_to_seat_customer(id);

    // TODO 1: try to get ownership of the lock
    customer_waiting = id;

    // TODO 2: Signal to the waiter that customer is waiting

    // TODO: 4 Wait to be seated

    // TODO: 6 place order and signal waiter
    order_placed_by_customer(id, dish);

    service_completed_for_customer(id);

}


void waiter(void)
{
    if (served_customers >= MAX_CUSTOMERS)
        return;

    // seat waiting customer

    // TODO 3: replace busy loop with predicate/cond_wait construct
    while (customer_waiting == -1);
    int customer = customer_waiting;

    // TODO 5: seat the customer and then signal the customer thread
    seat_customer(customer);

    // TODO 6: wait for customer to place order then determine the order number

    bring_food_to_customer(customer, 0); // second arg should be zero in starter

    served_customers++;
}
