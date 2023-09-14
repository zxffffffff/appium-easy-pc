import sys
import pytest
import xml.etree.ElementTree as ET

from conftest import AppiumDriver, AppiumElement, Common


def test_pc_connect(appium_pc_connect):
    """
    测试连接
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    print('driver = ', driver)


def test_getPageSource(appium_pc_connect):
    """
    测试获取页面xml数据
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.get_page_source())
    print('ROOT = ', root.tag, root.attrib)
    for ele in root:
        print('ELE = ', ele.tag, ele.attrib)


def test_getScreenshot(appium_pc_connect):
    """
    测试OCR识别截图
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    image = driver.getScreenshot()
    text = Common.OCR(image)
    assert (len(text))


def test_getAttribute(appium_pc_connect):
    """
    测试获取element属性
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.get_page_source())
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    find_text = root[0].attrib['text']
    print('find_text = ', find_text)
    # find
    ele: AppiumElement = driver.find_element_by_path(find_path)
    print('find_element_by_path(find_path) = ', ele)
    text = ele.get_attribute('text')
    print('get_attribute(text) = ', text)
    assert text == find_text


def test_click(appium_pc_connect):
    """
    测试点击
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.get_page_source())
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    ele: AppiumElement = driver.find_element_by_path(find_path)
    print('find_element_by_path(find_path) = ', ele)
    ele.click(15, 15, 2)


def test_r_click(appium_pc_connect):
    """
    测试右键
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.get_page_source())
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    ele: AppiumElement = driver.find_element_by_path(find_path)
    print('find_element_by_path(find_path) = ', ele)
    ele.rclick()


def test_input(appium_pc_connect):
    """
    测试输入
    """
    print(sys._getframe().f_code.co_name)
    driver: AppiumDriver = appium_pc_connect
    # do test
    root = ET.fromstring(driver.get_page_source())
    find_path = "/{}/{}".format(root.tag, root[0].tag)
    print('find_path = ', find_path)
    ele: AppiumElement = driver.find_element_by_path(find_path)
    print('find_element_by_path(find_path) = ', ele)
    ele.input("123")
    ele.input_enter()
