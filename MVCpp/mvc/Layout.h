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

    float setWidth = NAN;
    float setHeight = NAN;

    float setWidthRatio = NAN;
    float setHeightRatio = NAN;

    void SetWidthStr(const char *width) {
      strcpy_s(widthStr, 10, width);
      setWidth = NAN;
      setWidthRatio = NAN;
    }

    void SetHeightStr(const char *height) {
      strcpy_s(heightStr, 10, height);
      setHeight = NAN;
      setHeightRatio = NAN;
    }


    // 注意：如果width大于1，则设定的是绝对值，
    // 如果width小于1，则设定的是百分比。
    void SetWidth(float width) {
      if (width >= 1) {
        setWidth = width;
        setWidthRatio = NAN;
      }
      else {
        setWidthRatio = width;
        setWidth = NAN;
      }
      widthStr[0] = 0;
    }


    // 注意：如果height大于1，则设定的是绝对值，
    // 如果height小于1，则设定的是百分比。
    void SetHeight(float height) {
      if (height >= 1) {
        setHeight = height;
        setHeightRatio = NAN;
      }
      else {
        setHeightRatio = height;
        setHeight = NAN;
      }
      heightStr[0] = 0;
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

  // Layout类将一个平面在横纵方向上分割成m行n列的多个子区域(cell)。
  // 
  // 对一个Layout对象，可以指定其包含的行数和列数，并定义各行的
  // 高度和各列宽度(可以是绝对值，百分比和*，*代表未定义值)。在此基础上，
  // 设置Layout的宽度和高度时，Layout对象会自动的调整各个区域的位置和大小。
  // 
  // Layout 调整算法的细节（以调整列宽度为例）
  // 
  // 1.计算绝对值和百分比列的宽度（设为a）
  // 2.剩余宽度 = Layout宽度 - a的合计宽度
  // 3.根据剩余宽度进行处理：
  //   - 如果剩余宽度<0，将a中的各列宽度等比例缩小，使剩余宽度为零。
  //   - 如果剩余宽度>0, 根据未定义列的个数（设为n）进行处理：
  //     - 如果n>0，未定义列的宽度 = 剩余宽度 / n
  //     - 如果n=0，将a中的各列宽度等比例扩大，使剩余宽度为零。
  // 
  // 每个view对象都会包含一个layout对象，负责将view的表面分割成多个
  // 部分。向view对象添加子view时，需要指定所处的Layout的行列位置以及
  // 相对偏移量，由此来指定子view的具体位置。
  //
  // 子view定位算法的细节
  // 
  // 对每一个子view对象，根据其所处的layout的行列设置该子view的坐标。
  // 1.根据所处的行列得到cell
  // 2.根据cell的宽度和高度，并结合子view的高度和宽度设置，计算出实际的
  //   高度和宽度。（详细参见ViewBase.h中UpdatePositionAndSize的注释）
  // 3.根据子view的偏移量设置，计算出相对于cell的实际偏移量。
  //   （详细参见ViewBase.h中UpdatePositionAndSize的注释）
  // 4.如果子view的内部还包含有下一级子view，那么针对下一级子view递归的
  //   更新它们的大小和偏移量。
  class Layout {
  private:
    vector<vector<GridCell>> cells;
    int rowCnt;
    int colCnt;

    float m_width = NAN;
    float m_height = NAN;

    bool m_defaultRowColSet;

    bool isNumber(const char *str) {
      auto len = strlen(str);
      // check for empty string.
      if (len == 0) return false;
      bool retval = true;
      for (size_t i = 0; i < len; i++) {
        retval = retval && (str[i] >= '0') && (str[i] <= '9');
      }
      return retval;
    }

    bool isPercent(const char *str) {
      auto len = strlen(str);
      // check for empty and the only one character '%' 
      // should return false.
      if (len <= 1) return false;
      bool retval = true;
      for (size_t i = 0; i < len - 1; i++) {
        retval = retval && (str[i] >= '0') && (str[i] <= '9');
      }

      retval = retval && (str[len - 1] == '%');
      return retval;
    }

  public:

    Layout() {
      GridCell cell;
      cell.SetWidthStr("*");
      cell.SetHeightStr("*");
      vector<GridCell> row;
      row.push_back(cell);
      cells.push_back(row);
      rowCnt = 1;
      colCnt = 1;
      m_defaultRowColSet = true;
    }

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

      if (m_defaultRowColSet) {
        cells[0].clear();
        cells.clear();
        colCnt = 0;
        rowCnt = 0;
        m_defaultRowColSet = false;
      }

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

      if (m_defaultRowColSet) {
        cells[0].clear();
        cells.clear();
        colCnt = 0;
        rowCnt = 0;
        m_defaultRowColSet = false;
      }

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

    // 追加新的一行并设定行的高度。
    // 注意：此处的height如果大于1，则设定的是绝对高度
    //      如果小于1，则设定的是百分比。
    void AddRow(float height) {

      if (m_defaultRowColSet) {
        cells[0].clear();
        cells.clear();
        colCnt = 0;
        rowCnt = 0;
        m_defaultRowColSet = false;
      }

      if (!rowCnt && !colCnt) {
        GridCell cell;

        // 如果height大于1，则设定的是绝对值，
        // 如果height小于1，则设定的是百分比。
        cell.SetHeight(height);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else if (!rowCnt && colCnt) {
        for (auto &c : cells[0]) {

        // 如果height大于1，则设定的是绝对值，
        // 如果height小于1，则设定的是百分比。
          c.SetHeight(height);
        }
      }
      else if (rowCnt && !colCnt) {
        GridCell cell;

        // 如果height大于1，则设定的是绝对值，
        // 如果height小于1，则设定的是百分比。
        cell.SetHeight(height);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else {
        vector<GridCell> row = cells[0];
        for (auto &c : row) {

          // 如果height大于1，则设定的是绝对值，
          // 如果height小于1，则设定的是百分比。
          c.SetHeight(height);
        }
        cells.push_back(row);
      }
      rowCnt++;
    }

    void AddCol(float width) {

      if (m_defaultRowColSet) {
        cells[0].clear();
        cells.clear();
        colCnt = 0;
        rowCnt = 0;
        m_defaultRowColSet = false;
      }

      if (!rowCnt && !colCnt) {
        // 既没有行也没有列时，生成一个cell，仅设置其宽度
        GridCell cell;

        // 如果width大于1，则设定的是绝对值，
        // 如果width小于1，则设定的是百分比。
        cell.SetWidth(width);
        vector<GridCell> row;
        row.push_back(cell);
        cells.push_back(row);
      }
      else if (!rowCnt && colCnt) {
        // 已经存在列，但是还没有行的时候，
        // 直接对第一行追加列并设置其宽度
        GridCell cell;

        // 如果width大于1，则设定的是绝对值，
        // 如果width小于1，则设定的是百分比。
        cell.SetWidth(width);
        cells[0].push_back(cell);
      }
      else if (rowCnt && !colCnt) {
        // 已经存在行，但是还没有列的时候，
        // 直接对已经存在的行设置其宽度
        // 将其转换为列。
        for (auto &r : cells) {

          // 如果width大于1，则设定的是绝对值，
          // 如果width小于1，则设定的是百分比。
          r[0].SetWidth(width);
        }
      }
      else {
        // 有行有列时，从第一列拷贝出新的一列，
        // 该列的高度与其他列相同但是宽度具有自己的值
        for (auto &r : cells) {
          GridCell c = r[0];

          // 如果width大于1，则设定的是绝对值，
          // 如果width小于1，则设定的是百分比。
          c.SetWidth(width);
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

        if (!isnan(c.setWidth)) {
          c.width = c.setWidth;
          ttlWidth += c.width;
        }
        else if (!isnan(c.setWidthRatio)) {
          c.width = c.setWidthRatio * width;
          ttlWidth += c.width;
        }
        else if (isNumber(c.widthStr)) {
          c.width = (float)atoi(c.widthStr);
          ttlWidth += c.width;
        }
        else if (isPercent(c.widthStr)) {
          char dg[20] = { 0 };
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

        if (!isnan(c.setHeight)) {
          c.height = c.setHeight;
          ttlHeight += c.height;
        }
        else if (!isnan(c.setHeightRatio)) {
          c.height = c.setHeightRatio * height;
          ttlHeight += c.height;
        }
        else if (isNumber(c.heightStr)) {
          c.height = (float)atoi(c.heightStr);
          ttlHeight += c.height;
        }
        else if (isPercent(c.heightStr)) {
          char dg[20] = { 0 };
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
