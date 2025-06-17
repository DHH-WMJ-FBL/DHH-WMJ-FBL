import QtQuick
import QtQuick.Window

Window {
    width: 500
    height: 600
    visible: true
    title: "中国象棋棋盘"

    Canvas {
        id: board
        anchors.fill: parent
        onPaint: {
            var ctx = board.getContext("2d")
            ctx.clearRect(0, 0, width, height)

            var cols = 9
            var rows = 10
            var margin = 40
            var gridWidth = width - 2 * margin
            var gridHeight = height - 2 * margin
            var cellWidth = gridWidth / (cols - 1)
            var cellHeight = gridHeight / (rows - 1)

            ctx.strokeStyle = "black"
            ctx.lineWidth = 2

            //画出棋盘
            for (var r = 0; r < rows; r++) {
                ctx.beginPath()
                ctx.moveTo(margin, margin + r * cellHeight)
                ctx.lineTo(width - margin, margin + r * cellHeight)
                ctx.stroke()
            }

            for (var c = 0; c < cols; c++) {
                ctx.beginPath()
                ctx.moveTo(margin + c * cellWidth, margin)
                if (c === 0 || c === cols - 1) {
                    ctx.lineTo(margin + c * cellWidth, height - margin)
                } else {
                    ctx.lineTo(margin + c * cellWidth, margin + 4 * cellHeight)
                    ctx.moveTo(margin + c * cellWidth, margin + 5 * cellHeight)
                    ctx.lineTo(margin + c * cellWidth, height - margin)
                }
                ctx.stroke()
            }

            //写出“楚河汉界”
            ctx.font = "20px sans-serif"
            ctx.fillStyle = "red"
            ctx.fillText("楚 河", width / 4 - 20, margin + 5 * cellHeight - 10)
            ctx.fillText("汉 界", 3 * width / 4 - 20, margin + 5 * cellHeight - 10)

            // 画“士”的可移动路径（斜线）
            function drawDiagonal(fromCol, fromRow, toCol, toRow) {
                var x1 = margin + fromCol * cellWidth
                var y1 = margin + fromRow * cellHeight
                var x2 = margin + toCol * cellWidth
                var y2 = margin + toRow * cellHeight

                ctx.beginPath()
                ctx.strokeStyle = "black"
                ctx.lineWidth = 2
                ctx.moveTo(x1, y1)
                ctx.lineTo(x2, y2)
                ctx.stroke()
            }

            // 上方士的斜线
            drawDiagonal(3, 0, 5, 2)
            drawDiagonal(5, 0, 3, 2)
            // 下方士的斜线
            drawDiagonal(3, 9, 5, 7)
            drawDiagonal(5, 9, 3, 7)
        }

        Component.onCompleted: board.requestPaint()
    }
}
