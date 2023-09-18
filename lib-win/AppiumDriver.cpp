#include "AppiumDriver.h"
#include "AppiumDriverP.h"
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

static int g_listen_port = 0;

namespace
{
    // 兼容自绘控件，模拟一个不存在的node
    class SimulateWindow : public SWindow
    {
        SOUI_CLASS_NAME(SimulateWindow, L"SimulateWindow")
    public:
        SimulateWindow(SWindow* parent, int index)
            : simulate_parent(parent)
            , simulate_index(index)
        {
            SetContainer(parent->GetContainer());
        }

        void init(string name, CRect pos, string text, string other)
        {
            simulate_name = toSString(name);
            simulate_rect = pos;
            SetWindowText(toSString(text));
            SetOther(toSString(other));
        }

        virtual LPCWSTR       GetName() const override
        {
            return simulate_name;
        }
        virtual CRect         GetClientRect() const override
        {
            return simulate_rect;
        }

        SWindow* simulate_parent;
        int simulate_index;
        SString simulate_name;
        CRect simulate_rect;
    };

    std::map<SWindow*, std::map<std::string, SimulateWindow*>> g_simulateWindows;

    SimulateWindow* AppendSimulateWindow(SWindow* parent, const std::string& name)
    {
        SimulateWindow* pWnd = g_simulateWindows[parent][name];
        if (pWnd)
            return pWnd;

        pWnd = new SimulateWindow(parent, g_simulateWindows[parent].size());
        g_simulateWindows[parent][name] = pWnd;
        return pWnd;
    }

    SimulateWindow* FindSimulateWindow(SWindow* parent, const std::string& name)
    {
        SimulateWindow* pWnd = g_simulateWindows[parent][name];
        return pWnd;
    }
}

SHostWnd* getHostWnd(HWND hWnd)
{
    DWORD  dwChild = ::SendMessage(hWnd, SPYMSG_SWNDENUM, 0, GSW_FIRSTCHILD);
    SWindow* pChild = SWindowMgr::GetWindow((SWND)dwChild);
    SHostWnd* pHost = pChild ? (SHostWnd*)pChild->GetContainer() : nullptr;
    return pHost;
}

std::vector<SWindow*> getWndChildren(SWindow* pWnd)
{
    std::vector<SWindow*> children;

    SWindow* pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
    while (pChild)
    {
        // 控件隐藏则不递归查找，提高速度
        if (pChild->IsVisible(TRUE))
        {
            children.push_back(pChild);
        }
        pChild = pChild->GetWindow(GSW_NEXTSIBLING);
    }

    const std::string classType = toStdString(pWnd->GetObjectClass());
    if (classType == TabType1 || classType == TabType2 || classType == TabType3)
    {
        int cnt = toInt(pWnd->GetAttribute(L"count"));
        int sel = toInt(pWnd->GetAttribute(L"select"));

        pWnd->SetTestDump(L"item_count", toSString(cnt));
        pWnd->SetTestDump(L"item_select", toSString(sel));

        for (int i = 0; i < cnt; i++)
        {
            const string item = "item_" + to_string(i);
            const string attrs = toStdString(pWnd->GetAttribute(toSString(item)));
            auto list = Split(attrs, ',');
            if (list.size() == 6)
            {
                CRect rect = pWnd->GetClientRect();
                int l = stoi(list[0]) + rect.left;
                int t = stoi(list[1]) + rect.top;
                int r = l + stoi(list[2]);
                int b = t + stoi(list[3]);
                string& text = list[4];
                string& other = list[5];

                SimulateWindow* pSimulate = AppendSimulateWindow(pWnd, item);
                pSimulate->init(item, { l, t, r, b }, text, other);
                if (sel == i)
                {
                    pSimulate->SetTestDump(L"selected", L"true");
                }
                children.push_back(pSimulate);
            }
        }
    }
    else if (classType == GridType1 || classType == GridType2)
    {
        int rowCount = toInt(pWnd->GetAttribute(L"rowCount"));
        int colCount = toInt(pWnd->GetAttribute(L"colCount"));
        int sel = toInt(pWnd->GetAttribute(L"rowSelect"));
        int rowBegin = 0;
        int rowEnd = rowCount - 1;
        string screenRowCol = toStdString(pWnd->GetAttribute(L"screenRowRange"));
        auto screenV = Split(screenRowCol, ',');
        if (screenV.size() == 2)
        {
            rowBegin = stoi(screenV[0]);
            rowEnd = stoi(screenV[1]);
        }

        pWnd->SetTestDump(L"item_rowCount", toSString(rowCount));
        pWnd->SetTestDump(L"item_colCount", toSString(colCount));
        pWnd->SetTestDump(L"item_rowSelect", toSString(sel));
        pWnd->SetTestDump(L"item_rowBegin", toSString(rowBegin));
        pWnd->SetTestDump(L"item_rowEnd", toSString(rowEnd));

        for (int col = 0; col < colCount; col++)
        {
            const string item = "headItem_" + to_string(col);
            const string attrs = toStdString(pWnd->GetAttribute(toSString(item)));
            auto list = Split(attrs, ',');
            if (list.size() == 6)
            {
                CRect rect = pWnd->GetClientRect();
                int l = stoi(list[0]) + rect.left;
                int t = stoi(list[1]) + rect.top;
                int r = l + stoi(list[2]);
                int b = t + stoi(list[3]);
                string& text = list[4];
                string& other = list[5];

                SimulateWindow* pSimulate = AppendSimulateWindow(pWnd, item);
                pSimulate->init(item, { l, t, r, b }, text, other);
                children.push_back(pSimulate);
            }
        }
        for (int row = rowBegin; row <= rowEnd; row++)
        {
            for (int col = 0; col < colCount; col++)
            {
                const string item = "item_" + to_string(row) + "_" + to_string(col);
                const string attrs = toStdString(pWnd->GetAttribute(toSString(item)));
                auto list = Split(attrs, ',');
                if (list.size() == 6)
                {
                    CRect rect /*= m_swnd->GetClientRect()*/;
                    int l = stoi(list[0]) + rect.left;
                    int t = stoi(list[1]) + rect.top;
                    int r = l + stoi(list[2]);
                    int b = t + stoi(list[3]);
                    string& text = list[4];
                    string& other = list[5];

                    SimulateWindow* pSimulate = AppendSimulateWindow(pWnd, item);
                    pSimulate->init(item, { l, t, r, b }, text, other);
                    if (sel == row)
                    {
                        pSimulate->SetTestDump(L"selected", L"true");
                    }
                    children.push_back(pSimulate);
                }
            }
        }
    }
    else if (classType == GridType3 || classType == GridType4)
    {
        int cnt = toInt(pWnd->GetAttribute(L"count"));

        pWnd->SetTestDump(L"item_count", toSString(cnt));

        for (int i = 0; i < cnt; i++)
        {
            const string item = "item_" + to_string(i);
            const string attrs = toStdString(pWnd->GetAttribute(toSString(item)));
            auto list = Split(attrs, ',');
            if (list.size() == 6)
            {
                CRect rect = pWnd->GetClientRect();
                int l = stoi(list[0]) + rect.left;
                int t = stoi(list[1]) + rect.top;
                int r = l + stoi(list[2]);
                int b = t + stoi(list[3]);
                string& text = list[4];
                string& other = list[5];

                SimulateWindow* pSimulate = AppendSimulateWindow(pWnd, item);
                pSimulate->init(item, { l, t, r, b }, text, other);
                children.push_back(pSimulate);
            }
        }
    }
    return children;
}

std::string getHostWndName(HWND hWnd)
{
    wchar_t buf[256];
    ::GetWindowTextW(hWnd, buf, sizeof(buf));
    std::string name = toStdString(buf);
    if (name == "")
        name = "no-name";
    return name;
}

std::string sanitizeTagName(const std::string& tagName) {
    std::string illegalCharacters = ":";
    std::string replacementCharacter = "_";

    std::string sanitizedTagName = tagName;

    // Replace illegal characters with underscore
    for (char c : illegalCharacters) {
        sanitizedTagName.erase(std::remove(sanitizedTagName.begin(), sanitizedTagName.end(), c), sanitizedTagName.end());
    }
    std::replace(sanitizedTagName.begin(), sanitizedTagName.end(), '+', '_');
    std::replace(sanitizedTagName.begin(), sanitizedTagName.end(), ' ', '_');
    std::replace(sanitizedTagName.begin(), sanitizedTagName.end(), '\n', '_');

    // Replace multiple underscores with a single underscore
    while (sanitizedTagName.find("__") != std::string::npos) {
        sanitizedTagName.replace(sanitizedTagName.find("__"), 2, "_");
    }

    // Trim leading and trailing underscores
    size_t start = sanitizedTagName.find_first_not_of('_');
    size_t end = sanitizedTagName.find_last_not_of('_');
    sanitizedTagName = sanitizedTagName.substr(start, end - start + 1);

    return sanitizedTagName;
}

std::string getWndTagName(SWindow* pWnd)
{
    std::string name = "";
    if (pWnd->GetObjectClass() == SHostWnd::GetClassName())
    {
        name = toStdString(pWnd->GetObjectClass())
            + "_" + getHostWndName(static_cast<SHostWnd*>(pWnd)->m_hWnd);
    }
    else {
        name = toStdString(pWnd->GetObjectClass())
            + "_" + toStdString(pWnd->GetName());
    }
    return sanitizeTagName(name);
}

std::string getWndText(SWindow* pWnd, BOOL bRawText = FALSE)
{
    if (pWnd->GetObjectClass() == SHostWnd::GetClassName())
    {
        return getHostWndName(static_cast<SHostWnd*>(pWnd)->m_hWnd);
    }
    SString text = pWnd->GetWindowText(bRawText);
    if (!bRawText)
        text = GETSTRING(text); // 兼容自绘
    return toStdString(text);
}

std::string getWndClass(SWindow* pWnd)
{
    SString class_name = pWnd->GetObjectClass();
    if (class_name == SimulateWindow::GetClassName())
    {
        auto pSimulate = static_cast<SimulateWindow*>(pWnd);
        return getWndClass(pSimulate->simulate_parent) + "_" + toStdString(class_name);
    }
    return toStdString(class_name);
}

std::string getResId(SWindow* pWnd)
{
    // 不建议使用 XPath 定位器，这可能会导致脆弱的测试。请您的开发团队提供独特的辅助定位器! 
    return "";
}

std::string getWndEleId(SWindow* pWnd)
{
    if (pWnd->GetObjectClass() == SimulateWindow::GetClassName())
    {
        return "[simulate]_" + std::to_string(uint64_t(static_cast<SimulateWindow*>(pWnd)->simulate_parent->GetSwnd())) + '_' + toStdString(pWnd->GetName());
    }
    else if (pWnd->GetObjectClass() == SHostWnd::GetClassName())
    {
        return "[hostwnd]_" + std::to_string(uint64_t(static_cast<SHostWnd*>(pWnd)->m_hWnd));
    }
    return "[swnd]_" + std::to_string(uint64_t(pWnd->GetSwnd()));
}

SWindow* GetWndByElementId(const std::string& elementId)
{
    if (elementId.empty())
        return nullptr;

    if (elementId.substr(0, 7) == "[swnd]_")
    {
        uint64_t iwnd = std::stoull(elementId.substr(7));
        return SWindowMgr::GetWindow(SWND(iwnd));
    }
    else if (elementId.substr(0, 10) == "[hostwnd]_")
    {
        uint64_t iwnd = std::stoull(elementId.substr(10));
        return getHostWnd(HWND(iwnd));
    }
    else if (elementId.substr(0, 11) == "[simulate]_")
    {
        auto s = elementId.substr(11);
        auto ite = std::find(s.begin(), s.end(), '_');
        assert(ite != s.end());
        auto s1 = std::string(s.begin(), ite);
        auto s2 = std::string(ite + 1, s.end());

        uint64_t iwnd = std::stoull(s1);
        SWindow* parent = SWindowMgr::GetWindow(SWND(iwnd));

        return FindSimulateWindow(parent, s2);
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
    HWND hwnd = NULL;

    std::string elementId; // m_hWnd or swnd;
    std::string tag_name;
    int index = 0;
    //std::string package; // package="com.android.settings"
    std::string s_class; // class="android.widget.FrameLayout"
    std::string text;
    std::string original_text;
    std::string resource_id; //[可选] resource-id="com.android.settings:id/main_content"
    //bool checkable = false;
    bool checked = false;
    //bool clickable = false;
    bool enabled = false;
    bool focusable = false;
    bool focused = false;
    //bool password = false;
    //bool scrollable = false;
    bool selected = false;
    int offset_to_screen_x = 0;
    int offset_to_screen_y = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool displayed = false;
    //std::string hint; //[可选] hint="Enter name"
    std::map<std::string, std::string> dump;

    bool valid()
    {
        return (!tag_name.empty() && !elementId.empty());
    }
};

struct UINode
{
    UIInfo info;
    std::vector<std::shared_ptr<UINode>> children;
};

UIInfo parseNodeInfo(SWindow* pWnd, int index = 0)
{
    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    CPoint offset_to_screen;
    ::ClientToScreen(pHostMain->m_hWnd, &offset_to_screen);

    CPoint offset_to_main;
    SHostWnd* pHost = pWnd->GetHost();
    if (pHost != pHostMain)
    {
        ::ClientToScreen(pHost->m_hWnd, &offset_to_main);
        offset_to_main.Offset(-offset_to_screen);
    }

    UIInfo info;
    info.hwnd = pWnd->GetHostWnd();

    info.elementId = getWndEleId(pWnd);
    info.tag_name = getWndTagName(pWnd);
    info.index = index;
    info.s_class = getWndClass(pWnd);
    info.text = getWndText(pWnd, FALSE);
    info.original_text = getWndText(pWnd, TRUE);
    info.resource_id = getResId(pWnd);
    info.checked = pWnd->GetState() & WndState_Check;
    info.enabled = pWnd->IsEnable();
    info.focusable = pWnd->IsFocusable();
    info.focused = pWnd->IsFocused();
    info.offset_to_screen_x = offset_to_screen.x;
    info.offset_to_screen_y = offset_to_screen.y;
    info.x = offset_to_main.x + pWnd->GetClientRect().left;
    info.y = offset_to_main.y + pWnd->GetClientRect().top;
    info.width = pWnd->GetClientRect().Width();
    info.height = pWnd->GetClientRect().Height();
    info.displayed = pWnd->IsVisible(TRUE);

    auto testDump = pWnd->GetTestDump();
    info.dump["other"] = toStdString(pWnd->GetOther());
    for (auto ite = testDump.begin(); ite != testDump.end(); ++ite)
    {
        info.dump[toStdString(ite->first)] = toStdString(ite->second);
    }

    assert(!info.elementId.empty());
    assert(!info.tag_name.empty());
    assert(!info.s_class.empty());

    return info;
}

std::shared_ptr<UINode> parseNodeRecursive(SWindow* pWnd, int index)
{
    std::shared_ptr<UINode> node = std::make_shared<UINode>();

    node->info = parseNodeInfo(pWnd, index);

    auto children = getWndChildren(pWnd);
    for (auto pChild : children)
    {
        node->children.push_back(parseNodeRecursive(pChild, node->children.size()));
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

    CRect realScreenRect = WindowHelper::GetTotalScreen();
    CRect mainGeometry = WindowHelper::GetGeometry(pHostMain);
    CPoint mainOffset = mainGeometry.TopLeft();
    CRect screenRect = mainGeometry;

    top_node->info = parseNodeInfo(pHostMain);
    top_node->info.tag_name = ROOT_NAME;
    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = getHostWnd(allWnds[i]);
        std::string name = getHostWndName(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        CPoint offset;
        if (pHost == pHostMain)
        {
            offset = mainOffset;
        }
        else
        {
            ::ClientToScreen(pHost->m_hWnd, &offset);
            offset.Offset(-mainOffset);
        }

        top_node->children.push_back(parseNodeRecursive(pHost, top_node->children.size()));
    }

    return top_node;
}

std::vector<SWindow*> findEleByPathRecursive(SWindow* pWnd, std::vector<std::string> path_name, std::map<std::string, int>& repeat_name)
{
    if (path_name.empty())
        return {};

    std::string name = getWndTagName(pWnd);
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
                return {};
            }
        }
        else
        {
            // 后缀未匹配
            return {};
        }
    }
    else
    {
        // 未匹配
        return {};
    }

    std::vector<SWindow*> v;

    path_name.erase(path_name.begin());
    if (path_name.empty())
    {
        // found
        v.push_back(pWnd);
        return v;
    }

    auto children = getWndChildren(pWnd);
    std::map<std::string, int> repeat_name2;
    for (auto pChild : children)
    {
        auto v2 = findEleByPathRecursive(pChild, path_name, repeat_name2);
        v.insert(v.end(), v2.begin(), v2.end());
    }
    return v;
}

std::vector<std::string> findEleByPath(const std::string& path)
{
    auto path_name = Split(path.substr(1), '/');
    if (path_name.empty())
        return {};

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return {};

    vector<HWND> allWnds = getVisibleWindow();
    if (allWnds.empty())
        return {};

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return {};

    if (path_name.front() != ROOT_NAME)
        return {};
    path_name.erase(path_name.begin());

    std::vector<std::string> v;
    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = getHostWnd(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        std::map<std::string, int> repeat_name;
        auto v2 = (findEleByPathRecursive(pHost, path_name, repeat_name));
        for (auto pWnd : v2)
            v.push_back(getWndEleId(pWnd));
    }
    return v;
}

// 模糊匹配，通配符 '?' '*'
bool fuzzyMatch(string str1, string str2) {
    int m = str1.length();
    int n = str2.length();

    vector<vector<bool>> dp(m + 1, vector<bool>(n + 1, false));

    dp[0][0] = true;

    for (int i = 1; i <= m; i++) {
        dp[i][0] = false;
    }

    for (int j = 1; j <= n; j++) {
        dp[0][j] = false;
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else if (str2[j - 1] == '?') {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else if (str2[j - 1] == '*') {
                dp[i][j] = dp[i - 1][j] || dp[i][j - 1];
            }
            else {
                dp[i][j] = false;
            }
        }
    }

    return dp[m][n];
}

std::vector<SWindow*> findEleByTextRecursive(SWindow* pWnd, const std::string& text, bool matchMore = false)
{
    std::vector<SWindow*> v;

    if (text.empty())
        return v;

    bool match = matchMore ?
        (fuzzyMatch(text, getWndText(pWnd)) || fuzzyMatch(text, getWndTagName(pWnd)) || fuzzyMatch(text, getWndClass(pWnd))) :
        (fuzzyMatch(text, getWndText(pWnd)));
    if (match)
        v.push_back(pWnd);

    auto children = getWndChildren(pWnd);
    for (auto pChild : children)
    {
        auto v2 = findEleByTextRecursive(pChild, text, matchMore);
        v.insert(v.end(), v2.begin(), v2.end());
    }
    return v;
}

std::vector<std::string> findEleByText(const std::string& text)
{
    if (text.empty())
        return {};

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return {};

    vector<HWND> allWnds = getVisibleWindow();
    if (allWnds.empty())
        return {};

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return {};

    std::vector<std::string> v;
    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = getHostWnd(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        auto v2 = (findEleByTextRecursive(pHost, text));
        for (auto pWnd : v2)
            v.push_back(getWndEleId(pWnd));
    }
    return v;
}

std::vector<std::string> findEleById(const std::string& id)
{
    auto keys = Split(id, '/');
    if (keys.empty())
        return {};

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return {};

    vector<HWND> allWnds = getVisibleWindow();
    if (allWnds.empty())
        return {};

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return {};

    std::vector<SWindow*> v;
    for (int i = 0; i < allWnds.size(); i++)
    {
        SHostWnd* pHost = getHostWnd(allWnds[i]);

        if (!pHost) continue;
        if (!pHost->IsWindowVisible()) continue;

        v.push_back(pHost);
    }
    for (auto& key : keys)
    {
        auto v_recursive = std::move(v);
        for (auto& pWnd : v_recursive)
        {
            auto v2 = findEleByTextRecursive(pWnd, key, true);
            v.insert(v.end(), v2.begin(), v2.end());
        }
    }

    std::vector<std::string> v_ele;
    for (auto& pWnd : v)
        v_ele.push_back(getWndEleId(pWnd));
    return v_ele;
}

CRect getMainGeometry()
{
    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return CRect();

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return CRect();

    CRect mainGeometry = WindowHelper::GetGeometry(pHostMain);
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

    // 将目标窗口设为前台窗口
    //SetForegroundWindow(hMainWnd); 会将弹窗挡住

    CRect realScreenRect = WindowHelper::GetTotalScreen();
    CRect mainGeometry = WindowHelper::GetGeometry(pHostMain);

    int x = mainGeometry.left;
    int y = mainGeometry.top;
    int width = mainGeometry.Width();
    int height = mainGeometry.Height();

    // 获取屏幕设备上下文
    HDC hScreenDC = GetDC(NULL);
    HDC hCaptureDC = CreateCompatibleDC(hScreenDC);

    // 创建位图
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hCaptureDC, hCaptureBitmap);

    // 复制屏幕指定范围到位图
    BitBlt(hCaptureDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

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
    ReleaseDC(NULL, hScreenDC);

    // 释放 Bitmap 对象
    delete bitmap;

    return base64String;
}

// 模拟鼠标点击
void simulateMouseClick(int x, int y, int count = 1, DWORD dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP)
{
    // 创建鼠标事件
    std::vector<INPUT> inputs(count);
    for (auto& input : inputs)
    {
        memset(&input, 0, sizeof(INPUT));
        input.type = INPUT_MOUSE;
        input.mi.dx = x * 65536 / GetSystemMetrics(SM_CXSCREEN);
        input.mi.dy = y * 65536 / GetSystemMetrics(SM_CYSCREEN);
        input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | dwFlags;
    }

    // 发送鼠标事件
    SendInput(count, inputs.data(), sizeof(INPUT));
}

// 模拟键盘输入 Ctrl+A => KeyInput(VK_A, true);
void simulateKeyCodeInput(WORD virtualKeyCode, bool isCtrlPressed = false)
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

// 模拟键盘输入 Enter Escape Delete
void simulateKeyInput(WORD virtualKey)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &input, sizeof(INPUT));

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

// 模拟文字输入
void simulateTextInput(const std::string& text)
{
    std::wstring unicode = toStdWString(text);
    for (int i = 0; i < unicode.size(); ++i)
    {
        const wchar_t c = unicode[i];

        // 重复的字符会被丢弃，添加一个垃圾字符
        if (i > 0 && c == unicode[i - 1])
        {
            INPUT inputs[2];

            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_SHIFT;
            inputs[0].ki.dwFlags = 0;
            inputs[0].ki.time = 0;
            inputs[0].ki.dwExtraInfo = 0;

            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_SHIFT;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            inputs[1].ki.time = 0;
            inputs[1].ki.dwExtraInfo = 0;

            SendInput(2, inputs, sizeof(INPUT));
        }

        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &input, sizeof(INPUT));
    }
}

void simulateKeyboardInput(const std::string& in_text)
{
    static std::map<std::string, WORD> keys = {
        {u8"\ue003", VK_BACK},   // Keys.BACKSPACE
        {u8"\ue006", VK_RETURN}, // Keys.RETURN
        {u8"\ue007", VK_RETURN}, // Keys.ENTER
        {u8"\ue00c", VK_ESCAPE}, // Keys.ESCAPE
        {u8"\ue00d", VK_SPACE},  // Keys.SPACE
        {u8"\ue017", VK_DELETE}, // Keys.DELETE
    };
    static int keyLen = keys.begin()->first.size();

    std::string text = in_text;
    while (text.size())
    {
        // 功能按键
        auto ite = keys.find(text.substr(0, keyLen));
        if (ite != keys.end())
        {
            simulateKeyInput(ite->second);
            text = text.substr(keyLen);
            continue;
        }

        // TODO 兼容多个功能键
        break;
    }

    if (text.size())
        simulateTextInput(text);
}

tinyxml2::XMLElement* node2xml_recursive(std::shared_ptr<UINode> node, tinyxml2::XMLElement* parent)
{
    auto element = parent->InsertNewChildElement(node->info.tag_name.c_str());
    element->SetAttribute("tag_name", node->info.tag_name.c_str());
    element->SetAttribute("index", node->info.index);
    element->SetAttribute("eleId", node->info.elementId.c_str());
    //element->SetAttribute("package", node->info.package.c_str());
    element->SetAttribute("class", node->info.s_class.c_str());
    element->SetAttribute("text", node->info.text.c_str());
    element->SetAttribute("original-text", node->info.original_text.c_str());
    element->SetAttribute("resource-id", node->info.resource_id.c_str());
    //element->SetAttribute("checkable", node->info.checkable);
    element->SetAttribute("checked", node->info.checked);
    //element->SetAttribute("clickable", node->info.clickable);
    element->SetAttribute("enabled", node->info.enabled);
    element->SetAttribute("focusable", node->info.focusable);
    element->SetAttribute("focused", node->info.focused);
    //element->SetAttribute("password", node->info.password);
    //element->SetAttribute("scrollable", node->info.scrollable);
    element->SetAttribute("selected", node->info.selected);
    element->SetAttribute("offset_to_screen_x", node->info.offset_to_screen_x);
    element->SetAttribute("offset_to_screen_y", node->info.offset_to_screen_y);
    element->SetAttribute("x", node->info.x);
    element->SetAttribute("y", node->info.y);
    element->SetAttribute("width", node->info.width);
    element->SetAttribute("height", node->info.height);
    element->SetAttribute("displayed", node->info.displayed);

    // dump可以覆盖已有key
    for (auto ite = node->info.dump.begin(); ite != node->info.dump.end(); ++ite)
    {
        {
            // python XML 不支持的字符
            auto& key = ite->first;
            assert(key.find('<') == std::string::npos);
            assert(key.find('>') == std::string::npos);
            assert(key.find('&') == std::string::npos);
            assert(key.find('\'') == std::string::npos);
            assert(key.find('\"') == std::string::npos);
            assert(key.find('/') == std::string::npos);
        }
        element->SetAttribute(ite->first.c_str(), ite->second.c_str());
    }

    for (auto child : node->children)
        node2xml_recursive(child, element);
    return element;
}

std::string node2xml(std::shared_ptr<UINode> node)
{
    tinyxml2::XMLDocument doc;
    auto root = doc.NewElement(node->info.tag_name.c_str());
    root->SetAttribute("index", node->info.index);
    root->SetAttribute("class", node->info.s_class.c_str());
    root->SetAttribute("width", node->info.width);
    root->SetAttribute("height", node->info.height);

    for (auto child : node->children)
    {
        auto ele = node2xml_recursive(child, root);
    }

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
    if (strategy.empty() || selector.empty())
        return "";

    std::vector<std::string> eles;
    if (strategy == "id")
    {
        eles = findEleById(selector);
    }
    else if (strategy == "xpath")
    {
        eles = findEleByPath(selector);
    }
    else if (strategy == "name")
    {
        eles = findEleByText(selector);
    }

    if (eles.empty())
        return "";

    string str = multiple;
    std::transform(str.begin(), str.end(), str.begin(), [](auto& c) { return std::tolower(c); });
    bool multi =
        (str == "true") ? true :
        (str == "false") ? false :
        std::atoi(str.c_str());
    if (multi)
    {
        nlohmann::json json_array;
        for (auto& ele : eles) {
            nlohmann::json json = {
                { W3C_ELEMENT_KEY, ele }
            };
            json_array.push_back(json);
        }
        return json_array.dump();
    }
    else
    {
        nlohmann::json json = {
            { W3C_ELEMENT_KEY, eles.front() },
        };
        return json.dump();
    }
}

UIInfo getEleInfo(const std::string& elementId)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return {};

    HWND hMainWnd = SApplication::getSingleton().GetMainWnd();
    if (!hMainWnd)
        return {};

    SHostWnd* pHostMain = getHostWnd(hMainWnd);
    if (!pHostMain)
        return {};

    CRect mainGeometry = WindowHelper::GetGeometry(pHostMain);
    CPoint mainOffset = mainGeometry.TopLeft();

    SHostWnd* pHost = pWnd->GetHost();
    CPoint offset;
    if (pHost == pHostMain)
    {
        offset = mainOffset;
    }
    else
    {
        ::ClientToScreen(pHost->m_hWnd, &offset);
        offset.Offset(-mainOffset);
    }

    auto info = parseNodeInfo(pWnd);
    return info;
}

std::string getAttribute(const std::string& name, const std::string& elementId)
{
    auto info = getEleInfo(elementId);
    if (!info.valid())
        return "error";

    if (name == "name" || name == "tag_name")
        return info.tag_name;
    if (name == "eleId")
        return info.elementId;

    // dump可以覆盖已有key
    auto ite = info.dump.find(name);
    if (ite != info.dump.end())
        return ite->second;

    //if (name == "package")
    //    return info.package;
    if (name == "class")
        return info.s_class;
    if (name == "text")
        return info.text;
    if (name == "original-text")
        return info.original_text;
    if (name == "resource-id")
        return info.resource_id;
    //if (name == "checkable")
    //    return info.checkable ? "true" : "false";
    if (name == "checked")
        return info.checked ? "true" : "false";
    //if (name == "clickable")
    //    return info.clickable ? "true" : "false";
    if (name == "enabled")
        return info.enabled ? "true" : "false";
    if (name == "focusable")
        return info.focusable ? "true" : "false";
    if (name == "focused")
        return info.focused ? "true" : "false";
    //if (name == "password")
    //    return info.password ? "true" : "false";
    //if (name == "scrollable")
    //    return info.scrollable ? "true" : "false";
    if (name == "selected")
        return info.selected ? "true" : "false";
    if (name == "offset_to_screen_x")
        return std::to_string(info.offset_to_screen_x);
    if (name == "offset_to_screen_y")
        return std::to_string(info.offset_to_screen_y);
    if (name == "x")
        return std::to_string(info.x);
    if (name == "y")
        return std::to_string(info.y);
    if (name == "width")
        return std::to_string(info.width);
    if (name == "height")
        return std::to_string(info.height);
    if (name == "xywh")
        return std::to_string(info.x) + ',' + std::to_string(info.y) + ',' + std::to_string(info.width) + ',' + std::to_string(info.height);
    if (name == "ltrb")
        return std::to_string(info.x) + ',' + std::to_string(info.y) + ',' + std::to_string(info.x + info.width) + ',' + std::to_string(info.y + info.height);
    if (name == "displayed")
        return std::to_string(info.displayed);
    return "";
}

std::string performTouch(
    const std::string& action,
    const std::string& elementId,
    const std::string& duration = "",
    const std::string& x = "",
    const std::string& y = "",
    const std::string& count = "")
{
    auto info = getEleInfo(elementId);
    if (!info.valid())
        return "error";

    // 将目标窗口设为前台窗口
    SetForegroundWindow(info.hwnd);

    int pos_x = info.offset_to_screen_x + info.x + info.width / 2;
    if (x.size())
    {
        int nx = std::stoi(x);
        if (nx >= 0)
            pos_x = info.offset_to_screen_x + info.x + nx;
        else
            pos_x = info.offset_to_screen_x + info.x + info.width + nx;
    }

    int pos_y = info.offset_to_screen_y + info.y + info.height / 2;
    if (y.size())
    {
        int ny = std::stoi(y);
        if (ny >= 0)
            info.offset_to_screen_y + info.y + ny;
        else
            info.offset_to_screen_y + info.y + info.height + ny;
    }

    int n_count = count.empty() ? 1 : std::stoi(count);

    if (action == "click")
    {
        simulateMouseClick(pos_x, pos_y, n_count);
    }
    else if (action == "tap")
    {
        simulateMouseClick(pos_x, pos_y, n_count);
    }
    else if (action == "longPress")
    {
        // 右键
        simulateMouseClick(pos_x, pos_y, n_count, MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP);
    }
    return "ok";
}

std::string getText(const std::string& elementId)
{
    auto info = getEleInfo(elementId);
    if (!info.valid())
        return "error";

    return info.text;
}

std::string setValue(const std::string& elementId, const std::string& text)
{
    auto info = getEleInfo(elementId);
    if (!info.valid())
        return "error";

    // 将目标窗口设为前台窗口
    SetForegroundWindow(info.hwnd);

    simulateKeyboardInput(text);
    return "ok";
}

std::string clear(const std::string& elementId)
{
    auto info = getEleInfo(elementId);
    if (!info.valid())
        return "error";

    // 将目标窗口设为前台窗口
    SetForegroundWindow(info.hwnd);

    // 多删几次防止意外
    for (int i = 0; i < 10; ++i)
    {
        simulateMouseClick(
            info.offset_to_screen_x + info.x + info.width / 2,
            info.offset_to_screen_y + info.y + info.height / 2
        );

        // 模拟按下 Ctrl+A Backspace
        simulateKeyCodeInput('A', true);
        simulateKeyCodeInput(VK_BACK, false);
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
    
    svr.Get("/getAttribute", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"getAttribute");

        std::string name = req.get_param_value("name");
        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getAttribute(name, elementId);
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
            *sp_ret = performTouch("click", elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");

        chrono.Stop();
        chrono.OutputDebug();
    });
    
    svr.Get("/performTouch", [](const httplib::Request& req, httplib::Response& res) {
        Chrono chrono;
        chrono.Start(L"performTouch");

        std::string action = req.get_param_value("action");
        std::string element = req.get_param_value("element");
        std::string duration = req.get_param_value("duration");
        std::string x = req.get_param_value("x");
        std::string y = req.get_param_value("y");
        std::string count = req.get_param_value("count");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = performTouch(action, element, duration, x, y, count);
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

    if (g_listen_port <= 0)
        return false;
    bool ok = svr.listen("0.0.0.0", g_listen_port);
    return ok;
}

void AppiumDriver::Run(int port, bool thread)
{
    g_listen_port = port;
    if (thread) {
        std::thread tr(std::bind(run));
        tr.detach();
    }
    else {
        run();
    }
}
