#include <stdio.h>

#include <nrf_error.h>

#include <timer.h>
#include <tock.h>

#include "ble_http.h"


// void sensing_timer_callback (int a, int b, int c, void* ud);
// void start_sensing (void);

// char _post[512];
// int _post_len = 0;

// char AIO_KEY[]   =  "3e83a80b0ed94a338a3b3d26998b0dbe";
// char USERNAME[]  = "bradjc";
// char FEED_NAME[] = "hail-test";


// void sensing_timer_callback (__attribute__ ((unused)) int a,
//                              __attribute__ ((unused)) int b,
//                              __attribute__ ((unused)) int c,
//                              __attribute__ ((unused)) void* ud) {
//   char value[200];
//   int light;

//   light = isl29035_read_light_intensity();
//   snprintf(value, 200, "%i", light);

//   _post_len = create_post_to_feed(_post, 512, USERNAME, FEED_NAME, AIO_KEY, value);

//   blehttp_send_http_message(_post, _post_len);
// }

typedef enum {
  HTTP_MESSAGE = 1, // This buffer contains an HTTP message to send.
} msg_type_e;

static void ipc_callback(int pid, int len, int buf, __attribute__ ((unused)) void* ud) {
  if (len < 1) {
    printf("Error! IPC message too short.\n");
    ipc_notify_client(pid);
    return;
  }

  switch (buf[0]) {
    case HTTP_MESSAGE: {
      blehttp_send_http_message(buf+1, len-1);
      break;
    }
  }

  // sensor_update_t *update = (sensor_update_t*) buf;

  // if (conn_handle != BLE_CONN_HANDLE_INVALID) {
  //   switch (update->type) {
  //     case SENSOR_TEMPERATURE:
  //       env_sense_update_temperature(conn_handle, update->value);
  //       break;
  //     case SENSOR_IRRADIANCE:
  //       env_sense_update_irradiance(conn_handle,  update->value);
  //       break;
  //     case SENSOR_HUMIDITY:
  //       env_sense_update_humidity(conn_handle,  update->value);
  //       break;
  //   }
  // }
  // ipc_notify_client(pid);
}


/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main (void) {
  uint32_t err_code;

  printf("[BLE] HTTP Gateway\n");

  // Setup the BLE HTTP library.
  err_code = blehttp_init();
  if (err_code != NRF_SUCCESS) {
    printf("error setting up BLE HTTP: %lx\n", err_code);
  }

  // Listen for IPC requests to make a web request.
  ipc_register_svc(ipc_callback, NULL);
}