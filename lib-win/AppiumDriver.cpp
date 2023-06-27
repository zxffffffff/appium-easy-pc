#include "AppiumDriver.h"
#include "AppiumDriverUI.h"
#include "httplib.h"
#include "json.hpp"
#include <functional>

using Json = nlohmann::json;

#define W3C_ELEMENT_KEY "element-6066-11e4-a52e-4f735466cecf"

void node2xml(std::stringstream& xml, std::shared_ptr<UINode> node, int deep = 0)
{
    if (deep == 0)
    {
        //xml << "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>" << "\n";
        xml << "<" << node->info.name
            << " index=\"" << node->info.index
            << "\" class=\"" << node->info.s_class
            << "\" rotation=\"0\" width=\"" << node->info.bounds.Width()
            << "\" height=\"" << node->info.bounds.Height()
            << "\">\n";

        for (auto child : node->children)
            node2xml(xml, child, deep + 1);

        xml << "</" << node->info.name 
            << ">\n";
    }
    else
    {
        xml << "<" << node->info.name
            << " index=\"" << node->info.index
            << "\" eleId=\"" << node->info.elementId
            << "\" package=\"" << node->info.package
            << "\" class=\"" << node->info.s_class
            << "\" text=\"" << node->info.text
            << "\" textRaw=\"" << node->info.textRaw
            << "\" resource-id=\"" << node->info.resource_id
            << "\" checkable=\"" << (node->info.checkable ? "true" : "false")
            << "\" checked=\"" << (node->info.checked ? "true" : "false")
            << "\" clickable=\"" << (node->info.clickable ? "true" : "false")
            << "\" enabled=\"" << (node->info.enabled ? "true" : "false")
            << "\" focusable=\"" << (node->info.focusable ? "true" : "false")
            << "\" focused=\"" << (node->info.focused ? "true" : "false")
            << "\" password=\"" << (node->info.password ? "true" : "false")
            << "\" scrollable=\"" << (node->info.scrollable ? "true" : "false")
            << "\" selected=\"" << (node->info.selected ? "true" : "false")
            << "\" bounds=\"[" << node->info.bounds.left << "," << node->info.bounds.top << "][" << node->info.bounds.right << "," << node->info.bounds.bottom << "]"
            << "\" displayed=\"" << (node->info.displayed ? "true" : "false")
            << "\">\n";

        for (auto child : node->children)
            node2xml(xml, child, deep + 1);

        xml << "</" << node->info.name
            << ">\n";
    }
}

/* The UI hierarchy in a platform-appropriate format (e.g., HTML for a web page)
 * http://appium.io/docs/en/2.0/reference/interfaces/appium_types.ExternalDriver/#getpagesource
 */
std::string getPageSource()
{
    std::shared_ptr<UINode> node = GetAllNodes();
    std::stringstream xml;
    node2xml(xml, node);
    return xml.str();
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
    Json json = {
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
        auto path_name = XXCommon::SplitStr(selector.substr(1), '/');

        std::vector<std::string> nodes = findNodes(path_name);
        if (nodes.empty())
            return "";

        if (multi)
        {
            Json json_array;
            for (auto& node : nodes)
                json_array.push_back(node);

            Json json = {
                { W3C_ELEMENT_KEY, json_array},
            };
            return json.dump();
        }
        else
        {
            Json json = {
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

    auto rect = XXWindowHelper::GetGeometry(pWnd);
    int x = rect.CenterPoint().x;
    int y = rect.CenterPoint().y;

    mouseClick(x, y);

    return "ok";
}

std::string getText(const std::string& elementId)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "";

    return XXToF8(pWnd->GetWindowText());
}

std::string setValue(const std::string& elementId, const std::string& text)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "error";

    auto rect = XXWindowHelper::GetGeometry(pWnd);
    int x = rect.CenterPoint().x;
    int y = rect.CenterPoint().y;

    mouseClick(x, y);
    textInput(CXFunc::str2wstr(text));

    return "ok";
}

std::string clear(const std::string& elementId)
{
    auto pWnd = GetWndByElementId(elementId);
    if (!pWnd)
        return "error";

    auto rect = XXWindowHelper::GetGeometry(pWnd);
    int x = rect.CenterPoint().x;
    int y = rect.CenterPoint().y;

    int flag = 0;
    while (!pWnd->GetWindowText().IsEmpty())
    {
        mouseClick(x, y);

        // 模拟按下 Ctrl+A Backspace
        keyBoardInput('A', true);
        keyBoardInput(VK_BACK, false);

        if (++flag > 100)
            return "error: dead loop";
    }

    return "ok";
}

bool run()
{
    // HTTP
    httplib::Server svr;

    svr.Get("/getPageSource", [](const httplib::Request& req, httplib::Response& res) {
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getPageSource();
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
    });

    svr.Get("/getScreenshot", [](const httplib::Request& req, httplib::Response& res) {
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getScreenshot();
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
    });
    
    svr.Get("/getWindowRect", [](const httplib::Request& req, httplib::Response& res) {
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getWindowRect();
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
    });
    
    svr.Get("/findElOrEls", [](const httplib::Request& req, httplib::Response& res) {
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
    });
    
    svr.Get("/click", [](const httplib::Request& req, httplib::Response& res) {
        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = click(elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
    });
    
    svr.Get("/getText", [](const httplib::Request& req, httplib::Response& res) {
        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = getText(elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
    });

    svr.Get("/setValue", [](const httplib::Request& req, httplib::Response& res) {
        std::string text = req.get_param_value("text");
        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = setValue(elementId, text);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
    });
    
    svr.Get("/clear", [](const httplib::Request& req, httplib::Response& res) {
        std::string elementId = req.get_param_value("elementId");
        auto sp_ret = std::make_shared<std::string>();

        JumpBase jumpbase;
        auto future = jumpbase.Jump([=] {
            *sp_ret = clear(elementId);
        }, __FUNCTIONW__);
        future.wait();

        res.set_content(*sp_ret, "text/plain");
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
