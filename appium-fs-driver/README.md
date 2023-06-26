# appium-fs-driver

Demo filesystem driver for Appium (not for real use). No, I'm serious, this is only a toy for learning purposes and is basically a backdoor to your file system, so don't ever run it for real. The primary context for its development was for use in a live coding workshop meant to teach how to build drivers for Appium 2.x.

Appium的演示文件系统驱动程序(不是实际使用)。不，我是认真的，这只是一个用于学习的玩具，基本上是文件系统的后门，所以永远不要真正运行它。其开发的主要背景是在一个实时编码研讨会上使用，该研讨会旨在教授如何构建Appium 2.x的驱动程序。

## Build a driver workshop

To see how this driver was built in stages, you can compare the diff between each branch that was created during development:

* [Stage 1](https://github.com/jlipps/appium-fs-driver/compare/workshop/0/start...workshop/1/init): Scaffold the project with all the appropriate tools

使用所有适当的工具为项目搭建脚手架

* [Stage 2](https://github.com/jlipps/appium-fs-driver/compare/workshop/1/init...workshop/2/empty-driver): Get an empty Appium 2.x compatible driver building and running

获得一个空的Appium 2。X兼容驱动程序构建和运行

* [Stage 3](https://github.com/jlipps/appium-fs-driver/compare/workshop/2/empty-driver...workshop/3/page-source): Implement the getPageSource command

实现getPageSource命令

* [Stage 4](https://github.com/jlipps/appium-fs-driver/compare/workshop/3/page-source...workshop/4/find-elements): Implement the element finding commands

实现元素查找命令

* [Stage 5](https://github.com/jlipps/appium-fs-driver/compare/workshop/4/find-elements...workshop/5/get-text): Implement the getText command for file elements

为文件元素实现getText命令

* [Stage 6](https://github.com/jlipps/appium-fs-driver/compare/workshop/5/get-text...workshop/6/send-and-clear): Implement set and clear methods for elements

实现元素的set和clear方法

* [Stage 7](https://github.com/jlipps/appium-fs-driver/compare/workshop/6/send-and-clear...workshop/7/execute): Implement a custom 'executeScript override' method for deleting files

实现一个自定义的` executeScript override `方法来删除文件

* [Stage 8](https://github.com/jlipps/appium-fs-driver/compare/workshop/7/execute...workshop/8/chroot): Require the use of a CLI argument to 'chroot' the use of this driver

需要使用CLI参数来` chroot `使用此驱动程序

And then of course you can diff that against `main` to see if any development has happened since!

当然，你可以将它与`main`进行比较，看看从那以后是否发生了任何开发!
