import pytest

# This sample code uses the Appium python client v2
# pip install Appium-Python-Client
# Then you can paste this into a file and simply run with Python

from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy

# For W3C actions
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.actions import interaction
from selenium.webdriver.common.actions.action_builder import ActionBuilder
from selenium.webdriver.common.actions.pointer_input import PointerInput


# 连接到 Appium
@pytest.fixture()
def appium_pc_connect():
    print('\n\n--- appium 开始会话 ---')
    caps = {}
    caps["appium:driverName"] = "pc"
    caps["appium:automationName"] = "pc"
    caps["platformName"] = "Windows"
    caps["appium:appPath"] = "todo"
    caps["appium:newCommandTimeout"] = 3600
    caps["appium:connectHardwareKeyboard"] = True
    driver = webdriver.Remote("http://127.0.0.1:4723", caps)
    yield driver

    print('\n--- appium 结束会话 ---')
    driver.quit()
