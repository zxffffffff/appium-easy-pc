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

class AppiumDriver:
    driver

    def __init__(self):
        print("AppiumDriver __init__")
        caps = {}
        caps["appium:driverName"] = "pc"
        caps["appium:automationName"] = "pc"
        caps["platformName"] = "Windows"
        caps["appium:newCommandTimeout"] = 3600
        caps["appium:connectHardwareKeyboard"] = True

        self.driver = webdriver.Remote("http://127.0.0.1:4723", caps)

    def __del__(self):
        print("AppiumDriver __del__")
        self.driver.quit()
