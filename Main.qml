import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 500
    height: 600
    visible: true
    title: "中国象棋"

       minimumWidth: 500
       minimumHeight: 600
       maximumWidth: 500
       maximumHeight: 600

    // 基础参数：和棋子QML保持一致
    property int cellWidth: 50
    property int cellHeight: 50
    property int offsetX: 40
    property int offsetY: 40

    Canvas {
        id: board
        anchors.fill: parent
        z: -1

        onPaint: {
            const ctx = board.getContext("2d")
            ctx.clearRect(0, 0, width, height)

            const cols = 9
            const rows = 10
            const margin = offsetX
            const gridWidth = cellWidth * (cols - 1)
            const gridHeight = cellHeight * (rows - 1)

            ctx.strokeStyle = "black"
            ctx.lineWidth = 2

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

        Component.onCompleted: board.requestPaint()
    }

    Loader {
        id: chessLoader
        anchors.fill: parent
        source: "chessman.qml"
        z: 1
    }
}
