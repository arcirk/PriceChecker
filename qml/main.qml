import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
//import QtQuick.Dialogs 1.3

import Settings 1.0
import WebSocketClient 1.0
import BarcodeParser 1.0
import BarcodeInfo 1.0

ApplicationWindow {
    width: 1280
    height: 800
    visible: true
    title: qsTr("Прайс - чекер")

    property string myTheme: "Dark"
    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark

    property BarcodeParser wsBarcodeParser: BarcodeParser{
        onBarcode: function(value){
            //console.log(value)
            wsClient.get_barcode_information(value, wsBarcodeInfo)
        }
    }

    property BarcodeInfo wsBarcodeInfo: BarcodeInfo {
        id: barcodeInfo;

        onBarcodeInfoChanged: {
            pageStart.setBarcode(wsBarcodeInfo);
            if(wsBarcodeInfo.isLongImgLoad)
                wsClient.get_image_data(wsBarcodeInfo);
        }

        onImageSourceChanged: {
            //console.log("onImageSourceChanged");
            pageStart.changeImageSource(wsBarcodeInfo);
        }

        onClearData: {
            pageStart.setBarcode(wsBarcodeInfo);
        }
    }

    Item {
        //При использовании клавиатурного ввода на сканере
        id: attachFocus
        focus: wsSettings.keyboardInputMode
        Keys.onPressed: (event)=>{
            if(!attachFocus.focus)
                return;
            //console.log("" + event.key + " " + event.text);
            wsBarcodeParser.addSumbol(event.key, event.text);
        }
    }

    OptionsDialog {
        id: optionsDlg
        visible: false

        onWebSocketConnect: {
            if(!wsClient.isStarted()){
                wsClient.setUrl(wsSettings.url())
                wsClient.open(wsSettings);
            }
        }

        onVisibleChanged: {
            attachFocus.focus = !optionsDlg.visible;
        }
    }

    EnterBarcodeDialog {
        id: enterBarcodeDlg
        visible: false

        onEnterBarcodeFromKeyboard: function(value){
            wsClient.get_barcode_information(value, wsBarcodeInfo)
        }

        onVisibleChanged: {
            attachFocus.focus = !enterBarcodeDlg.visible;
        }
    }


    property Settings wsSettings: Settings{
        //deviceId: qtAndroidService.deviceId

        onUpdateWorkplaceView: function(org, suborg, stok, price){
            pageStart.organization = org;
            pageStart.subdivision = suborg;
            pageStart.warehouse = stok;
            pageStart.price = price;
            pageScanner.organization = org;
            pageScanner.subdivision = suborg;
            pageScanner.warehouse = stok;
            pageScanner.price = price;
        }
    }

    property WebSocketClient wsClient: WebSocketClient{
        id: webSocketClient

        onConnectionSuccess: {
            //console.log("onConnectionSuccess")

            //wsClient.updateHttpServiceConfiguration();
        }
        onDisplayError: function(what, err){
            //console.error(what + ": " + err)
            popupMessage.showError(what, err);
        }
        onConnectionChanged: function(state){
            //btnConnectionState.icon.source = state ?  "qrc:/img/lan_check_online.png" : "qrc:/img/lan_check_offline.png"
            mainToolBar.updateIcons(state)
            if(state){
                popupMessage.show("Успешное подключение к серверу");
            }else
                popupMessage.showError("Ошибка", "Сервер не доступен!");

            if(optionsDlg.visible)
                optionsDlg.connectionState(state);
        }

        onUpdateHsConfiguration: function(hsHost, hsUser, hsPwd){
            optionsDlg.updateHttpServiceConfiguration(hsHost, hsUser, hsPwd);
        }

        onNotify: function(message){
            popupMessage.show(message);
        }
    }

    menuBar: ToolBar{
        id: mainToolBar
        width: parent.width
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        function updateIcons(connectionState){
            if(connectionState)
               btnConnectionState.icon.source = "qrc:/img/lan_check_online.png"
            else
               btnConnectionState.icon.source = "qrc:/img/lan_check_offline.png"
        }

        RowLayout{
            ToolButton{
                id: btnScan
                icon.source: "qrc:/img/qr16.png"
                onClicked: {
//                    if(stackView.currentItem != pageScanner){
//                        var item = stackView.find(function(item) {
//                            return item.objectName === "pageScanner";
//                        })
//                        if(item === null)
//                            stackView.push(pageScanner);
//                        else{
//                            stackView.pop(item)
//                        }
//                    }else
//                        stackView.pop()
                    enterBarcodeDlg.br = ""
                    enterBarcodeDlg.visible = true
                }
            }
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
                        if(!wsClient.isStarted()){
                            wsClient.setUrl(wsSettings.url());
                            wsClient.open(wsSettings);
                        }else{
                            wsClient.close();
                        }
                        attachFocus.focus = true;
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

    onWindowStateChanged: function(windowState) {
        console.log( "onWindowStateChanged (Window), state: " + windowState );
        if(!wsClient.isStarted() && windowState !== Qt.WindowMinimized){
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

    Connections {
        target: qtAndroidService
        function onMessageFromService(message) {

//            var item = stackView.find(function(item) {
//                return item.objectName === "pageScanner";
//            })
//            if(item === null){
//                stackView.push(pageScanner);
//            }else{
//                if(stackView.currentItem !== item)
//                     stackView.push(pageScanner);
//            }

//            pageScanner.setBarcode(message)
            if(wsClient.isStarted()){
                //wsClient.sendBarcode(message, pageScanner.document_id)
                wsClient.get_barcode_information(message, wsBarcodeInfo)
            }

            console.log("barcode: " + message)
        }
    }
}
