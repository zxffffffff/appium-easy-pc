import pytest

# This sample code uses the Appium python client v2
# pip install Appium-Python-Client
# Then you can paste this into a file and simply run with Python

from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy
from appium.webdriver.common.touch_action import TouchAction

# For W3C actions
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.actions import interaction
from selenium.webdriver.common.actions.action_builder import ActionBuilder
from selenium.webdriver.common.actions.pointer_input import PointerInput


class AppiumElement:
    def __init__(self, driver, ele):
        self.driver = driver
        self.ele = ele

    def get_attribute(self, key: str) -> str:
        return self.ele.get_attribute(key)

    def click(self):
        self.ele.click()

    def rclick(self):
        actions = TouchAction(self.driver)
        actions.long_press(self.ele)
        actions.perform()

    def input(self, text: str):
        self.ele.send_keys(text)


class AppiumDriver:
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

    def find_element_by_path(self, path) -> AppiumElement:
        ele = self.driver.find_element(by=AppiumBy.XPATH, value=path)
        return AppiumElement(self.driver, ele)

    def find_element_by_id(self, id) -> AppiumElement:
        ele = self.driver.find_element(by=AppiumBy.ID, value=id)
        return AppiumElement(self.driver, ele)


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
