import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page {

    id: thisPage
    property string theme: "Dark"
    padding: 10

    property alias organization: org.text
    property alias subdivision: suborg.text
    property alias warehouse: stock.text
    property alias price: priceText.text

    GridLayout{
        id: grid
        columns: 2

           Text{
               leftPadding: 10
               topPadding: 10
               color: "gray"
               text: "Организация:"
           }
           Text{
               id: org
               topPadding: 10
               color: "gray"
           }

           Text{
               leftPadding: 10
               text: "Подразделение:"
               color: "gray"
           }
           Text{
               id: suborg
               color: "gray"

           }
           Text{
               leftPadding: 10
               text: "Склад:"
               color: "gray"
           }
           Text{
               color: "gray"
               id: stock
           }
           Text{
               leftPadding: 10
               text: "Тип цен:"
               color: "gray"
           }
           Text{
               color: "gray"
               id: priceText
           }
    }

}
