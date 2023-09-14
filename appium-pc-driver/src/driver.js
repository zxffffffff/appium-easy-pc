import path from 'path'
import { fs, util } from '@appium/support'
import { BaseDriver, errors } from '@appium/base-driver'
import { W3C_ELEMENT_KEY } from '@appium/base-driver/build/lib/constants'
import xpath from 'xpath'
import { DOMParser } from 'xmldom'
import log from './logger'
const http = require('http');
const bl = require('bl');

async function httpGet(api, map_args) {
  var args = "";
  if (map_args) {
    for (const [key, value] of map_args.entries()) {
      if (args.length == 0) {
        args += "?";
      } else {
        args += "&";
      }
      args += key + "=" + value;
    }
  }
  const url = "http://127.0.0.1:4724/" + api + args;
  return new Promise((resolve, reject) => {
    http.get(url, response => {
      response.setEncoding('utf8');
      response.pipe(bl((err, data) => {
        if (err) {
          reject(err);
        }
        resolve(data.toString());
      }));
    }).on('error', (err) => {
      reject(err);
    });
  }).catch((err) => {
    if (err.code === 'ECONNRESET') {
      // 处理 socket hang up 错误
      reject(err);
    } else {
      // 处理其他错误
      reject(err);
    }
  });
}

class AppiumPCDriver extends BaseDriver {

  locatorStrategies = [
    'id',
    'xpath',
    'name',
  ];
  elementCache = {}
  desiredCapConstraints = { appPath: { presence: true, isString: true } }

  async createSession(...args) {
    const res = await super.createSession(...args)
    // sessionId, caps
    log.info(`appPath = '${this.opts.appPath}'`)
    return res
  }

  async deleteSession() {
    // do your own cleanup here
    await super.deleteSession();
  }

  // 在 pc 端实现 http server
  // throw new errors.NotYetImplementedError('Finding an element from another element not supported')

  async getScreenshot() {
    const res = await httpGet('getScreenshot')
    return res;
  }

  async getWindowRect() {
    const res = await httpGet('getWindowRect')
    const obj = JSON.parse(res);
    return obj;
  }

  async getPageSource() {
    const res = await httpGet('getPageSource')
    return res;
  }

  async findElOrEls(strategy, selector, multiple, context) {
    const res = await httpGet('findElOrEls', new Map([
      ["strategy", strategy ?? ""],
      ["selector", selector ?? ""],
      ["multiple", multiple ?? ""],
      ["context", context ?? ""]
    ]));
    if (res.length < 1) {
      throw new errors.NoSuchElementError()
    }
    const obj = JSON.parse(res);
    return obj;
  }

  async findElements(strategy, selector) {
    return await this.findElOrEls(strategy, selector, true, "");
  }

  async findElement(strategy, selector) {
    return await this.findElOrEls(strategy, selector, false, "");
  }

  async click(elementId) {
    const res = await httpGet('click', new Map([["elementId", elementId ?? ""]]));
  }

  async performTouch(args) {
    args.forEach(async obj => {
      const { action, options: { element, duration, x, y, count } } = obj;
      const res = await httpGet('performTouch', new Map([
        ["action", action ?? ""],
        ["element", element ?? ""],
        ["duration", duration ?? ""],
        ["x", x ?? ""],
        ["y", y ?? ""],
        ["count", count ?? ""],
      ]));
    });
  }

  async touchLongClick(elementId) {
    const res = await httpGet('touchLongClick', new Map([["elementId", elementId ?? ""]]));
  }

  async getText(elementId) {
    const res = await httpGet('getText', new Map([["elementId", elementId ?? ""]]));
    return res;
  }

  async setValue(text, elementId) {
    if (Array.isArray(text)) {
      // 字符串数组 -> 字符串
      text = text.join('');
    }
    const res = await httpGet('setValue', new Map([
      ["text", text ?? ""],
      ["elementId", elementId ?? ""]
    ]));
  }

  async clear(elementId) {
    const res = await httpGet('clear', new Map([["elementId", elementId ?? ""]]));
  }

  async getAttribute(name, elementId) {
    const res = await httpGet('getAttribute', new Map([
      ["name", name ?? ""],
      ["elementId", elementId ?? ""]
    ]));
    return res
  }
}

export { AppiumPCDriver }
export default AppiumPCDriver
