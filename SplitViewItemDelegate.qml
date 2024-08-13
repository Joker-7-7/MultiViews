import QtQuick
import QtQuick.Controls

Item {
    id: root
    objectName: "<constructing ItemDelegate>"

    property int myIID: 0
    property var mySplitBase: this
    property var mySplitView: SplitView.view

    anchors.fill: root.mySplitView ? null : parent

    SplitView.preferredWidth: root.mySplitView
                  ? root.mySplitView.orientation === Qt.Horizontal
                    ? root.mySplitView.width / root.mySplitView.count
                    : root.mySplitView.width
                  : 0

    SplitView.preferredHeight: root.mySplitView
                  ? root.mySplitView.orientation === Qt.Vertical
                    ? root.mySplitView.height / root.mySplitView.count
                    : root.mySplitView.height
                  : 0

    function split(orientation) {
        if (root.mySplitView) {
            root.mySplitView.split(this, orientation)
        }
        else {
            console.warn("YIKES!! DynamicSplitView.ItemDelegate.split() called when this item is NOT inside a QML SplitView");
        }
    }

    function unsplit() {
        if (root.mySplitView) {
            root.mySplitView.unsplit(this);
        }
        else {
            console.warn("YIKES!! DynamicSplitView.ItemDelegate.unsplit() called when this item is NOT inside a QML SplitView");
        }
    }

    Component.onDestruction: {
      console.log("destructed", this, root.mySplitView)
    }
}
