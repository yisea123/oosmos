//
// OOSMOS dnstest
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "oosmos.h"
#include "dns.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_TESTS   15
#define MAX_ANSWERS 3

typedef struct dnstestTag dnstest;

struct dnstestTag {
  const char * pDomain;
  dns        * pDNS;
  uint32_t     IP[MAX_ANSWERS];
  bool         Done;

  oosmos_sActiveObject ActiveObject;
};

static int ActiveInstances = 0;

static void Run(void * pObject)
{
  dnstest * pDnsTest = (dnstest *) pObject;

  if (pDnsTest->Done)
    return;

  printf("Checking %s...\n", pDnsTest->pDomain);

  if (dnsQuery(pDnsTest->pDNS, pDnsTest->pDomain, pDnsTest->IP, MAX_ANSWERS)) {
    int Answer;

    for (Answer = 0; Answer < MAX_ANSWERS; Answer++) {
      const uint32_t IP = pDnsTest->IP[Answer];

      if (IP == 0)
        break;

      printf("%s[%d]: %u.%u.%u.%u\n", pDnsTest->pDomain, Answer, IP>>24 & 0xff, IP>>16 & 0xff, IP>>8 & 0xff, IP & 0xff);
    }

    ActiveInstances -= 1;
    pDnsTest->Done = true;
  }
}

static dnstest * dnstestNew(const char * pDomain)
{
  oosmos_Allocate(pDnsTest, dnstest, MAX_TESTS, NULL);

  pDnsTest->pDNS    = dnsNew();
  pDnsTest->pDomain = pDomain;
  pDnsTest->Done    = false;

  oosmos_ActiveObjectInit(pDnsTest, ActiveObject, Run);

  ActiveInstances += 1;

  return pDnsTest;
}

extern int main(void)
{
  (void) dnstestNew("cnn.com");
  (void) dnstestNew("example.com");
  (void) dnstestNew("ibm.com");
  (void) dnstestNew("ycombinator.com");
  (void) dnstestNew("apple.com");
  (void) dnstestNew("google-public-dns-a.google.com.com");

  do  {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  } while (ActiveInstances > 0);

  return 0;
}
