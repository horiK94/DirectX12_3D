// SharedPtr.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <memory>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif//defined(DEBUG) || defined(_DEBUG)

struct Object
{
    int a = 10;
    int b = 20;

    void show() const
    {
        std::cout << a << " " << b << "\n";
    }
};

int main()
{
    //visual studioのみ使用可能
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(85);
#endif//defined(DEBUG) || defined(_DEBUG)

    //int* p = new int;
    int* q = (int*)std::malloc(128);
    //free(q);
}

//std::cout << "Hello World!\n";
//
//{
//    Object* p = new Object();
//    p->a = 30;
//    p->show();
//
//    delete p;
//}
//{
//    std::unique_ptr<Object> u = std::make_unique<Object>();       //heap領域
//    u->a = 100;
//    u->show();
//
//    //std::unique_ptr<Object> u2 = u;     //エラー
//}
//
//{
//    std::shared_ptr<Object> s = std::make_shared<Object>();
//    s->b = 99;
//    s->show();
//
//    std::shared_ptr<Object> s2 = s;
//    s->a = 11;
//    s2->b = 1;
//    s2->show();
//}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
