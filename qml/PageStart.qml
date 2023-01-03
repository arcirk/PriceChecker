import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page {

    id: pageConnanctions
    property string theme: "Dark"

    GridLayout{
        id: grid
        columns: 2
        //Layout.fillHeight: true
        //Layout.fillWidth: true


           Text{
               leftPadding: 10
               topPadding: 10
               color: "gray"
               text: "Организация:"
           }
           Text{
               topPadding: 10
               text: "Тест"
               color: "gray"
           }

           Text{
               leftPadding: 10
               text: "Подразделение:"
               color: "gray"
           }
           Text{
               color: "gray"
               text: "Тест"
           }
           Text{
               leftPadding: 10
               text: "Склад:"
               color: "gray"
           }
           Text{
               color: "gray"
               text: "Тест"
           }
           Text{
               leftPadding: 10
               text: "Тип цен:"
               color: "gray"
           }
           Text{
               color: "gray"
               text: "Тест"
           }
    }

}
