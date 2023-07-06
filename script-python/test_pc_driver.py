import sys
import pytest
import xml.etree.ElementTree as ET

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

# 测试连接
def test_pc_connect(appium_pc_connect):
    print(sys._getframe().f_code.co_name)
    driver = appium_pc_connect
    # do test
    print('driver = ', driver)

# 测试获取页面xml数据
def test_getPageSource(appium_pc_connect):
    print(sys._getframe().f_code.co_name)
    driver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.page_source)
    print('ROOT = ', root.tag, root.attrib)
    for ele in root:
        print('ELE = ', ele.tag, ele.attrib)

# 测试获取element属性
def test_getAttribute(appium_pc_connect):
    print(sys._getframe().f_code.co_name)
    driver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.page_source)
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    find_text = root[0].attrib['text']
    print('find_text = ', find_text)
    # find
    ele = driver.find_element(by=AppiumBy.XPATH, value=find_path)
    print('find_element(find_path) = ', ele)
    text = ele.get_attribute('text')
    print('get_attribute(text) = ', text)
    assert text == find_text

# 测试点击
def test_click(appium_pc_connect):
    print(sys._getframe().f_code.co_name)
    driver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.page_source)
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    ele = driver.find_element(by=AppiumBy.XPATH, value=find_path)
    print('find_element(find_path) = ', ele)
    ele.click()

# 测试右键
def test_r_click(appium_pc_connect):
    print(sys._getframe().f_code.co_name)
    driver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.page_source)
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    ele = driver.find_element(by=AppiumBy.XPATH, value=find_path)
    print('find_element(find_path) = ', ele)
    actions = TouchAction(driver)
    actions.long_press(ele)
    actions.perform()

# 测试输入
def test_r_click(appium_pc_connect):
    print(sys._getframe().f_code.co_name)
    driver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.page_source)
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    ele = driver.find_element(by=AppiumBy.XPATH, value=find_path)
    print('find_element(find_path) = ', ele)
    ele.send_keys("123")
