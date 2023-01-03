import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
//import QtQuick.Dialogs 1.3

import Settings 1.0
import WebSocketClient 1.0


ApplicationWindow {
    width: 1280
    height: 800
    visible: true
    title: qsTr("Прайс - чекер")

    property string myTheme: "Dark"
    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark

    OptionsDialog {
        id: optionsDlg
        visible: false
    }

    property Settings wsSettings: Settings{
        //deviceId: qtAndroidService.deviceId
    }

    property WebSocketClient wsClient: WebSocketClient{
        id: webSocketClient

        onConnectionSuccess: {
            console.log("onConnectionSuccess")
        }
        onDisplayError: function(what, err){
            console.error(what + ": " + err)
        }
        onConnectionChanged: function(state){
            btnConnectionState.icon.source = state ?  "qrc:/img/lan_check_online.png" : "qrc:/img/lan_check_offline.png"
            if(state){
                popupMessage.show("Успешное подключение к серверу")
            }else
                popupMessage.showError("Ошибка", "Сервер не доступен!")

            //pageOptions.connectionChanged(state)
        }

    }

    menuBar: ToolBar{
        id: mainToolBar
        width: parent.width
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        RowLayout{
            //Layout.fillWidth: true
            Label{
                id: lblTitle
                Layout.fillWidth: true
                text:  ""
            }
            width: parent.width
            Row{
                //Layout.alignment: Qt.AlignRigh
                ToolButton{
                    id: btnConnectionState
                    icon.source: "qrc:/img/lan_check_offline.png"

                    onClicked: {

                    }
                }
                ToolButton{
                    id: btnOptions
                    icon.source: "qrc:/img/setting.png"

                    onClicked: {
                        optionsDlg.visible = true
                    }
                }
            }


        }
    }

    onWindowStateChanged: {
        console.log( "onWindowStateChanged (Window), state: " + windowState );
        if(!wsClient.isStarted() && windowState != 1){
            wsClient.setUrl(wsSettings.url())
//            wsClient.setName(wsSettings.userName)
//            wsClient.setHash(wsSettings.hash)
//            wsClient.setAppName("litescanner")
//            wsClient.setDeviceId(wsSettings.deviceId)
//            wsSettings.save()
            wsClient.open(wsSettings);
        }
    }

    StackView{
        id: stackView
        anchors.fill: parent

        initialItem: PageStart{
            id: pageStart
            objectName: "pageStart"
            theme: myTheme

//            onSelectWorkplace: function(uuid, job_desc, job){
//                console.log("onSelectWorkplace: " + uuid + " " + job_desc + " " + job)
//                var item = stackView.find(function(item) {
//                    return item.objectName === "pageDocuments";
//                })
//                if(item === null){
//                    pageDocuments.workplace = job_desc
//                    pageDocuments.uuidRecipient = uuid
//                }else{
//                    item.workplace = job_desc
//                    item.uuidRecipient = uuid
//                }
//                wsClient.currentRecipient = uuid
//                wsClient.command_to_client(uuid, "get_list_forms", "{\"recipient\": \"" + wsClient.uuid() + "\"}", "")
//            }
        }


    }

    PageScanner{
        objectName: "pageScanner"
        id: pageScanner
        visible: false
    }
    Row{
        id: poupText
        anchors.top: stackView.bottom
        width: stackView.width
        Layout.fillWidth: true
        PopupMessage{
            id: popupMessage
            parent: poupText
            anchors.centerIn: parent
        }
    }

}
