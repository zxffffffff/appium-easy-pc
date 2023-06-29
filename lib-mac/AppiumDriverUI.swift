//
//  AppiumDriverUI.swift
//  mac-appium-driver
//
//  Created by 朱晓枫 on 2023/6/28.
//

import Foundation
import Cocoa
import AppKit

func GetAllWnds() -> [NSWindow] {
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
    var textRaw: String = "";
    var resource_id: String = ""; //[可选] resource-id="com.android.settings:id/main_content"
    var checkable: Bool = false;
    var checked: Bool = false;
    var clickable: Bool = false;
    var enabled: Bool = false;
    var focusable: Bool = false;
    var focused: Bool = false;
    var password: Bool = false;
    var scrollable: Bool = false;
    var selected: Bool = false;
    var x: CGFloat = 0.0;
    var y: CGFloat = 0.0;
    var width: CGFloat = 0.0;
    var height: CGFloat = 0.0;
    var displayed: Bool = false;
    var hint: String = ""; //[可选] hint="Enter name"
};

struct UINode
{
    var info: UIInfo = UIInfo();
    var children: [UINode] = [];
};

func getName(_ window: NSWindow) -> String {
    // todo
    return "\(window.className)_\(window.title)";
}

func getName(_ view: NSView) -> String {
    // todo
    return view.className;
}

func getText(_ view: NSView) -> String {
    if let label = view as? NSTextField {
        return label.stringValue
    } else if let button = view as? NSButton {
        return button.title
    } else if let textField = view as? NSTextField {
        return textField.stringValue
    } else {
        let mirror = Mirror(reflecting: view)
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

func ParseNodeInfo(_ window: NSWindow) -> UIInfo {
    let info = UIInfo(
        elementId: "\(window.hash)",
        name: getName(window),
        s_class: "\(type(of: window))",
        focused: window.isKeyWindow,
        width: window.frame.width,
        height: window.frame.height,
        displayed: window.isVisible
    );
    print("ParseNodeInfo info=\(info)")
    return info;
}

func ParseNodeInfo(_ view: NSView) -> UIInfo {
    var newY = 0.0
    if let window = view.window {
        newY = window.frame.height - (view.frame.height + view.frame.origin.y)
    }
    let info = UIInfo(
        elementId: "\(view.hash)",
        name: getName(view),
        s_class: String(describing: type(of: view)),
        x: view.frame.origin.x,
        y: newY, // view.frame.origin.y,
        width: view.frame.width,
        height: view.frame.height,
        displayed: !view.isHidden
    );
    //print("ParseNodeInfo info=\(info)")
    return info;
}

func ParseNodeRecursive(_ window: NSWindow) -> UINode {
    var node = UINode();
    
    node.info = ParseNodeInfo(window);
    
    for view in window.contentView?.subviews ?? [] {
        node.children.append(ParseNodeRecursive(view));
    }
    
    return node;
}

func ParseNodeRecursive(_ view: NSView) -> UINode {
    var node = UINode();
    
    node.info = ParseNodeInfo(view);
    
    for subview in view.subviews where !subview.isHidden {
        node.children.append(ParseNodeRecursive(subview));
    }
    
    return node;
}

func GetAllNodes() -> UINode {
    var top_node = UINode();
    
    top_node.info.name = "root";
    
    let windows = GetAllWnds();
    for window in windows {
        top_node.children.append(ParseNodeRecursive(window));
    }
    
    return top_node;
}

//#pragma mark - NSView 转 NSImage
//+ (NSImage *)imageFromView:(NSView *)cview {
//    // 从view、data、CGImage获取BitmapImageRep
//    //    NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:data];
//    //    NSBitmapImageRep *bitmap = [[[NSBitmapImageRep alloc] initWithCGImage:CGImage];
//    NSBitmapImageRep *bitmap =  [cview bitmapImageRepForCachingDisplayInRect:[cview visibleRect]];
//    [cview cacheDisplayInRect:[cview visibleRect] toBitmapImageRep:bitmap];
//    NSImage *image = [[NSImage alloc] initWithSize:cview.frame.size];
//    [image addRepresentation:bitmap];
//    return image;
//}

func getScreenshotBase64() -> String {
    let windows = GetAllWnds();
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
            
            guard let imageData = Data(base64Encoded: base64String) else {
                return ""
            }
            
            let fileManager = FileManager.default
            let temporaryDirectory = fileManager.temporaryDirectory
            
            do {
                let fileName = UUID().uuidString + ".png"
                let fileURL = temporaryDirectory.appendingPathComponent(fileName)
                try imageData.write(to: fileURL)
                print(fileURL)
            } catch {
                print("Failed to save Base64 as PNG file: \(error)")
                return ""
            }
            
            return base64String
        }
    }
    
    return ""
}

// 模拟鼠标点击
func simulateMouseClick(x: CGFloat, y: CGFloat) {
    let mouseDownEvent = CGEvent(mouseEventSource: nil, mouseType: .leftMouseDown, mouseCursorPosition: CGPoint(x: x, y: y), mouseButton: .left)
    let mouseUpEvent = CGEvent(mouseEventSource: nil, mouseType: .leftMouseUp, mouseCursorPosition: CGPoint(x: x, y: y), mouseButton: .left)
    
    mouseDownEvent?.post(tap: .cghidEventTap)
    mouseUpEvent?.post(tap: .cghidEventTap)
}

// 模拟键盘输入
func simulateKeyboardInput(text: String, commandPressed: Bool) {
    let source = CGEventSource(stateID: .hidSystemState)
    
    if commandPressed {
        let commandDownEvent = CGEvent(keyboardEventSource: source, virtualKey: 0x37, keyDown: true)
        commandDownEvent?.post(tap: .cghidEventTap)
    }
    
    for character in text {
        let keyDownEvent = CGEvent(keyboardEventSource: source, virtualKey: 0, keyDown: true)
        let keyUpEvent = CGEvent(keyboardEventSource: source, virtualKey: 0, keyDown: false)
        
        keyDownEvent?.keyboardSetUnicodeString(stringLength: 1, unicodeString: [UniChar(character.utf16.first!)])
        keyDownEvent?.post(tap: .cghidEventTap)
        keyUpEvent?.post(tap: .cghidEventTap)
    }
    
    if commandPressed {
        let commandUpEvent = CGEvent(keyboardEventSource: source, virtualKey: 0x37, keyDown: false)
        commandUpEvent?.post(tap: .cghidEventTap)
    }
}

// 模拟键盘输入命令+A（选择全部文本）
func selectAllText() {
    let source = CGEventSource(stateID: .hidSystemState)
    
    let commandDownEvent = CGEvent(keyboardEventSource: source, virtualKey: 0x37, keyDown: true)
    commandDownEvent?.post(tap: .cghidEventTap)
    
    let aDownEvent = CGEvent(keyboardEventSource: source, virtualKey: 0, keyDown: true)
    let aUpEvent = CGEvent(keyboardEventSource: source, virtualKey: 0, keyDown: false)
    
    aDownEvent?.keyboardSetUnicodeString(stringLength: 1, unicodeString: [UniChar("a".utf16.first!)])
    aDownEvent?.post(tap: .cghidEventTap)
    aUpEvent?.post(tap: .cghidEventTap)
    
    let commandUpEvent = CGEvent(keyboardEventSource: source, virtualKey: 0x37, keyDown: false)
    commandUpEvent?.post(tap: .cghidEventTap)
}

// 模拟键盘输入Backspace（删除）
func deleteText() {
    let source = CGEventSource(stateID: .hidSystemState)
    
    let deleteDownEvent = CGEvent(keyboardEventSource: source, virtualKey: 0x33, keyDown: true)
    let deleteUpEvent = CGEvent(keyboardEventSource: source, virtualKey: 0x33, keyDown: false)
    
    deleteDownEvent?.post(tap: .cghidEventTap)
    deleteUpEvent?.post(tap: .cghidEventTap)
}
