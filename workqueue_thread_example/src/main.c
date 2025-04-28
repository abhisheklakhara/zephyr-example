#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void my_work_handler(struct k_work *work)
{
    printk("Work is RUNNING\n");

    k_sleep(K_MSEC(100));

    printk("Work DONE\n");
}

K_WORK_DEFINE(my_work, my_work_handler);

void main(void)
{
    printk("Submitting work...\n");

    k_work_submit(&my_work);

    k_sleep(K_MSEC(10));

    if (k_work_is_pending(&my_work)) {
        printk("Work is QUEUED or RUNNING\n");

        int ret = k_work_cancel(&my_work);
        if (ret == 0) {
            printk("Work was RUNNING and couldn't be canceled\n");
        } else {
            printk("Work canceled before execution\n");
        }
    } else {
        printk("Work is not pending\n");
    }

    k_sleep(K_SECONDS(1));
}

