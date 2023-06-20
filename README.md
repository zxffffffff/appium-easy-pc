# appium-easy-pc
使用 Electron 封装 Appium 和自定义的 Drivers，简化操作，支持 Win 和 Mac

# 流程记录
- 创建 `react` `easy-pc` 项目
```js
npm install --global create-react-app
create-react-app easy-pc
cd .\easy-pc\
```

- 添加 `electron` 到 `easy-pc`
```js
npm install electron --save-dev
npm install --save-dev @electron-forge/cli
npx electron-forge import
// 文件路径修改 package.json
"homepage": ".",
```

- 添加 `appium`
```js
npm install appium@next
```
