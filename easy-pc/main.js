const { app, BrowserWindow } = require('electron')
const srv_start_appium = require('./srv/srv_appium.js')
const path = require('path')
const { env } = require('process')

function createWindow() {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    })

    if (env.EASY_PC_HOT_RELOAD == "1") {
        win.loadURL("http://localhost:3000/")
    }
    else {
        win.loadFile('build/index.html')
    }
}

app.whenReady().then(() => {
    createWindow()

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow()
        }
    })

    // Appium
    srv_start_appium()
        .then(result => {
            console.log("appium start!");
        })
        .catch(error => {
            console.log("appium start error!");
        })
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})
