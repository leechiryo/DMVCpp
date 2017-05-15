#pragma once

#include <vector>

using namespace std;

namespace mvc {
  class GridCell {
  public:
    float top = NAN;
    float left = NAN;
    float bottom = NAN;
    float right = NAN;

    float width = NAN;
    float height = NAN;

    char widthStr[10];
    char heightStr[10];

    void SetWidthStr(const char *width) {
      strcpy_s(widthStr, 10, width);
    }

    void SetHeightStr(const char *height) {
      strcpy_s(heightStr, 10, height);
    }

    void ClearWidth(){
      width = NAN;
    }

    void ClearHeight(){
      height = NAN;
    }

    string ToString() {
      string retval;
      char buf[100];
      sprintf_s(buf, 100, "(%f, %f)", width, height);
      retval += buf;
      sprintf_s(buf, 100, "(%s, %s)", widthStr, heightStr);
      retval += buf;
      return retval;
    }
  };

  class Layout {
  private:
    vector<vector<GridCell>> cells;
    int rowCnt = 0;
    int colCnt = 0;

    float m_width = NAN;
    float m_height = NAN;

    bool isNumber(const char *str) {
      int len = strlen(str);
      // check for empty string.
      if (len == 0) return false;
      bool retval = true;
      for (int i = 0; i < len; i++) {
        retval = retval && (str[i] >= '0') && (str[i] <= '9');
      }
      return retval;
    }

    bool isPercent(const char *str) {
      int len = strlen(str);
      // check for empty and the only one character '%' 
      // should return false.
      if (len <= 1) return false;
      bool retval = true;
      for (int i = 0; i < len - 1; i++) {
        retval = retval && (str[i] >= '0') && (str[i] <= '9');
      }

      retval = retval && (str[len - 1] == '%');
      return retval;
    }

  public:

    const string ToDebugString() {
      string retval;
      char buf[100];

      sprintf_s(buf, 100, "(%f, %f)\n", m_width, m_height);
      retval += buf;

      for (auto &r : cells) {
        for (auto &c : r) {
          retval += c.ToString();
          retval += ' ';
        }
        retval += '\n';
      }

      return retval;
    }

    const GridCell * GetCell(int row, int col){
      return &(cells[row][col]);
    }

    void AddRow(const char *height) {
      if (!rowCnt && !colCnt) {
        GridCell cell;
        cell.SetHeightStr(height);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else if (!rowCnt && colCnt) {
        for (auto &c : cells[0]) {
          c.SetHeightStr(height);
        }
      }
      else if (rowCnt && !colCnt) {
        GridCell cell;
        cell.SetHeightStr(height);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else {
        vector<GridCell> row = cells[0];
        for (auto &c : row) {
          c.SetHeightStr(height);
        }
        cells.push_back(row);
      }
      rowCnt++;
    }

    void AddCol(const char *width) {
      if (!rowCnt && !colCnt) {
        // 既没有行也没有列时，生成一个cell，仅设置其宽度
        GridCell cell;
        cell.SetWidthStr(width);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else if (!rowCnt && colCnt) {
        // 已经存在列，但是还没有行的时候，
        // 直接对第一行追加列并设置其宽度
        GridCell cell;
        cell.SetWidthStr(width);
        cells[0].push_back(cell);
      }
      else if (rowCnt && !colCnt) {
        // 已经存在行，但是还没有列的时候，
        // 直接对已经存在的行设置其宽度
        // 将其转换为列。
        for (auto &r : cells) {
          r[0].SetWidthStr(width);
        }
      }
      else {
        // 有行有列时，从第一列拷贝出新的一列，
        // 该列的高度与其他列相同但是宽度具有自己的值
        for (auto &r : cells) {
          GridCell c = r[0];
          c.SetWidthStr(width);
          r.push_back(c);
        }
      }
      colCnt++;
    }

    void SetWidth(float width) {

      if (cells.size() == 0) return;

      // clear the width info (if exist.)
      for (auto &c : cells[0]){
        c.ClearWidth();
      }

      float ttlWidth = 0.0;
      int nonWidthColsCnt = 0;
      for (auto &c : cells[0]) {
        if (isNumber(c.widthStr)) {
          c.width = (float)atoi(c.widthStr);
          ttlWidth += c.width;
        }
        else if (isPercent(c.widthStr)) {
          char dg[10] = { 0 };
          strcpy_s(dg, strlen(c.widthStr) + 1, c.widthStr);
          dg[strlen(c.widthStr) - 1] = 0;
          int a = atoi(dg);
          c.width = a * width / 100.0f;
          ttlWidth += c.width;
        }
        else {
          nonWidthColsCnt++;
        }
      }

      if (ttlWidth > width) {
        for (auto &c : cells[0]) {
          if (!isnan(c.width)) {
            c.width = c.width * width / ttlWidth;
          }
          else {
            c.width = 0.0f;
          }
        }
      }
      else if (ttlWidth < width && !nonWidthColsCnt) {
        for (auto &c : cells[0]) {
          c.width = c.width * width / ttlWidth;
        }
      }
      else if (ttlWidth < width && nonWidthColsCnt) {
        for (auto &c : cells[0]) {
          if (isnan(c.width)) {
            c.width = (width - ttlWidth) / nonWidthColsCnt;
          }
        }
      }

      // update the left and right pos
      cells[0][0].left = 0.0;
      cells[0][0].right = cells[0][0].width;
      for (int j = 1; j < colCnt; j++) {
        cells[0][j].left = cells[0][j - 1].right;
        cells[0][j].right = cells[0][j].left + cells[0][j].width;
      }
      cells[0][colCnt - 1].right = width;

      for (int i = 1; i < rowCnt; i++) {
        for (int j = 0; j < colCnt; j++) {
          cells[i][j].width = cells[0][j].width;
          cells[i][j].left = cells[0][j].left;
          cells[i][j].right = cells[0][j].right;
        }
      }

      m_width = width;

    }

    void SetHeight(float height) {

      if (cells.size() == 0) return;

      // clear the height info (if exists.)
      for (auto &r : cells){
        r[0].ClearHeight();
      }

      float ttlHeight = 0.0;
      int nonHeightRowsCnt = 0;
      for (auto &r : cells) {
        auto &c = r[0];
        if (isNumber(c.heightStr)) {
          c.height = (float)atoi(c.heightStr);
          ttlHeight += c.height;
        }
        else if (isPercent(c.heightStr)) {
          char dg[10] = { 0 };
          strcpy_s(dg, strlen(c.heightStr) + 1, c.heightStr);
          dg[strlen(c.heightStr) - 1] = 0;
          int a = atoi(dg);
          c.height = a * height / 100.0f;
          ttlHeight += c.height;
        }
        else {
          nonHeightRowsCnt++;
        }
      }

      if (ttlHeight > height) {
        for (auto &r : cells) {
          auto &c = r[0];
          if (!isnan(c.height)) {
            c.height = c.height * height / ttlHeight;
          }
          else {
            c.height = 0.0f;
          }
        }
      }
      else if (ttlHeight < height && !nonHeightRowsCnt) {
        for (auto &r : cells) {
          auto &c = r[0];
          c.height = c.height * height / ttlHeight;
        }
      }
      else if (ttlHeight < height && nonHeightRowsCnt) {
        for (auto &r : cells) {
          auto &c = r[0];
          if (isnan(c.height)) {
            c.height = (height - ttlHeight) / nonHeightRowsCnt;
          }
        }
      }

      // update the top and bottom pos
      cells[0][0].top = 0.0;
      cells[0][0].bottom = cells[0][0].height;
      for (int i = 1; i < rowCnt; i++) {
        cells[i][0].top = cells[i - 1][0].bottom;
        cells[i][0].bottom = cells[i][0].top + cells[i][0].height;
      }
      cells[rowCnt - 1][0].bottom = height;

      for (int i = 0; i < rowCnt; i++) {
        for (int j = 1; j < colCnt; j++) {
          cells[i][j].height = cells[i][0].height;
          cells[i][j].top = cells[i][0].top;
          cells[i][j].bottom = cells[i][0].bottom;
        }
      }

      m_height = height;
    }
  };
}
