from datetime import datetime
import os
import time
import pytest
from PIL import Image
from io import BytesIO

# for OCR
# import pytesseract  # Tesseract-OCR
from paddleocr import PaddleOCR  # PaddleOCR

# This sample code uses the Appium python client v2
# pip install Appium-Python-Client
# Then you can paste this into a file and simply run with Python
from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy
from appium.webdriver.common.touch_action import TouchAction
from selenium.webdriver.common.keys import Keys

# For W3C actions
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.actions import interaction
from selenium.webdriver.common.actions.action_builder import ActionBuilder
from selenium.webdriver.common.actions.pointer_input import PointerInput

dir_path = os.path.dirname(os.path.abspath(__file__))


class AppiumElement:
    driver: webdriver.Remote = None
    ele: webdriver.WebElement = None

    def __init__(self, driver, ele):
        self.driver = driver
        self.ele = ele

    def get_attribute(self, key: str) -> str:
        return self.ele.get_attribute(key)

    def getScreenshot(self) -> Image:
        screenshot = self.driver.get_screenshot_as_png()
        attr = self.get_attribute("ltrb")
        ltrb = attr.split(',')
        return Image.open(BytesIO(screenshot)).crop((int(ltrb[0]), int(ltrb[1]), int(ltrb[2]), int(ltrb[3])))

    def click(self, x: int = None, y: int = None, count: int = 1):
        actions = TouchAction(self.driver)
        actions.tap(self.ele, x, y, count)
        actions.perform()

    def rclick(self):
        actions = TouchAction(self.driver)
        actions.long_press(self.ele)
        actions.perform()

    def input(self, text: str, click=True):
        if (click):
            self.click()
        self.ele.send_keys(text)

    def input_backspace(self, click=False):
        if (click):
            self.click()
        self.ele.send_keys(Keys.BACKSPACE)

    def input_enter(self, click=False):
        if (click):
            self.click()
        self.ele.send_keys(Keys.ENTER)

    def input_escape(self, click=False):
        if (click):
            self.click()
        self.ele.send_keys(Keys.ESCAPE)

    def input_space(self, click=False):
        if (click):
            self.click()
        self.ele.send_keys(Keys.SPACE)

    def input_delete(self, click=False):
        if (click):
            self.click()
        self.ele.send_keys(Keys.DELETE)


class AppiumDriver:
    driver: webdriver.Remote = None

    def __init__(self):
        caps = {}
        caps["appium:driverName"] = "pc"
        caps["appium:automationName"] = "pc"
        caps["platformName"] = "Windows"
        caps["appium:appPath"] = "todo"
        caps["appium:newCommandTimeout"] = 3600
        caps["appium:connectHardwareKeyboard"] = True
        self.driver = webdriver.Remote("http://127.0.0.1:4723", caps)

    def quit(self):
        self.driver.quit()

    def get_page_source(self) -> str:
        return self.driver.page_source

    def getScreenshot(self) -> Image:
        screenshot = self.driver.get_screenshot_as_png()
        return Image.open(BytesIO(screenshot))

    def find_element_by_path(self, path, wait_sec: float = 3) -> AppiumElement:
        start_time = time.time()
        while True:
            try:
                ele = self.driver.find_element(by=AppiumBy.XPATH, value=path)
                return AppiumElement(self.driver, ele)
            except:
                elapsed_time = time.time() - start_time
                if elapsed_time >= wait_sec:
                    break
        return None

    def find_element_by_id(self, id, wait_sec: float = 3) -> AppiumElement:
        start_time = time.time()
        while True:
            try:
                ele = self.driver.find_element(by=AppiumBy.ID, value=id)
                return AppiumElement(self.driver, ele)
            except:
                elapsed_time = time.time() - start_time
                if elapsed_time >= wait_sec:
                    break
        return None


def singleton(cls):
    """
    单例装饰器
    """
    instances = {}

    def wrapper(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]

    return wrapper


# @singleton
# class TesseractOCR_singleton:
#     """
#     安装 Tesseract-OCR 可执行程序并添加到 PATH
#     安装 pytesseract 库
#     模型路径 C:/Program Files/Tesseract-OCR/tessdata/*.traineddata

#     图片分割模式（PSM） tesseract有13种图片分割模式（page segmentation mode，psm）：
#     0 — Orientation and script detection (OSD) only. 方向及语言检测（Orientation and script detection，OSD)
#     1 — Automatic page segmentation with OSD. 自动图片分割
#     2 — Automatic page segmentation, but no OSD, or OCR. 自动图片分割，没有OSD和OCR
#     3 — Fully automatic page segmentation, but no OSD. (Default) 完全的自动图片分割，没有OSD
#     4 — Assume a single column of text of variable sizes. 假设有一列不同大小的文本
#     5 — Assume a single uniform block of vertically aligned text. 假设有一个垂直对齐的文本块
#     6 — Assume a single uniform block of text. 假设有一个对齐的文本块（推荐，用于多行文本且字体相同）
#     7 — Treat the image as a single text line. 图片为单行文本（推荐，用于单行文本）
#     8 — Treat the image as a single word. 图片为单词
#     9 — Treat the image as a single word in a circle. 图片为圆形的单词
#     10 — Treat the image as a single character. 图片为单个字符
#     11 — Sparse text. Find as much text as possible in no particular order. 稀疏文本。查找尽可能多的文本，没有特定的顺序。
#     12 — Sparse text with OSD. OSD稀疏文本
#     13 — Raw line. Treat the image as a single text line, bypassing hacks that are Tesseract-specific. 原始行。将图像视为单个文本行。

#     OCR引擎模式（OEM） 有4种OCR引擎模式：
#     0 — Legacy engine only.仅旧版引擎（3.x以前）。
#     1 — Neural nets LSTM engine only.仅神经网络 LSTM 引擎
#     2 — Legacy + LSTM engines.混合模式（传统 + LSTM 引擎）
#     3 — Default, based on what is available.默认，基于可用的内容
#     """

#     def __init__(self):
#         # 已安装的语言包列表
#         self.list = pytesseract.get_languages(config='')

#     def OCR(image: Image, whitelist=None) -> list:
#         # 转换成灰度图像
#         gray_image = image.convert('L')

#         # 字符白名单（不支持中文）
#         config = '--oem 1 --psm 6'
#         if (whitelist != None):
#             config = config + ' -c tessedit_char_whitelist=' + whitelist
#         result: str = pytesseract.image_to_string(
#             gray_image, lang='chi_sim', config=config)
#         return result.replace(' ', '').splitlines()


@singleton
class PaddleOCR_singleton:
    """
    安装 PaddleOCR 库
    版本选择 https://www.paddlepaddle.org.cn/install/quick?docurl=/documentation/docs/zh/install/pip/windows-pip.html
    """

    def __init__(self):
        # print(paddle.__version__)
        # paddle.utils.run_check()
        self.ocr: PaddleOCR = PaddleOCR(
            use_gpu=False, use_angle_cls=True, lang="ch")

    def OCR_file(self, file: str) -> list:
        result = self.ocr.ocr(file)
        text = ""
        for idx in range(len(result)):
            res = result[idx]
            # p1, p2, p3, p4 = res[0]
            s, percent = res[1]
            text = text + s + '\n'
        return text.splitlines()

    def OCR(self, image: Image) -> list:
        now = datetime.now()
        formatted_time = now.strftime(
            "%Y-%m-%d_%H-%M-%S") + "_" + str(now.microsecond // 1000)
        file = os.path.abspath(dir_path + "/temp_" + formatted_time + ".png")
        image.save(file)
        ret = self.OCR_file(file)
        os.remove(file)
        return ret


class Common:
    def OCR(image: Image) -> str:
        arr = PaddleOCR_singleton().OCR(image)
        return '\n'.join(arr)

    def price_to_float(price: str) -> tuple[float, float]:
        try:
            # 移除千分位','
            price = price.replace(',', '')
            # 处理单位 K,M,B,T
            unit = price[-1]
            if unit == 'K':
                return float(price[:-1]), 1000
            elif unit == 'M':
                return float(price[:-1]), 1000 * 1000
            elif unit == 'B':
                return float(price[:-1]), 1000 * 1000 * 1000
            elif unit == 'T':
                return float(price[:-1]), 1000 * 1000 * 1000 * 1000
            elif unit == '万':
                return float(price[:-1]), 10000
            elif unit == '亿':
                return float(price[:-1]), 10000 * 10000
            elif price[-2] == '万亿':
                return float(price[:-2]), 10000 * 10000 * 10000
            return float(price), 1
        except:
            return 0, 0


@pytest.fixture()
def appium_pc_connect() -> AppiumDriver:
    """
    所有测试用例的入口：

    def test_xxx(appium_pc_connect):
        driver: AppiumDriver = appium_pc_connect
        ele: AppiumElement = driver.find_element_by_path(find_path)
        ele.click()
    """
    driver = AppiumDriver()
    print('\n\n--- appium 开始会话 ---')
    yield driver
    print('\n--- appium 结束会话 ---')
    driver.quit()
