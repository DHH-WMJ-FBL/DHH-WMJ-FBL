import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 700
    height: 600
    visible: true
    title: "中国象棋"

    minimumWidth: 700
    minimumHeight: 600
    maximumWidth: 700
    maximumHeight: 600

    // 基础参数：和棋子QML保持一致
    property int cellWidth: 50
    property int cellHeight: 50
    property int offsetX: 40
    property int offsetY: 40

    // 通用按钮组件
    component CustomButton: Rectangle {
        property string text: ""
        property var onClicked: function() {}
        
        Layout.fillWidth: true
        height: 40
        color: tapHandler.pressed ? "#cccccc" : (hoverHandler.hovered ? "#dddddd" : "#000000")
        radius: 5
        border.color: "gray"
        border.width: 1

        Text {
            text: parent.text
            font.bold: true
            color: "white"
            anchors.centerIn: parent
        }

        TapHandler {
            id: tapHandler
            onTapped: parent.onClicked()
        }

        HoverHandler {
            id: hoverHandler
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10

        // 左侧棋盘区域
        Item {
            Layout.preferredWidth: 500
            Layout.fillHeight: true

            Canvas {
                id: board
                anchors.fill: parent
                z: -1

                onPaint: {
                    const ctx = board.getContext("2d")
                    ctx.clearRect(0, 0, width, height)

                    const cols = 9, rows = 10, margin = offsetX
                    const gridWidth = cellWidth * (cols - 1)
                    const gridHeight = cellHeight * (rows - 1)

                    ctx.strokeStyle = "black"
                    ctx.lineWidth = 2

                    // 绘制棋盘线
                    for (let r = 0; r < rows; r++) {
                        ctx.beginPath()
                        ctx.moveTo(margin, margin + r * cellHeight)
                        ctx.lineTo(margin + gridWidth, margin + r * cellHeight)
                        ctx.stroke()
                    }

                    for (let c = 0; c < cols; c++) {
                        ctx.beginPath()
                        ctx.moveTo(margin + c * cellWidth, margin)
                        if (c === 0 || c === cols - 1) {
                            ctx.lineTo(margin + c * cellWidth, margin + gridHeight)
                        } else {
                            ctx.lineTo(margin + c * cellWidth, margin + 4 * cellHeight)
                            ctx.moveTo(margin + c * cellWidth, margin + 5 * cellHeight)
                            ctx.lineTo(margin + c * cellWidth, margin + gridHeight)
                        }
                        ctx.stroke()
                    }

                    // 楚河汉界
                    ctx.font = "24px sans-serif"
                    ctx.fillStyle = "red"
                    ctx.fillText("楚 河", width / 4 - 30, margin + 5 * cellHeight - 10)
                    ctx.fillText("汉 界", width * 3 / 4 - 30, margin + 5 * cellHeight - 10)

                    // 将士斜线
                    function drawDiagonal(fromCol, fromRow, toCol, toRow) {
                        ctx.beginPath()
                        ctx.moveTo(margin + fromCol * cellWidth, margin + fromRow * cellHeight)
                        ctx.lineTo(margin + toCol * cellWidth, margin + toRow * cellHeight)
                        ctx.stroke()
                    }

                    drawDiagonal(3, 0, 5, 2)
                    drawDiagonal(5, 0, 3, 2)
                    drawDiagonal(3, 9, 5, 7)
                    drawDiagonal(5, 9, 3, 7)
                }

                Component.onCompleted: board.requestPaint()
            }

            Loader {
                id: chessLoader
                anchors.fill: parent
                source: "chessman.qml"
                z: 1
            }
            
            // 将军提示
            Rectangle {
                width: 150
                height: 50
                radius: 10
                color: "#80ff0000"
                border.color: "red"
                border.width: 2
                anchors.centerIn: parent
                visible: controller && controller.isCheck && !controller.gameOver && !controller.selfCheckMove
                
                Text {
                    anchors.centerIn: parent
                    text: (controller && controller.isCheckMate) ? "绝杀!" : "将军!"
                    font.pixelSize: 24
                    font.bold: true
                    color: "white"
                }
            }
            
            // 自己被将军提示
            Rectangle {
                width: 150
                height: 50
                radius: 10
                color: "#80ff0000"
                border.color: "red"
                border.width: 2
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
                visible: controller && controller.selfCheckMove
                
                Text {
                    anchors.centerIn: parent
                    text: "将军!"
                    font.pixelSize: 24
                    font.bold: true
                    color: "white"
                }
            }
        }

        // 右侧信息面板
        Rectangle {
            Layout.preferredWidth: 180
            Layout.fillHeight: true
            border.color: "gray"
            border.width: 1
            radius: 5
            clip: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    height: 50
                    color: (controller && controller.currentPlayer === "红") ? "#ffdddd" : "#dddddd"
                    radius: 5
                    border.color: "gray"
                    border.width: 1

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 2

                        Text {
                            text: "回合: " + (controller ? controller.roundNumber : "0")
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Text {
                            text: "当前: " + (controller ? controller.currentPlayer : "红") + "方"
                            color: (controller && controller.currentPlayer === "红") ? "red" : "black"
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }
                        
                        Text {
                            text: (controller && controller.isCheck) ? ((controller.isCheckMate && !controller.gameOver) ? "绝杀!" : "将军!") : ""
                            color: "red"
                            font.bold: true
                            visible: controller && controller.isCheck && !controller.gameOver
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }

                CustomButton {
                    text: (controller && controller.isAiMode) ? "切换双人模式" : "切换人机模式"
                    onClicked: function() {
                        if (controller) {
                            controller.toggleAIMode()
                            console.log("切换为 " + (controller.isAiMode ? "人机对战" : "双人对战"))
                        }
                    }
                }

                CustomButton {
                    text: "重新开始"
                    onClicked: function() {
                        if (controller) controller.resetGame()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "white"
                    radius: 5
                    border.color: "gray"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: 5

                        Text {
                            text: "吃子记录"
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            
                            ListView {
                                model: controller ? controller.capturedPieces : []
                                spacing: 5
                                delegate: Rectangle {
                                    width: parent.width
                                    height: 40
                                    color: modelData.color === "红" ? "#ffeeee" : "#eeeeee"
                                    radius: 3
                                    border.color: "lightgray"
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 5
                                        spacing: 5
                                        
                                        Image {
                                            source: "qrc:/image/" + modelData.icon
                                            width: 30
                                            height: 30
                                            Layout.preferredWidth: 30
                                            Layout.preferredHeight: 30
                                        }
                                        
                                        Column {
                                            Layout.fillWidth: true
                                            spacing: 2
                                            
                                            Text {
                                                text: "回合 " + modelData.round + ": " + modelData.name
                                                font.pixelSize: 12
                                            }
                                            
                                            Text {
                                                text: "被 " + modelData.capturedBy + " 吃掉"
                                                font.pixelSize: 10
                                                color: "gray"
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 胜利界面
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        visible: controller && controller.gameOver
        z: 10

        Rectangle {
            width: 300
            height: 200
            radius: 10
            color: "white"
            border.color: (controller && controller.winner === "红") ? "red" : "black"
            border.width: 2
            anchors.centerIn: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15

                Text {
                    text: (controller && controller.gameOver && controller.winner && controller.isCheck) ? (controller.isCheckMate ? "绝杀!" : "将军!") : "游戏结束"
                    font.pixelSize: 24
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: (controller ? controller.winner : "红") + "方胜利！"
                    font.pixelSize: 20
                    color: (controller && controller.winner === "红") ? "red" : "black"
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Rectangle {
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 40
                    Layout.alignment: Qt.AlignHCenter
                    color: tapHandler.pressed ? "#cccccc" : (hoverHandler.hovered ? "#dddddd" : "#eeeeee")
                    radius: 5
                    border.color: "gray"
                    border.width: 1

                    Text {
                        text: "再来一局"
                        font.pixelSize: 16
                        color: "black"
                        anchors.centerIn: parent
                    }
                    
                    TapHandler {
                        id: tapHandler
                        onTapped: {
                            if (controller) controller.resetGame()
                        }
                    }

                    HoverHandler {
                        id: hoverHandler
                    }
                }
            }
        }
    }
}
