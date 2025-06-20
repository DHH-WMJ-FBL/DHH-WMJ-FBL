import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 700
    width: 700
    height: 600
    visible: true
    title: "中国象棋"

    minimumWidth: 700
    minimumHeight: 600
    maximumWidth: 700
    maximumHeight: 600
    minimumWidth: 700
    minimumHeight: 600
    maximumWidth: 700
    maximumHeight: 600

    // 基础参数：和棋子QML保持一致
    property int cellWidth: 50
    property int cellHeight: 50
    property int offsetX: 40
    property int offsetY: 40

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
                onPaint: {
                    const ctx = board.getContext("2d")
                    ctx.clearRect(0, 0, width, height)

                    const cols = 9
                    const rows = 10
                    const margin = offsetX
                    const gridWidth = cellWidth * (cols - 1)
                    const gridHeight = cellHeight * (rows - 1)
                    const cols = 9
                    const rows = 10
                    const margin = offsetX
                    const gridWidth = cellWidth * (cols - 1)
                    const gridHeight = cellHeight * (rows - 1)

                    ctx.strokeStyle = "black"
                    ctx.lineWidth = 2
                    ctx.strokeStyle = "black"
                    ctx.lineWidth = 2

                    // 横线
                    for (let r = 0; r < rows; r++) {
                        ctx.beginPath()
                        ctx.moveTo(margin, margin + r * cellHeight)
                        ctx.lineTo(margin + gridWidth, margin + r * cellHeight)
                        ctx.stroke()
                    }
                    // 横线
                    for (let r = 0; r < rows; r++) {
                        ctx.beginPath()
                        ctx.moveTo(margin, margin + r * cellHeight)
                        ctx.lineTo(margin + gridWidth, margin + r * cellHeight)
                        ctx.stroke()
                    }

                    // 竖线
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
                    // 竖线
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
                    // 楚河汉界
                    ctx.font = "24px sans-serif"
                    ctx.fillStyle = "red"
                    ctx.fillText("楚 河", width / 4 - 30, margin + 5 * cellHeight - 10)
                    ctx.fillText("汉 界", width * 3 / 4 - 30, margin + 5 * cellHeight - 10)

                    // 将士斜线
                    function drawDiagonal(fromCol, fromRow, toCol, toRow) {
                        let x1 = margin + fromCol * cellWidth
                        let y1 = margin + fromRow * cellHeight
                        let x2 = margin + toCol * cellWidth
                        let y2 = margin + toRow * cellHeight
                    // 将士斜线
                    function drawDiagonal(fromCol, fromRow, toCol, toRow) {
                        let x1 = margin + fromCol * cellWidth
                        let y1 = margin + fromRow * cellHeight
                        let x2 = margin + toCol * cellWidth
                        let y2 = margin + toRow * cellHeight

                        ctx.beginPath()
                        ctx.moveTo(x1, y1)
                        ctx.lineTo(x2, y2)
                        ctx.stroke()
                    }
                        ctx.beginPath()
                        ctx.moveTo(x1, y1)
                        ctx.lineTo(x2, y2)
                        ctx.stroke()
                    }

                    drawDiagonal(3, 0, 5, 2)
                    drawDiagonal(5, 0, 3, 2)
                    drawDiagonal(3, 9, 5, 7)
                    drawDiagonal(5, 9, 3, 7)
                }
                    drawDiagonal(3, 0, 5, 2)
                    drawDiagonal(5, 0, 3, 2)
                    drawDiagonal(3, 9, 5, 7)
                    drawDiagonal(5, 9, 3, 7)
                }

                Component.onCompleted: board.requestPaint()
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
                id: checkIndicator
                width: 150
                height: 50
                radius: 10
                color: "#80ff0000"  // 半透明红色
                border.color: "red"
                border.width: 2
                anchors.centerIn: parent
                // 在双方回合都显示将军提示，但在游戏结束或自己被将军提示显示时不显示
                visible: controller.isCheck && !controller.gameOver && !controller.selfCheckMove
                
                Text {
                    anchors.centerIn: parent
                    text: controller.isCheckMate ? "绝杀!" : "将军!"
                    font.pixelSize: 24
                    font.bold: true
                    color: "white"
                }
            }
            
            // 自己被将军提示
            Rectangle {
                id: selfCheckIndicator
                width: 150
                height: 50
                radius: 10
                color: "#80ff0000"  // 半透明红色
                border.color: "red"
                border.width: 2
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 20
                }
                visible: controller.selfCheckMove
                
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
                    color: controller.currentPlayer === "红" ? "#ffdddd" : "#dddddd"
                    radius: 5
                    border.color: "gray"
                    border.width: 1

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 2

                        Text {
                            text: "回合: " + controller.roundNumber
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Text {
                            text: "当前: " + controller.currentPlayer + "方"
                            color: controller.currentPlayer === "红" ? "red" : "black"
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }
                        
                        Text {
                            text: controller.isCheck ? (controller.isCheckMate && !controller.gameOver ? "绝杀!" : "将军!") : ""
                            color: "red"
                            font.bold: true
                            visible: controller.isCheck && !controller.gameOver
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }

                // 重新开始按钮
                Button {
                    Layout.fillWidth: true
                    height: 40
                    text: "重新开始"
                    font.bold: true
                    
                    background: Rectangle {
                        color: parent.down ? "#cccccc" : (parent.hovered ? "#dddddd" : "#eeeeee")
                        radius: 5
                        border.color: "gray"
                        border.width: 1
                    }
                    
                    onClicked: {
                        controller.resetGame()
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
                                model: controller.capturedPieces
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
        id: victoryScreen
        anchors.fill: parent
        color: "#80000000"  // 半透明黑色背景
        visible: controller.gameOver
        z: 10

        Rectangle {
            width: 300
            height: 200
            radius: 10
            color: "white"
            border.color: controller.winner === "红" ? "red" : "black"
            border.width: 2
            anchors.centerIn: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15

                Text {
                    text: controller.gameOver && controller.winner && controller.isCheck ? (controller.isCheckMate ? "绝杀!" : "将军!") : "游戏结束"
                    font.pixelSize: 24
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: controller.winner + "方胜利！"
                    font.pixelSize: 20
                    color: controller.winner === "红" ? "red" : "black"
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    text: "再来一局"
                    font.pixelSize: 16
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 40
                    Layout.alignment: Qt.AlignHCenter
                    
                    background: Rectangle {
                        color: parent.down ? "#cccccc" : (parent.hovered ? "#dddddd" : "#eeeeee")
                        radius: 5
                        border.color: "gray"
                        border.width: 1
                    }
                    
                    onClicked: {
                        controller.resetGame()
                    }
                }
            }
        }
    }
}
