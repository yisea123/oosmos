//
// OOSMOS matrix Class
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

#ifndef matrixMAX
#define matrixMAX 1
#endif

#ifndef matrixMAX_ROWS
#define matrixMAX_ROWS 8
#endif

#ifndef matrixMAX_COLS
#define matrixMAX_COLS 8
#endif

static const int RowOnSettleTimeUS  = 50;
static const int RowOffSettleTimeUS = 50;

#include "oosmos.h"
#include "matrix.h"
#include "sw.h"
#include "pin.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

struct matrixTag
{
  oosmos_sObjectThread m_ObjectThread;

  pin * m_pRowPins[matrixMAX_ROWS];
  pin * m_pColumnPins[matrixMAX_COLS];
  sw  * m_pSwitch[matrixMAX_ROWS][matrixMAX_COLS];

  int m_Rows;
  int m_Columns;

  //
  // State variables...
  //
  int m_CurrentRowIndex;
};

static void InterrogateColumns(const matrix * pMatrix)
{
  const int RowIndex = pMatrix->m_CurrentRowIndex;
  const int Columns  = pMatrix->m_Columns;

  for (int ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++) {
    sw * pSwitch = pMatrix->m_pSwitch[RowIndex][ColumnIndex];

    if (pSwitch != NULL) {
      swRunStateMachine(pSwitch);
    }
  }
}

static void Thread(matrix * pMatrix, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      for (pMatrix->m_CurrentRowIndex = 0; pMatrix->m_CurrentRowIndex < pMatrix->m_Rows; pMatrix->m_CurrentRowIndex++) {
        if (pMatrix->m_pRowPins[pMatrix->m_CurrentRowIndex] == NULL) {
          continue;
        }

        pinOn(pMatrix->m_pRowPins[pMatrix->m_CurrentRowIndex]);
        oosmos_ThreadDelayMS(RowOnSettleTimeUS);

        InterrogateColumns(pMatrix);

        pinOff(pMatrix->m_pRowPins[pMatrix->m_CurrentRowIndex]);
        oosmos_ThreadDelayMS(RowOffSettleTimeUS);
      }
    }
  oosmos_ThreadEnd();
}

static void AddRow(matrix * pMatrix, const int Row, pin * pPin)
{
  const int RowIndex = Row - 1;

  pMatrix->m_pRowPins[RowIndex] = pPin;

  if (Row > pMatrix->m_Rows) {
    pMatrix->m_Rows = Row;
  }
}

static void AddColumn(matrix * pMatrix, const int Column, pin * pPin)
{
  const int ColumnIndex = Column - 1;
  pMatrix->m_pColumnPins[ColumnIndex] = pPin;

  if (Column > pMatrix->m_Columns) {
    pMatrix->m_Columns = Column;
  }
}

extern matrix * matrixNew(int Rows, int Columns, ...)
{
  oosmos_Allocate(pMatrix, matrix, matrixMAX, NULL);

  pMatrix->m_Rows    = 0;
  pMatrix->m_Columns = 0;

  for (int RowIndex = 0; RowIndex < matrixMAX_ROWS; RowIndex++) {
    pMatrix->m_pRowPins[RowIndex] = NULL;

    for (int ColumnIndex = 0; ColumnIndex < matrixMAX_COLS; ColumnIndex++) {
      pMatrix->m_pColumnPins[ColumnIndex]       = NULL;
      pMatrix->m_pSwitch[RowIndex][ColumnIndex] = NULL;
    }
  }

  oosmos_ObjectThreadInit(pMatrix, m_ObjectThread, Thread, true);

  va_list ArgList;
  va_start(ArgList, Columns);

  for (int Row = 1; Row <= Rows; Row += 1) {
    AddRow(pMatrix, Row, va_arg(ArgList, pin *));
  }

  for (int Column = 1; Column <= Columns; Column += 1) {
    AddColumn(pMatrix, Column, va_arg(ArgList, pin *));
  }

  va_end(ArgList);

  return pMatrix;
}

extern void matrixAssignSwitch(matrix * pMatrix, sw * pSwitch, int Row, int Column)
{
  const int RowIndex    = Row - 1;
  const int ColumnIndex = Column - 1;

  //
  // Check if this Row/Column slot has already been assigned.
  //
  if (pMatrix->m_pSwitch[RowIndex][ColumnIndex] != NULL) {
    oosmos_FOREVER();
  }

  pMatrix->m_pSwitch[RowIndex][ColumnIndex] = pSwitch;
}

extern pin * matrixGetColumnPin(const matrix * pMatrix, int Column)
{
  const int ColumnIndex = Column - 1;
  return pMatrix->m_pColumnPins[ColumnIndex];
}
