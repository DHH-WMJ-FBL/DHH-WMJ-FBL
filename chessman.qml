import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    id: root
    width: 540
    height: 600

    property int cellWidth: 50
    property int cellHeight: 50
    property int offsetX: 40
    property int offsetY: 40

    property var chessData: []   // 从 C++ 注入
    property int selectedIndex: -1
    property string currentPlayer: "红" // 默认红方先行
    property bool gameOver: false
    property bool isCheck: false
    property bool isCheckMate: false
    signal requestMove(int fromIndex, int toX, int toY)

    // 调试函数，列出所有棋子
    function logAllPieces() {
        console.log("当前所有棋子状态:");
        for (var i = 0; i < root.chessData.length; i++) {
            var piece = root.chessData[i];
            if (piece) {
                console.log(i + ": " + piece.name + " 位置:(" + piece.x + "," + piece.y + ") 颜色:" + piece.color);
            }
        }
    }

    // 棋盘点击事件处理 - 简化版本
    function handleBoardClick(mouseX, mouseY) {
        // 如果游戏已结束，不处理点击事件
        if (root.gameOver) {
            console.log("游戏已结束，不处理点击事件");
            return;
        }
        
        console.log("处理棋盘点击:", mouseX, mouseY)
        
        const boardX = Math.round((mouseX - offsetX) / cellWidth)
        const boardY = Math.round((mouseY - offsetY) / cellHeight)
        
        // 检查点击的位置是否在棋盘范围内
        if (boardX < 0 || boardX >= 9 || boardY < 0 || boardY >= 10) {
            console.log("点击位置超出棋盘范围:", boardX, boardY)
            return
        }
        
        console.log("棋盘坐标:", boardX, boardY)
        
        // 检查当前位置是否有棋子
        let clickedPieceIndex = -1
        for (let i = 0; i < root.chessData.length; i++) {
            let p = root.chessData[i]
            if (p && p.x === boardX && p.y === boardY) {
                clickedPieceIndex = i
                console.log("找到棋子:", p.name, "索引:", i)
                break
            }
        }
        
        if (selectedIndex >= 0) {
            // 已经选中了棋子，尝试移动
            if (clickedPieceIndex === selectedIndex) {
                // 点击了已选中的棋子，取消选择
                console.log("取消选择")
                selectedIndex = -1
            } else {
                // 走棋或吃子
                console.log("尝试移动:", selectedIndex, "到", boardX, boardY)
                root.requestMove(selectedIndex, boardX, boardY)
                selectedIndex = -1
            }
        } else {
            // 没有选中棋子，尝试选择一个
            if (clickedPieceIndex >= 0) {
                let piece = root.chessData[clickedPieceIndex]
                if (piece.color === controller.currentPlayer) {
                    console.log("选择棋子:", piece.name)
                    selectedIndex = clickedPieceIndex
                } else {
                    console.log("不是当前玩家的棋子")
                }
            } else {
                console.log("点击位置没有棋子")
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: function(mouse) {
            handleBoardClick(mouse.x, mouse.y)
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
            id: pieceItem
            property int index: model.index
            property bool isCurrentPlayerPiece: modelData ? modelData.color === controller.currentPlayer : false
            property bool isKing: modelData ? modelData.name.indexOf("King") !== -1 : false
            
            visible: modelData && modelData.x >= 0 && modelData.y >= 0
            width: cellWidth
            height: cellHeight
            
            x: visible ? (offsetX + modelData.x * cellWidth - width / 2) : -500
            y: visible ? (offsetY + modelData.y * cellHeight - height / 2) : -500
            
            z: root.selectedIndex === index ? 2 : 1

            Image {
                id: icon
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
            
            // 当前玩家的棋子添加高亮效果
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: isCurrentPlayerPiece ? (controller.currentPlayer === "红" ? "#ff6666" : "#666666") : "transparent"
                border.width: 1
                radius: width / 2
                opacity: 0.5
                visible: isCurrentPlayerPiece && root.selectedIndex !== index && !root.gameOver
            }
            
                            // 将军状态下的将/帅添加特殊高亮
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "red"
                    border.width: 3
                    radius: width / 2
                    opacity: 0.8
                    // 只高亮被将军的一方的将/帅，但在双方回合都可见，游戏结束时不显示
                    visible: isKing && controller.isCheck && modelData.color === controller.checkedPlayer && !controller.gameOver
                }
        }
    }

    Component.onCompleted: {
        root.chessData = controller.getPieces()
        root.currentPlayer = controller.currentPlayer
        root.gameOver = controller.gameOver
        root.isCheck = controller.isCheck
        root.isCheckMate = controller.isCheckMate
        console.log("初始化完成，棋子数量:", root.chessData.length)
        logAllPieces()
    }

    Connections {
        target: controller
        function onChessDataChanged() {
            console.log("棋盘数据已更新")
            root.chessData = controller.getPieces()
            logAllPieces()
        }
        
        function onCurrentPlayerChanged() {
            console.log("当前玩家已更改:", controller.currentPlayer)
            root.currentPlayer = controller.currentPlayer
            root.selectedIndex = -1  // 切换玩家时清除选择
            logAllPieces()
        }
        
        function onCapturedPiecesChanged() {
            console.log("有棋子被吃掉，更新视图")
            root.chessData = controller.getPieces()
            logAllPieces()
        }
        
        function onGameOverChanged() {
            console.log("游戏状态已更改:", controller.gameOver)
            if (controller.gameOver) {
                console.log("游戏结束，获胜方:", controller.winner, "是否为将死:", controller.isCheckMate)
            }
            root.gameOver = controller.gameOver
            root.selectedIndex = -1  // 游戏结束时清除选择
        }
        
        function onIsCheckChanged() {
            console.log("将军状态已更改:", controller.isCheck)
            root.isCheck = controller.isCheck
        }
        
        function onIsCheckMateChanged() {
            console.log("将死状态已更改:", controller.isCheckMate)
            root.isCheckMate = controller.isCheckMate
            if (controller.isCheckMate) {
                console.log("检测到将死状态，游戏结束状态:", controller.gameOver)
            }
        }
        
        function onCheckedPlayerChanged() {
            console.log("被将军的玩家已更改:", controller.checkedPlayer)
        }
        
        function onSelfCheckMoveChanged() {
            console.log("自己被将军状态更改:", controller.selfCheckMove)
        }
    }

    onRequestMove: function(fromIndex, toX, toY) {
        console.log("发送移动请求:", fromIndex, toX, toY)
        controller.handleMove(fromIndex, toX, toY)
    }
}
