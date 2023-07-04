#include "AppiumDriver.h"
#include <functional>
#include <sstream>
#include <cassert>
using namespace std;

#include <Windows.h>
#include <wingdi.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "tinyxml2.h"
#include "httplib.h"
#include "json.hpp"

#define ROOT_NAME "root"
#define W3C_ELEMENT_KEY "element-6066-11e4-a52e-4f735466cecf"


SHostWnd* getHostWnd(HWND hWnd)
{
    DWORD  dwChild = ::SendMessage(hWnd, SPYMSG_SWNDENUM, 0, GSW_FIRSTCHILD);
    SWindow* pChild = SWindowMgr::GetWindow((SWND)dwChild);
    SHostWnd* pHost = pChild ? (SHostWnd*)pChild->GetContainer() : nullptr;
    return pHost;
}

std::string getHostWndName(HWND hWnd)
{
    wchar_t buf[256];
    ::GetWindowTextW(hWnd, buf, sizeof(buf));
    std::string name = XXSToF8(buf);
    if (name == "")
        name = "no-name";
    return name;
}

std::string getWndName(SWindow* pWnd)
{
    if (XXString(pWnd->GetObjectClass()) == L"hostwnd")
    {
        return getHostWndName(static_cast<SHostWnd*>(pWnd)->m_hWnd);
    }

    std::string name = XXSToF8(pWnd->GetName());
    if (name == "")
        name = "no-name";
    return name;
}

std::string getWndEleId(SWindow* pWnd)
{
    if (XXString(pWnd->GetObjectClass()) == L"hostwnd")
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
        return getHostWnd(HWND(iwnd));
    }
    else if (type == "[swnd]")
    {
        return SWindowMgr::GetWindow(SWND(iwnd));
    }

    return nullptr;
}

std::vector<HWND> getVisibleWindow()
{
    //auto EnumWindowsProc = [](HWND hwnd, LPARAM lParam) {
    //    std::vector<HWND>& allWnds = *reinterpret_cast<std::vector<HWND>*>(lParam);
    //    allWnds.push_back(hwnd);
    //    return TRUE;
    //};
    //std::vector<HWND> allWnds;
    //EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&allWnds));

    // 过滤
    auto allWnds = SApplication::getSingleton().GetPopWnds();
    auto ite = allWnds.begin();
    while (ite != allWnds.end())
    {
        SHostWnd* pHost = getHostWnd(*ite);
        if (pHost && pHost->IsWindowVisible())
            ++ite;
        else
            ite = allWnds.erase(ite);
    }

    // 排序 z-order
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
    //std::string package; // package="com.android.settings"
    std::string s_class; // class="android.widget.FrameLayout"
    std::string text;
    std::string textRaw;
    std::string resource_id; //[可选] resource-id="com.android.settings:id/main_content"
    //bool checkable = false;
    bool checked = false;
    //bool clickable = false;
    bool enabled = false;
    bool focusable = false;
    bool focused = false;
    //bool password = false;
    //bool scrollable = false;
    //bool selected = false;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool displayed = false;
    //std::string hint; //[可选] hint="Enter name"
};

struct UINode
{
    UIInfo info;
    std::vector<std::shared_ptr<UINode>> children;
};

UIInfo parseNodeInfo(SWindow* pWnd, CPoint offset = CPoint(), int index = 0)
{
    UIInfo info;

    info.elementId = getWndEleId(pWnd);
    info.name = getWndName(pWnd);
    info.index = index;
    info.s_class = XXSToF8(pWnd->GetObjectClass());
    info.text = XXSToF8(pWnd->GetWindowText(FALSE));
    info.textRaw = XXSToF8(pWnd->GetWindowText(TRUE));
    info.resource_id = XXSToF8(pWnd->GetAttribute(L"skin"));
    info.checked = pWnd->GetState() & WndState_Check;
    info.enabled = pWnd->IsEnable();
    info.focusable = pWnd->IsFocusable();
    info.focused = pWnd->IsFocused();
    info.x = offset.x + pWnd->GetClientRect().left;
    info.y = offset.y + pWnd->GetClientRect().top;
    info.width = pWnd->GetClientRect().Width();
    info.height = pWnd->GetClientRect().Height();
    info.displayed = pWnd->IsVisible(TRUE);

    assert(!info.elementId.empty());
    assert(!info.name.empty());
    assert(!info.s_class.empty());

    return info;
}

std::shared_ptr<UINode> parseNodeRecursive(SWindow* pWnd, CPoint offset = CPoint(), int index = 0)
{
    std::shared_ptr<UINode> node = std::make_shared<UINode>();

    node->info = parseNodeInfo(pWnd, offset, index);

    SWindow* pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
    while (pChild)
    {
        // 控件隐藏则不递归查找，提高速度
        if (pChild->IsVisible(TRUE))
        {
            node->children.push_back(parseNodeRecursive(pChild, offset, node->children.size()));
        }
        pChild = pChild->GetWindow(GSW_NEXTSIBLING);
    }

    return node;
}

std::shared_ptr<UINode> getAllNodes()
{
    std::shared_ptr<UINode> top_node = std::make_shared<UINode>();

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return top_node;

    vector<HWND> allWnds = getVisibleWindow();
    if (allWnds.empty())
        return top_node;

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return top_node;
    std::string nameMain = getHostWndName(hMainWnd);

    CRect realScreenRect = XXSWindowHelper::GetTotalScreen();
    CRect mainGeometry = XXSWindowHelper::GetGeometry(pHostMain);
    CPoint mainOffset = mainGeometry.TopLeft();
    CRect screenRect = mainGeometry;

    top_node->info = parseNodeInfo(pHostMain);
    top_node->info.name = ROOT_NAME;
    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = getHostWnd(allWnds[i]);
        std::string name = getHostWndName(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        CPoint screenOffset;
        if (pHost != pHostMain)
        {
            ::ClientToScreen(pHost->m_hWnd, &screenOffset);
            screenOffset.Offset(-mainOffset);
        }

        top_node->children.push_back(parseNodeRecursive(pHost, screenOffset, top_node->children.size()));
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

    std::string name = getWndName(pWnd);
    std::string name2 = path_name.front();
    auto mismatchIt = std::mismatch(name.begin(), name.end(), name2.begin(), name2.end());
    if (mismatchIt.first == name.end())
    {
        // 前缀匹配
        std::string diff = name2.substr(mismatchIt.second - name2.begin());
        if (diff.empty())
        {
            // 全部匹配
        }
        else if (diff.front() == '[' && diff.back() == ']')
        {
            diff.erase(diff.begin());
            diff.erase(diff.end() - 1);
            int index = std::stoi(diff);
            if (++repeat_name[name] != index)
            {
                // 索引不匹配
                return v;
            }
        }
        else
        {
            // 后缀未匹配
            return v;
        }
    }
    else
    {
        // 未匹配
        return v;
    }

    path_name.erase(path_name.begin());
    if (path_name.empty())
    {
        // found
        v.push_back(getWndEleId(pWnd));
        return v;
    }

    std::map<std::string, int> repeat_name2;
    SWindow* pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
    while (pChild)
    {
        // 控件隐藏则不递归查找，提高速度
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

    vector<HWND> allWnds = getVisibleWindow();
    if (allWnds.empty())
        return v;

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return v;

    if (path_name.front() != ROOT_NAME)
        return v;
    path_name.erase(path_name.begin());

    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = getHostWnd(allWnds[i]);

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

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return CRect();

    CRect mainGeometry = XXSWindowHelper::GetGeometry(pHostMain);
    return mainGeometry;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;           // 编码器数量
    UINT size = 0;          // 编码器数组大小

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // 失败

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // 失败

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT i = 0; i < num; ++i)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;  // 成功
        }
    }

    free(pImageCodecInfo);
    return -1;  // 失败
}

void saveBitmapToMemory(Bitmap* bitmap, vector<BYTE>& memoryBuffer)
{
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    // 创建内存流
    IStream* stream;
    CreateStreamOnHGlobal(NULL, TRUE, &stream);

    // 保存位图到内存流
    bitmap->Save(stream, &pngClsid);

    // 获取内存流大小
    STATSTG stats;
    stream->Stat(&stats, STATFLAG_DEFAULT);
    ULONG streamSize = static_cast<ULONG>(stats.cbSize.QuadPart);

    // 分配内存
    memoryBuffer.resize(streamSize);

    // 将内存流复制到内存缓冲区
    LARGE_INTEGER zeroOffset;
    zeroOffset.QuadPart = 0;
    stream->Seek(zeroOffset, STREAM_SEEK_SET, NULL);
    stream->Read(&memoryBuffer[0], streamSize, NULL);

    // 释放内存流
    stream->Release();
}

void convertMemoryToBase64(const vector<BYTE>& memoryBuffer, string& base64String)
{
    // Base64 编码
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

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return "";

    CRect realScreenRect = XXSWindowHelper::GetTotalScreen();
    CRect mainGeometry = XXSWindowHelper::GetGeometry(pHostMain);

    int x = mainGeometry.left;
    int y = mainGeometry.top;
    int width = mainGeometry.Width();
    int height = mainGeometry.Height();

    // 获取屏幕设备上下文
    HDC XXScreenDC = GetDC(NULL);
    HDC hCaptureDC = CreateCompatibleDC(XXScreenDC);

    // 创建位图
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(XXScreenDC, width, height);
    SelectObject(hCaptureDC, hCaptureBitmap);

    // 复制屏幕指定范围到位图
    BitBlt(hCaptureDC, 0, 0, width, height, XXScreenDC, x, y, SRCCOPY);

    // 保存位图到 GDI+ 的 Bitmap 对象
    Bitmap* bitmap = Bitmap::FromHBITMAP(hCaptureBitmap, NULL);

#ifdef _DEBUG
    // 保存 Bitmap 对象到文件
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    bitmap->Save(L"C:\\Users\\xiaofeng.zhu\\Desktop\\debug.png", &pngClsid, NULL);
#endif

    // 保存位图到内存
    vector<BYTE> memoryBuffer;
    saveBitmapToMemory(bitmap, memoryBuffer);

    // 将内存转换为 Base64 字符串
    std::string base64String;
    convertMemoryToBase64(memoryBuffer, base64String);

    // 释放资源
    DeleteObject(hCaptureBitmap);
    DeleteDC(hCaptureDC);
    ReleaseDC(NULL, XXScreenDC);

    // 释放 Bitmap 对象
    delete bitmap;

    return base64String;
}

// 模拟鼠标点击
void simulateMouseClick(int x, int y, DWORD dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP)
{
    XXString flag;
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

// 模拟键盘输入 Ctrl+A => KeyInput(VK_A, true);
void simulateKeyCodeInput(WORD virtualKeyCode, bool isCtrlPressed)
{
    // 转换为扫描码
    BYTE scanCode = MapVirtualKey(virtualKeyCode, MAPVK_VK_TO_VSC);

    // 模拟按下
    KEYBDINPUT kbDown = { 0 };
    kbDown.wScan = scanCode;
    kbDown.dwFlags = KEYEVENTF_SCANCODE;
    if (isCtrlPressed) {
        kbDown.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    INPUT inputDown = { 0 };
    inputDown.type = INPUT_KEYBOARD;
    inputDown.ki = kbDown;

    // 模拟释放
    KEYBDINPUT kbUp = { 0 };
    kbUp.wScan = scanCode;
    kbUp.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    if (isCtrlPressed) {
        kbUp.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    INPUT inputUp = { 0 };
    inputUp.type = INPUT_KEYBOARD;
    inputUp.ki = kbUp;

    // 发送输入事件
    INPUT inputs[2] = { inputDown, inputUp };
    SendInput(2, inputs, sizeof(INPUT));
}

// 模拟文字输入
void simulateKeyboardInput(const std::wstring& text)
{
    for (wchar_t ch : text)
    {
        // 转换为扫描码
        WORD vk = VkKeyScan(ch);
        BYTE scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);

        // 模拟按下
        KEYBDINPUT kbDown = { 0 };
        kbDown.wScan = scanCode;
        kbDown.dwFlags = KEYEVENTF_SCANCODE;

        INPUT inputDown = { 0 };
        inputDown.type = INPUT_KEYBOARD;
        inputDown.ki = kbDown;

        // 模拟释放
        KEYBDINPUT kbUp = { 0 };
        kbUp.wScan = scanCode;
        kbUp.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

        INPUT inputUp = { 0 };
        inputUp.type = INPUT_KEYBOARD;
        inputUp.ki = kbUp;

        // 发送输入事件
        INPUT inputs[2] = { inputDown, inputUp };
        SendInput(2, inputs, sizeof(INPUT));
    }
}

void node2xml_recursive(std::shared_ptr<UINode> node, tinyxml2::XMLElement* parent)
{
    auto element = parent->InsertNewChildElement(node->info.name.c_str());
    element->SetAttribute("index", node->info.index);
    element->SetAttribute("eleId", node->info.elementId.c_str());
    //element->SetAttribute("package", node->info.package.c_str());
    element->SetAttribute("class", node->info.s_class.c_str());
    element->SetAttribute("text", node->info.text.c_str());
    element->SetAttribute("textRaw", node->info.textRaw.c_str());
    element->SetAttribute("resource-id", node->info.resource_id.c_str());
    //element->SetAttribute("checkable", node->info.checkable);
    element->SetAttribute("checked", node->info.checked);
    //element->SetAttribute("clickable", node->info.clickable);
    element->SetAttribute("enabled", node->info.enabled);
    element->SetAttribute("focusable", node->info.focusable);
    element->SetAttribute("focused", node->info.focused);
    //element->SetAttribute("password", node->info.password);
    //element->SetAttribute("scrollable", node->info.scrollable);
    //element->SetAttribute("selected", node->info.selected);
    element->SetAttribute("x", node->info.x);
    element->SetAttribute("y", node->info.y);
    element->SetAttribute("width", node->info.width);
    element->SetAttribute("height", node->info.height);
    element->SetAttribute("displayed", node->info.displayed);

    for (auto child : node->children)
        node2xml_recursive(child, element);
}

std::string node2xml(std::shared_ptr<UINode> node)
{
    tinyxml2::XMLDocument doc;
    auto root = doc.NewElement(node->info.name.c_str());
    root->SetAttribute("index", node->info.index);
    root->SetAttribute("class", node->info.s_class.c_str());
    root->SetAttribute("width", node->info.width);
    root->SetAttribute("height", node->info.height);

    for (auto child : node->children)
        node2xml_recursive(child, root);

    doc.InsertEndChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    return printer.CStr();
}

/* The UI hierarchy in a platform-appropriate format (e.g., HTML for a web page)
 * http://appium.io/docs/en/2.0/reference/interfaces/appium_types.ExternalDriver/#getpagesource
 */
std::string getPageSource()
{
    std::shared_ptr<UINode> node = getAllNodes();
    auto xml = node2xml(node);
    return xml;
}

/* A base64-encoded string representing the PNG image data
 * http://appium.io/docs/en/2.0/reference/interfaces/appium_types.ExternalDriver/#getscreenshot
 */
std::string getScreenshot()
{
    std::string s = getScreenshotBase64();
    return s;
}

/* Get the size and position of the current window
 * http://appium.io/docs/en/2.0/reference/interfaces/appium_types.ExternalDriver/#getwindowrect
 */
std::string getWindowRect()
{
    CRect rect = getMainGeometry();
    nlohmann::json json = {
        { "x", 0 },
        { "y", 0 },
        { "width", rect.Width() },
        { "height",rect.Height() }
    };
    return json.dump();
}

/* A helper method that returns one or more UI elements based on the search criteria
 * http://appium.io/docs/en/2.0/reference/interfaces/appium_types.ExternalDriver/#findelorels
 */
std::string findElOrEls(
    const std::string& strategy, // xpath
    const std::string& selector, // /root/华盛通/v_Main/caption/no-name[1]
    const std::string& multiple, // true or false
    const std::string& context
) {
    if (strategy == "xpath")
    {
        if (selector.empty())
            return "";

        bool multi = (multiple == "true");
        auto path_name = XXSCommon::SplitStr(selector.substr(1), '/');

        std::vector<std::string> nodes = findNodes(path_name);
        if (nodes.empty())
            return "";

        if (multi)
        {
            nlohmann::json json_array;
            for (auto& node : nodes)
                json_array.push_back(node);

            nlohmann::json json = {
                { W3C_ELEMENT_KEY, json_array},
            };
            return json.dump();
        }
        else
        {
            nlohmann::json json = {
                { W3C_ELEMENT_KEY, nodes.front() },
            };
            return json.dump();
        }
    }
    return "";
}

std::string click(const std::string& elementId)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "error";

    auto rect = XXSWindowHelper::GetGeometry(pWnd);
    int x = rect.CenterPoint().x;
    int y = rect.CenterPoint().y;

    simulateMouseClick(x, y);

    return "ok";
}

std::string getText(const std::string& elementId)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "";

    return XXSToF8(pWnd->GetWindowText());
}

std::string setValue(const std::string& elementId, const std::string& text)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "error";

    auto rect = XXSWindowHelper::GetGeometry(pWnd);
    int x = rect.CenterPoint().x;
    int y = rect.CenterPoint().y;

    int flag = 0;
    while (!pWnd->GetWindowText().IsEmpty())
    {
        simulateMouseClick(x, y);

        // 模拟按下 Ctrl+A Backspace
        simulateKeyCodeInput('A', true);
        simulateKeyCodeInput(VK_BACK, false);

        if (++flag > 10)
            break; //"error: dead loop";
    }

    simulateMouseClick(x, y);
    simulateKeyboardInput(CXFunc::str2wstr(text));

    return "ok";
}

std::string clear(const std::string& elementId)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "error";

    auto rect = XXSWindowHelper::GetGeometry(pWnd);
    int x = rect.CenterPoint().x;
    int y = rect.CenterPoint().y;

    int flag = 0;
    while (!pWnd->GetWindowText().IsEmpty())
    {
        simulateMouseClick(x, y);

        // 模拟按下 Ctrl+A Backspace
        simulateKeyCodeInput('A', true);
        simulateKeyCodeInput(VK_BACK, false);

        if (++flag > 10)
            return "error: dead loop";
    }

    return "ok";
}

bool run()
{
    // HTTP
    httplib::Server svr;

    svr.Get("/getPageSource", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"getPageSource");

        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getPageSource();
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });

    svr.Get("/getScreenshot", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"getScreenshot");

        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getScreenshot();
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    svr.Get("/getWindowRect", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"getWindowRect");

        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getWindowRect();
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    svr.Get("/findElOrEls", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"findElOrEls");

        std::string strategy = req.get_param_value("strategy");
        std::string selector = req.get_param_value("selector");
        std::string multiple = req.get_param_value("multiple");
        std::string context = req.get_param_value("context");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = findElOrEls(strategy, selector, multiple, context);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    svr.Get("/click", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"click");

        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = click(elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    svr.Get("/getText", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"getText");

        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getText(elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });

    svr.Get("/setValue", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"setValue");

        std::string text = req.get_param_value("text");
        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = setValue(elementId, text);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    svr.Get("/clear", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"clear");

        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = clear(elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    bool ok = svr.listen("0.0.0.0", 4724);
    return ok;
}

void AppiumDriver::Run(bool thread)
{
    if (thread) {
        std::thread tr(std::bind(run));
        tr.detach();
    }
    else {
        run();
    }
}
