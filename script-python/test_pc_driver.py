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

from common.utils import appium_close, appium_open


def test_appium_connect():
    driver = appium_open()
    # do test

def test_click():
    driver = appium_open()
    # do test


    appium_close(driver)

def test_get_attribute():
    driver = appium_open()
    # do test
    el1 = driver.find_element(by=AppiumBy.XPATH, value="/root/xxx")
    print(el1)
    tit = el1.get_attribute('text')
    print(tit)
    assert tit == "xxx"
    # finished
    appium_close(driver)
