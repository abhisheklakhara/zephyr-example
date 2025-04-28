#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>


#define STACK_SIZE 1024
#define PRIORITY 5

K_SEM_DEFINE(my_semaphore, 0, 1);
//K_POLL_SIGNAL_DEFINE(my_signal);
struct k_poll_signal my_signal;
// Thread definitions
K_THREAD_STACK_DEFINE(polling_thread_stack, STACK_SIZE);
struct k_thread polling_thread_data;

void polling_thread(void *p1, void *p2, void *p3)
{
    struct k_poll_event events[2] = {
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SEM_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, &my_semaphore),
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &my_signal),
    };

    while (1) {
        printk("Polling thread waiting for semaphore or signal...\n");

        int ret = k_poll(events, 2, K_SECONDS(10)); // wait for 10 seconds

        if (ret == 0) {
            if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
                printk("Semaphore was given!\n");
                // Reset the event for next poll
                events[0].state = K_POLL_STATE_NOT_READY;
        	k_sleep(K_SECONDS(2));

            }
            if (events[1].state == K_POLL_STATE_SIGNALED) {
                printk("Signal was received!\n");
                // Reset the event and signal
                k_poll_signal_reset(&my_signal);
                events[1].state = K_POLL_STATE_NOT_READY;
            }
        } else if (ret == -EAGAIN) {
            printk("Polling timed out, no event happened.\n");
        } else {
            printk("Polling failed with error code: %d\n", ret);
        }

        k_sleep(K_MSEC(500)); // Sleep before next poll
    }
}

void main(void)
{
    printk("Starting semaphore demonstration...\n");
k_poll_signal_init(&my_signal);
    k_thread_create(&polling_thread_data, polling_thread_stack, STACK_SIZE,
                    polling_thread, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    while (1) {
        k_sleep(K_SECONDS(7));
        printk("Giving the semaphore...\n");
        k_sem_give(&my_semaphore);

        k_sleep(K_SECONDS(5));
        printk("Sending a signal...\n");
        k_poll_signal_raise(&my_signal, 0);
    }
}
