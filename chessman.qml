import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    property var chessData: []
    property string currentPlayer: "红"
    property bool gameOver: false
    property bool isCheck: false
    property bool isCheckMate: false
    property int selectedIndex: -1

    signal requestMove(int fromIndex, int toX, int toY)

    readonly property int cellWidth: 50
    readonly property int cellHeight: 50
    readonly property int offsetX: 40
    readonly property int offsetY: 40

    function safeControllerAccess(callback) {
        if (controller) callback()
    }

    function logAllPieces() {
        console.log("当前所有棋子状态:")
        for (let i = 0; i < root.chessData.length; i++) {
            let piece = root.chessData[i]
            console.log(i + ": " + piece.name + " 位置:(" + piece.x + "," + piece.y + ") 颜色:" + piece.color)
        }
    }

    function handleBoardClick(clickX, clickY) {
        let boardX = Math.round((clickX - offsetX) / cellWidth)
        let boardY = Math.round((clickY - offsetY) / cellHeight)
        
        if (boardX < 0 || boardX >= 9 || boardY < 0 || boardY >= 10) return

        let clickedPieceIndex = -1
        for (let i = 0; i < root.chessData.length; i++) {
            let p = root.chessData[i]
            if (p && p.x === boardX && p.y === boardY) {
                clickedPieceIndex = i
                break
            }
        }
        
        if (selectedIndex >= 0) {
            if (clickedPieceIndex === selectedIndex) {
                selectedIndex = -1
            } else {
                root.requestMove(selectedIndex, boardX, boardY)
                selectedIndex = -1
            }
        } else {
            if (clickedPieceIndex >= 0) {
                let piece = root.chessData[clickedPieceIndex]
                if (controller && piece.color === controller.currentPlayer) {
                    selectedIndex = clickedPieceIndex
                }
            }
        }
    }

    TapHandler {
        onTapped: function(eventPoint) {
            handleBoardClick(eventPoint.position.x, eventPoint.position.y)
        }
    }

    Canvas {
        id: chessCanvas
        anchors.fill: parent
        z: -1
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.lineWidth = 1
            ctx.strokeStyle = "#000"

            for (var y = 0; y < 10; y++) {
                ctx.beginPath()
                ctx.moveTo(offsetX, offsetY + y * cellHeight)
                ctx.lineTo(offsetX + 8 * cellWidth, offsetY + y * cellHeight)
                ctx.stroke()
            }

            for (var x = 0; x < 9; x++) {
                ctx.beginPath()
                ctx.moveTo(offsetX + x * cellWidth, offsetY)
                ctx.lineTo(offsetX + x * cellWidth, offsetY + 4 * cellHeight)
                ctx.stroke()

                ctx.beginPath()
                ctx.moveTo(offsetX + x * cellWidth, offsetY + 5 * cellHeight)
                ctx.lineTo(offsetX + x * cellWidth, offsetY + 9 * cellHeight)
                ctx.stroke()
            }
        }
    }
    
    Repeater {
        model: root.chessData

        delegate: Item {
            property int index: model.index
            property bool isCurrentPlayerPiece: modelData && controller ? modelData.color === controller.currentPlayer : false
            property bool isKing: modelData ? modelData.name.indexOf("King") !== -1 : false
            
            visible: modelData && modelData.x >= 0 && modelData.y >= 0
            width: cellWidth
            height: cellHeight
            
            x: visible ? (offsetX + modelData.x * cellWidth - width / 2) : -500
            y: visible ? (offsetY + modelData.y * cellHeight - height / 2) : -500
            
            z: root.selectedIndex === index ? 2 : 1

            Image {
                anchors.fill: parent
                source: modelData ? "qrc:/image/" + modelData.icon : ""
            }

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: root.selectedIndex === index ? "blue" : "transparent"
                border.width: 3
                radius: width / 2
            }
            
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: isCurrentPlayerPiece ? ((controller && controller.currentPlayer === "红") ? "#ff6666" : "#666666") : "transparent"
                border.width: 1
                radius: width / 2
                opacity: 0.5
                visible: isCurrentPlayerPiece && root.selectedIndex !== index && !root.gameOver
            }
            
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "red"
                border.width: 3
                radius: width / 2
                opacity: 0.8
                visible: isKing && controller && controller.isCheck && modelData.color === controller.checkedPlayer && !controller.gameOver
            }
        }
    }

    Component.onCompleted: {
        safeControllerAccess(function() {
            root.chessData = controller.getPieces()
            root.currentPlayer = controller.currentPlayer
            root.gameOver = controller.gameOver
            root.isCheck = controller.isCheck
            root.isCheckMate = controller.isCheckMate
            logAllPieces()
        })
    }

    Connections {
        target: controller
        
        function onChessDataChanged() {
            safeControllerAccess(function() {
                root.chessData = controller.getPieces()
                logAllPieces()
            })
        }
        
        function onCurrentPlayerChanged() {
            safeControllerAccess(function() {
                root.currentPlayer = controller.currentPlayer
                root.selectedIndex = -1
                logAllPieces()
            })
        }
        
        function onCapturedPiecesChanged() {
            safeControllerAccess(function() {
                root.chessData = controller.getPieces()
                logAllPieces()
            })
        }
        
        function onGameOverChanged() {
            safeControllerAccess(function() {
                root.gameOver = controller.gameOver
                root.selectedIndex = -1
            })
        }
        
        function onIsCheckChanged() {
            safeControllerAccess(function() {
                root.isCheck = controller.isCheck
            })
        }
        
        function onIsCheckMateChanged() {
            safeControllerAccess(function() {
                root.isCheckMate = controller.isCheckMate
            })
        }
        
        function onCheckedPlayerChanged() {}
        function onSelfCheckMoveChanged() {}
    }

    onRequestMove: function(fromIndex, toX, toY) {
        safeControllerAccess(function() {
            controller.handleMove(fromIndex, toX, toY)
        })
    }
}
