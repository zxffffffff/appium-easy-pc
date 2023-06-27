#pragma once
#include "pch.h"
#include <sstream>
#include <cassert>
using namespace std;
#include <Windows.h>
#include <wingdi.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define ROOT_NAME "root"


SHostWnd* GetHostWnd(HWND hWnd)
{
    DWORD  dwChild = ::SendMessage(hWnd, SPYMSG_SWNDENUM, 0, GSW_FIRSTCHILD);
    SWindow* pChild = SWindowMgr::GetWindow((SWND)dwChild);
    SHostWnd* pHost = pChild ? (SHostWnd*)pChild->GetContainer() : nullptr;
    return pHost;
}

std::string GetHostName(HWND hWnd)
{
    wchar_t buf[256];
    ::GetWindowTextW(hWnd, buf, sizeof(buf));
    std::string name = XXToF8(buf);
    if (name == "")
        name = "no-name";
    return name;
}

std::string GetName(SWindow* pWnd)
{
    if (XString(pWnd->GetObjectClass()) == L"hostwnd")
    {
        return GetHostName(static_cast<SHostWnd*>(pWnd)->m_hWnd);
    }

    std::string name = XXToF8(pWnd->GetName());
    if (name == "")
        name = "no-name";
    return name;
}

std::string GetElementId(SWindow* pWnd)
{
    if (XString(pWnd->GetObjectClass()) == L"hostwnd")
    {
        return "[hwnd]" + std::to_string(uint64_t(static_cast<SHostWnd*>(pWnd)->m_hWnd));
    }
    return "[swnd]" + std::to_string(uint64_t(pWnd->GetSwnd()));
}

SWindow* GetWndByElementId(const std::string& elementId)
{
    if (elementId.empty())
        return nullptr;

    auto type = elementId.substr(0, 6);
    auto iwnd = std::stoull(elementId.substr(6));
    if (type == "[hwnd]")
    {
        return GetHostWnd(HWND(iwnd));
    }
    else if (type == "[swnd]")
    {
        return SWindowMgr::GetWindow(SWND(iwnd));
    }

    return nullptr;
}

std::vector<HWND> GetAllWnds()
{
    //auto EnumWindowsProc = [](HWND hwnd, LPARAM lParam) {
    //    std::vector<HWND>& allWnds = *reinterpret_cast<std::vector<HWND>*>(lParam);
    //    allWnds.push_back(hwnd);
    //    return TRUE;
    //};
    //std::vector<HWND> allWnds;
    //EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&allWnds));

    // ����
    auto allWnds = SApplication::getSingleton().GetPopWnds();
    auto ite = allWnds.begin();
    while (ite != allWnds.end())
    {
        SHostWnd* pHost = GetHostWnd(*ite);
        if (pHost && pHost->IsWindowVisible())
            ++ite;
        else
            ite = allWnds.erase(ite);
    }

    // ���� z-order
    std::vector<HWND> sortedWnds;
    HWND currentWnd = GetTopWindow(NULL);
    while (currentWnd != NULL) {
        sortedWnds.push_back(currentWnd);
        currentWnd = GetNextWindow(currentWnd, GW_HWNDNEXT);
    }

    std::sort(allWnds.begin(), allWnds.end(), [&](HWND a, HWND b) {
        auto aPos = std::find(sortedWnds.begin(), sortedWnds.end(), a);
        auto bPos = std::find(sortedWnds.begin(), sortedWnds.end(), b);
        return aPos > bPos;
    });

    assert(!allWnds.empty());
    return allWnds;
}

struct UIInfo
{
    std::string elementId; // m_hWnd or swnd;
    std::string name;
    int index = 0;
    std::string package; // package="com.android.settings"
    std::string s_class; // class="android.widget.FrameLayout"
    std::string text;
    std::string textRaw;
    std::string resource_id; //[��ѡ] resource-id="com.android.settings:id/main_content"
    bool checkable = false;
    bool checked = false;
    bool clickable = false;
    bool enabled = false;
    bool focusable = false;
    bool focused = false;
    bool password = false;
    bool scrollable = false;
    bool selected = false;
    CRect bounds; // bounds="[30,440][1050,602]" l,t,r,b
    bool displayed = false;
    std::string hint; //[��ѡ] hint="Enter name"
};

struct UINode
{
    UIInfo info;
    std::vector<std::shared_ptr<UINode>> children;
};

UIInfo ParseNodeInfo(SWindow* pWnd, CPoint offset = CPoint(), int index = 0, const std::string& package = "")
{
    UIInfo info;

    info.elementId = GetElementId(pWnd);
    info.name = GetName(pWnd);
    info.index = index;
    info.package = package;
    info.s_class = XXToF8(pWnd->GetObjectClass());
    info.text = XXToF8(pWnd->GetWindowText(FALSE));
    info.textRaw = XXToF8(pWnd->GetWindowText(TRUE));
    info.resource_id = XXToF8(pWnd->GetAttribute(L"skin"));
    //info.checkable
    info.checked = pWnd->GetState() & WndState_Check;
    //info.clickable
    info.enabled = pWnd->IsEnable();
    info.focusable = pWnd->IsFocusable();
    info.focused = pWnd->IsFocused();
    //info.password
    //info.scrollable
    //info.selected
    info.bounds = CRect(offset + pWnd->GetClientRect().TopLeft(), pWnd->GetClientRect().Size());
    info.displayed = pWnd->IsVisible(TRUE);
    info.hint = XXToF8(pWnd->GetToolTipText());

    assert(!info.elementId.empty());
    assert(!info.name.empty());
    assert(!info.s_class.empty());

    return info;
}

std::shared_ptr<UINode> ParseNodeRecursive(SWindow* pWnd, CPoint offset = CPoint(), int index = 0)
{
    std::shared_ptr<UINode> node = std::make_shared<UINode>();

    node->info = ParseNodeInfo(pWnd, offset, index);

    SWindow* pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
    while (pChild)
    {
        // �ؼ������򲻵ݹ���ң�����ٶ�
        if (pChild->IsVisible(TRUE))
        {
            node->children.push_back(ParseNodeRecursive(pChild, offset, node->children.size()));
        }
        pChild = pChild->GetWindow(GSW_NEXTSIBLING);
    }
    
    return node;
}

std::shared_ptr<UINode> GetAllNodes()
{
    std::shared_ptr<UINode> top_node = std::make_shared<UINode>();

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return top_node;

    vector<HWND> allWnds = GetAllWnds();
    if (allWnds.empty())
        return top_node;

    SHostWnd* pHostMain = GetHostWnd(hMainWnd);
    if (!pHostMain)
        return top_node;
    std::string nameMain = GetHostName(hMainWnd);

    CRect realScreenRect = XXWindowHelper::GetTotalScreen();
    CRect mainGeometry = XXWindowHelper::GetGeometry(pHostMain);
    CPoint mainOffset = mainGeometry.TopLeft();
    CRect screenRect = mainGeometry;

    top_node->info = ParseNodeInfo(pHostMain);
    top_node->info.name = ROOT_NAME;
    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = GetHostWnd(allWnds[i]);
        std::string name = GetHostName(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        CPoint screenOffset;
        if (pHost != pHostMain)
        {
            ::ClientToScreen(pHost->m_hWnd, &screenOffset);
            screenOffset.Offset(-mainOffset);
        }

        top_node->children.push_back(ParseNodeRecursive(pHost, screenOffset, top_node->children.size()));
    }

    return top_node;
}

std::vector<std::string> findNodesRecursive(
    SWindow* pWnd,
    std::vector<std::string> path_name,
    std::map<std::string, int>& repeat_name
) {
    std::vector<std::string> v;

    if (path_name.empty())
        return v;

    std::string name = GetName(pWnd);
    std::string name2 = path_name.front();
    auto mismatchIt = std::mismatch(name.begin(), name.end(), name2.begin(), name2.end());
    if (mismatchIt.first == name.end())
    {
        // ǰ׺ƥ��
        std::string diff = name2.substr(mismatchIt.second - name2.begin());
        if (diff.empty())
        {
            // ȫ��ƥ��
        }
        else if (diff.front() == '[' && diff.back() == ']')
        {
            diff.erase(diff.begin());
            diff.erase(diff.end() - 1);
            int index = std::stoi(diff);
            if (++repeat_name[name] != index)
            {
                // ������ƥ��
                return v;
            }
        }
        else
        {
            // ��׺δƥ��
            return v;
        }
    }
    else
    {
        // δƥ��
        return v;
    }

    path_name.erase(path_name.begin());
    if (path_name.empty())
    {
        // found
        v.push_back(GetElementId(pWnd));
        return v;
    }

    std::map<std::string, int> repeat_name2;
    SWindow* pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
    while (pChild)
    {
        // �ؼ������򲻵ݹ���ң�����ٶ�
        if (pChild->IsVisible(TRUE))
        {
            auto v2 = findNodesRecursive(pChild, path_name, repeat_name2);
            v.insert(v.end(), v2.begin(), v2.end());
        }
        pChild = pChild->GetWindow(GSW_NEXTSIBLING);
    }

    return v;
}

std::vector<std::string> findNodes(std::vector<std::string> path_name)
{
    std::vector<std::string> v;

    if (path_name.empty())
        return v;

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return v;

    vector<HWND> allWnds = GetAllWnds();
    if (allWnds.empty())
        return v;

    SHostWnd* pHostMain = GetHostWnd(hMainWnd);
    if (!pHostMain)
        return v;

    if (path_name.front() != ROOT_NAME)
        return v;
    path_name.erase(path_name.begin());

    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = GetHostWnd(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        std::map<std::string, int> repeat_name;
        auto v2 = (findNodesRecursive(pHost, path_name, repeat_name));
        v.insert(v.end(), v2.begin(), v2.end());
    }

    return v;
}


CRect getMainGeometry()
{
    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return CRect();

    SHostWnd* pHostMain = GetHostWnd(hMainWnd);
    if (!pHostMain)
        return CRect();

    CRect mainGeometry = XXWindowHelper::GetGeometry(pHostMain);
    return mainGeometry;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;           // ����������
    UINT size = 0;          // �����������С

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // ʧ��

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // ʧ��

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT i = 0; i < num; ++i)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;  // �ɹ�
        }
    }

    free(pImageCodecInfo);
    return -1;  // ʧ��
}

void SaveBitmapToMemory(Bitmap* bitmap, vector<BYTE>& memoryBuffer)
{
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    // �����ڴ���
    IStream* stream;
    CreateStreamOnHGlobal(NULL, TRUE, &stream);

    // ����λͼ���ڴ���
    bitmap->Save(stream, &pngClsid);

    // ��ȡ�ڴ�����С
    STATSTG stats;
    stream->Stat(&stats, STATFLAG_DEFAULT);
    ULONG streamSize = static_cast<ULONG>(stats.cbSize.QuadPart);

    // �����ڴ�
    memoryBuffer.resize(streamSize);

    // ���ڴ������Ƶ��ڴ滺����
    LARGE_INTEGER zeroOffset;
    zeroOffset.QuadPart = 0;
    stream->Seek(zeroOffset, STREAM_SEEK_SET, NULL);
    stream->Read(&memoryBuffer[0], streamSize, NULL);

    // �ͷ��ڴ���
    stream->Release();
}

void ConvertMemoryToBase64(const vector<BYTE>& memoryBuffer, string& base64String)
{
    // Base64 ����
    static constexpr char base64Chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    stringstream ss;
    int i = 0;
    int j = 0;
    BYTE a3[3];
    BYTE a4[4];

    for (BYTE byte : memoryBuffer)
    {
        a3[i++] = byte;

        if (i == 3)
        {
            a4[0] = (a3[0] & 0xfc) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
            a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
            a4[3] = a3[2] & 0x3f;

            for (int k = 0; k < 4; k++)
                ss << base64Chars[a4[k]];

            i = 0;
        }
    }

    if (i != 0)
    {
        for (j = i; j < 3; j++)
            a3[j] = 0;

        a4[0] = (a3[0] & 0xfc) >> 2;
        a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
        a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);

        for (int k = 0; k < i + 1; k++)
            ss << base64Chars[a4[k]];

        while (i++ < 3)
            ss << '=';
    }

    base64String = ss.str();
}

std::string getScreenshotBase64()
{
    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return "";

    SHostWnd* pHostMain = GetHostWnd(hMainWnd);
    if (!pHostMain)
        return "";

    CRect realScreenRect = XXWindowHelper::GetTotalScreen();
    CRect mainGeometry = XXWindowHelper::GetGeometry(pHostMain);

    int x = mainGeometry.left;
    int y = mainGeometry.top;
    int width = mainGeometry.Width();
    int height = mainGeometry.Height();

    // ��ȡ��Ļ�豸������
    HDC hScreenDC = GetDC(NULL);
    HDC hCaptureDC = CreateCompatibleDC(hScreenDC);

    // ����λͼ
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hCaptureDC, hCaptureBitmap);

    // ������Ļָ����Χ��λͼ
    BitBlt(hCaptureDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

    // ����λͼ�� GDI+ �� Bitmap ����
    Bitmap* bitmap = Bitmap::FromHBITMAP(hCaptureBitmap, NULL);

#ifdef _DEBUG
    // ���� Bitmap �����ļ�
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    bitmap->Save(L"C:\\Users\\xiaofeng.zhu\\Desktop\\debug.png", &pngClsid, NULL);
#endif

    // ����λͼ���ڴ�
    vector<BYTE> memoryBuffer;
    SaveBitmapToMemory(bitmap, memoryBuffer);

    // ���ڴ�ת��Ϊ Base64 �ַ���
    std::string base64String;
    ConvertMemoryToBase64(memoryBuffer, base64String);

    // �ͷ���Դ
    DeleteObject(hCaptureBitmap);
    DeleteDC(hCaptureDC);
    ReleaseDC(NULL, hScreenDC);

    // �ͷ� Bitmap ����
    delete bitmap;

    return base64String;
}

// ģ�������
void mouseClick(int x, int y, DWORD dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP)
{
    XString flag;
    if (dwFlags & MOUSEEVENTF_LEFTDOWN)
        flag += L"LD";
    if (dwFlags & MOUSEEVENTF_RIGHTDOWN)
        flag += L"RD";
    if (dwFlags & MOUSEEVENTF_LEFTUP)
        flag += L"LU";
    if (dwFlags & MOUSEEVENTF_RIGHTUP)
        flag += L"RU";

    SetCursorPos(x, y);
    mouse_event(dwFlags, x, y, 0, 0);
}

// ģ��������� Ctrl+A => KeyInput(VK_A, true);
void keyBoardInput(WORD virtualKeyCode, bool isCtrlPressed)
{
    // ת��Ϊɨ����
    BYTE scanCode = MapVirtualKey(virtualKeyCode, MAPVK_VK_TO_VSC);

    // ģ�ⰴ��
    KEYBDINPUT kbDown = { 0 };
    kbDown.wScan = scanCode;
    kbDown.dwFlags = KEYEVENTF_SCANCODE;
    if (isCtrlPressed) {
        kbDown.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    INPUT inputDown = { 0 };
    inputDown.type = INPUT_KEYBOARD;
    inputDown.ki = kbDown;

    // ģ���ͷ�
    KEYBDINPUT kbUp = { 0 };
    kbUp.wScan = scanCode;
    kbUp.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    if (isCtrlPressed) {
        kbUp.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    INPUT inputUp = { 0 };
    inputUp.type = INPUT_KEYBOARD;
    inputUp.ki = kbUp;

    // ���������¼�
    INPUT inputs[2] = { inputDown, inputUp };
    SendInput(2, inputs, sizeof(INPUT));
}

// ģ����������
void textInput(const std::wstring& text)
{
    for (wchar_t ch : text)
    {
        // ת��Ϊɨ����
        WORD vk = VkKeyScan(ch);
        BYTE scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);

        // ģ�ⰴ��
        KEYBDINPUT kbDown = { 0 };
        kbDown.wScan = scanCode;
        kbDown.dwFlags = KEYEVENTF_SCANCODE;

        INPUT inputDown = { 0 };
        inputDown.type = INPUT_KEYBOARD;
        inputDown.ki = kbDown;

        // ģ���ͷ�
        KEYBDINPUT kbUp = { 0 };
        kbUp.wScan = scanCode;
        kbUp.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

        INPUT inputUp = { 0 };
        inputUp.type = INPUT_KEYBOARD;
        inputUp.ki = kbUp;

        // ���������¼�
        INPUT inputs[2] = { inputDown, inputUp };
        SendInput(2, inputs, sizeof(INPUT));
    }
}
