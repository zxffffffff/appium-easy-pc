# appium-pc-driver

## 自定义 Driver 接口

- `getScreenshot` @returns A base64-encoded string representing the PNG image data
```C
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
```

- `getWindowRect` @returns A `Rect` JSON object with x, y, width, and height properties
```json
{
	"x": 1,
	"y": 1,
	"w": 1,
	"h": 1
}
```

- `getPageSource` @returns The UI hierarchy in a platform-appropriate format (e.g., HTML for a web page)
参考 Android 字段
```xml
<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<hierarchy index="0" class="hierarchy" rotation="0" width="1080" height="2029">
  <android.widget.FrameLayout index="0" package="com.android.settings" class="android.widget.FrameLayout" text="" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,0][1080,2029]" displayed="true">
    <android.widget.FrameLayout index="0" package="com.android.settings" class="android.widget.FrameLayout" text="" resource-id="miui:id/action_bar_overlay_layout" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,0][1080,2029]" displayed="true">
      <android.widget.FrameLayout index="0" package="com.android.settings" class="android.widget.FrameLayout" text="" resource-id="miui:id/action_bar_container" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,0][1080,402]" displayed="true">
        <android.widget.FrameLayout index="0" package="com.android.settings" class="android.widget.FrameLayout" text="" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,0][1080,402]" displayed="true" />
        <android.view.ViewGroup index="1" package="com.android.settings" class="android.view.ViewGroup" text="" resource-id="miui:id/action_bar" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,85][1080,402]" displayed="true">
          <android.widget.LinearLayout index="0" package="com.android.settings" class="android.widget.LinearLayout" text="" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[66,85][1014,245]" displayed="true">
            <android.widget.Button index="0" package="com.android.settings" class="android.widget.Button" text="" content-desc="Cancel" resource-id="android:id/button1" checkable="false" checked="false" clickable="true" enabled="true" focusable="true" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[66,115][166,215]" displayed="true" />
            <android.widget.Button index="2" package="com.android.settings" class="android.widget.Button" text="" content-desc="OK" resource-id="android:id/button2" checkable="false" checked="false" clickable="true" enabled="true" focusable="true" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[914,115][1014,215]" displayed="true" />
          </android.widget.LinearLayout>
          <android.widget.FrameLayout index="2" package="com.android.settings" class="android.widget.FrameLayout" text="" resource-id="miui:id/action_bar_movable_container" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,245][1080,402]" displayed="true">
            <android.widget.LinearLayout index="0" package="com.android.settings" class="android.widget.LinearLayout" text="" checkable="false" checked="false" clickable="true" enabled="true" focusable="true" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[74,252][698,369]" displayed="true">
              <android.widget.TextView index="0" package="com.android.settings" class="android.widget.TextView" text="Rename phone" resource-id="miui:id/action_bar_title" checkable="false" checked="false" clickable="false" enabled="true" focusable="true" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[74,252][698,369]" displayed="true" />
            </android.widget.LinearLayout>
          </android.widget.FrameLayout>
        </android.view.ViewGroup>
      </android.widget.FrameLayout>
      <android.widget.FrameLayout index="1" package="com.android.settings" class="android.widget.FrameLayout" text="" resource-id="android:id/content" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
        <android.widget.LinearLayout index="0" package="com.android.settings" class="android.widget.LinearLayout" text="" resource-id="com.android.settings:id/content_parent" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
          <android.widget.FrameLayout index="0" package="com.android.settings" class="android.widget.FrameLayout" text="" resource-id="com.android.settings:id/content_frame" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
            <android.widget.LinearLayout index="0" package="com.android.settings" class="android.widget.LinearLayout" text="" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
              <android.widget.FrameLayout index="0" package="com.android.settings" class="android.widget.FrameLayout" text="" resource-id="com.android.settings:id/main_content" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
                <android.view.ViewGroup index="0" package="com.android.settings" class="android.view.ViewGroup" text="" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
                  <android.widget.ScrollView index="0" package="com.android.settings" class="android.widget.ScrollView" text="" resource-id="com.android.settings:id/scrollview" checkable="false" checked="false" clickable="false" enabled="true" focusable="true" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,2029]" displayed="true">
                    <android.widget.LinearLayout index="0" package="com.android.settings" class="android.widget.LinearLayout" text="" checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,402][1080,725]" displayed="true">
                      <android.widget.EditText index="0" package="com.android.settings" class="android.widget.EditText" text="小风的MI 8 SE" resource-id="com.android.settings:id/device_name" checkable="false" checked="false" clickable="true" enabled="true" focusable="true" focused="true" long-clickable="true" password="false" scrollable="false" selected="false" bounds="[30,440][1050,602]" displayed="true" hint="Enter name" />
                      <android.widget.TextView index="1" package="com.android.settings" class="android.widget.TextView" text="Other nearby devices can see this name." checkable="false" checked="false" clickable="false" enabled="true" focusable="false" focused="false" long-clickable="false" password="false" scrollable="false" selected="false" bounds="[0,602][1080,684]" displayed="true" />
                    </android.widget.LinearLayout>
                  </android.widget.ScrollView>
                </android.view.ViewGroup>
              </android.widget.FrameLayout>
            </android.widget.LinearLayout>
          </android.widget.FrameLayout>
        </android.widget.LinearLayout>
      </android.widget.FrameLayout>
    </android.widget.FrameLayout>
  </android.widget.FrameLayout>
</hierarchy>
```
参考 mac 字段
```c
  NSString *wdAttributeName = [FBElementUtils wdAttributeNameForAttributeName:name];
  if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, frame)]) {
    return self.am_rect;
  } else if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, elementType)]) {
    return [NSString stringWithFormat:@"%lu", self.elementType];
  } else if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, placeholderValue)]) {
    return self.placeholderValue;
  } else if ([wdAttributeName isEqualToString:@"hittable"]) {
    return FBBoolToStr(self.hittable);
  } else if ([wdAttributeName isEqualToString:@"enabled"]) {
    return FBBoolToStr(self.enabled);
  } else if ([wdAttributeName isEqualToString:@"focused"]) {
    return FBBoolToStr(self.am_hasKeyboardInputFocus);
  } else if ([wdAttributeName isEqualToString:@"selected"]) {
    return FBBoolToStr(self.selected);
  } else if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, label)]) {
    return self.label;
  } else if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, title)]) {
    return self.title;
  } else if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, value)]) {
    return [FBElementUtils stringValueWithValue:self.value];
  } else if ([wdAttributeName isEqualToString:FBStringify(XCUIElement, identifier)]) {
    return self.identifier;
  }
```
```xml
<?xml version="1.0" encoding="UTF-8"?>
<XCUIElementTypeApplication elementType="2" identifier="" label="" title="华盛通" enabled="false" selected="false" x="0" y="0" width="0" height="0">
  <XCUIElementTypeWindow elementType="4" identifier="_NS:9" label="" title="" enabled="false" selected="false" x="540" y="215" width="360" height="476">
    <XCUIElementTypeButton elementType="9" identifier="_NS:17" label="" title="Button" enabled="true" selected="false" x="574" y="526" width="45" height="18"/>
    <XCUIElementTypeImage elementType="43" identifier="_NS:29" label="" title="" enabled="true" selected="false" x="625" y="277" width="190" height="72"/>
    <XCUIElementTypeCheckBox elementType="12" identifier="_NS:56" value="1" label="" title="记住密码" enabled="true" selected="false" x="610" y="579" width="70" height="24"/>
    <XCUIElementTypeCheckBox elementType="12" identifier="_NS:67" value="1" label="" title="自动登录" enabled="true" selected="false" x="610" y="603" width="70" height="24"/>
    <XCUIElementTypeCheckBox elementType="12" identifier="_NS:75" value="1" label="" title="忘记密码" enabled="true" selected="false" x="775" y="579" width="55" height="24"/>
    <XCUIElementTypeStaticText elementType="48" identifier="" value="登录" label="" title="" enabled="true" selected="false" x="700" y="526" width="38" height="18"/>
    <XCUIElementTypeCheckBox elementType="12" identifier="_NS:109" value="1" label="" title="注册账号" enabled="true" selected="false" x="775" y="603" width="55" height="24"/>
    <XCUIElementTypeImage elementType="43" identifier="_NS:125" label="" title="" enabled="true" selected="false" x="572" y="403" width="296" height="42"/>
    <XCUIElementTypeImage elementType="43" identifier="_NS:132" label="" title="" enabled="true" selected="false" x="572" y="461" width="296" height="42"/>
    <XCUIElementTypeSecureTextField elementType="50" identifier="_NS:147" value="" label="" title="" placeholderValue="登录密码" enabled="true" selected="false" x="578" y="473" width="276" height="24"/>
    <XCUIElementTypeTextField elementType="49" identifier="_NS:162" value="13545243550" label="" title="" placeholderValue="手机号" enabled="true" selected="false" x="643" y="416" width="204" height="24"/>
    <XCUIElementTypeCheckBox elementType="12" identifier="_NS:175" value="0" label="" title="" enabled="true" selected="false" x="848" y="419" width="12" height="12"/>
    <XCUIElementTypeStaticText elementType="48" identifier="_NS:185" value="+86" label="" title="" enabled="true" selected="false" x="582" y="416" width="27" height="16"/>
    <XCUIElementTypeImage elementType="43" identifier="_NS:192" label="HSLogin more" title="" enabled="true" selected="false" x="616" y="418" width="12" height="12"/>
    <XCUIElementTypeStaticText elementType="48" identifier="" value="" label="" title="" enabled="true" selected="false" x="606" y="408" width="0" height="32"/>
    <XCUIElementTypeStaticText elementType="48" identifier="" value="使用其他方式登录" label="" title="" enabled="true" selected="false" x="665" y="643" width="108" height="16"/>
    <XCUIElementTypeButton elementType="9" identifier="_XCUI:CloseWindow" label="" title="" enabled="true" selected="false" x="547" y="221" width="14" height="16"/>
  </XCUIElementTypeWindow>
  <XCUIElementTypeMenuBar elementType="55" identifier="_NS:19" label="" title="" enabled="true" selected="false" x="0" y="0" width="1440" height="24">
    <XCUIElementTypeMenuBarItem elementType="56" identifier="" label="" title="Apple" enabled="true" selected="false" x="10" y="0" width="33" height="24">
      <XCUIElementTypeMenu elementType="53" identifier="" label="" title="" enabled="true" selected="false" x="10" y="24" width="209" height="275">
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="关于本机" enabled="true" selected="false" x="0" y="0" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="系统信息…" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="22" width="209" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="系统设置…" enabled="true" selected="false" x="0" y="33" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="App Store…" enabled="true" selected="false" x="0" y="55" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="77" width="209" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="最近使用的项目" enabled="true" selected="false" x="0" y="88" width="209" height="22">
          <XCUIElementTypeMenu elementType="53" identifier="" label="" title="" enabled="true" selected="false" x="20891" y="-2664" width="278" height="627">
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="应用程序" enabled="false" selected="false" x="0" y="0" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="安装器.app" enabled="true" selected="false" x="0" y="22" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“安装器.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="归档实用工具.app" enabled="true" selected="false" x="0" y="44" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“归档实用工具.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="活动监视器.app" enabled="true" selected="false" x="0" y="66" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“活动监视器.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="文本编辑.app" enabled="true" selected="false" x="0" y="88" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“文本编辑.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="Appium Inspector.app" enabled="true" selected="false" x="0" y="110" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“Appium Inspector.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="Safari浏览器.app" enabled="true" selected="false" x="0" y="132" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“Safari浏览器.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="System Settings.app" enabled="true" selected="false" x="0" y="154" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“System Settings.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="VBrokers.app" enabled="true" selected="false" x="0" y="176" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“VBrokers.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="WebDriverAgent.app" enabled="true" selected="false" x="0" y="198" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“WebDriverAgent.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="Xcode.app" enabled="true" selected="false" x="0" y="220" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“Xcode.app”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="242" width="278" height="11"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="文稿" enabled="false" selected="false" x="0" y="253" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="FBElementCommands.m" enabled="true" selected="false" x="0" y="275" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“FBElementCommands.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="FBElementUtils.m" enabled="true" selected="false" x="0" y="297" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“FBElementUtils.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="FBFindElementCommands.h" enabled="true" selected="false" x="0" y="319" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“FBFindElementCommands.h”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="FBFindElementCommands.m" enabled="true" selected="false" x="0" y="341" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“FBFindElementCommands.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="FBScreenshotCommands.m" enabled="true" selected="false" x="0" y="363" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“FBScreenshotCommands.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="FBSessionCommands.m" enabled="true" selected="false" x="0" y="385" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“FBSessionCommands.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="main.m" enabled="true" selected="false" x="0" y="407" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“main.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="XCUIApplication+AMActiveElement.h" enabled="true" selected="false" x="0" y="429" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“XCUIApplication+AMActiveElement.h”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="XCUIApplication+AMActiveElement.m" enabled="true" selected="false" x="0" y="451" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“XCUIApplication+AMActiveElement.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="XCUIElement+AMAttributes.m" enabled="true" selected="false" x="0" y="473" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="在访达中显示“XCUIElement+AMAttributes.m”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="495" width="278" height="11"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="服务器" enabled="false" selected="false" x="0" y="506" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="婚纱照1" enabled="true" selected="false" x="0" y="528" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="婚纱照2" enabled="true" selected="false" x="0" y="550" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="婚纱照3" enabled="true" selected="false" x="0" y="572" width="278" height="22"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="594" width="278" height="11"/>
            <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="清除菜单" enabled="true" selected="false" x="0" y="605" width="278" height="22"/>
          </XCUIElementTypeMenu>
        </XCUIElementTypeMenuItem>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="110" width="209" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="强制退出…" enabled="true" selected="false" x="0" y="121" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="强制退出“华盛通”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="143" width="209" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="睡眠" enabled="true" selected="false" x="0" y="154" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="重新启动…" enabled="true" selected="false" x="0" y="176" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="重新启动" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="关机…" enabled="true" selected="false" x="0" y="198" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="关机" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="220" width="209" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="锁定屏幕" enabled="true" selected="false" x="0" y="231" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="退出登录“朱晓枫”…" enabled="true" selected="false" x="0" y="253" width="209" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="退出登录“朱晓枫”" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
      </XCUIElementTypeMenu>
    </XCUIElementTypeMenuBarItem>
    <XCUIElementTypeMenuBarItem elementType="56" identifier="_NS:18" label="" title="华盛通" enabled="true" selected="false" x="43" y="0" width="60" height="24">
      <XCUIElementTypeMenu elementType="53" identifier="_NS:32" label="" title="" enabled="true" selected="false" x="43" y="24" width="154" height="143">
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:37" label="" title="关于华盛通" enabled="true" selected="false" x="0" y="0" width="154" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:42" label="" title="" enabled="false" selected="false" x="0" y="22" width="154" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:50" label="" title="隐藏华盛通" enabled="true" selected="false" x="0" y="33" width="154" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:55" label="" title="隐藏其他" enabled="true" selected="false" x="0" y="55" width="154" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:59" label="" title="" enabled="false" selected="false" x="0" y="77" width="154" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:60" label="" title="切换用户" enabled="true" selected="false" x="0" y="88" width="154" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:64" label="" title="" enabled="false" selected="false" x="0" y="110" width="154" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:65" label="" title="退出华盛通" enabled="true" selected="false" x="0" y="121" width="154" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="NSAlternateQuitMenuItem" label="" title="退出并保留窗口" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
      </XCUIElementTypeMenu>
    </XCUIElementTypeMenuBarItem>
    <XCUIElementTypeMenuBarItem elementType="56" identifier="_NS:117" label="" title="编辑" enabled="true" selected="false" x="103" y="0" width="47" height="24">
      <XCUIElementTypeMenu elementType="53" identifier="_NS:121" label="" title="" enabled="true" selected="false" x="103" y="24" width="157" height="198">
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:126" label="" title="撤销" enabled="false" selected="false" x="0" y="0" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:131" label="" title="重做" enabled="false" selected="false" x="0" y="22" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:136" label="" title="" enabled="false" selected="false" x="0" y="44" width="157" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:137" label="" title="剪切" enabled="false" selected="false" x="0" y="55" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:142" label="" title="复制" enabled="false" selected="false" x="0" y="77" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:147" label="" title="粘贴" enabled="true" selected="false" x="0" y="99" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:152" label="" title="全选" enabled="true" selected="false" x="0" y="121" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="143" width="157" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="startDictation:" label="" title="开始听写…" enabled="true" selected="false" x="0" y="154" width="157" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="orderFrontCharacterPalette:" label="" title="表情与符号" enabled="true" selected="false" x="0" y="176" width="157" height="22"/>
      </XCUIElementTypeMenu>
    </XCUIElementTypeMenuBarItem>
    <XCUIElementTypeMenuBarItem elementType="56" identifier="_NS:238" label="" title="服务" enabled="true" selected="false" x="150" y="0" width="47" height="24">
      <XCUIElementTypeMenu elementType="53" identifier="_NS:242" label="" title="" enabled="true" selected="false" x="150" y="24" width="108" height="143">
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:247" label="" title="存入资金" enabled="false" selected="false" x="0" y="0" width="108" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:251" label="" title="提取资金" enabled="false" selected="false" x="0" y="22" width="108" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:255" label="" title="证期互转" enabled="false" selected="false" x="0" y="44" width="108" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:259" label="" title="统一购买力" enabled="false" selected="false" x="0" y="66" width="108" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:267" label="" title="" enabled="false" selected="false" x="0" y="88" width="108" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:268" label="" title="修改登录密码" enabled="true" selected="false" x="0" y="99" width="108" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:272" label="" title="修改交易密码" enabled="false" selected="false" x="0" y="121" width="108" height="22"/>
      </XCUIElementTypeMenu>
    </XCUIElementTypeMenuBarItem>
    <XCUIElementTypeMenuBarItem elementType="56" identifier="_NS:310" label="" title="窗口" enabled="true" selected="false" x="197" y="0" width="47" height="24">
      <XCUIElementTypeMenu elementType="53" identifier="_NS:314" label="" title="" enabled="true" selected="false" x="197" y="24" width="196" height="286">
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:319" label="" title="关闭窗口" enabled="true" selected="false" x="0" y="0" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="closeAll:" label="" title="全部关闭" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:324" label="" title="缩放" enabled="false" selected="false" x="0" y="22" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="zoomAll:" label="" title="全部缩放" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_zoomLeft:" label="" title="将窗口移到屏幕左侧" enabled="false" selected="false" x="0" y="44" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_zoomRight:" label="" title="将窗口移到屏幕右侧" enabled="false" selected="false" x="0" y="66" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_changeWindowTileLocation:" label="" title="替换拼贴窗口" enabled="false" selected="false" x="0" y="88" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="toggleFullScreen:" label="" title="进入全屏幕" enabled="false" selected="false" x="0" y="110" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="132" width="196" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_removeWindowFromStageManagerSet:" label="" title="从组中移除窗口" enabled="false" selected="false" x="0" y="143" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="" enabled="false" selected="false" x="0" y="165" width="196" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_moveToDisplay:" label="" title="移到“Q24V3W1”" enabled="true" selected="false" x="0" y="176" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:328" label="" title="最小化" enabled="false" selected="false" x="0" y="198" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="miniaturizeAll:" label="" title="全部最小化" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:333" label="" title="" enabled="false" selected="false" x="0" y="220" width="196" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:334" label="" title="主窗口" enabled="true" selected="false" x="0" y="231" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:338" label="" title="" enabled="false" selected="false" x="0" y="253" width="196" height="11"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="_NS:340" label="" title="前置所有窗口" enabled="true" selected="false" x="0" y="264" width="196" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="alternateArrangeInFront:" label="" title="排在前面" enabled="true" selected="false" x="0" y="0" width="0" height="0"/>
      </XCUIElementTypeMenu>
    </XCUIElementTypeMenuBarItem>
    <XCUIElementTypeMenuBarItem elementType="56" identifier="_NS:159" label="" title="帮助" enabled="true" selected="false" x="244" y="0" width="47" height="24">
      <XCUIElementTypeMenu elementType="53" identifier="" label="" title="" enabled="true" selected="false" x="244" y="24" width="95" height="110">
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="检测更新" enabled="true" selected="false" x="0" y="0" width="95" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="快捷键帮助" enabled="true" selected="false" x="0" y="22" width="95" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="使用帮助" enabled="true" selected="false" x="0" y="44" width="95" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="意见反馈" enabled="true" selected="false" x="0" y="66" width="95" height="22"/>
        <XCUIElementTypeMenuItem elementType="54" identifier="" label="" title="免责声明" enabled="true" selected="false" x="0" y="88" width="95" height="22"/>
      </XCUIElementTypeMenu>
    </XCUIElementTypeMenuBarItem>
  </XCUIElementTypeMenuBar>
  <XCUIElementTypeTouchBar elementType="81" identifier="" label="" title="" enabled="false" selected="false" x="80" y="30" width="685" height="30">
    <XCUIElementTypePopUpButton elementType="14" identifier="" label="表情与符号" title="" enabled="true" selected="false" x="81" y="31" width="70" height="28"/>
    <XCUIElementTypeDisclosureTriangle elementType="13" identifier="" value="1" label="" title="候选字栏" enabled="false" selected="false" x="320" y="30" width="15" height="30"/>
  </XCUIElementTypeTouchBar>
</XCUIElementTypeApplication>
```

- `findElOrEls` @returns A single element or list of elements
- https://appium.io/docs/en/2.0/reference/interfaces/appium_types.IFindCommands/#findelorels
- 参数
```js
strategy // xpath
selector // /root/xxx/v_Main/caption/no-name[1]
multiple // true or false
context
```
定义字段 elementId
```json
{
    "element-6066-11e4-a52e-4f735466cecf": "123"
}
```

- `click`

- `getText`

- `setValue`

- `clear`

- `getAttribute` 
- https://appium.io/docs/en/2.0/reference/classes/appium_fake_driver.FakeDriver/#getattribute
