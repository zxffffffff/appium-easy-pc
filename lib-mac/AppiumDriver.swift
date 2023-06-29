//
//  AppiumDriver.swift
//  mac-appium-driver
//
//  Created by 朱晓枫 on 2023/6/28.
//

import Foundation
import Swifter

func node2xml_recursive(_ node: UINode) -> XMLElement {
    // 创建子节点
    let element = XMLElement(name: node.info.name)
    element.setAttributesAs([
        "index" : node.info.index,
        "eleId" : node.info.elementId,
        "package" : node.info.package,
        "class" : node.info.s_class,
        "text" : node.info.text,
        "textRaw" : node.info.textRaw,
        "resource-id" : node.info.resource_id,
        "checkable" : "\(node.info.checkable)",
        "checked" : "\(node.info.checked)",
        "clickable" : "\(node.info.clickable)",
        "enabled" : "\(node.info.enabled)",
        "focusable" : "\(node.info.focusable)",
        "focused" : "\(node.info.focused)",
        "password" : "\(node.info.password)",
        "scrollable" : "\(node.info.scrollable)",
        "selected" : "\(node.info.selected)",
        "x" : "\(node.info.x)",
        "y" : "\(node.info.y)",
        "width" : "\(node.info.width)",
        "height" : "\(node.info.height)",
        "displayed" : "\(node.info.displayed)"
    ])
    
    for child in node.children {
        element.addChild(node2xml_recursive(child));
    }
    
    return element;
}

func node2xml(_ node: UINode) -> String {
    // 创建XML对象
    let xmlDoc = XMLDocument()

    // 创建根节点
    let rootElement = XMLElement(name: node.info.name)
    rootElement.setAttributesAs([
        "index" : node.info.index,
        "class" : node.info.s_class,
        "x" : "\(node.info.x)",
        "y" : "\(node.info.y)",
        "width" : "\(node.info.width)",
        "height" : "\(node.info.height)"
    ])
    xmlDoc.setRootElement(rootElement)

    for child in node.children {
        rootElement.addChild(node2xml_recursive(child))
    }
    
    if let xmlData = xmlDoc.xmlData(options: .nodePrettyPrint) as Data? {
        // 执行操作
        if let xmlString = String(data: xmlData, encoding: .utf8) {
            return (xmlString)
        } else {
            return ("无法将XML数据转换为字符串")
        }
    } else {
        return ("无法获取XML数据")
    }
}

func getPageSource() -> String {
    let node = GetAllNodes();
    let xml = node2xml(node);
    return xml;
}

func getScreenshot() -> String {
    return getScreenshotBase64();
}

func getWindowRect() -> String {
    return "";
}

func findElOrEls(_ strategy:String, _ selector:String, _ multiple:String, _ context:String) -> String {
    return "";
}

func click(_ elementId:String) -> String {
    return "";
}

func getText(_ elementId:String) -> String {
    return "";
}

func setValue2(_ elementId:String, _ text:String) -> String {
    return "";
}

func clear(_ elementId:String) -> String {
    return "";
}

let server = HttpServer()

@objcMembers
class AppiumDriver: NSObject {
    static func Run() {
        server["/getPageSource"] = { request in
            var ret = "";
            DispatchQueue.main.sync {
                ret = getPageSource();
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/getScreenshot"] = { request in
            var ret = "";
            DispatchQueue.main.sync {
                ret = getScreenshot();
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/getWindowRect"] = { request in
            var ret = "";
            DispatchQueue.main.sync {
                ret = getWindowRect();
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/findElOrEls"] = { request in
            let strategy = request.params["strategy"] ?? "";
            let selector = request.params["selector"] ?? "";
            let multiple = request.params["multiple"] ?? "";
            let context = request.params["context"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = findElOrEls(strategy, selector, multiple, context);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/click"] = { request in
            let elementId = request.params["elementId"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = click(elementId);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/getText"] = { request in
            let elementId = request.params["elementId"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = getText(elementId);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/setValue"] = { request in
            let elementId = request.params["elementId"] ?? "";
            let text = request.params["text"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = setValue2(elementId, text);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/clear"] = { request in
            let elementId = request.params["elementId"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = clear(elementId);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        do {
            try server.start(4724, forceIPv4: true)
            print("Server has started (port: \(try server.port()))")
        } catch {
            print("Server start error: \(error)")
        }
    }
}
