import path from 'path'
import { fs, util } from '@appium/support'
import { BaseDriver, errors } from '@appium/base-driver'
import { W3C_ELEMENT_KEY } from '@appium/base-driver/build/lib/constants'
import xpath from 'xpath'
import { DOMParser } from 'xmldom'
import log from './logger'
const http = require('http');
const bl = require('bl');

async function httpGet(api) {
  const url = "http://127.0.0.1:4724/" + api;
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
  desiredCapConstraints = { appPath: { presence: true, isString: true } }

  async createSession(...args) {
    const res = await super.createSession(...args)
    // sessionId, caps
    return res
  }

  async deleteSession() {
    // do your own cleanup here
    await super.deleteSession();
  }

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
    //return `<files>\n${this.opts.appPath}</files>`
    // 在 pc 端实现 http server
    const res = await httpGet('getPageSource')
    log.info(`getPageSource ` /*+ res*/);
    return res;
  }

  async findElOrEls(strategy, selector, multiple, context) {
    if (context) {
      // TODO could probably support finding elements from within directories
      throw new errors.NotYetImplementedError('Finding an element from another element not supported')
    }

    log.info(`[PC-Driver] Running XPath query '${selector}' against the current source`)
    const source = await this.getPageSource()
    const xmlDom = new DOMParser().parseFromString(source)
    const nodes = xpath.select(selector, xmlDom)

    if (multiple) {
      return nodes.map(this._xmlNodeToElement.bind(this))
    }

    if (nodes.length < 1) {
      throw new errors.NoSuchElementError()
    }

    return this._xmlNodeToElement(nodes[0])
  }

  _xmlNodeToElement(node) {
    let filePath = null;
    for (const attribute of Object.values(node.attributes)) {
      if (attribute.name === 'path') {
        filePath = attribute.value
        break
      }
    }
    if (!filePath) {
      throw new Error(`Found element had no path attribute`)
    }
    const elementId = util.uuidV4()
    this.elementCache[elementId] = filePath
    return { [W3C_ELEMENT_KEY]: elementId }
  }

  async fileFromElement(elementId) {
    const filePath = this.elementCache[elementId]

    if (!filePath) {
      throw new errors.NoSuchElementError()
    }

    if (!await fs.exists(filePath)) {
      throw new errors.StaleElementReferenceError()
    }

    return filePath
  }

  async getText(elementId) {
    const filePath = await this.fileFromElement(elementId)
    return await fs.readFile(filePath, 'utf8')
  }

  async setValue(text, elementId) {
    const filePath = await this.fileFromElement(elementId)
    return await fs.appendFile(filePath, text)
  }

  async clear(elementId) {
    const filePath = await this.fileFromElement(elementId)
    return await fs.writeFile(filePath, '')
  }

  async execute(script, args) {
    if (script === 'fs: delete') {
      const filePath = await this.fileFromElement(args[0])
      return await fs.unlink(filePath)
    }

    throw new Error(`Don't know how to run script '${script}'`)
  }

}

export { AppiumPCDriver }
export default AppiumPCDriver
