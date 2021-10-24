/**
 * @file ErrorReboot.cpp
 * @author Christoph Honal
 * @brief Provides crash dump handling
 * @version 0.1
 * @date 2021-07-12
 */

#include <mbed.h>


mbed_error_status_t err_status; //!< Stores crash info

/**
 * @brief Reboot hook handler
 * 
 * @param error_context Additional error information
 */
void mbed_error_reboot_callback(mbed_error_ctx *error_context)
{
    err_status = error_context->error_status;
    printf("\n\n(before main) mbed_error_reboot_callback invoked with the following error context:\n");
    printf("    Status      : 0x%lX\n", (uint32_t)error_context->error_status);
    printf("    Value       : 0x%lX\n", (uint32_t)error_context->error_value);
    printf("    Address     : 0x%lX\n", (uint32_t)error_context->error_address);
    printf("    Reboot count: 0x%lX\n", (uint32_t)error_context->error_reboot_count);
    printf("    CRC         : 0x%lX\n", (uint32_t)error_context->crc_error_ctx);
    mbed_reset_reboot_error_info();
    mbed_reset_reboot_count();
}
