# start-appium
自定义的 Appium PC Driver，需要 Win 和 Mac 开启 Http Server

# appium 环境记录
- 创建 `node.js` 项目
```js
npm init
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

# python 脚本记录

## `pytest`
```py
pip install Appium-Python-Client pytest
pytest
```
