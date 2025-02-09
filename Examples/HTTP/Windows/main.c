//
// OOSMOS - HTTP socket example.
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
#include "httptest.h"
#include <stdio.h>

extern int main(int argc, char *argv[])
{
  int Count;

  printf("\nTest does not end on its own. Press CNTL-C to terminate.\n\n");

  for (Count = 1; Count <= 5; Count++) {
    httptestNew("www.example.com", 80, Count);
  }

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
