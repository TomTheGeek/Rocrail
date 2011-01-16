/*
 * bidib_message.c
 *
 *  Created on: Jan 16, 2011
 *      Author: rob
 */

#include "rocdigs/impl/bidib/bidib_messages.h"
#include "rocdigs/impl/bidib/crc_8bit.h"

static int bidib_tx0_msg_num = 0;
//--------------------------------------------------------------------
//
// basic output routines

static void bidib_send(unsigned char c) {
  if ((c == BIDIB_PKT_MAGIC) | (c == BIDIB_PKT_ESCAPE)) {
    while (!tx_fifo_ready()) {
    }; // busy waiting (but shouldn't wait)
    tx_fifo_write(BIDIB_PKT_ESCAPE); // escape char
    c = c ^ 0x20;
  }
  while (!tx_fifo_ready()) {
  }; // busy waiting
  tx_fifo_write(c);
}

static void bidib_send_delimiter(void) {
  while (!tx_fifo_ready()) {
  }; // busy waiting
  tx_fifo_write(BIDIB_PKT_MAGIC);
}

// Single Message TX, no concatenation of messages
// at this point, message could either be a native message from here or a
// message from a sub.
// parameter: message: pointer to complete message
void send_bidib_message(unsigned char *message) {
  unsigned char i = 0;
  unsigned char length;
  unsigned char tx_crc = 0;

  while (!tx_fifo_ready())
    ; // busy waiting!!!
  // vorläufig kein Timeout
  bidib_send_delimiter();

  length = message[0];
  bidib_send(length);
  tx_crc = crc_array[length ^ tx_crc];

  for (i = 0; i < length; i++) {
    bidib_send(message[i]);
    tx_crc = crc_array[message[i] ^ tx_crc];
  }
  bidib_send(tx_crc);
}

//-------------------------------------------------------------------------------
//
// useful typedefs for bidib messages

typedef struct {
  unsigned char size;
  unsigned char node_addr;
  unsigned char index;
  unsigned char msg_type;
} t_node_message_header;

typedef struct {
  t_node_message_header header;
  unsigned char data;
} t_node_message1;

typedef struct {
  t_node_message_header header;
  unsigned char data[2]; // be sure, not to load more data here!
} t_node_message2;

typedef struct {
  t_node_message_header header;
  unsigned char data[10];
} t_node_message10;

typedef struct {
  t_node_message_header header;
  unsigned char data[18];
} t_node_message18;

//-- helper functions

static void bidib_send_single_msg(unsigned char message) {
  t_node_message_header header;
  header.node_addr = 0;
  bidib_tx0_msg_num++;
  if (bidib_tx0_msg_num == 0)
    bidib_tx0_msg_num++;
  header.index = bidib_tx0_msg_num;
  header.msg_type = message;
  header.size = 3 + 0;

  send_bidib_message((unsigned char *) &header);
}

static void bidib_send_sys_magic(void) {
  //TODO: send_bidib_message((unsigned char *) bidib_sys_magic);
  bidib_tx0_msg_num = 0;
}

//Und zu guter letzt der Empfänger:
int run_bidib_host_if(void) {
  unsigned char data;

  // TODO: Local vars?
  unsigned char bidib_rx_paket[256];
  int bidib_rx_paket_ready = 0;
  int bidib_rx_total = 0;
  int bidib_rx_index = 0;
  int bidib_rx_crc = 0;
  int bidib_rx_msg_num = 0;
  int escape_hot = 0;

  // first check whether we already can process a complete paket

  if (bidib_rx_paket_ready) {
    if (bidib_paket_is_processable()) {
      // yes, we got paket with messages in it, lets loop thro all messages
      bidib_rx_total = bidib_rx_index - 1;
      bidib_rx_index = 0;
      while (bidib_rx_index < bidib_rx_total) {
        bidib_rx_index += process_bidib_host_message(
            &bidib_rx_paket[bidib_rx_index]);
      }
      bidib_rx_index = 0;
      bidib_rx_paket_ready = 0;
    }
    else {
      // oops: we got stuck - more messages from the host than we could process
      // we return here, other tasks could process the downstream bidibus,
      // our input fifo will automatically stop the host
      return (0); // ready again
    }
  }

  // accumulate data to get a complete paket

  while (rx_fifo_ready()) {
    data = rx_fifo_read(); // reads one byte
    if (data == BIDIB_PKT_MAGIC) {
      if (bidib_rx_crc) // check crc
      {
        bidib_send_error(BIDIB_ERR_CRC, bidib_rx_msg_num);
        bidib_rx_crc = 0;
        bidib_rx_index = 0; // if failed: kill complete paket
      }
      else {
        bidib_rx_paket_ready = 1;
      }
    }
    else if (data == BIDIB_PKT_ESCAPE) {
      escape_hot = 1;
    }
    else {
      if (escape_hot) {
        data ^= 0x20;
        escape_hot = 0;
      }
      // now collect data
      if (bidib_rx_index < sizeof(bidib_rx_paket)) {
        bidib_rx_paket[bidib_rx_index++] = data;
        bidib_rx_crc = crc_array[data ^ bidib_rx_crc];
      }
      else {
        bidib_send_error(BIDIB_ERR_SIZE, bidib_rx_msg_num);
      }
    }
  }
  return (-1);
}

