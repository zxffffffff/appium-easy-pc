//
//  AppiumDriver.swift
//  mac-appium-driver
//
//  Created by 朱晓枫 on 2023/6/28.
//

import Foundation
import Swifter
import Cocoa
import AppKit

let ROOT_NAME = "root"
let W3C_ELEMENT_KEY = "element-6066-11e4-a52e-4f735466cecf"

func getVisibleWindow() -> [NSWindow] {
    let windows = NSApplication.shared.windows.filter { $0.isVisible && $0.isReleasedWhenClosed == false }
    return windows.sorted(by: { $0.windowNumber < $1.windowNumber })
}

struct UIInfo
{
    var elementId: String = ""; // m_hWnd or swnd;
    var name: String = "";
    var index: Int = 0;
    var package: String = ""; // package="com.android.settings"
    var s_class: String = ""; // class="android.widget.FrameLayout"
    var text: String = "";
    //var textRaw: String = "";
    var resource_id: String = ""; //[可选] resource-id="com.android.settings:id/main_content"
    //var checkable: Bool = false;
    //var checked: Bool = false;
    //var clickable: Bool = false;
    //var enabled: Bool = false;
    //var focusable: Bool = false;
    var focused: Bool = false;
    //var password: Bool = false;
    //var scrollable: Bool = false;
    //var selected: Bool = false;
    var x: CGFloat = 0.0;
    var y: CGFloat = 0.0;
    var width: CGFloat = 0.0;
    var height: CGFloat = 0.0;
    var displayed: Bool = false;
    //var hint: String = ""; //[可选] hint="Enter name"
};

struct UINode
{
    var info: UIInfo = UIInfo();
    var children: [UINode] = [];
};

func sanitizeTagName(_ tagName: String) -> String {
    let illegalCharacterSet = CharacterSet(charactersIn: ":").union(CharacterSet.whitespacesAndNewlines)
    let replacementCharacter = "_"

    var sanitizedTagName = tagName

    // Replace illegal characters with underscore
    sanitizedTagName = sanitizedTagName.components(separatedBy: illegalCharacterSet).joined(separator: replacementCharacter)

    // Replace multiple underscores with a single underscore
    while sanitizedTagName.contains("__") {
        sanitizedTagName = sanitizedTagName.replacingOccurrences(of: "__", with: "_")
    }

    // Trim leading and trailing underscores
    sanitizedTagName = sanitizedTagName.trimmingCharacters(in: CharacterSet(charactersIn: "_"))

    return sanitizedTagName
}

func getWindowViewName(_ obj: Any) -> String {
    var name = "";
    if let window = obj as? NSWindow {
        let id :String = window.identifier?.rawValue ?? "";
        name = "\(window.className)_\(window.title)_\(id)";
    } else if let view = obj as? NSView {
        let id :String = view.identifier?.rawValue ?? "";
        name = "\(view.className)_\(view.tag)_\(id)";
    }
    return sanitizeTagName(name);
}

func getWindowViewText(_ obj: Any) -> String {
    if let window = obj as? NSWindow {
        return window.title
    } else if let label = obj as? NSTextField {
        return label.stringValue
    } else if let button = obj as? NSButton {
        return button.title
    } else if let textField = obj as? NSTextField {
        return textField.stringValue
    } else {
        let mirror = Mirror(reflecting: obj)
        for child in mirror.children {
            if child.label == "stringValue" {
                if let textValue = child.value as? String {
                    return textValue
                }
            }
        }
    }
    return ""
}

func getWindowViewEleId(_ obj: Any) -> String {
    if let window = obj as? NSWindow {
        let id :String = window.identifier?.rawValue ?? "";
        return "[window]_[\(window.hash)]_[\(id)]"
    } else if let view = obj as? NSView {
        let id :String = view.identifier?.rawValue ?? "";
        return "[view]_[\(view.hash)]_[\(id)]_[\(view.tag)]"
    }
    return "";
}

func getWindowViewRect(_ obj: Any) -> CGRect {
    if let window = obj as? NSWindow {
        guard let screen = window.screen else {
            return CGRect();
        }
        return CGRect(
            x: window.frame.origin.x,
            y: screen.frame.height - (window.frame.origin.y + window.frame.height),
            width: window.frame.width,
            height:window.frame.height
        );
    } else if let view = obj as? NSView {
        guard let window = view.window else {
            return CGRect();
        }
        let viewFrame = view.convert(view.bounds, to: window.contentView);
        return CGRect(
            x: viewFrame.origin.x,
            y: window.frame.height - (viewFrame.origin.y + viewFrame.height),
            width: viewFrame.width,
            height: viewFrame.height
        );
    }
    return CGRect();
}

func parseNodeInfo(_ window: NSWindow) -> UIInfo {
    let rect = getWindowViewRect(window)
    let info = UIInfo(
        elementId: getWindowViewEleId(window),
        name: getWindowViewName(window),
        s_class: window.className,
        text: getWindowViewText(window),
        focused: window.isKeyWindow,
        x: 0, //rect.origin.x,
        y: 0, //rect.origin.y,
        width: rect.width,
        height: rect.height,
        displayed: window.isVisible
    );
    //print("ParseNodeInfo info=\(info)")
    return info;
}

func parseNodeInfo(_ view: NSView) -> UIInfo {
    let rect = getWindowViewRect(view)
    let info = UIInfo(
        elementId: getWindowViewEleId(view),
        name: getWindowViewName(view),
        s_class: view.className,
        text: getWindowViewText(view),
        x: rect.origin.x,
        y: rect.origin.y,
        width: rect.width,
        height: rect.height,
        displayed: !view.isHidden
    );
    //print("ParseNodeInfo info=\(info)")
    return info;
}

func parseNodeRecursive(_ window: NSWindow) -> UINode {
    var node = UINode();
    
    node.info = parseNodeInfo(window);
    
    for view in window.contentView?.subviews ?? [] {
        node.children.append(parseNodeRecursive(view));
    }
    
    return node;
}

func parseNodeRecursive(_ view: NSView) -> UINode {
    var node = UINode();
    
    node.info = parseNodeInfo(view);
    
    for subview in view.subviews where !subview.isHidden {
        node.children.append(parseNodeRecursive(subview));
    }
    
    return node;
}

func getAllNodes() -> UINode {
    var top_node = UINode();
    top_node.info.name = ROOT_NAME;
    
    let windows = getVisibleWindow();
    if (windows.isEmpty) {
        return top_node;
    }
    let window = windows[0];
    
    top_node.info.width = window.frame.width;
    top_node.info.height = window.frame.height;
    
    for window in windows {
        top_node.children.append(parseNodeRecursive(window));
    }
    
    return top_node;
}

func findNodesRecursive(_ obj: Any, _ path_name2: [String], _ repeat_name: inout [String:Int]) -> [String] {
    var v: [String] = [];
    
    var path_name = path_name2;
    if (path_name.isEmpty) {
        return [];
    }
    
    let name = getWindowViewName(obj);
    let name2 = path_name[0];
    if (name2.hasPrefix(name)) {
        // 前缀匹配
        let commonPrefix = name.commonPrefix(with: name2)
        var diff = name2.replacingOccurrences(of: commonPrefix, with: "", options: .literal, range: nil)
        if (diff.isEmpty) {
            // 全部匹配
        }
        else if (diff.first == "[" && diff.last == "]") {
            diff.removeFirst()
            diff.removeLast()
            let index = Int(diff);
            var repeat_index = repeat_name[name] ?? 0;
            repeat_index += 1;
            repeat_name[name] = repeat_index;
            if (repeat_index != index) {
                // 索引不匹配
                return [];
            }
        }
        else {
            // 后缀未匹配
            return [];
        }
    }
    else {
        // 未匹配
        return [];
    }
    
    path_name.removeFirst()
    if (path_name.isEmpty) {
        // found
        v.append(getWindowViewEleId(obj))
        return v;
    }
    if let window = obj as? NSWindow {
        var repeat_name2: [String:Int] = [:]
        for view in window.contentView?.subviews ?? [] {
            v.append(contentsOf: findNodesRecursive(view, path_name, &repeat_name2))
        }
    } else if let view = obj as? NSView {
        var repeat_name2: [String:Int] = [:]
        for subview in view.subviews where !subview.isHidden {
            v.append(contentsOf: findNodesRecursive(subview, path_name, &repeat_name2))
        }
    }
    return v;
}

func findNodes(_ path_name2: [String]) -> [String] {
    var v: [String] = [];
    
    var path_name = path_name2;
    if (path_name.isEmpty) {
        return [];
    }
    
    let windows = getVisibleWindow();
    if (windows.isEmpty) {
        return [];
    }
    
    if (path_name.first != ROOT_NAME) {
        return [];
    }
    path_name.removeFirst();
    
    var repeat_name2: [String:Int] = [:]
    for window in windows {
        v.append(contentsOf: findNodesRecursive(window, path_name, &repeat_name2))
    }
    
    return v;
}

func findNodeByEleId(_ view: NSView, _ elementId: String) -> NSView? {
    if (getWindowViewEleId(view) == elementId) {
        return view;
    }
    
    for subview in view.subviews where !subview.isHidden {
        if let ret_view = findNodeByEleId(subview, elementId) {
            return ret_view;
        }
    }
    
    return nil;
}

func findNodeByEleId(_ elementId: String) -> Any? {
    let windows = getVisibleWindow();
    if (windows.isEmpty) {
        return nil;
    }
    
    for window in windows {
        if (getWindowViewEleId(window) == elementId) {
            return window;
        }
        
        for view in window.contentView?.subviews ?? [] {
            if let ret_view = findNodeByEleId(view, elementId) {
                return ret_view;
            }
        }
    }
    
    return nil;
}

func getScreenshotBase64() -> String {
    let windows = getVisibleWindow();
    if (windows.isEmpty) {
        return "";
    }
    let window = windows[0];
    
    var newY = 0.0
    if let screen = window.screen {
        newY = screen.frame.height - (window.frame.height + window.frame.origin.y)
    }
    
    let imageRect = NSRect(x:window.frame.origin.x, y:newY, width:window.frame.width, height:window.frame.height)
    print("getScreenshotBase64 imageRect: \(imageRect)");
    
    if let cgImage = CGWindowListCreateImage(imageRect, .optionIncludingWindow, CGWindowID(window.windowNumber), .bestResolution) {
        let bitmapRep = NSBitmapImageRep(cgImage: cgImage)
        let pngData = bitmapRep.representation(using: .png, properties: [:])
        if let base64String = pngData?.base64EncodedString() {
            
            //if let imageData = Data(base64Encoded: base64String) {
            //    let fileManager = FileManager.default
            //    let temporaryDirectory = fileManager.temporaryDirectory
            //
            //    do {
            //        let fileName = UUID().uuidString + ".png"
            //        let fileURL = temporaryDirectory.appendingPathComponent(fileName)
            //        try imageData.write(to: fileURL)
            //        print(fileURL)
            //    } catch {
            //        print("Failed to save Base64 as PNG file: \(error)")
            //        return ""
            //    }
            //}
            
            return base64String
        }
    }
    
    return ""
}

func getMainRect() -> CGRect {
    let windows = getVisibleWindow();
    if (windows.isEmpty) {
        return CGRect();
    }
    let window = windows[0];
    return CGRect(x: 0, y: 0, width: window.frame.width, height: window.frame.height)
}

class EventSender {
    private var taskQueue: [EventTask] = []
    private var timer: Timer?
    
    struct EventTask {
        var events: [CGEvent] = []
    }

    func addTaskToQueue(events: [CGEvent]) {
        taskQueue.append(EventTask(events: events))
        
        // 如果定时器为空，则开始定时器
        if timer == nil {
            startTimer()
        }
    }
    
    private func startTimer() {
        // 创建一个每隔一段时间触发的定时器
        timer = Timer.scheduledTimer(withTimeInterval: 0.05, repeats: true, block: { [weak self] (_) in
            self?.sendNextEvent()
        })
    }
    
    private func sendNextEvent() {
        guard let task = taskQueue.first else {
            // 事件队列为空，停止定时器
            stopTimer()
            return
        }
        
        // 发送事件
        for event in task.events{
            event.post(tap: .cghidEventTap)
        }
        
        // 从队列中移除已发送的事件
        taskQueue.removeFirst()
    }
    
    private func stopTimer() {
        timer?.invalidate()
        timer = nil
    }
}

let eventSender = EventSender()

// 模拟鼠标点击
func simulateMouseClick(_ view: NSView, dbClick: Bool = false) {
    // 获取目标视图的中间坐标
    let viewRect = getWindowViewRect(view)
    let windowRect = getWindowViewRect(view.window)
    let screenMidPoint = CGPoint(
        x: viewRect.midX + windowRect.origin.x,
        y: viewRect.midY + windowRect.origin.y
    )
    print("simulate MouseClick \(screenMidPoint)")

    // 创建鼠标事件
    var events: [CGEvent] = []
    if let downEvent = CGEvent(mouseEventSource: nil, mouseType: .leftMouseDown, mouseCursorPosition: screenMidPoint, mouseButton: .left),
       let upEvent = CGEvent(mouseEventSource: nil, mouseType: .leftMouseUp, mouseCursorPosition: screenMidPoint, mouseButton: .left){
        events.append(downEvent)
        events.append(upEvent)
    }
    if (dbClick) {
        if let downEvent = CGEvent(mouseEventSource: nil, mouseType: .leftMouseDown, mouseCursorPosition: screenMidPoint, mouseButton: .left),
           let upEvent = CGEvent(mouseEventSource: nil, mouseType: .leftMouseUp, mouseCursorPosition: screenMidPoint, mouseButton: .left){
            events.append(downEvent)
            events.append(upEvent)
        }
    }
    eventSender.addTaskToQueue(events: events)
}

// 模拟输入文字
func simulateKeyboardInput(text: String) {
    print("simulate text \(text)")
    let eventSource = CGEventSource(stateID: .hidSystemState)

    for character in text {
        let keyDown = CGEvent(keyboardEventSource: eventSource, virtualKey: 0, keyDown: true)
        let keyUp = CGEvent(keyboardEventSource: eventSource, virtualKey: 0, keyDown: false)

        if let keyDown = keyDown, let keyUp = keyUp {
            let characterString = String(character)
            var keyCode: [UInt16] = []
            characterString.unicodeScalars.forEach { scalar in
                keyCode.append(UInt16(scalar.value))
            }

            keyDown.keyboardSetUnicodeString(stringLength: Int(keyCode.count), unicodeString: &keyCode)
            keyUp.keyboardSetUnicodeString(stringLength: Int(keyCode.count), unicodeString: &keyCode)

            // keyDown.post(tap: .cghidEventTap)
            // keyUp.post(tap: .cghidEventTap)
            eventSender.addTaskToQueue(events: [keyDown, keyUp])
        }
    }
}

func simulateCommandA() {
    print("simulate Command + A")
    let eventSource = CGEventSource(stateID: .hidSystemState)
    
    let commandKeyDown = CGEvent(keyboardEventSource: eventSource, virtualKey: 0x37, keyDown: true)
    let commandKeyUp = CGEvent(keyboardEventSource: eventSource, virtualKey: 0x37, keyDown: false)
    
    let aKeyDown = CGEvent(keyboardEventSource: eventSource, virtualKey: 0x00, keyDown: true)
    let aKeyUp = CGEvent(keyboardEventSource: eventSource, virtualKey: 0x00, keyDown: false)
    
    commandKeyDown?.flags = .maskCommand
    commandKeyUp?.flags = .maskCommand
    
    aKeyDown?.flags = .maskCommand
    aKeyUp?.flags = .maskCommand
    
    if let commandKeyDown = commandKeyDown,
       let commandKeyUp = commandKeyUp,
       let aKeyDown = aKeyDown,
       let aKeyUp = aKeyUp {
        // commandKeyDown.post(tap: .cghidEventTap)
        // aKeyDown.post(tap: .cghidEventTap)
        // aKeyUp.post(tap: .cghidEventTap)
        // commandKeyUp.post(tap: .cghidEventTap)
        eventSender.addTaskToQueue(events: [commandKeyDown, aKeyDown, aKeyUp, commandKeyUp])
    }
}

func simulateBackspace() {
    print("simulate Backspace")
    let eventSource = CGEventSource(stateID: .hidSystemState)
    
    let backspaceKeyDown = CGEvent(keyboardEventSource: eventSource, virtualKey: 0x33, keyDown: true)
    let backspaceKeyUp = CGEvent(keyboardEventSource: eventSource, virtualKey: 0x33, keyDown: false)
    
    if let backspaceKeyDown = backspaceKeyDown, let backspaceKeyUp = backspaceKeyUp {
        // backspaceKeyDown.post(tap: .cghidEventTap)
        // backspaceKeyUp.post(tap: .cghidEventTap)
        eventSender.addTaskToQueue(events: [backspaceKeyDown, backspaceKeyUp])
    }
}

func map2jsonStr(_ obj: [String: Any]) -> String {
    do {
        let jsonData = try JSONSerialization.data(withJSONObject: obj, options: .prettyPrinted)
        if let jsonString = String(data: jsonData, encoding: .utf8) {
            return (jsonString)
        }
    } catch {
        return ("转换为 JSON 失败: \(error)")
    }
    return "";
}

func node2xml_recursive(_ node: UINode) -> XMLElement {
    // 创建子节点
    let element = XMLElement(name: node.info.name)
    element.setAttributesAs([
        "index" : node.info.index,
        "eleId" : node.info.elementId,
        "package" : node.info.package,
        "class" : node.info.s_class,
        "text" : node.info.text,
        //"textRaw" : node.info.textRaw,
        "resource-id" : node.info.resource_id,
        //"checkable" : "\(node.info.checkable)",
        //"checked" : "\(node.info.checked)",
        //"clickable" : "\(node.info.clickable)",
        //"enabled" : "\(node.info.enabled)",
        //"focusable" : "\(node.info.focusable)",
        "focused" : "\(node.info.focused)",
        //"password" : "\(node.info.password)",
        //"scrollable" : "\(node.info.scrollable)",
        //"selected" : "\(node.info.selected)",
        "x" : "\(node.info.x)",
        "y" : "\(node.info.y)",
        "width" : "\(node.info.width)",
        "height" : "\(node.info.height)",
        "displayed" : "\(node.info.displayed)",
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
    let node = getAllNodes();
    let xml = node2xml(node);
    return xml;
}

func getScreenshot() -> String {
    return getScreenshotBase64();
}

func getWindowRect() -> String {
    let rect = getMainRect();
    let dic: [String: Any] = [
        "x": 0,
        "y": 0,
        "width": rect.width,
        "height": rect.height
    ]
    return map2jsonStr(dic)
}

func findElOrEls(_ strategy:String, _ selector:String, _ multiple:String, _ context:String) -> String {
    if (strategy == "xpath") {
        if (selector.isEmpty) {
            return "";
        }
        
        let multi = (multiple == "true");
        let components = selector.components(separatedBy: "/").filter { !$0.isEmpty }
        
        let nodes = findNodes(components);
        if (nodes.isEmpty) {
            return "";
        }
        
        if (multi) {
            let dic: [String: Any] = [
                W3C_ELEMENT_KEY : nodes
            ]
            return map2jsonStr(dic);
        }
        else {
            let dic: [String: Any] = [
                W3C_ELEMENT_KEY : nodes.first
            ]
            return map2jsonStr(dic);
        }
    }
    return "";
}

func click(_ elementId:String) -> String {
    if let obj = findNodeByEleId(elementId) {
        if let window = obj as? NSWindow {
            return "todo"
        } else if let view = obj as? NSView {
            simulateMouseClick(view)
            return "ok"
        }
    }
    return "error";
}

func getText(_ elementId:String) -> String {
    return "";
}

func setValue2(_ elementId:String, _ text:String) -> String {
    if let obj = findNodeByEleId(elementId) {
        if let window = obj as? NSWindow {
            return "todo"
        } else if let view = obj as? NSView {
            simulateMouseClick(view, dbClick: true)
            
            simulateCommandA()
            simulateBackspace()
            
            simulateKeyboardInput(text: text)
            return "ok"
        }
    }
    return "error";
}

func clear(_ elementId:String) -> String {
    if let obj = findNodeByEleId(elementId) {
        if let window = obj as? NSWindow {
            return "todo"
        } else if let view = obj as? NSView {
            simulateMouseClick(view, dbClick: true)
            
            simulateCommandA()
            simulateBackspace()
            return "ok"
        }
    }
    return "error";
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
            let dic = Dictionary(uniqueKeysWithValues: request.queryParams)
            let strategy = dic["strategy"] ?? "";
            let selector = dic["selector"] ?? "";
            let multiple = dic["multiple"] ?? "";
            let context = dic["context"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = findElOrEls(strategy, selector.removingPercentEncoding ?? selector, multiple, context);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/click"] = { request in
            let dic = Dictionary(uniqueKeysWithValues: request.queryParams)
            let elementId = dic["elementId"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = click(elementId);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/getText"] = { request in
            let dic = Dictionary(uniqueKeysWithValues: request.queryParams)
            let elementId = dic["elementId"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = getText(elementId);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/setValue"] = { request in
            let dic = Dictionary(uniqueKeysWithValues: request.queryParams)
            let elementId = dic["elementId"] ?? "";
            let text = dic["text"] ?? "";
            var ret = "";
            DispatchQueue.main.sync {
                ret = setValue2(elementId, text.removingPercentEncoding ?? text);
            }
            return HttpResponse.ok(.text(ret))
        }
        
        server["/clear"] = { request in
            let dic = Dictionary(uniqueKeysWithValues: request.queryParams)
            let elementId = dic["elementId"] ?? "";
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
        
        // test
        Test();
    }
    
    static func Test() {
        let testCases = [
            "_NS:8",              // "_NS_8"
            "ABC__NS:8",          // "ABC_NS_8"
            "my tag name",        // "my_tag_name"
            "1invalid-tag!name$", // "1invalid_tag_name_"
            ":tag:name:",         // "_tag_name_"
            "__test__tag__",      // "_test_tag_"
            "__",                 // "_"
            "___",                // "_"
            "",                   // ""
        ]
        for testCase in testCases {
            let sanitizedTagName = sanitizeTagName(testCase)
            print("Original: \(testCase), Sanitized: \(sanitizedTagName)")
        }
    }
}
