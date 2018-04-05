// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")
#pragma comment(lib, "sqlite3.lib")

#include "MyController.h"

using namespace mvc;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  shared_ptr<Window> view;

  try{
    // 准备 Model
    m<int>("groupVal", 0);
    m<wstring>("start_date", L"");
    m<vector<TickPrice>>("last_tick", {});
    m<wstring>("btnStart", L"START");
    m<wstring>("pubno", L"");
    m<wstring>("time_frame", L"M1");

    // 准备 View
    load_views(IDR_RCDATA1);
    auto view = getv<Window>("main_window");
    auto btn = getv<Button>("btnOption");
    auto btnStart = getv<Button>("btnStart");
    auto tbx = getv<TextBox>("tbxStartDate");
    auto lbl2 = getv<Label>("lbl2");
    auto dialog = getv<Dialog>("dialog1");
    auto line = getv<Line>("line1");
    auto lbl4 = getv<Label>("lbl4");

    auto btnTimeFrame = getv<Button>("btnTimeFrame");
    auto btnBuy = getv<Button>("btnBuy");
    auto btnSell = getv<Button>("btnSell");


    // 设置直线的阴影效果
    // 阴影效果通过以下特效的组合来达成。
    //
    // ┌───┐                                ┌────────┐
    // │src│ ─→ shadow → affine transfrom → │  com-  │
    // │img│ ─────────────────────────────→ │ posite │ → 最终结果
    // └───┘                                └────────┘
    //
    line->SetColor(0x23ff00);
    auto shadowEffect = line->CreateEffect(CLSID_D2D1Shadow, 0);
    auto affineTransEffect = line->CreateEffect(CLSID_D2D12DAffineTransform);
    auto compositeEffect = line->CreateEffect(CLSID_D2D1Composite, 1);

    affineTransEffect->SetInputEffect(0, shadowEffect.ptr());
    D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(-5, 5);
    affineTransEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);

    compositeEffect->SetInputEffect(0, affineTransEffect.ptr());
    line->EffectOn();

    // 绑定 Model 和 View

    // 直接绑定。绑定的Model对象类型和自身表达的类型一致。
    // *直接绑定是双向绑定。即Model值的改变会自动反映到View上，
    // *反之View上的改变，也会自动反映到Model上。
    // *直接绑定也可以在view.xml上定义。

    // 将lbl2的text属性绑定到id为groupVal的model上。
    // 但是，这不是一个直接绑定，因为groupVal的类型为int型
    // 而lbl2的text表达的是wstring型，所以为了绑定到int型需要附加一个转换函数
    // 负责将int类型的值转换成wstring。这种绑定方式称为间接绑定。
    // 转换函数可以使用函数指针，或者lambda表达式。
    // 编译器不能识别出模板类型，所以要添加一个模板提示（即Bind<int>(...)）。
    // *间接绑定是单向绑定。即Model值的改变会自动反映到View上，
    // *但是View上的改变不会反映到Model上。

    // 关于性能：间接绑定时的处理函数会在每次画面更新时（画面以60hz的频率更新）
    // 调用，因此处理函数对性能有相对严格的要求，只能进行相当简单的变换操作。
    // 对于复杂的处理，应通过事件响应的方式实现。
    lbl2->text->Bind<int>("groupVal", [](const int * iptr, wstring& s){
      if (!(*iptr)){
        s = L"Select radio value.";
      }
      else{
        wchar_t buf[] = L"You selected: x";
        int index = sizeof(buf) / 2 - 2;
        buf[index] = to_wstring(*iptr)[0];
        s = buf;
      }
    });

    // 通过绑定来更新画面的表现
    lbl4->text->Bind<wstring>("start_date", [](const wstring * path, wstring& s) {
      s = L"bound: " + (*path);
    });

    // 设置事件处理 Controller
    btn->AddEventHandler(WM_LBUTTONUP, MyController::UpdateTitle);
    dialog->closebtn->AddEventHandler(WM_LBUTTONUP, MyController::CloseDialog);
    btnStart->AddEventHandler(WM_LBUTTONUP, MyController::StartChart);
    btnTimeFrame->AddEventHandler(WM_LBUTTONUP, MyController::ChangeFrame);
    btnBuy->AddEventHandler(WM_LBUTTONUP, MyController::OnBuy);
    btnSell->AddEventHandler(WM_LBUTTONUP, MyController::OnSell);

    // 通过事件机制（TEXTCHANGED）来更新画面的表现
    tbx->AddEventHandler(TextBox::MSG_TEXTCHANGED, MyController::OnTextChanged);

    view->Show();
  }
  catch (std::runtime_error re){
    if (view){
      MessageBoxA(view->GetHandle(), re.what(), "ERROR", MB_OK);
    }
    else{
      MessageBoxA(NULL, re.what(), "ERROR", MB_OK);
    }
  }

}
