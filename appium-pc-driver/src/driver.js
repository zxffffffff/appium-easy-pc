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
    });
  });
}

class AppiumPCDriver extends BaseDriver {

  locatorStrategies = ['xpath']
  elementCache = {}
  // 参数：desiredCapConstraints = { appPath: { presence: true, isString: true } }

  async createSession(...args) {
    const res = await super.createSession(...args)
    // sessionId, caps
    return res
  }

  async deleteSession() {
    // do your own cleanup here
    await super.deleteSession();
  }

  // 在 pc 端实现 http server
  async getScreenshot() {
    const res = await httpGet('getScreenshot')
    log.info(`getScreenshot ` /*+ res*/);
    return res;
  }

  async getWindowRect() {
    const res = await httpGet('getWindowRect')
    log.info(`getWindowRect ` + res);
    const obj = JSON.parse(res);
    return obj;
  }

  async getPageSource() {
    // 参数：`${this.opts.appPath}`
    const res = await httpGet('getPageSource')
    log.info(`getPageSource ` /*+ res*/);
    return res;
  }

  async findElOrEls(strategy, selector, multiple, context) {
    if (context) {
      // TODO could probably support finding elements from within directories
      throw new errors.NotYetImplementedError('Finding an element from another element not supported')
    }

    const res = await httpGet('findElOrEls', new Map([
      ["strategy", strategy ?? ""],
      ["selector", selector ?? ""],
      ["multiple", multiple ?? ""],
      ["context", context ?? ""]
    ]));
    log.info(`findElOrEls ` + res);
    if (res.length < 1) {
      throw new errors.NoSuchElementError()
    }
    const obj = JSON.parse(res);
    return obj;
  }

  async click(elementId) {
    const res = await httpGet('click', new Map([["elementId", elementId ?? ""]]));
    log.info(`click ` + res);
  }
  
  async touchLongClick(elementId) {
    const res = await httpGet('touchLongClick', new Map([["elementId", elementId ?? ""]]));
    log.info(`touchLongClick ` + res);
  }
  
  async getText(elementId) {
    const res = await httpGet('getText', new Map([["elementId", elementId ?? ""]]));
    log.info(`getText ` + res);
    return res;
  }

  async setValue(text, elementId) {
    const res = await httpGet('setValue', new Map([
      ["text", text ?? ""],
      ["elementId", elementId ?? ""]
    ]));
    log.info(`setValue ` + res);
  }

  async clear(elementId) {
    const res = await httpGet('clear', new Map([["elementId", elementId ?? ""]]));
    log.info(`clear ` + res);
  }
}

export { AppiumPCDriver }
export default AppiumPCDriver
