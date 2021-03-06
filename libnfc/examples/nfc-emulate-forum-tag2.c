/*-
 * Public platform independent Near Field Communication (NFC) library examples
 *
 * Copyright (C) 2011, Romuald Conty
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1) Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  2 )Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Note that this license only applies on the examples, NFC library itself is under LGPL
 *
 */

/**
 * @file nfc-emulate-forum-tag2.c
 * @brief Emulates a NFC Forum Tag Type 2 with a NDEF message
 * This example allow to emulate an NFC Forum Tag Type 2 that contains a read-only NDEF message.
 *
 * It have been developed using PN533 USB hardware as target and Google Nexus S phone as initiator.
 *
 * This is know to NOT work with Nokia 6212 Classic and could not work with
 * several NFC Forum compliant devices due to these reasons:
 *  - The emulated target only have a 4 bytes UID where 7 bytes UID (as a real
 *  Mifare Ultralight tag) are usually attempted;
 *  - The chip is emulating a ISO/IEC 14443-3 tag, without any hardware helper.
 *  If the initiator have too short timeouts for software-based emulation
 *  (which is usually the case), this example will failed, this is not a bug
 *  and we can't do anything using this kind of hardware (PN531/PN533).
 */

/*
 * This implementation was written based on information provided by the
 * following documents:
 *
 * NFC Forum Type 2 Tag Operation
 *  Technical Specification
 *  NFCForum-TS-Type-2-Tag_1.0 - 2007-07-09
 *
 * ISO/IEC 14443-3
 *  First edition - 2001-02-01
 *  Identification cards — Contactless integrated circuit(s) cards — Proximity cards
 *  Part 3: Initialization and anticollision
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif // HAVE_CONFIG_H

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <nfc/nfc.h>
#include <nfc/nfc-emulation.h>

#include "utils/nfc-utils.h"

static nfc_device *pnd;

static void
stop_emulation(int sig)
{
  (void)sig;
  if (pnd) {
    nfc_abort_command(pnd);
  } else {
    exit(EXIT_FAILURE);
  }
}

static uint8_t __nfcforum_tag2_memory_area[] = {
  0x00, 0x00, 0x00, 0x00,  // Block 0
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xFF,  // Block 2 (Static lock bytes: CC area and data area are read-only locked)
  0xE1, 0x10, 0x06, 0x0F,  // Block 3 (CC - NFC-Forum Tag Type 2 version 1.0, Data area (from block 4 to the end) is 48 bytes, Read-only mode)

  0x03, 33,   0xd1, 0x02,  // Block 4 (NDEF)
  0x1c, 0x53, 0x70, 0x91,
  0x01, 0x09, 0x54, 0x02,
  0x65, 0x6e, 0x4c, 0x69,

  0x62, 0x6e, 0x66, 0x63,
  0x51, 0x01, 0x0b, 0x55,
  0x03, 0x6c, 0x69, 0x62,
  0x6e, 0x66, 0x63, 0x2e,

  0x6f, 0x72, 0x67, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

#define READ 		0x30
#define WRITE 		0xA2
#define SECTOR_SELECT 	0xC2

#define HALT 		0x50
static int
nfcforum_tag2_io(struct nfc_emulator *emulator, const uint8_t *data_in, const size_t data_in_len, uint8_t *data_out, const size_t data_out_len)
{
  int res = 0;

  uint8_t *nfcforum_tag2_memory_area = (uint8_t *)(emulator->user_data);

  printf("    In: ");
  print_hex(data_in, data_in_len);

  switch (data_in[0]) {
    case READ:
      if (data_out_len >= 16) {
        memcpy(data_out, nfcforum_tag2_memory_area + (data_in[1] * 4), 16);
        res = 16;
      } else {
        res = -ENOSPC;
      }
      break;
    case HALT:
      printf("HALT sent\n");
      res = -ECONNABORTED;
      break;
    default:
      printf("Unknown command: 0x%02x\n", data_in[0]);
      res = -ENOTSUP;
  }

  if (res < 0) {
    ERR("%s (%d)", strerror(-res), -res);
  } else {
    printf("    Out: ");
    print_hex(data_out, res);
  }

  return res;
}

int
main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  nfc_target nt = {
    .nm = {
      .nmt = NMT_ISO14443A,
      .nbr = NBR_UNDEFINED, // Will be updated by nfc_target_init()
    },
    .nti = {
      .nai = {
        .abtAtqa = { 0x00, 0x04 },
        .abtUid = { 0x08, 0x00, 0xb0, 0x0b },
        .szUidLen = 4,
        .btSak = 0x00,
        .szAtsLen = 0,
      },
    }
  };

  struct nfc_emulation_state_machine state_machine = {
    .io = nfcforum_tag2_io
  };

  struct nfc_emulator emulator = {
    .target = &nt,
    .state_machine = &state_machine,
    .user_data = __nfcforum_tag2_memory_area,
  };

  signal(SIGINT, stop_emulation);
  nfc_init(NULL);
  pnd = nfc_open(NULL, NULL);

  if (pnd == NULL) {
    ERR("Unable to open NFC device");
    exit(EXIT_FAILURE);
  }

  printf("NFC device: %s opened\n", nfc_device_get_name(pnd));
  printf("Emulating NDEF tag now, please touch it with a second NFC device\n");

  if (nfc_emulate_target(pnd, &emulator) < 0) {
    goto error;
  }

  nfc_close(pnd);
  nfc_exit(NULL);

  exit(EXIT_SUCCESS);

error:
  if (pnd) {
    nfc_perror(pnd, argv[0]);
    nfc_close(pnd);
    nfc_exit(NULL);
  }
}
