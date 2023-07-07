# start-appium
自定义的 Appium PC Driver，需要 Win 和 Mac 开启 Http Server

![image](https://github.com/zxffffffff/start-appium/blob/main/doc/architecture.png)

# appium 环境记录
- 安装 `node.js` 环境
```js
npm install -g typescript
npm install -g make-dir-cli rimraf cpr // win
npm install --save
```

- 添加 `appium`
```js
npm install appium@next
```

- 添加 `driver`
```js
npm install ./appium-pc-driver
npm install ./appium-mac2-driver
```

# driver 流程记录

## `appium-pc-driver`
- 模仿 fs-driver 实现最基础的 api
- 参考 `appium-pc-driver\README.md`

## `lib-win` `lib-mac`
- pc 端创建 http server
- 利用 win32/mac 接口实现截图、鼠标、键盘等交互
```json
{
  "appium:driverName": "pc",
  "appium:automationName": "pc",
  "platformName": "Windows",
  "appPath": "C:\\Program Files\\Mozilla Firefox\\firefox.exe"
}
```
# python 脚本记录

## `pytest`
```py
pip install Appium-Python-Client pytest
pytest
```
