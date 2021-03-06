#include "ota.h"

// OTA data write buffer ready to write to the flash
static char ota_write_data[BUFF_SIZE + 1] = {0 };
// Packet receive buffer
static char text[BUFF_SIZE + 1] = {0 };
// Image total length
static int binary_file_length = 0;
// socket id
static int socket_id = -1;

unsigned int blinking_pin_1_g, blinking_pin_2_g;
static esp_timer_handle_t upgrade_timer;

static void __attribute__((noreturn)) task_fatal_error() {
   #ifdef ALLOW_USE_PRINTF
   printf("\nExiting task due to fatal error...\n");
   #endif

   close(socket_id);
   esp_restart();
}

static void esp_ota_firm_init(esp_ota_firm_t *ota_firm, const esp_partition_t *update_partition) {
   memset(ota_firm, 0, sizeof(esp_ota_firm_t));

   ota_firm->state = ESP_OTA_INIT;
   ota_firm->ota_num = get_ota_partition_count();
   ota_firm->update_ota_num = update_partition->subtype - ESP_PARTITION_SUBTYPE_APP_OTA_0;

   #ifdef ALLOW_USE_PRINTF
   printf("Total OTA number %d update to %d part\n", ota_firm->ota_num, ota_firm->update_ota_num);
   #endif
}

// Read buffer by byte still delim, return read bytes counts
static int read_until(const char *buffer, char delim, int len) {
   // TODO: delim check,buffer check,further: do an buffer length limited
   int i = 0;
   while (buffer[i] != delim && i < len) {
      ++i;
   }
   return i + 1;
}

static bool _esp_ota_firm_parse_http(esp_ota_firm_t *ota_firm, const char *text, size_t total_len, size_t *parse_len) {
   // i means current position
   int i = 0, i_read_len = 0;
   char *ptr = NULL, *ptr2 = NULL;
   char length_str[32];

   while (text[i] != 0 && i < total_len) {
      ptr = (char *) strstr(text, "Content-Length");

      if (ota_firm->content_len == 0 && ptr != NULL) {
         ptr += 16;
         ptr2 = (char *) strstr(ptr, "\r\n");

         memset(length_str, 0, sizeof(length_str));
         memcpy(length_str, ptr, ptr2 - ptr);

         ota_firm->content_len = atoi(length_str);

         ota_firm->ota_size = ota_firm->content_len;
         ota_firm->ota_offset = 0;

         #ifdef ALLOW_USE_PRINTF
         printf("Parse Content-Length: %d, ota_size %d\n", ota_firm->content_len, ota_firm->ota_size);
         #endif
      }

      i_read_len = read_until(&text[i], '\n', total_len - i);

      if (i_read_len > total_len - i) {
         #ifdef ALLOW_USE_PRINTF
         printf("\nrecv. malformed HTTP header\n");
         #endif

         task_fatal_error();
      }

      // if resolve \r\n line, HTTP header is finished
      if (i_read_len == 2) {
         if (ota_firm->content_len == 0) {
            #ifdef ALLOW_USE_PRINTF
            printf("\nDid not parse Content-Length item\n");
            #endif

            task_fatal_error();
         }

         *parse_len = i + 2;

         return true;
      }

      i += i_read_len;
   }
   return false;
}

static size_t esp_ota_firm_do_parse_msg(esp_ota_firm_t *ota_firm, const char *in_buf, size_t in_len) {
   size_t tmp;
   size_t parsed_bytes = in_len;

   switch (ota_firm->state) {
      case ESP_OTA_INIT:
         if (_esp_ota_firm_parse_http(ota_firm, in_buf, in_len, &tmp)) {
            ota_firm->state = ESP_OTA_PREPARE;

            #ifdef ALLOW_USE_PRINTF
            //printf("HTTP parsed %d bytes", tmp);
            #endif

            parsed_bytes = tmp;
         }

         break;
      case ESP_OTA_PREPARE:
         ota_firm->read_bytes += in_len;

         if (ota_firm->read_bytes >= ota_firm->ota_offset) {
            ota_firm->buf = &in_buf[in_len - (ota_firm->read_bytes - ota_firm->ota_offset)];
            ota_firm->bytes = ota_firm->read_bytes - ota_firm->ota_offset;
            ota_firm->write_bytes += ota_firm->read_bytes - ota_firm->ota_offset;
            ota_firm->state = ESP_OTA_START;

            #ifdef ALLOW_USE_PRINTF
            //printf("Received %d bytes and start to update\n", ota_firm->read_bytes);
            //printf("Write %d total %d\n", ota_firm->bytes, ota_firm->write_bytes);
            #endif
         }

         break;
      case ESP_OTA_START:
         if (ota_firm->write_bytes + in_len > ota_firm->ota_size) {
            ota_firm->bytes = ota_firm->ota_size - ota_firm->write_bytes;
            ota_firm->state = ESP_OTA_RECVED;
         } else {
            ota_firm->bytes = in_len;
         }

         ota_firm->buf = in_buf;
         ota_firm->write_bytes += ota_firm->bytes;

         #ifdef ALLOW_USE_PRINTF
         //printf("Write %d total %d\n", ota_firm->bytes, ota_firm->write_bytes);
         #endif

         break;
      case ESP_OTA_RECVED:
         parsed_bytes = 0;
         ota_firm->state = ESP_OTA_FINISH;

         break;
      default:
         parsed_bytes = 0;

         #ifdef ALLOW_USE_PRINTF
         printf("\nState is %d\n", ota_firm->state);
         #endif

         break;
   }

   return parsed_bytes;
}

static void esp_ota_firm_parse_msg(esp_ota_firm_t *ota_firm, const char *in_buf, size_t in_len) {
   size_t parse_bytes = 0;

   #ifdef ALLOW_USE_PRINTF
   //printf("Input %d bytes", in_len);
   #endif

   do {
      size_t bytes = esp_ota_firm_do_parse_msg(ota_firm, in_buf + parse_bytes, in_len - parse_bytes);

      #ifdef ALLOW_USE_PRINTF
      //printf("Parsed %d bytes", bytes);
      #endif

      if (bytes) {
         parse_bytes += bytes;
      }
   } while (parse_bytes != in_len);
}

static inline int esp_ota_firm_can_write(esp_ota_firm_t *ota_firm) {
   return (ota_firm->state == ESP_OTA_START || ota_firm->state == ESP_OTA_RECVED);
}

static inline const char *esp_ota_firm_get_write_buf(esp_ota_firm_t *ota_firm) {
   return ota_firm->buf;
}

static inline size_t esp_ota_firm_get_write_bytes(esp_ota_firm_t *ota_firm) {
   return ota_firm->bytes;
}

static inline int esp_ota_firm_is_finished(esp_ota_firm_t *ota_firm) {
   return ota_firm->state == ESP_OTA_FINISH || ota_firm->state == ESP_OTA_RECVED;
}

void blink() {
   if (gpio_get_level(blinking_pin_1_g)) {
      gpio_set_level(blinking_pin_1_g, 0);
      gpio_set_level(blinking_pin_2_g, 1);
   } else {
      gpio_set_level(blinking_pin_1_g, 1);
      gpio_set_level(blinking_pin_2_g, 0);
   }
}

void turn_off_blinking_leds() {
   gpio_set_level(blinking_pin_1_g, 0);
   gpio_set_level(blinking_pin_2_g, 0);
}

static void update_firmware_task() {
   esp_err_t err;
   // update handle : set by esp_ota_begin(), must be freed via esp_ota_end()
   esp_ota_handle_t update_handle = 0;

   #ifdef ALLOW_USE_PRINTF
   printf("\nStarting OTA... Flash: %s\n", CONFIG_ESPTOOLPY_FLASHSIZE);
   #endif

   const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
   assert(update_partition != NULL);

   #ifdef ALLOW_USE_PRINTF
   printf("Writing to partition label %s at offset 0x%X, subtype: 0x%X, size: 0x%X\n",
         update_partition->label, update_partition->address, update_partition->subtype, update_partition->size);
   #endif

   const char *request_parameters[] = {"", SERVER_IP_ADDRESS, NULL};
   request_parameters[0] = PROJECT_NAME".bin";
   char *http_request = set_string_parameters(FIRMWARE_UPDATE_GET_REQUEST, request_parameters);

   #ifdef ALLOW_USE_PRINTF
   printf("GET HTTP request: %s\n", http_request);
   #endif

   socket_id = connect_to_http_server();

   if (socket_id == -1) {
      free(http_request);

      #ifdef ALLOW_USE_PRINTF
      printf("\nError on server connection for updating\n");
      #endif

      task_fatal_error();
   }

   int res = send(socket_id, http_request, strlen(http_request), 0);

   free(http_request);

   if (res < 0) {
      #ifdef ALLOW_USE_PRINTF
      printf("\nSend GET request to server failed\n");
      #endif

      task_fatal_error();
   } else {
      #ifdef ALLOW_USE_PRINTF
      printf("Send GET request to server succeeded\n");
      #endif
   }

   turn_off_blinking_leds();

   // Flash sector is erased here
   err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);

   if (err != ESP_OK) {
      #ifdef ALLOW_USE_PRINTF
      printf("\nesp_ota_begin failed, error=%d\n", err);
      #endif

      task_fatal_error();
   }
   #ifdef ALLOW_USE_PRINTF
   printf("esp_ota_begin succeeded\n");
   #endif

   bool flag = true;
   esp_ota_firm_t ota_firm;

   esp_ota_firm_init(&ota_firm, update_partition);

   // deal with all receive packet
   while (flag) {
      memset(text, 0, TEXT_BUFF_SIZE);
      memset(ota_write_data, 0, BUFF_SIZE);

      int buff_len = recv(socket_id, text, TEXT_BUFF_SIZE, 0);

      if (buff_len < 0) { // receive error
         #ifdef ALLOW_USE_PRINTF
         printf("\nError: receive data error! Error no.=%d\n", errno);
         #endif

         task_fatal_error();
      } else if (buff_len > 0) { // deal with response body
         esp_ota_firm_parse_msg(&ota_firm, text, buff_len);

         if (!esp_ota_firm_can_write(&ota_firm)) {
            continue;
         }

         memcpy(ota_write_data, esp_ota_firm_get_write_buf(&ota_firm), esp_ota_firm_get_write_bytes(&ota_firm));
         buff_len = esp_ota_firm_get_write_bytes(&ota_firm);

         err = esp_ota_write(update_handle, (const void *) ota_write_data, buff_len);

         if (err != ESP_OK) {
            #ifdef ALLOW_USE_PRINTF
            printf("\nError: esp_ota_write failed! err=0x%X\n", err);
            #endif

            task_fatal_error();
         }

         binary_file_length += buff_len;
      } else if (buff_len == 0) { // packet over
         flag = false;

         #ifdef ALLOW_USE_PRINTF
         printf("Connection closed, all packets received\n");
         #endif

         shutdown_and_close_socket(socket_id);
      } else {
         #ifdef ALLOW_USE_PRINTF
         printf("\nUnexpected recv. result\n");
         #endif
      }

      if (esp_ota_firm_is_finished(&ota_firm)) {
         break;
      }

      blink();
   }

   #ifdef ALLOW_USE_PRINTF
   printf("Total write binary data length: %d\n", binary_file_length);
   #endif

   if (esp_ota_end(update_handle) != ESP_OK) {
      #ifdef ALLOW_USE_PRINTF
      printf("\nesp_ota_end failed!\n");
      #endif

      task_fatal_error();
   }

   err = esp_ota_set_boot_partition(update_partition);

   if (err != ESP_OK) {
      #ifdef ALLOW_USE_PRINTF
      printf("\nesp_ota_set_boot_partition failed! err=0x%X\n", err);
      #endif

      task_fatal_error();
   }

   #ifdef ALLOW_USE_PRINTF
   printf("Prepare to restart system!\n");
   #endif

   esp_restart();
}

static void on_update_timeout() {
   #ifdef ALLOW_USE_PRINTF
   printf("\nUpdate timeout\n");
   #endif

   esp_restart();
}

void update_firmware(unsigned int blinking_pin_1, unsigned int blinking_pin_2) {
   blinking_pin_1_g = blinking_pin_1;
   blinking_pin_2_g = blinking_pin_2;

   // Resolves an exception error when calling esp_restart() in the end of updating
   disable_wifi_event_handler();

   esp_timer_create_args_t timer_config = {
         .callback = &on_update_timeout
   };
   ESP_ERROR_CHECK(esp_timer_create(&timer_config, &upgrade_timer))
   ESP_ERROR_CHECK(esp_timer_start_once(upgrade_timer, 120 * 1000 * 1000))

   xTaskCreate(update_firmware_task, "update_firmware_task", configMINIMAL_STACK_SIZE * 6, NULL, 5, NULL);
}
