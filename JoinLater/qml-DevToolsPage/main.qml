// import QtQuick 2.14
// import QtQuick.Window 2.14
// import QtQuick.Controls 2.14
// import QtQuick.Layouts 1.14
// import QtWebEngine 1.10
// 
// Window {
//     id: window
//     width: 1200 // 窗口宽度要能容纳两个视图
//     height: 800
//     visible: true
//     title: "WebEngineView with Toggleable DevTools"
// 
//     // 使用 SplitView 实现左右分栏，并允许拖动调整大小
//     SplitView {
//         id: splitView
//         anchors.fill: parent
//         orientation: Qt.Horizontal
// 
//         // 左侧 WebEngineView：显示网页内容
//         WebEngineView {
//             id: webView
//             objectName: "myWebEngineView"
//             Layout.fillWidth: true // 自动填充可用宽度
//             Layout.minimumWidth: 400
//             url: "https://www.qt.io"
//         }
// 
//         // 右侧 WebEngineView：显示开发者工具
//         WebEngineView {
//             id: devToolsView
//             Layout.fillWidth: true
//             Layout.minimumWidth: 400
//             // 关键：将它绑定到 webView 的 inspectedView 属性
//             inspectedView: webView
//             // 默认隐藏，需要按快捷键才显示
//             visible: false
//         }
//     }
// 
//     // 快捷键：切换开发者工具的可见性
//     Shortcut {
//         sequence: "Ctrl+D"
//         onActivated: {
//             // 切换 devToolsView 的可见性
//             devToolsView.visible = !devToolsView.visible;
//         }
//     }
// }



import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtWebEngine 1.10

// 这是主窗口部分，现在可以在这里使用 devToolsComponent 了
Window {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "WebEngineView with External DevTools"

    WebEngineView {
        id: webView
        anchors.fill: parent
        url: "https://www.baidu.com"
    }
    Component {
        id: devToolsComponent
        Window {
            id: devWindow
            width: 800
            height: 600
            title: "DevTools"
            visible: true

            // 声明一个属性来接收要调试的 WebEngineView 实例
            property WebEngineView inspectedWebView: null

            WebEngineView {
                anchors.fill: parent
                // 直接使用我们传递进来的实例
                inspectedView: devWindow.inspectedWebView
            }
        }
    }
    Shortcut {
        sequence: "Ctrl+D"
        onActivated: {
            var devToolsWindow = devToolsComponent.createObject(window);
            devToolsWindow.inspectedWebView = webView
            devToolsWindow.title=webView.url
            if (devToolsWindow === null) {
                console.log("Failed to create DevTools window.");
            }
        }
    }
}