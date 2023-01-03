import QtQuick 2.15
import QtQuick.Controls 2.15

Item{

    function show(message){
        toolTip.visible = true
        var text = qsTr("<table width=\"100%\" cols=2 align=\"center\">
                         <tr><td width=\"20\" align=\"center\"><img src='qrc:/img/info16.png'/></td><td style=\"text-align:center\">" + message + "</td></tr></table>")
        toolTip.show(text)
    }
    function showError(what, err){
        toolTip.visible = true
        var text = qsTr("<table width=\"100%\ cols=2  align=\"center\>
                         <tr><td width=\"20\" align=\"center\"><img src='qrc:/img/error16.png'/></td><td style=\"text-align:center\"> " + what + ": " + err + "</td></tr></table>")
        toolTip.show(text)
    }
    Row{
        ToolTip {
            id: toolTip
            parent: parent
            visible: false
            delay: 1000
            timeout: 5000
            anchors.centerIn: parent
            verticalPadding: 10
        }
    }
}


