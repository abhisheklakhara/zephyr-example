#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5

// Shared resources
int shared_data = 0;
bool data_available = false;

// Synchronization primitives
K_MUTEX_DEFINE(data_mutex);
K_CONDVAR_DEFINE(data_condvar);

// Thread stacks and data
K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);
struct k_thread producer_data;
struct k_thread consumer_data;

void producer_thread(void)
{
    int count = 0;
    while (1) {
        k_sleep(K_SECONDS(2)); // Simulate data preparation

        k_mutex_lock(&data_mutex, K_FOREVER);
        shared_data = ++count;
        data_available = true;

        printk("Producer: Produced data = %d\n", shared_data);

        k_condvar_signal(&data_condvar); // Notify consumer
        k_mutex_unlock(&data_mutex);
    }
}

void consumer_thread(void)
{
    while (1) {
        k_mutex_lock(&data_mutex, K_FOREVER);

        while (!data_available) {
            printk("Consumer: No data, waiting...\n");
            k_condvar_wait(&data_condvar, &data_mutex, K_FOREVER);
        }

        printk("Consumer: Consumed data = %d\n", shared_data);
        data_available = false; // Reset after consuming

        k_mutex_unlock(&data_mutex);

        k_sleep(K_SECONDS(1)); // Simulate processing
    }
}

void main(void)
{
    printk("Starting Producer-Consumer example...\n");

    k_thread_create(&producer_data, producer_stack, STACK_SIZE,
                    producer_thread, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&consumer_data, consumer_stack, STACK_SIZE,
                    consumer_thread, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);
}

