#ifndef LAB10_H
#define LAB10_H

#define MAX_CUSTOMERS 1

#define VANILLA_ONLY 0


void initialize(void);
void customer(int id, int dish);
void waiter(void);

void seat_customer(int customer_id);
void bring_food_to_customer(int customer_id, int dish);
void ready_to_seat_customer(int customer_id);
void order_placed_by_customer(int customer_id, int dish);
void service_completed_for_customer(int customer_id);

#endif
