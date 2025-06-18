import QtQuick 2.15

Item {
    width: 540
    height: 600
    z: 1

    property int cellWidth: 50
    property int cellHeight: 50
    property int offsetX: 40
    property int offsetY: 40
    property int selectedX: -1
    property int selectedY: -1

    property string imagePath: "qrc:/image/"

    function pos(x, y) {
        return Qt.point(
            x * cellWidth + offsetX - 25,
            y * cellHeight + offsetY - 25
        )
    }

    function createPiece(src, x, y) {
        return Qt.createQmlObject(`
            import QtQuick 2.15;
            Item {
                width: 50; height: 50;
                x: ${pos(x, y).x};
                y: ${pos(x, y).y};

                property int px: ${x}
                property int py: ${y}

                Image {
                    anchors.fill: parent
                    source: "${imagePath + src}"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        selectedX = px
                        selectedY = py
                        console.log("选中棋子:", px, py)
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    z: 99
                    color: "transparent"
                    border.color: selectedX === px && selectedY === py ? "blue" : "transparent"
                    border.width: 3
                    radius: width / 2
                }
            }
        `, chessContainer);  // ✅ 用 chessContainer 替代 root
    }

    Item {
        id: chessContainer
        anchors.fill: parent

        Component.onCompleted: {
            // 红方
            createPiece("rook_red.png", 0, 9);
            createPiece("horse_red.png", 1, 9);
            createPiece("elephant_red.png", 2, 9);
            createPiece("advisor_red.png", 3, 9);
            createPiece("king_red.png", 4, 9);
            createPiece("advisor_red.png", 5, 9);
            createPiece("elephant_red.png", 6, 9);
            createPiece("horse_red.png", 7, 9);
            createPiece("rook_red.png", 8, 9);
            createPiece("cannon_red.png", 1, 7);
            createPiece("cannon_red.png", 7, 7);
            for (let i = 0; i < 5; i++)
                createPiece("soldier_red.png", i * 2, 6);

            // 黑方
            createPiece("rook_black.png", 0, 0);
            createPiece("horse_black.png", 1, 0);
            createPiece("elephant_black.png", 2, 0);
            createPiece("advisor_black.png", 3, 0);
            createPiece("king_black.png", 4, 0);
            createPiece("advisor_black.png", 5, 0);
            createPiece("elephant_black.png", 6, 0);
            createPiece("horse_black.png", 7, 0);
            createPiece("rook_black.png", 8, 0);
            createPiece("cannon_black.png", 1, 2);
            createPiece("cannon_black.png", 7, 2);
            for (let i = 0; i < 5; i++)
                createPiece("soldier_black.png", i * 2, 3);
        }
    }
}
