#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMANDS_PER_LINE 10
#define STACK_SIZE 1024
#define PRIORITY 5

K_LIFO_DEFINE(cmd_lifo);

struct motor_cmd_t {
    void *reserved;
    uint8_t command_id;     // 0x01 = move, 0x02 = stop
    int angle;
};

void motor_control_thread(void)
{
    struct motor_cmd_t *cmd;
    while (1) {
        cmd = k_lifo_get(&cmd_lifo, K_FOREVER);
        if (cmd->command_id == 0x01) {
            printk(">> Executing: Move motor to %d degrees\n", cmd->angle);
        } else if (cmd->command_id == 0x02) {
            printk(">> Executing: Emergency stop triggered!\n");
        } else {
            printk(">> Executing: Unknown command\n");
        }
        k_free(cmd);
        k_sleep(K_MSEC(300));
    }
}

void uart_input_thread(void)
{
    const struct device *uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    if (!device_is_ready(uart_dev)) {
        printk("UART not ready\n");
        return;
    }

    char buf[128];
    int pos = 0;
    printk("Enter command (move <angle> / stop): ");

    while (1) {
        uint8_t ch;
        if (uart_poll_in(uart_dev, &ch) == 0) {
            if (ch == '\r' || ch == '\n') {
                buf[pos] = '\0';
                printk("\n");

                if (pos == 0) {
                    printk("Enter command (move <angle> / stop): ");
                    continue;
                }

                // Copy buffer before using strtok, to ensure it's not corrupted by later use
                char parse_buf[128];
                strncpy(parse_buf, buf, sizeof(parse_buf));
                parse_buf[sizeof(parse_buf)-1] = '\0';

                struct motor_cmd_t *cmd_list[MAX_COMMANDS_PER_LINE];
                int cmd_count = 0;

                char *token = strtok(parse_buf, " ");
                while (token && cmd_count < MAX_COMMANDS_PER_LINE) {
                    struct motor_cmd_t *cmd = k_malloc(sizeof(struct motor_cmd_t));
                    if (!cmd) {
                        printk("Allocation failed\n");
                        break;
                    }

                    if (strcmp(token, "move") == 0) {
                        token = strtok(NULL, " ");
                        if (!token) {
                            printk("Missing angle\n");
                            k_free(cmd);
                            break;
                        }
                        cmd->command_id = 0x01;
                        cmd->angle = atoi(token);
                        printk("Parsed command: move %d\n", cmd->angle);
                    } else if (strcmp(token, "stop") == 0) {
                        cmd->command_id = 0x02;
                        cmd->angle = 0;
                        printk("Parsed command: stop\n");
                    } else {
                        printk("Unknown command: %s\n", token);
                        k_free(cmd);
                        token = strtok(NULL, " ");
                        continue;
                    }

                    cmd_list[cmd_count++] = cmd;
                    token = strtok(NULL, " ");
                }

                // Push into LIFO in reverse order
for (int i = 0; i < cmd_count; i++) {
    k_lifo_put(&cmd_lifo, cmd_list[i]);
}

                pos = 0;
                printk("Enter command (move <angle> / stop): ");
            } else if (pos < sizeof(buf) - 1) {
                buf[pos++] = ch;
                uart_poll_out(uart_dev, ch);
            }
        }

        k_sleep(K_MSEC(10));
    }
}

K_THREAD_STACK_DEFINE(uart_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(motor_stack, STACK_SIZE);
struct k_thread uart_thread_data;
struct k_thread motor_thread_data;

int main(void)
{
    printk("== Motor Control using Zephyr LIFO + UART Poll Input ==\n");

    k_thread_create(&uart_thread_data, uart_stack, STACK_SIZE,
        uart_input_thread, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&motor_thread_data, motor_stack, STACK_SIZE,
        motor_control_thread, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);
}

