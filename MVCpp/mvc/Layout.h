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

    char widthStr[10] = { 0 };
    char heightStr[10] = { 0 };

    void SetWidth(const char *width) {
      strcpy_s(widthStr, 10, width);
    }

    void SetHeight(const char *height) {
      strcpy_s(heightStr, 10, height);
    }

    string ToString() {
      string retval;
      retval += widthStr;
      retval += ",";
      retval += heightStr;
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
    void AddRow(const char *height) {
      if (!rowCnt && !colCnt) {
        GridCell cell;
        cell.SetHeight(height);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else if (!rowCnt && colCnt) {
        for (auto &c : cells[0]) {
          c.SetHeight(height);
        }
      }
      else if (rowCnt && !colCnt) {
        GridCell cell;
        cell.SetHeight(height);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else {
        vector<GridCell> row = cells[0];
        for (auto &c : row) {
          c.SetHeight(height);
        }
        cells.push_back(row);
      }
      rowCnt++;
    }

    void AddCol(const char *width) {
      if (!rowCnt && !colCnt) {
        // 既没有行也没有列时，生成一个cell，仅设置其宽度
        GridCell cell;
        cell.SetWidth(width);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else if (!rowCnt && colCnt) {
        // 已经存在列，但是还没有行的时候，
        // 直接对第一行追加列并设置其宽度
        GridCell cell;
        cell.SetWidth(width);
        cells[0].push_back(cell);
      }
      else if (rowCnt && !colCnt) {
        // 已经存在行，但是还没有列的时候，
        // 直接对已经存在的行设置其宽度
        // 将其转换为列。
        for (auto &r : cells) {
          r[0].SetWidth(width);
        }
      }
      else {
        // 有行有列时，从第一列拷贝出新的一列，
        // 该列的高度与其他列相同但是宽度具有自己的值
        for (auto &r : cells) {
          GridCell c = r[0];
          c.SetWidth(width);
          r.push_back(c);
        }
      }
      colCnt++;
    }

    const string ToString() {
      string retval;

      for (auto &r : cells) {
        for (auto &c : r) {
          retval += c.ToString();
          retval += ' ';
        }
        retval += '\n';
      }

      return retval;
    }

    void SetWidth(float width) {

      if (cells.size() == 0) return;

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

      for (int i = 1; i < rowCnt; i++) {
        for (int j = 0; j < colCnt; j++) {
          cells[i][j].width = cells[0][j].width;
        }
      }

      m_width = width;
    }
  };
}
