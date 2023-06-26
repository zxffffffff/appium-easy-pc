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
