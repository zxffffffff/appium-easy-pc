const { app, BrowserWindow } = require('electron')
const path = require('path')
const argv = require('minimist')(process.argv.slice(2));

console.log(argv)

function createWindow() {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    })

    if (argv._.includes('hot-reload')) {
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
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})
